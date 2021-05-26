CFLAGS_PRIV = -Wall -Wextra -g3 -pedantic -std=c99 -I${INC} -D_XOPEN_SOURCE=700 $(CFLAGS)
LDFLAGS_PRIV = -L$(LIB) -lmemorymanagement $(LDFLAGS)
SHAREDFLAGS_PRIV=-fPIC -shared $(SHAREDFLAGS)
BIN = bin
INC = include
OBJ = obj
SRC = src
TEST = test
MKDIR = mkdir

LIB = lib
MKDIR = mkdir

LIB_NAME = memorymanagement

.PHONY: all directories compileall runall clean cleanall

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	STRIP = strip --strip-unneeded
	LIBSHAREDFLAGS=-shared -Wl,-rpath,lib$(LIB_NAME).so.$(MAJORVERSION)
	LD=ld
endif

ifeq ($(UNAME), Darwin)
	STRIP = strip -X -x -S
	LIBSHAREDFLAGS=-dynamic -install_name lib${LIB_NAME}.so.$(MAJORVERSION).$(MINORVERSION).$(RELEASENUMBER) -current_version $(MAJORVERSION)
	LD=libtool
endif


MAJORVERSION=1
MINORVERSION=1
RELEASENUMBER=4
SONAME = ${LIB}/lib${LIB_NAME}.so.$(MAJORVERSION)
REALNAME = ${LIB}/lib${LIB_NAME}.so.$(MAJORVERSION).$(MINORVERSION).$(RELEASENUMBER)
SHARED = ${LIB}/lib${LIB_NAME}.so
FIRSTLINK = $(patsubst $(LIB)/%,%,$(REALNAME))
SECONDLINK = $(patsubst $(LIB)/%,%,$(SONAME))
libshared : directories $(SHARED)
	@ if ! [ -f $(SONAME) ]; then ln -s $(FIRSTLINK) $(SONAME); fi
	@ if ! [ -f $(SHARED) ]; then ln -s $(SECONDLINK) $(SHARED); fi

# +-----------+
# | Cible all |
# +-----------+

all: directories compileall

# +-------------+
# | Cible test  |
# +-------------+

TESTS = $(patsubst $(TEST)/%.c,$(BIN)/%,$(wildcard $(TEST)/test*.c))
tests : directories libstatic $(TESTS)
	@for test in ${TESTS}; do \
		echo "**** Testing $$test"; \
	echo "---- end of ${TESTSTRING}"; \
	done
	@#	./"$$test" \

test% : $(OBJ)/Point.o $(BIN)/test%
	@echo "**** Testing $@";
	@$(BIN)/$@
	@echo "end of $@";


valgrind% : $(BIN)/test%
	@valgrind  --track-origins=yes --leak-check=full --show-reachable=yes $<


MEMORYMANAGEMENTSTATIC = ${LIB}/lib$(LIB_NAME).a
libstatic : directories $(MEMORYMANAGEMENTSTATIC)

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
	$(CC) -c -o $@ $< ${CFLAGS_PRIV} $(SHAREDFLAGS_PRIV)

${OBJ}/%.o : ${TEST}/%.c
	$(CC) -c -o $@ $< ${CFLAGS_PRIV}

${BIN}/% : ${OBJ}/%.o $(OBJ)/Point.o
	${CC} -o $@ $^ ${LDFLAGS_PRIV}

${BIN}/% : ${OBJ}/%.o
	${CC} -o $@ $< ${LDFLAGS_PRIV}


${LIB}/lib${LIB_NAME}.a : $(OBJ)/memory_management.o
	${AR} r ${LIB}/lib${LIB_NAME}.a $<

${LIB}/lib${LIB_NAME}.so : $(SONAME)

${LIB}/lib${LIB_NAME}.so.$(MAJORVERSION) : $(REALNAME)

${LIB}/lib${LIB_NAME}.so.$(MAJORVERSION).$(MINORVERSION).$(RELEASENUMBER) : $(patsubst ${SRC}/%.c,${OBJ}/%.o,$(wildcard ${SRC}/*.c))
	$(CC) $(CFLAGS_PRIV) $(SHAREDFLAGS_PRIV) $(LIBSHAREDFLAGS) -o $@  $? -lc -lpthread
	#$(LD) $(LIBSHAREDFLAGS) -o $@ $? -lc -lpthread
	$(STRIP) $@

# +------------------+
# | Cible compileall |
# +------------------+

compileall: libstatic libshared

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
	rm -f $(LIB)/*
	rmdir ${OBJ} ${BIN}
	rm -f ${INC}/*~ ${SRC}/*~ *~

