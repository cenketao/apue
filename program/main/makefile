SQLITEPATH=`pwd`/../../usr/local/lib/
SQLITEINCLUDE=`pwd`/../../usr/local/include/
LIBPATH=`pwd`/../lib/
LIBINCLUDE=`pwd`/../include/
LIBNAME=mycrypto

all:run

run:
	gcc *.c -lsqlite3 -L${SQLITEPATH} -I${SQLITEINCLUDE} -l${LIBNAME} -L${LIBPATH} -I${LIBINCLUDE}

