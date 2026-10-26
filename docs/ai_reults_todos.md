# webserv – Stand & TODOs (AI-Review, 2026-10-26)

Basis: kompletter Code auf Branch `rgohrig-2` (Commit `783c69f`) gelesen, gebaut
(`make` warning-frei) und mit `curl`/`nc` gegen eine Test-Config angetestet.

**Kurzfazit:** Das Fundament (Config-Parser, epoll-Loop, Request-Parser,
GET/POST/DELETE für statische Dateien, Pfad-Sicherheit) steht und ist
ordentlich. Für die Abgabe fehlen aber noch **mehr als nur CGI + Tests**:
`autoindex`, `return` (Redirects), `error_page` und Timeouts sind zwar teils
im Config-Parser vorhanden, haben aber **keine Runtime-Implementierung**. Dazu
kommt ein **harter Subject-Verstoß** (errno-Check nach recv/send).

Grobe Schätzung bis abgabereif: ca. **55–65 % geschafft**. CGI ist der größte
Einzelblock (~30 % der Restarbeit), danach Timeouts/Resilience, die fehlenden
Config-Features und Tests/Demo-Configs.

---

## 1. Smoke-Test-Ergebnisse (was tatsächlich läuft)

| Request | Ergebnis | Bewertung |
|---|---|---|
| `GET /` , `GET /assets/style.css` | 200 | ✅ |
| `GET /nothere` | 404 (text/plain) | ✅ |
| `POST /` bei `limit_except GET` | 405 + `Allow`-Header | ✅ |
| `POST --data hello /up/a.txt` | 201, Datei angelegt | ✅ |
| POST über `client_max_body_size` | 413 | ✅ (aber erst nach komplettem Empfang, s.u.) |
| `DELETE /up/a.txt` | 204 | ✅ |
| Chunked POST | 201, korrekt entchunkt | ✅ |
| `GET /../../etc/passwd` (path-as-is) | 403 | ✅ |
| `autoindex on` auf Verzeichnis | **403** | ❌ nicht implementiert |
| `location /redir { return 301 ...; }` | **200** (liefert index.html) | ❌ `return` wird ignoriert |
| `error_page 404 /nope.html;` | **Config-Parse-Fehler, Server startet nicht** | ❌ Parser-Bug |
| Server-Block ohne `location` | **404 für alles** | ❌ server-level `root` wird nie benutzt |
| `HEAD` / `PUT` | 400 | ⚠️ sollte 405 bzw. 501 sein |
| Idle-Verbindung (nc, nichts senden) | **hängt ewig** | ❌ kein Timeout |
| 3× großen Download (50 MB) abbrechen | Server lebt weiter | ✅ (SIGPIPE-Risiko besteht trotzdem, s.u.) |

---

## 2. KRITISCH – Muss vor Abgabe (Subject-Verstöße / Pflicht-Features)

Sortiert nach Wichtigkeit.

1. **errno-Check nach `recv`/`send` entfernen** – *klein, aber Note 0-Risiko*
   - [Connection.cpp](../src/server/Connection.cpp) `receive()` und `send()`
     prüfen `errno == EAGAIN/EINTR/EWOULDBLOCK`. Subject: *"Checking the value
     of errno ... after performing a read or write operation is strictly
     forbidden."*
   - Fix: bei `< 0` einfach Verbindung schließen (epoll garantiert ja
     Readiness). Kein `strerror(errno)`-Logging dort.

