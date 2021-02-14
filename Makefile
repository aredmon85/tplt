CC=gcc
LDLIBS = -lcurl

build: tplt
	$(CC) -o tplt tplt.c $(LDLIBS)
