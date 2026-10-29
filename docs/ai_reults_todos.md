# webserv – Stand, Design & TODOs (AI-Review, 2026-10-28)

Ersetzt die alte Review vom 2026-10-26 **und** `docs/isue.md` (State-Machine-Design).

Basis: Branch `rgohrig-2`, Commit `6dad107` (+ ungetrackt: `scripts/smoke_test.sh`).
Gebaut (`make re`, `make both` warning-frei) und mit `curl`/`nc` angetestet.

**Kurzfazit:** Seit der letzten Review sind die meisten Quick-Wins erledigt
(errno-Checks, `MSG_NOSIGNAL`, `SO_REUSEADDR`, `SOMAXCONN`, Signal-Handler,
einfacher Timeout, `accept`-Fehler) und der Umbau zur State-Machine hat
begonnen. Größter offener Block bleibt **CGI** (nur Config-Parsing + ein
fork-Gerüst). Danach: `autoindex`, `return`, `error_page`, implizite Location,
Status-Codes, Upload (multipart) und Demo-Configs.

Grobe Schätzung bis abgabereif: ca. **60–70 % geschafft**.

---

## 1. Smoke-Test-Ergebnisse (Stand jetzt)

Automatisiert: `make && ./scripts/smoke_test.sh` (15 Checks, alle grün,
auch mit `./webserv_debug`/ASan).

| Request | Ergebnis | Bewertung |
|---|---|---|
| `GET /`, 404, Path-Traversal (403) | 200 / 404 / 403 | ✅ |
| `POST` neu / existiert / chunked | 201 / 204 / 201 | ✅ |
| `DELETE` / nochmal | 204 / 404 | ✅ |
| POST über `client_max_body_size` | 413 | ✅ (aber erst nach komplettem Empfang) |
| Müll-Request | 400 | ✅ |
| 5 MB Download, byte-identisch | 200 | ✅ |
| 20 parallele Requests | alle 200, Server lebt | ✅ |
| Stille Verbindung (`nc`) | nach 10 s geschlossen | ✅ (ohne 408, s.u.) |
| `autoindex on` auf Verzeichnis | **403** | ❌ nicht implementiert |
| `location /redir { return 301 ...; }` | **403** | ❌ `return` wird ignoriert |
| `error_page 404 /e.html;` | **Config-Fehler, Server startet nicht** | ❌ Parser-Bug |
| `index a.html b.html;` | **Config-Fehler** | ❌ nur ein Wert |
| `cgi .bla ...;` (42-Tester) | **Config-Fehler** | ❌ Whitelist `.py`/`.php` |
| Server-Block ohne `location` | **404 für alles** | ❌ server-`root` ungenutzt |
| `HEAD` / `PUT` | 400 | ⚠️ sollte 405 bzw. 501 sein |
| `HTTP/2.0` im Request | 400 | ⚠️ sollte 505 sein |
| Verzeichnis ohne `/` (`/dir`) | 403 | ⚠️ sollte 301 auf `/dir/` sein |

---

## 2. Erledigt seit der letzten Review

- errno-Checks nach `recv`/`send` entfernt (nur noch auskommentiert – ganz löschen).
- `send(..., MSG_NOSIGNAL)`, SIGPIPE-/SIGINT-Handler, sauberes Beenden über
  `g_stop_server`.
- `epoll_wait == -1` beendet den Server nicht mehr (nur Log).
- `SO_REUSEADDR`, `listen(fd, SOMAXCONN)`.
- `SocketFd::accept()` legt bei Fehler keine Connection mehr an.
- Einfacher Timeout: `last_activity_` + `check_timeouts()` im Main-Loop.
- `handle_*` aus `Request` herausgelöst: freie Funktionen, die eine
  `Response &` befüllen (`convert_to_error/empty/not_allowed`).
- `Connection::set_state()` eingeführt, `state_` wird nur noch dort gesetzt
  (`grep -n "state_ =" src/` → 1 Treffer). `stop_listening()` setzt keinen
  State mehr.
