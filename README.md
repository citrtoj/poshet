# Poshet

Poshet este un client cu interfață grafică pentru serviciile de mail electronic POP3 (pentru obținerea mailurilor) și SMTP (pentru trimiterea de mailuri). Printre funcționalitățile acestuia se numără: vizualizarea și organizarea mailurilor primite, trimiterea de mailuri noi însoțite de atașamente, reply la mailuri primite, sau forwarding de mailuri.

## Compilare

În acest folder se află un fișier `Makefile`. După rularea acestuia prin `make` ar trebui să apară executabilul `Poshet` care poate fi rulat prin comanda `./Poshet`.

Librării/pachete externe necesare:
- CMake
- wxWidgets
- SQLite3
- Mimetic (dacă proiectul este descărcat prin GitHub, este necesară comanda `git submodule update --init --recursive` pentru a descărca codul ei sursă în `poshet/thirdparty`).

Pentru a construi și compila proiectul fără fișierul `Makefile`, presupunând că vă aflați în același folder cu acest fișier:

`sudo apt install cmake`

`sudo apt install libwxgtk3.0-gtk3-0v5 libwxgtk3.0-gtk3-dev` (dacă au fost actualizate librăriile între timp, se recomandă căutarea `wxgtk` pe `apt` și instalarea noilor versiuni; pentru acest program sunt necesare atât Runtime Toolkit cât și Development Toolkit)

`sudo apt install sqlite3 libsqlite3-dev`

`cd poshet/thirdparty/mimetic && ./configure && make && sudo make install && make clean && cd ../../..` (pentru a construi și instala local librăria Mimetic care se află în folderul `poshet/thirdparty`)

`cd poshet`

`cmake -S . -B build`

`cd build`

`make`

`./poshet` (nu `./Poshet`!)