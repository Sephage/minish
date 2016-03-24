all : minish

minish : parse.c input.c lib.h
	gcc -o minish parse.c input.c -pthread -g -Wall

print : paulMinish
	echo "Pour mettre ce bash en standard : sudo cp ./<Exe> /usr/bin/"
	echo "puis chsh /usr/bin/<Exe>"
	echo "pour revenir a l'ancien : chsh /bin/bash"

clean : minish
	rm minish 
