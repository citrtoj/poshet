# Poshet

Poshet este un client cu interfață grafică pentru serviciile de mail electronic POP3 (pentru obținerea mailurilor) și SMTP (pentru trimiterea de mailuri). Printre funcționalitățile acestuia se numără: vizualizarea și organizarea mailurilor primite, trimiterea de mailuri noi însoțite de atașamente, reply la mailuri primite, sau forwarding de mailuri.

## Compilare

În acest folder se află un fișier `Makefile`. După rularea acestuia prin `make` ar trebui să apară executabilul `Poshet` care poate fi rulat prin comanda `./Poshet`.

Librării/pachete externe necesare:
- CMake
- wxWidgets
- SQLite3
- GMime
- GLib (pentru GMime)

Dacă rularea `make` eșuează, puteți încerca rularea manuală a comenzilor din `Makefile`.