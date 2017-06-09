TARGET := ../library/libstegno.so
SRCS := $(wildcard *.c)
CFLG := -I ../include/

${TARGET} : ${SRCS}
	gcc -fPIC -shared ${CFLG} $^ -o $@

clean :
	rm ${TARGET}