2. **CGI komplett** – *größter Block*
   - Aktuell existiert nur `struct CGIConfig` ([CGIConfig.hpp](../include/cgi/CGIConfig.hpp))
     und das Feld `Location::cgi`. Es gibt **keine Config-Direktive** dafür
     (nicht in `directive_lookup`), keinen fork/execve, nichts zur Laufzeit.
   - Teilaufgaben:
     - [ ] Config-Direktive(n), z.B. `cgi .py /usr/bin/python3;` (+ optional
           `cgi_timeout`) in [Config.hpp](../include/server_config/Config.hpp)
           `directive_lookup` + `fill_cgi`; in `inherit.cpp` vererben.
     - [ ] In `handle_request`: wenn Extension der aufgelösten Datei in
           `cgi->interpreter_by_extension` → CGI statt statischer Datei.
     - [ ] `pipe()` ×2, `fork()`, im Kind `dup2`, `chdir()` ins Script-Verzeichnis
           (*"correct directory for relative path access"*), `execve`.
     - [ ] Env-Variablen: `REQUEST_METHOD`, `QUERY_STRING` (Split existiert schon
           in `Request::split_uri_target`), `CONTENT_LENGTH`, `CONTENT_TYPE`,
           `SCRIPT_FILENAME`, `SCRIPT_NAME`, `PATH_INFO`, `SERVER_PROTOCOL`,
           `GATEWAY_INTERFACE=CGI/1.1`, `SERVER_NAME/PORT`, `REMOTE_ADDR`,
           `HTTP_*` für alle Header, `REDIRECT_STATUS=200` (für php-cgi).
     - [ ] **Pipes non-blocking und über dieselbe epoll-Instanz**: neue
           `EpollHandler`-Unterklassen (z.B. `CgiWritePipe` für stdin,
           `CgiReadPipe` für stdout), die auf die `Connection` zurückzeigen.
           Body ist schon entchunkt → einfach reinschreiben, dann Pipe
           schließen (EOF).
     - [ ] `Connection`-State erweitern (`WAITING_CGI`), Connection darf nicht
           auf EPOLLIN warten während CGI läuft.
     - [ ] CGI-Output parsen: Header-Block (`Status:`, `Content-Type:`,
           `Location:`), Rest = Body; ohne `Content-Length` endet er bei EOF.
     - [ ] Timeout (`timeout_ms` ist im Struct schon da) → `kill` + 504.
     - [ ] `waitpid(..., WNOHANG)` zum Einsammeln (keine Zombies), Exit≠0 → 502.
     - [ ] Wenn Client während CGI abhaut: Kind killen, Pipes schließen.
   - Achtung: `SIGCHLD`/Signale unterbrechen `epoll_wait` → siehe Punkt 4.

3. **Timeouts – "A request should never hang indefinitely"**
   - Es gibt keinerlei Timeout. Eine offene, stille Verbindung bleibt für
     immer (getestet). Bei Stresstest / Slowloris läuft das voll.
   - Fix: `last_activity_` (z.B. `std::chrono::steady_clock`) in `Connection`,
     im Main-Loop nach `wait_epoll()` alle Connections prüfen (Timeout von
     `EPOLL_TIMEOUT_MS = 1000` passt dafür schon) → 408 senden oder schließen.
     Gleiches für CGI-Prozesse.

4. **Server darf nie abstürzen** – mehrere kleine Löcher
   - `epoll_wait` == -1 wirft Exception → `main` beendet den Server. Bei
     `EINTR` (Signal, später SIGCHLD von CGI) einfach weitermachen.
     ([AllServer.cpp](../src/server/AllServer.cpp) `wait_epoll`)
   - Jede Exception aus einem Handler (z.B. `std::bad_alloc`, eine
     filesystem-Exception) fliegt bis `main` und beendet den Server. →
     `on_epoll_event` in `wait_epoll` pro Event in `try/catch` packen, bei
     Fehler 500 senden bzw. Verbindung schließen.
   - **SIGPIPE**: `send()` ohne `MSG_NOSIGNAL` und kein `signal(SIGPIPE,
     SIG_IGN)`. Schreibt man auf einen vom Client geschlossenen Socket, kann
     der Prozess getötet werden. Im Test nicht reproduziert, aber reines Glück.
     → `MSG_NOSIGNAL` bei `send` (oder SIGPIPE ignorieren). Später auch beim
     Schreiben in CGI-Pipes relevant!
   - `SocketFd::accept()` gibt bei Fehler trotzdem eine `Connection` mit
     fd -1 zurück, die nie aufgeräumt wird → `nullptr` zurückgeben und in
     `on_epoll_event` prüfen.
   - Optional sauberes Beenden bei SIGINT/SIGTERM (Flag setzen, Loop verlassen).