- `cgi .py|.php <interpreter>;`-Direktive geparst und vererbt (`cgi_interpers`).
- `.gitignore` aufgeräumt (kein nginx-Block, `config/*.conf` nicht mehr ignoriert).
- Makefile: parallel (`-j` nur im Top-Level-make), `re` sequenziell, saubere Ausgabe.
- Eigenes Testskript `scripts/smoke_test.sh` (**noch nicht committed!**).

---

## 3. Ziel-Design: Connection-State-Machine

Die Event-Loop (`EpollHandler` + `data.ptr`, `CloseFd`, `unique_ptr` in
`all_fds_`, verzögertes Löschen über `remove_me_`) ist gut und bleibt. Es geht
um die Schicht darüber.

### Problem (Rest)

- `handle_request()` ist **synchron** und gibt immer eine fertige Response.
  Für CGI (eigener Prozess, asynchron) braucht es einen zweiten Ausgang.
- Die `Location` ist erst nach dem kompletten Parse bekannt → 405 / `return` /
  413 kommen erst, wenn der ganze Body schon im RAM liegt.
- Fehler werden noch pro Funktion behandelt statt über eine Stelle.
- `CgiHandler` kennt die `Connection` nicht und kann seine Ausgabe nirgends
  hinschicken.

### Zustände

```
READ_HEAD ──Header komplett──> Location finden, Methode/return/Limit prüfen
   │                                 │
   │                       Body? ──ja──> READ_BODY ──fertig──┐
   │                                 └─nein─────────────────┤
   │                                                         v
   │                                            handle_request() (synchron)
   │                                        statisch ──┐    │ CGI
   │                                                   v    v
   └──Fehler (fail)──────────────────────────────> WRITE <── WAIT_CGI
                                                     │
                                                  FINISHED
```

| Zustand | epoll-Maske | Was passiert | Timeout |
|---|---|---|---|
| `READ_HEAD` | `EPOLLIN` | recv, Header parsen | kurz → 408 |
| `READ_BODY` | `EPOLLIN` | recv, Body-Limit **laufend** prüfen → 413 | kurz → 408 |
| `WAIT_CGI` | `EPOLLRDHUP` | warten, bis `CgiHandler` sich meldet | lang → Kind killen, 504 |
| `WRITE` | `EPOLLOUT` | send mit Offset | normal → schließen |
| `FINISHED` | – | aus epoll raus, Löschen nach der Loop | – |

Die Verarbeitung (`handle_request`) ist **kein eigener Zustand mit
epoll-Event**: sie läuft direkt in `receive()`, sobald der Request komplett
ist. (Ein eigener State `PROCESS` / `IN_TO_OUT` ginge auch, muss dann aber
direkt nach `receive()` aufgerufen werden – auf ein epoll-Event zu warten
hängt, weil der Client nichts mehr schickt.)

**Aktueller Stand im Code:** `READ_HEAD`, `READ_BODY`, `WAIT_CGI`, `WRITE`,
`FINISHED` existieren. `set_state(WRITE)` ruft selbst `response_.serialize()`.
`READ_BODY` wird nie gesetzt (Parser kennt die Phase intern über `head_done_`),
`WAIT_CGI` wird nie betreten.

### Regeln

1. **`state_` nur über `set_state()`.** (✅ erfüllt) Damit können Zustand und
   epoll-Maske nie auseinanderlaufen.
2. **Jeder Fehler über `fail(status)`.** Eine Stelle, die `response_` komplett
   neu baut (keine Header-Reste) und `set_state(WRITE)` aufruft. Dort greift
   später auch `error_page`.
3. **Location direkt nach dem Header bestimmen** (`const Location *location_`).
   Dann sind 405, `return` und 413 (über `Content-Length`, bei chunked laufend)
   sofort prüfbar.
4. **Kein Socket/Pipe-I/O außerhalb des passenden epoll-Events.** Nach
   `set_state(WRITE)` nicht sofort `send()` – erst beim nächsten `EPOLLOUT`.
   Reguläre Dateien lesen/schreiben ist erlaubt.
