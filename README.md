<h1 style="text-align:center">
    Poshet
</h1>
<p style="text-align:center">
    Poshet is a GUI client, written in C++ using wxWidgets, for the POP3 (mail retrieval) and SMTP (mail sending) services. Its features include: SSL encryption, viewing and organization of received mail (including HTML mail), sending of new plaintext mail with attachments, and replying to (or forwarding) received mail (in plaintext form, keeping attachments).
</p>


### Supported servers:
- POP3 (with/without SSL enabled, with obligatory USER/PASS logging in)
- SMTP (with/without SSL enabled, with optional AUTH LOGIN support)

## Dependencies
- CMake
- wxWidgets
- SQLite3
- VMime (only for parsing of MIME messages, not for any network communication)
- OpenSSL

## Compilation

Running `make` will run both `make install` (which will attempt to install all the dependencies of the program) and `make build` (which will build the executable in `poshet/build` using CMake and move the executable in the working directory, under the name `./Poshet`).

## Running

Running `./Poshet` will create a `.poshet` folder in the same directory, containing `mail.db` (a SQLite database containing information about received mail) and the `mail` folder (containing filenames of received mail).

## Notes

More information about the project is contained in `docs/poshet.pdf`, a documentation file written in Romanian. The `docs/poshet.tex` file is available as well, which might make translation easier if necessary.