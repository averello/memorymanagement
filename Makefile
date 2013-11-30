CC = gcc
CFLAGS_PRIV = -Wall -Wextra -g3 -pedantic -std=c99 -I${INC} -D_XOPEN_SOURCE=700 $(CFLAGS)
LDFLAGS_PRIV = $(LDFLAGS)
BIN = bin
INC = include
OBJ = obj
SRC = src
MKDIR = mkdir

LIB = lib
MKDIR = mkdir

LIB_NAME = memorymanagement

.PHONY: all directories compileall runall clean cleanall

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	STRIP = strip --strip-unneeded
endif

ifeq ($(UNAME), Darwin)
	STRIP = strip -X -x -S
endif


# +-----------+
# | Cible all |
# +-----------+

all: directories compileall

# +-------------+
# | Cible test1 |
# +-------------+

TEST1 = main
test1: directories libstatic ${BIN}/${TEST1}
	@echo "**** Testing ${TEST1}"
	@${BIN}/${TEST1}
	@echo "---- end of ${TEST1}"


libstatic: directories $(LIB)/lib$(LIB_NAME).a

valgrind% : $(BIN)/%
	@valgrind  --track-origins=yes --leak-check=full --show-reachable=yes $<


# +-------------------+
# | Cible directories |
# +-------------------+

directories: ${OBJ} ${BIN} ${LIB}

${OBJ}:
	${MKDIR} -p ${OBJ}
${BIN}:
	${MKDIR} -p ${BIN}
${LIB}:
	${MKDIR} -p ${LIB}

# +------------------------------+
# | Cibles pour compiler avec -c |
# +------------------------------+

${OBJ}/%.o : ${SRC}/%.c
	$(CC) -c -o $@ $< ${CFLAGS_PRIV}

${BIN}/% : ${OBJ}/%.o
	${CC} -o $@ $< ${LDFLAGS_PRIV}

${LIB}/lib${LIB_NAME}.a : $(OBJ)/memory_management.o 
	${AR} r ${LIB}/lib${LIB_NAME}.a $?


# +------------------+
# | Cible compileall |
# +------------------+

compileall: libstatic

# +--------------+
# | Cible runall |
# +--------------+

runall: compileall
	$(MAKE) tests

# +-------------+
# | Cible clean |
# +-------------+

clean:
	rm -f ${OBJ}/* ${BIN}/*

# +----------------+
# | Cible cleanall |
# +----------------+

cleanall: clean
	rm -f ${OBJ}/* ${BIN}/*;
	rmdir ${OBJ} ${BIN}
	rm -f ${INC}/*~ ${SRC}/*~ *~