5. **In `on_epoll_event` nach State dispatchen *und* das Event-Bit prüfen**
   (`EPOLLIN` nur in `READ_*`, `EPOLLOUT` nur in `WRITE`). `EPOLLHUP`/`EPOLLERR`
   kommen unabhängig von der Maske.

### Handler-Schnittstelle (Request → Response | CGI)

`handle_request` bekommt `Request`, `Location` und die `Response &` – **nicht**
die `Connection` (sonst kann der Handler wieder State ändern / I/O machen). Er
meldet zurück, was als Nächstes passiert:

```cpp
enum class HandlerResult { RESPONSE_READY, START_CGI };

struct CgiJob
{
	std::filesystem::path script;
	std::filesystem::path interpreter;
};

HandlerResult handle_request(const Request &request, const Location &location,
							 Response &response, CgiJob &job);
```

CGI-Erkennung: **nach** der Pfad-Auflösung inkl. Index-Datei die Extension in
`location.cgi_interpers` suchen. Bei POST **vor** dem Upload-Pfad prüfen
(POST auf `form.py` führt das Skript aus, überschreibt es nicht).

### CGI-Ownership

- `CgiHandler` wird wie alle Handler über `AllServers::add_handler()` in
  `all_fds_` verwaltet, **nicht** als Member in `Connection`.
- `Connection` hält `CgiHandler *cgi_`, `CgiHandler` hält `Connection *connection_`.
- Wer zuerst stirbt, sagt dem anderen Bescheid:
  - Connection schließt (Client weg / Timeout) → `cgi_->abort()`: Kind killen,
    Pipe zu, `cgi_->connection_ = nullptr`.
  - CGI fertig → `connection_->on_cgi_done(...)` (füllt `response_`, `WRITE`).
  - CGI-Fehler / Exit≠0 → `connection_->fail(502)`, Timeout → `fail(504)`.
- Rohzeiger sind innerhalb einer Loop-Iteration sicher, weil erst danach in
  `cleanup_all_fds()` gelöscht wird. Beim Abmelden den Zeiger der Gegenseite
  auf `nullptr` setzen.
- Zombies: `waitpid(pid, &status, WNOHANG)` beim EOF der Pipe bzw. im Timeout.
- Faustregel: Handler ist synchron → gibt Ergebnis zurück. CGI ist asynchron →
  ruft zurück, aber nur über `on_cgi_done()` / `fail()`.

---

## 4. KRITISCH – Muss vor Abgabe

Sortiert nach Wichtigkeit.

1. **CGI komplett** – *größter Block*
   - [x] Direktive `cgi <ext> <interpreter>;` + Vererbung.
   - [ ] Validierung anpassen: **jede** Extension der Form `.xyz` erlauben
         (42-Tester braucht `.bla`), statt Whitelist `.py`/`.php`. Interpreter:
         absoluter Pfad + `access(path, X_OK)` beim Parsen. Doppelte Extension
         im selben Block → Fehler.
   - [ ] CGI-Erkennung im Handler + `HandlerResult`/`CgiJob` (s. 3.).
   - [ ] `CgiHandler` (Gerüst in [Cgi.cpp](../src/server/Cgi.cpp): socketpair +
         fork, argv/env hartkodiert, `exit(99)` bei execve-Fehler):
     - [ ] argv = Interpreter + Skriptpfad, `chdir()` ins Skript-Verzeichnis.
     - [ ] Env: `REQUEST_METHOD`, `QUERY_STRING` (`split_uri_target`),
           `CONTENT_LENGTH`, `CONTENT_TYPE`, `SCRIPT_FILENAME`, `SCRIPT_NAME`,
           `PATH_INFO`, `SERVER_PROTOCOL`, `GATEWAY_INTERFACE=CGI/1.1`,
           `SERVER_NAME/PORT`, `REMOTE_ADDR`, `HTTP_*`, `REDIRECT_STATUS=200`
           (php-cgi).
     - [ ] Body (schon entchunkt) über die Pipe schreiben – **nur nach
           `EPOLLOUT`**, nicht direkt nach `fork` – dann `shutdown(SHUT_WR)`
           als EOF für das Skript.
     - [ ] Ausgabe lesen bis EOF, Header-Block parsen (`Status:`,
           `Content-Type:`, `Location:`), Rest = Body.
     - [ ] Ownership/Abmelden, `waitpid(WNOHANG)`, 502/504, Kind killen bei
           Client-Abbruch (s. 3.).
     - [ ] `is_timeout()` implementieren (aktuell immer `false`).
   - Hinweis PHP: `/usr/bin/php-cgi` verwenden, nicht `php` (CLI gibt keine
     CGI-Header aus).

