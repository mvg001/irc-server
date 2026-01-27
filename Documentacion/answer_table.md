Todo mensaje IRC debe terminar con "\r\n".

| Mensaje del cliente  | Respuesta típica del servidor  |
|---|---|
| `PASS <password>` | Autenticación previa si tu servidor usa contraseña. Si es válida: nada inmediato; si es inválida: `:server 464 <nick> :Password incorrect` (opcional). Ejemplo: `PASS hunter2\r\n` |
| `NICK <nick>` | Aceptar o rechazar el nick. Si OK, puede no enviarse respuesta inmediata hasta USER; si en uso: `:server 433 * <nick> :Nickname is already in use\r\n`. Si cambia nick ya registrado: difundir `:<oldnick> NICK <newnick>\r\n`. |
| `USER <user> <mode> <unused> :<realname>` | Parte del registro. Tras recibir NICK y USER, servidor envía welcome numerics: `:server 001 <nick> :Welcome ...\r\n` `:server 002 <nick> :Your host is ...\r\n` `:server 003 <nick> :This server was created ...\r\n` `:server 004 <nick> servername version ...\r\n` |
| `CAP LS` | Capabilities listing. Si no implementas capabilities puedes responder algo simple o ignorar. Ej.: `:server CAP * LS :\r\n` luego el cliente negociará. |
| `JOIN #channel` | Añadir al canal, difundir JOIN: `:<nick>!<user>@<host> JOIN #channel\r\n` Enviar topic/numeric: `:server 331 <nick> #channel :No topic is set\r\n` Enviar NAMES: `:server 353 <nick> = #channel :nick1 nick2\r\n` `:server 366 <nick> #channel :End of /NAMES list.\r\n` |
| `PART #channel [:reason]` | Quitar del canal y difundir: `:<nick>!user@host PART #channel :reason\r\n` Si el cliente no está en el canal: `:server 442 <nick> #channel :You're not on that channel\r\n` |
| `PRIVMSG <target> :<text>` (target = nick o #channel) | Si target es usuario: enviar `:<nick>!user@host PRIVMSG <target> :<text>\r\n` al receptor. Si canal y el usuario no puede enviar: `:server 404 <nick> <channel> :Cannot send to channel\r\n` |
| `NOTICE <target> :<text>` | Similar a PRIVMSG pero sin respuestas automáticas de error (servers no responden con numeric por NOTICE). Se retransmite a target. |
| `PING <token>` | Cliente o servidor mantiene conexión. Debe responder: `PONG <token>\r\n` si la envía cliente/servidor opuesto. Ej.: cliente `PING :12345` → servidor `PONG :12345\r\n` |
| `PONG <token>` | Respuesta a PING; servidor también puede enviar PING para comprobar; PONG suele no requerir más respuesta. |
| `QUIT [:reason]` | Difundir QUIT: `:<nick>!user@host QUIT :reason\r\n` luego cerrar conexión. No numeric requerido. |
| `WHO <mask>` | Responder con lista `352` para cada usuario: `:server 352 <requester> <channel> <user> <host> <server> <nick> H :0 <realname>\r\n` y final `:server 315 <requester> <mask> :End of /WHO list.\r\n` |
| `WHOIS <nick>` | Respuestas varias: `311` (user info), `312` (server info), `317` (idle), `319` (channels), `318` (end of WHOIS). Ej.: `:server 311 req nick user host * :Real Name\r\n` … `:server 318 req nick :End of /WHOIS list.\r\n` |
| `NAMES [#channel]` | Si `NAMES #chan`: `:server 353 <nick> = #chan :nick1 nick2\r\n` `:server 366 <nick> #chan :End of /NAMES list.\r\n` Si sin canal: lista todos. |
| `LIST [#channel]` | Para cada canal listado: `:server 322 <nick> <channel> <#users> :<topic>\r\n` Al final: `:server 323 <nick> :End of /LIST\r\n` |
| `TOPIC #channel [:newtopic]` | Si consulta: `:server 332 <nick> #channel :<topic>\r\n` Si cambia topic (y tiene permisos): difundir `:<nick>!user@host TOPIC #channel :newtopic\r\n` |
| `MODE <target> ...` | Responder con `:server 324 <nick> <channel> <modes>` para mostrar modos de canal, o `:server 221 <nick> <usermodes>` para modos de usuario. También difundir cambios `:<nick> MODE #chan +o user\r\n` |
| `INVITE <nick> #channel` | Enviar invitación: notificar al invitado `:inviter!user@host INVITE <nick> :#channel\r\n` y responder al que invitó `:server 341 <inviter> <nick> #channel\r\n` (opcional). |
| `KICK #channel <user> [:reason]` | Quitar y difundir: `:<nick>!user@host KICK #channel <user> :reason\r\n` Si no tiene permiso: `:server 482 <nick> #channel :You're not channel operator\r\n` |
| Mensaje mal formado / falta de parámetros | `:server 461 <nick> <command> :Not enough parameters\r\n` |
| Comando a nick/canal inexistente | `:server 401 <nick> <target> :No such nick/channel\r\n` |
| NICK en uso | `:server 433 * <nick> :Nickname is already in use\r\n` |
| Operación sobre canal inexistente | `:server 403 <nick> <channel> :No such channel\r\n` |
| Comando que requiere registro antes de usar | `:server 451 <nick> :You have not registered\r\n` |
| Cliente envía datos binarios/TLS por error | Si no soportas TLS y ves ClientHello binario, puedes cerrar la conexión o responder con `ERROR :TLS required` (no estándar). Mejor: detectar y cerrar; cliente mostrará error. |
| Respuestas informativas del servidor | Mensajes numéricos de estado (001-004) al registrarse, y numerics informativos para errores o listados. Siempre con prefijo del servidor: `:server 001 nick :Welcome...`\r\n |
| `OPER <user> <password>` | Si implementas operadores: si OK `:server 381 <nick> :You are now an IRC operator\r\n` si fallo `:server 491 <nick> :No O-lines for your host` u `:server 464 <nick> :Password incorrect` |
| `AWAY [:message]` | Marcar como ausente: sin mensaje -> clear away; con mensaje -> `:server 306 <nick> :You have been marked as being away` (y a quien haga WHOIS del nick se le incluye 301/305). |
| `KILL <nick> :reason` (privilege server-to-server/operator) | Si autorizado, desconectar: `:server KILL <nick> :reason\r\n` (privado a IRC ops); no suele usarse por cliente normal. |
| Servidor solicita PING de control | `PING :<token>\r\n` — cliente debe responder `PONG :<token>\r\n` para evitar timeout. |

Notas y recomendaciones:
- Formato raw: cada línea que envíes desde el servidor debe terminar con CRLF: `\r\n`.
- Prefijo de mensaje de usuario: `:<nick>!<user>@<host> COMMAND ...` — úsalo al difundir acciones del usuario a otros.
- Numerics: los códigos como `001`, `353`, `366`, `433`, etc., son estándar; úsalos para compatibilidad con clientes.
- Estado de registro: considera un usuario “registrado” sólo después de recibir NICK + USER válidos; hasta entonces muchas operaciones deben devolver `451 :You have not registered`.
- CAP LS / STARTTLS: algunos clientes comienzan con CAP o intentan STARTTLS — si no lo soportas, puedes ignorarlo y continuar (el cliente intentará seguir).
- En sockets non-blocking debes gestionar que `send()` pueda no enviar todo: en ese caso encola el resto y espera EPOLLOUT.

Si quieres, genero:
- Una versión en forma de tabla descargable/pequeño CSV o Markdown con más detalles (mapeo de cada numeric RFC).
- Snippets listos para copiar/pegar para producir cada numeric (función `send_raw` + ejemplos concretos) para integrarlos en tu servidor. ¿Quieres eso?
