# Poshet

Poshet este un client cu interfață grafică pentru serviciile de mail electronic POP3 (pentru obținerea mailurilor) și SMTP (pentru trimiterea de mailuri). Printre funcționalitățile acestuia se numără: vizualizarea și organizarea mailurilor primite, trimiterea de mailuri noi însoțite de atașamente, reply la mailuri primite, sau forwarding de mailuri.

## Compilare

Librării externe necesare (alături de instrucțiuni de instalare pentru apt):
- wxWidgets (`sudo apt install libwxgtk3.0-gtk3-0v5 libwxgtk3.0-gtk3-dev`)
- SQLite3 (`sudo apt install sqlite3 libsqlite3-dev`)

Pentru a construi și compila proiectul, presupunând că vă aflați în același folder cu acest fișier:

`cd poshet`

`cmake -S . -B build`

`cd build`

`make`

Pentru a rula executabilul, aflat în folderul `build`:

`./poshet`