2. **`EPOLLHUP` in `AllServers::wait_epoll` nicht vorab abfangen**
   - Aktuell: bei `EPOLLERR|EPOLLHUP` sofort `stop_listening()`, der Handler
     wird nie gefragt. Bei CGI kommt das Ende als `EPOLLIN|EPOLLHUP` mit noch
     ungelesenen Daten → **letzter Teil jeder CGI-Ausgabe ginge verloren**.
   - Fix: Event an den Handler durchreichen, der entscheidet (Connection
     macht das in `on_epoll_event` schon selbst).

3. **Server darf nie abstürzen**
   - [ ] `on_epoll_event` pro Event in `try/catch` (`std::exception` → Handler
         stoppen, später `fail(500)`). Ohne explizites `throw` werfen trotzdem:
         `std::bad_alloc` (jedes `insert`/String-Concat/`make_unique`),
         später `std::stoul`, `.at()`, filesystem ohne `error_code`.
   - [ ] SIGPIPE-Handler ruft `LOG(...)` → nicht async-signal-safe
         (alloziert). Besser `signal(SIGPIPE, SIG_IGN)` – `MSG_NOSIGNAL` gibt
         es schon, für CGI-Pipes zählt das Ignorieren.
   - [ ] Optional: SIGTERM wie SIGINT behandeln.

4. **Timeouts pro Zustand**
   - Aktuell: 10 s ab letztem State-Wechsel, egal welcher State, Verbindung
     wird still geschlossen.
   - [ ] `last_activity_` auch in `receive()`/`send()` bei Bytes > 0
         aktualisieren – sonst wird ein langsamer, aber laufender Upload nach
         10 s gekillt.
   - [ ] `READ_*` → 408 senden, `WAIT_CGI` → Kind killen + 504.

5. **Location nach dem Header + `client_max_body_size` früh prüfen**
   - Parser in `parse_head()` / `parse_body()` trennen, nach dem Header
     `location_` bestimmen, dann 405 / `return` / 413 (über `Content-Length`,
     bei chunked laufend). Erst danach `READ_BODY`.
   - Aktuell landet ein 2-GB-Upload komplett im RAM, bevor 413 kommt.

6. **`return` (Redirect) zur Laufzeit**
   - Parser füllt `return_code`/`return_path`, `Location` setzt Platzhalter
     (`888`, `"./example"`), Handler ignoriert es → 403.
   - Fix: `std::optional` in `Location`, direkt nach Location-Lookup Response
     mit Code + `Location:`-Header. `fill_return`: `std::stoi` in try → sauberer
     Config-Fehler mit Zeile/Spalte, Code-Bereich prüfen.

7. **`autoindex`**
   - Geparst, aber `handle_get` gibt bei Verzeichnis ohne Index immer 403.
   - Fix: HTML-Liste per `opendir/readdir` bzw. `directory_iterator` – als
     reine Funktion `path → std::string`, gut parallel machbar.

8. **`error_page` – Parser-Bug + Runtime**
   - [fill_default.cpp](../src/server_config/fill_default.cpp)
     `fill_error_page`: Schleife ruft `get_next_word()` bis kein Wort kommt →
     wirft beim `;` *"expected a word"*. **Jede Config mit `error_page`
     startet nicht.**
   - Datenmodell: statt einer Code-Liste + einem Pfad →
     `std::map<int, std::filesystem::path>` (mehrere Zeilen möglich).
   - Runtime: in `fail(status)` konfigurierte Seite laden, sonst eingebaute
     Default-Seite (gern kleines HTML statt Plain-Text).

