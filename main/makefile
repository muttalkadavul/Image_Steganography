LFLAG := -L ../library -lstegno
IFLAG := -I ../include
SRCS := $(wildcard *.c)
TARGET := $(SRCS:.c=)

${TARGET} : ${SRCS}
	gcc $^ ${LFLAG} ${IFLAG} -o $@

clean :
	rm ${TARGET}