5. **`autoindex` (Directory Listing) implementieren**
   - Wird geparst und im `Location` gespeichert, aber
     [RequestHandlers.cpp](../src/messages/RequestHandlers.cpp) `handle_get`
     gibt bei Verzeichnis ohne Index-Datei immer 403 zurück.
   - Fix: wenn `location.autoindex` → HTML-Liste mit `opendir/readdir`
     (erlaubt) bzw. `std::filesystem::directory_iterator` erzeugen.

6. **`return` (HTTP-Redirect) zur Laufzeit**
   - Parser füllt `return_code`/`return_path`, `Location` hat Platzhalter
     (`888`, `"./example"`) und `handle_request` ignoriert es komplett.
   - Fix: in `handle_request` direkt nach `find_location`: wenn `return_code`
     gesetzt → Response mit Code + `Location:`-Header. Platzhalter durch
     `std::optional` ersetzen.

7. **`error_page` – Parser-Bug + Runtime fehlt**
   - Parser-Bug in [fill_default.cpp](../src/server_config/fill_default.cpp)
     `fill_error_page`: die Schleife ruft `get_next_word()` bis kein Wort mehr
     kommt → das wirft beim `;` sofort *"expected a word"*. **Jede Config mit
     `error_page` startet nicht.**
   - Datenmodell passt nicht: aktuell *eine* Liste Codes + *ein* Pfad pro
     Block. Mehrere `error_page`-Zeilen überschreiben sich. Besser
     `std::map<int, std::filesystem::path>`.
   - Runtime: `make_error_response(status)` muss die Location kennen und ggf.
     die konfigurierte Seite laden (Fallback: eingebaute Default-Seite).
   - Default-Fehlerseiten: aktuell Plain-Text `404 Not Found`. Reicht formal,
     schöner wäre eine kleine generierte HTML-Seite.

8. **Server-Block ohne `location` liefert immer 404**
   - [Server.cpp](../src/server/Server.cpp) baut `Location`s nur aus
     `locations_confs`; `root`/`index` auf Server-Ebene werden nie benutzt.
   - Fix: wenn keine `location /` existiert, implizit eine aus den
     Server-Werten anlegen (so verhält sich auch nginx).

9. **Upload-Speicherort + Browser-Upload (multipart)**
   - Subject: *"Uploading files ... is authorized, and storage location is
     provided."* Aktuell: POST schreibt den Raw-Body an den aufgelösten Pfad.
     Mit `curl --data` klappt das, ein **HTML-Formular aus dem Browser schickt
     aber `multipart/form-data`** → würde als Rohdatei inkl. Boundaries
     gespeichert, bei POST auf ein Verzeichnis gibt's 403.
   - Fix: Direktive wie `upload_store <dir>;` (oder POST-auf-Verzeichnis =
     Upload dorthin) + simplen multipart-Parser (Boundary aus
     `Content-Type`, `filename=` aus `Content-Disposition`).
   - Für die Evaluation (Browser-Demo) praktisch Pflicht.

10. **`client_max_body_size` früh prüfen**
    - Aktuell wird der komplette Body erst in den RAM gelesen, *dann* 413.
      Ein 2-GB-Upload landet komplett im Speicher. → Nach dem Header-Parse
      `Content-Length` gegen das Limit der Location prüfen, bei Chunked
      laufend. Dafür muss die Location schon beim Parsen bekannt sein
      (Parser bekommt `Server&`/Limit mit).