9. **Server-Block ohne `location` → immer 404**
   - [Server.cpp](../src/server/Server.cpp) baut Locations nur aus
     `locations_confs`. Fix: wenn keine `location /` existiert, implizit eine
     aus den Server-Werten anlegen (wie nginx).

10. **Upload (multipart) + Speicherort**
    - Browser-Formulare schicken `multipart/form-data` → würde aktuell
      inkl. Boundaries gespeichert; POST auf Verzeichnis → 403.
    - Fix: `upload_store <dir>;` (oder POST-auf-Verzeichnis = Upload dorthin)
      + simpler multipart-Parser (Boundary aus `Content-Type`, `filename=` aus
      `Content-Disposition`). Für die Browser-Demo praktisch Pflicht.

11. **Tests + Demo-Configs + Demo-Website**
    - [x] Eigenes Testskript `scripts/smoke_test.sh` – **committen** und pro
          neuem Feature eine `check`-Zeile ergänzen.
    - [ ] Pro Feature eine Demo-Config in `config/`. Die vorhandenen Configs
          verweisen auf `./html/...` bzw. `/var/www/html` – das gibt es nicht,
          nur `www/`.
    - [ ] `www/` erweitern: Upload-Formular, Fehlerseiten, Ordner für
          autoindex, `cgi-bin/` mit `.py`/`.php` (GET+POST, Query-String,
          Endlosschleife für Timeout-Test).
    - [ ] 42-Tester (`tests/tester`, `tests/cgi_tester`, braucht `.bla`),
          Stresstest `siege -b` (≥ 99.5 %).

12. **README.md** (Description / Instructions / Resources + AI-Usage).

---

## 5. WICHTIG, aber kleiner (je < 1h)

- [ ] **Status-Codes**: `HEAD`/`PUT` → 405 (mit `Allow`) bzw. 501 statt 400;
      falsche HTTP-Version → 505 statt 400.
- [ ] **Verzeichnis ohne `/`** (`/dir`) → 301 auf `/dir/`.
- [ ] **`index` mit mehreren Werten** (`fill_index` liest nur ein Wort).
- [ ] **`Location`-Platzhalter** (`999`, `888`, `"./example"`, `// todo`) auf
      `std::optional` umstellen.
- [ ] **MIME-Typen** ergänzen (`.gif`, `.ico`, `.pdf`, `.mp4`, `.webp`, …) –
      aktuell 8.
- [ ] **Datei lesen in `handle_get`**: `contents << file.rdbuf()` + `file.bad()`
      erkennt Lesefehler nicht zuverlässig und kopiert 4×. Besser: `file_size`
      → `std::string body(size)` → `file.read` → `gcount()` prüfen,
      `setBody(std::move(body))` (setBody per Value).
- [ ] **Zwei `server` mit gleichem `listen`** → `bind` schlägt fehl. Klare
      Fehlermeldung oder Socket teilen.
- [ ] **Aufräumen in `Connection`**: `enum State` → `enum class`,
      auskommentierte errno-Zeilen und Buffer-Member löschen, veralteter
      `IN_TO_OUT`-Kommentar am Ende von `on_epoll_event`, deklarierte aber nie
      definierte `make_response()` / `queue_response()` / `finish()` entfernen.
- [ ] **`Response.hpp`** inkludiert `Location.hpp` nur für eine Referenz →
      Forward-Declaration `class Location;`.
- [ ] **`LOG(LOG_DEBUG, "tick")`** in `main` bei jedem Loop entfernen.
- [ ] **`MAX_EVENTS 10`** ist klein (funktioniert, aber mehr Loop-Runden unter
      Last) → z.B. 64.

---

## 6. NICE TO HAVE

- Keep-Alive / Pipelining (aktuell immer `Connection: close` – ok).
- Große Dateien in `WRITE` stückweise streamen statt komplett in RAM.
- Request-Parser inkrementell (aktuell bei jedem `recv` von vorne → O(n²) bei
  großen Uploads).
