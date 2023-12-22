all: install build

install:
	sudo apt install cmake
	sudo apt install libwxgtk3.0-gtk3-0v5 libwxgtk3.0-gtk3-dev
	sudo apt install sqlite3 libsqlite3-dev
	cd poshet/thirdparty/mimetic && ./configure && make && sudo make install && make clean && cd ../../..

build:
	cd poshet && cmake -S . -B build && cd build && make && mv ./poshet ../../Poshet