11. **Tests + Demo-Configs + Demo-Website** (Subject: *"You must provide
    configuration files and default files to test and demonstrate that every
    feature works"*)
    - [ ] Pro Feature eine Demo-Config in `config/` (mehrere Ports, Uploads,
          Redirect, autoindex, error_page, CGI py/php, Body-Limit).
    - [ ] `www/` erweitern: Upload-Formular, Fehlerseiten, Ordner für
          autoindex, `cgi-bin/` mit `.py`-Skripten (GET+POST, Query-String,
          Endlosschleife für Timeout-Test).
    - [ ] Eigenes Testskript (Python mit `socket`/`http.client` oder Bash+curl)
          – Subject verlangt explizit eigene Tests, nicht nur ein Tool.
    - [ ] 42-Tester (`tests/tester`, `tests/cgi_tester`) durchlaufen lassen.
    - [ ] Stresstest mit `siege -b` (Availability ≥ 99.5 % erwartet).
    - Hinweis: `/tests/` steht in `.gitignore` → eigene Testskripte woanders
      ablegen oder die Zeile anpassen.

12. **README.md fertig schreiben** (Pflichtabschnitte Description /
    Instructions / Resources + AI-Usage). Aktuell nur Platzhaltertext.

---

## 3. WICHTIG, aber kleiner (sollte rein, je < 1h)

- [ ] **`SO_REUSEADDR`** auf Listen-Sockets ([SocketFd.cpp](../src/server/SocketFd.cpp)),
      sonst nach Neustart "Address already in use" (steht auch in `docs/todos.md`).
- [ ] **`backlog_ = 10`** ist für Stresstests zu klein → `SOMAXCONN`.
- [ ] **Status-Codes genauer**: unbekannte/nicht unterstützte Methode
      (`HEAD`, `PUT`) → aktuell 400. Besser 405 (mit `Allow`) bzw. 501.
      Falsche HTTP-Version → 505 statt 400. (Der 42-Tester prüft u.a. `HEAD`.)
- [ ] **Response-Version inkonsistent**: `Response()` setzt `HTTP/1.1`,
      `Response(int)` ruft den Basis-Ctor nicht explizit → `HTTP/1.0`.
      Einheitlich machen ([Response.cpp](../src/messages/Response.cpp)).
- [ ] **Verzeichnis ohne `/` am Ende** (`/dir` statt `/dir/`) → 301 auf `/dir/`,
      sonst brechen relative Links in Index-Seiten/autoindex.
- [ ] **`index` mit mehreren Werten**: `index a.html b.html;` wirft
      Parse-Fehler (`fill_index` liest nur ein Wort).
- [ ] **`fill_return`**: `std::stoi` ohne try → bei Müll kein schöner
      Config-Fehler mit Zeile/Spalte. Code-Bereich prüfen (300–399 / 444 …).
- [ ] **`Location.cpp`-Platzhalter** (`999`, `888`, `"./example"`, `// todo`)
      aufräumen – auf `std::optional` umstellen.
- [ ] **`LOG(LOG_DEBUG, "t")`** in `main` bei jedem Loop entfernen.
- [ ] **Zwei `server` mit gleichem `listen`** → `bind` schlägt fehl, Server
      startet nicht. Entweder klare Fehlermeldung oder Sockets teilen
      (Virtual Hosts sind laut Subject optional).
- [ ] **MIME-Typen** ergänzen (`.gif`, `.ico`, `.pdf`, `.mp4`, `.webp`, …) –
      Browser-Kompatibilität (Favicon!).
- [ ] **`.gitignore` aufräumen**: enthält am Ende einen kompletten
      nginx-Config-Block, und die `config/*.conf` stehen drin, obwohl sie
      getrackt sind (neue Configs würden nicht committed!).

---

## 4. NICE TO HAVE (nicht nötig für Abgabe)

- Keep-Alive / Pipelining (aktuell immer `Connection: close` → für
  HTTP/1.0-Verhalten völlig ok).
- Große Dateien streamen statt komplett in RAM zu laden (aktuell 3 Kopien:
  Datei → `body_` → `serialize()` → `response_buffer_`). Für Eval-Größen ok.
- Request-Parser inkrementell machen (aktuell wird bei jedem `recv` der ganze
  Buffer von vorne geparst → O(n²) bei großen Uploads; mit `std::search` über
  den Header bei jedem Chunk). Wird erst bei großen Bodies spürbar.
- `server_name` / Virtual Hosts (laut Subject out of scope, Gerüst existiert).
- `Date`/`Server`-Header, `Last-Modified`.
- IPv6 im `listen`.
- Mehrere CGI-Interpreter, Cookies/Session-Demo (Bonus).
- Bonus-Teil des Subjects (Cookies/Session, mehrere CGI) – erst wenn
  Mandatory 100 % sitzt.

---

## 5. Was gut ist (so lassen)

- **Config-Pipeline** (Lexer → Parser mit `directive_lookup`-Tabelle →
  Vererbung über `std::optional`): sauber, gute Fehlermeldungen mit
  Zeile/Spalte, leicht erweiterbar (neue Direktive = 1 Tabelleneintrag +
  `fill_*`). Ideal, um `cgi`, `upload_store` usw. nachzurüsten.
- **epoll-Architektur**: `EpollHandler`-Basisklasse mit `data.ptr`,
  `SocketFd`/`Connection` als Unterklassen, deferred cleanup über
  `remove_me_` + `cleanup_all_fds()`. Genau das richtige Muster, um CGI-Pipes
  als weitere Handler einzuhängen.
- **RAII für fds** (`CloseFd`, `AddrInfoPtr`), rule-of-five explizit.
- **Request-Parser**: Header-Limit (8 KiB), Content-Length *und* Chunked
  (inkl. Trailer, Overflow-Check), Konflikt CL+TE → 400, Host-Pflicht bei
  HTTP/1.1, Header-Namen-Validierung. Solide.
- **Pfad-Auflösung/Sicherheit** ([Resolver.cpp](../src/routing/Resolver.cpp)):
  Percent-Decoding, `..`-Ablehnung, `weakly_canonical` + Root-Check → kein
  Directory-Traversal (getestet).
- **Location-Matching**: Longest-Prefix mit Segmentgrenze (`/up` matcht nicht
  `/upload`). Korrekt.
- GET/POST/DELETE mit sinnvollen Codes (200/201/204/403/404/405/413).
- Build: warning-frei mit `-Wall -Wextra -Werror`, Debug-Build mit
  ASan/UBSan.

---

## 6. Empfohlene Reihenfolge (nächste Schritte)

1. **Quick-Wins, ~1–2 h**: errno-Checks raus, `MSG_NOSIGNAL`, EINTR,
   try/catch pro Event, `accept`-Fehler, `SO_REUSEADDR`, backlog.
2. **Fehlende Config-Features, ~1 Tag**: `error_page` (Parser + Runtime),
   `return`, `autoindex`, implizite Location, Directory-Redirect, Status-Codes.
3. **Timeouts, ~2–3 h** (wird für CGI sowieso gebraucht).
4. **CGI, ~2–4 Tage**: Config-Direktive → fork/execve → Pipes in epoll →
   Output-Parsing → Timeout/Kill/waitpid.
5. **Upload**: Body-Limit früh prüfen + multipart + Upload-Ordner.
6. **Demo-Configs + `www/` + Testskript**, dann 42-Tester, cgi_tester, siege.
7. **README** + Aufräumen (Platzhalter, `.gitignore`, Debug-Logs).

---

## 7. Sonstige Infos / Stolperfallen

- `docs/todos.md` ist veraltet (fast alles noch `[ ]`, obwohl vieles fertig
  ist) – entweder mit dieser Liste ersetzen oder abhaken.
- `CLAUDE.md` beschreibt den Runtime-Stand als "hardcoded canned response" –
  das ist überholt, der echte Request-Pfad (`Connection` →
  `request_parsing` → `handle_request`) ist live.
- Regel zur Eval: jeder `recv`/`send`/`read`/`write` auf Socket **oder
  CGI-Pipe** nur nach epoll-Readiness aus *demselben* `epoll_wait`. Beim
  CGI-Bau besonders aufpassen (nicht direkt nach `fork` in die Pipe schreiben!).
- `fork` nur für CGI – auch nicht für "Hilfsprozesse".
- Configs in `config/` verweisen auf `./html/...`, das Verzeichnis gibt es
  nicht (nur `www/`). Für die Eval müssen die Demo-Configs mit dem
  mitgelieferten Content funktionieren.