- `server_name` / Virtual Hosts (Gerüst existiert).
- `Date`/`Server`/`Last-Modified`-Header, IPv6 im `listen`.
- Bonus (Cookies/Session, mehrere CGI) – erst wenn Mandatory 100 % sitzt.

---

## 7. Was gut ist (so lassen)

- **Config-Pipeline**: Lexer → Parser mit `directive_lookup`-Tabelle →
  Vererbung über `std::optional`; gute Fehlermeldungen mit Zeile/Spalte. Neue
  Direktive = 1 Tabelleneintrag + `fill_*`.
- **epoll-Architektur**: `EpollHandler` mit `data.ptr`, deferred cleanup über
  `remove_me_`. Genau das richtige Muster für CGI-Pipes als weitere Handler.
- **RAII für fds** (`CloseFd`, `AddrInfoPtr`), rule-of-five explizit.
- **Request-Parser**: Header-Limit, Content-Length *und* chunked, CL+TE → 400,
  Host-Pflicht bei HTTP/1.1.
- **Pfad-Sicherheit** ([Resolver.cpp](../src/routing/Resolver.cpp)):
  Percent-Decoding, `..`-Ablehnung, `weakly_canonical` + Root-Check.
- **Location-Matching**: Longest-Prefix mit Segmentgrenze.
- **`set_state()`** als einzige Stelle für State + epoll-Maske.

---

## 8. Empfohlene Reihenfolge

1. **Sofort (~1h)**: `smoke_test.sh` committen, `EPOLLHUP` an Handler
   durchreichen, `try/catch` pro Event, `SIG_IGN` für SIGPIPE.
2. **State-Machine fertig (~0.5–1 Tag)**: `fail(status)`, Location nach dem
   Header, frühes 413, `return`, `HandlerResult`/`CgiJob`. Danach sind
   `autoindex`, Directory-Redirect und Status-Codes lokale Änderungen im
   Handler.
3. **CGI (~2–3 Tage)**, in dünnen Scheiben: Erkennung im Handler (erst nur
   „würde CGI starten“ + 501) → GET-Skript mit Ausgabe → Ownership/Zeiger →
   POST-Body → `waitpid`, 502/504, Kill.
4. **Timeouts pro State** (408 / 504) – mit der State-Machine wenige Zeilen.
5. **Upload**: multipart + Upload-Ordner.
6. **Demo-Configs + `www/` + Testskript erweitern**, dann 42-Tester,
   cgi_tester, siege.
7. **README** + Aufräumen.

### Parallel machbar (unabhängig vom Connection-Umbau)

Eine Person macht 2.–4. (alles in `Connection`/`Handler`). Parallel ohne
Konflikte:
- `error_page`-Parser-Fix + `std::map`-Datenmodell
- `index` mit mehreren Werten, `return` als `std::optional` + Validierung
- `cgi`-Validierung (`.bla`, `access(X_OK)`)
- autoindex-HTML-Generator (`path → std::string`)
- multipart-Parser (`body + boundary → Dateien`)
- MIME-Typen, Demo-Configs, `www/`

---

## 9. Stolperfallen

- Jeder `recv`/`send`/`read`/`write` auf Socket **oder CGI-Pipe** nur nach
  epoll-Readiness aus *demselben* `epoll_wait`. Nicht direkt nach `fork` in
  die Pipe schreiben, nicht direkt nach `set_state(WRITE)` senden.
- errno nach read/write prüfen ist verboten (auch nicht für Logging-Logik
  benutzen, die Verhalten ändert).
- `fork` nur für CGI.
- `docs/todos.md` ist veraltet → durch diese Datei ersetzen oder abhaken.
- `CLAUDE.md` beschreibt den Runtime-Stand als „hardcoded canned response“ –
  überholt.
- Makefile: `MAKEFLAGS += -j` nur bei `MAKELEVEL == 0`; kein
  `--output-sync` (puffert sonst die Server-Ausgabe bei `make run/debug`);
  kein `+` vor `compiledb -n make` (Endlosrekursion).
