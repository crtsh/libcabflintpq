libcablintpq.so:
	gcc -fpic -c cablintpq.c -I`pg_config --includedir-server` `PKG_CONFIG_PATH=/usr/local/ruby_2_0_0/lib/pkgconfig pkg-config --cflags ruby-2.0` -std=gnu99 -pedantic
	gcc -shared -fpic -o libcablintpq.so -Wl,-Bsymbolic -Wl,-Bsymbolic-functions cablintpq.o /usr/local/ruby_2_0_0/lib/libruby-static.a -lpthread -lrt -ldl -lcrypt -lm

install:
	su -c "cp libcablintpq.so `pg_config --pkglibdir`"

clean:
	rm -f *.o *.so *~
