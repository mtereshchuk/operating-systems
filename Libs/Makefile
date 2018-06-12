all: main libdif.so libmul.so

sum.o: sum.c
	gcc -c $< -o $@

sum.a: sum.o
	ar rcs $@ $<

dif.o: dif.c
	gcc -c -Wall -Werror -fPIC $< -o $@

libdif.so: dif.o
	gcc -shared -o $@ $^

mul.o: mul.c
	gcc -c -Wall -Werror -fPIC $< -o $@

libmul.so: mul.o
	gcc -shared -o $@ $^

main.o: main.c
	gcc -c $< -o $@

main: main.o sum.a libdif.so
	gcc -L. -Wl,-rpath=. $< sum.a -o $@ -ldif -ldl

clean:
	rm -f *.o *.a *.so main
