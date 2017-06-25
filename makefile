all:
	mkdir -p out
	gcc -Wall -Werror -Wextra -pedantic -std=c99 -fPIC -shared src/sd_journal_shim.c -o out/libjournal.so
	gcc -Wall -Werror -Wextra -pedantic -std=c99 -c src/sd_journal_shim.c -o out/libjournal.o
	ar cr out/libjournal.a out/libjournal.o

clean:
	rm -rf out
