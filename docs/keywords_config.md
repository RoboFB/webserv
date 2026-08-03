

Syntax:	http { ... }
Default:	—
Context:	main


Syntax:	error_page code ... [=[response]] uri;
Default:	—
Context:	http, server, location, if in location
Example:	error_page 404             /404.html;
			error_page 500 502 503 504 /50x.html;



Syntax:	client_max_body_size size;
Default:	client_max_body_size 1m;
Context:	http, server, location


Syntax:	autoindex on | off;
Default:	autoindex off;
Context:	http, server, location


Syntax:	root path;
Default:	root html;
Context:	http, server, location, if in location


Syntax:	index file ...;
Default:	index index.html;
Context:	http, server, location


Syntax:	server { ... }
Default:	—
Context:	http


Syntax:	listen address[:port] [default_server] [ssl] [http2 | quic] [proxy_protocol] [setfib=number] [fastopen=number] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [ipv6only=on|off] [reuseport] [multipath] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
listen port [default_server] [ssl] [http2 | quic] [proxy_protocol] [setfib=number] [fastopen=number] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [ipv6only=on|off] [reuseport] [multipath] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
listen unix:path [default_server] [ssl] [http2 | quic] [proxy_protocol] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
Default:	listen *:80 | *:8000;
Context:	server


Syntax:	server_name name ...;
Default:	server_name "";
Context:	server


Syntax:	return code [text];
return code URL;
return URL;
Default:	—
Context:	server, location, if


Syntax:	location [ = | ~ | ~* | ^~ ] uri { ... }
location @name { ... }
Default:	—
Context:	server, location


Syntax:	limit_except method ... { ... }
Default:	—
Context:	location
