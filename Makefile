libcablintpq.so:
	gcc -fpic -c cablintpq.c -I`pg_config --includedir-server` `pkg-config --cflags ruby-2.4` -std=gnu99
	gcc -shared -fpic -o libcablintpq.so -Wl,-Bsymbolic -Wl,-Bsymbolic-functions cablintpq.o `pkg-config --libs ruby-2.4`

install:
	su -c "cp libcablintpq.so `pg_config --pkglibdir`"

clean:
	rm -f *.o *.so *~
