PROJECTNAME=opengool
BIN=build/$(PROJECTNAME)
CC=g++

EXT=cpp
INCFOLDERS=include lib
INCDIRS=$(INCFOLDERS)

# make mode=release
ifeq ($(mode), release)
	OPT=-O3
else
	OPT=-Og -g
endif
DEPFLAGS=-MP -MD
MACROS=
FLAGS=-Wall -Wextra $(foreach F,$(INCDIRS),-I$(F)) $(OPT) $(DEPFLAGS) $(foreach M,$(MACROS),-D$(M)) -Llib -lX11

SRC=$(shell find . -name "*.$(EXT)" -path "./src/*")
OBJ=$(subst ./src/,./build/,$(SRC:.$(EXT)=.o))
LIB=$(shell find . -name "*.c" -path "./lib/*")
LIBO=$(subst ./lib/,./build/,$(LIB:.c=.o))
TEST=$(shell find . -name "*.$(EXT)" -path "./test/*")
TESTO=$(subst ./test/,./build/,$(TEST:.$(EXT)=.t))

$(shell mkdir -p build)


all : $(BIN)

$(BIN) : $(OBJ) $(LIBO)
	$(CC) -o $@ $^ lib/libglfw3.a $(FLAGS)

-include $(OBJ:.o=.d) $(LIBO:.o=.d)

build/%.o : src/%.$(EXT)
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -o $@ -c $<
build/%.o : lib/%.c
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -o $@ -c $<

# make test file=testGenID.cpp
test : build/$(file).t
	./build/$(file).t

alltest : $(TESTO)
	@for i in $(TESTO); do echo $$i; $$i; done

build/%.t : test/%.$(EXT)
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -o $@ $<

# make run input="program input"
run : $(BIN)
	./$< $(input)

clean :
	rm -rf build/*

check :
	cppcheck --enable=all --suppress=missingIncludeSystem $(foreach I,$(INCDIRS),-I$(I)) .
	flawfinder .

debug : $(BIN)
	gdb $< $(input)

# unzip : tar -xvf exemple.tgz
dist : clean
	$(info /!\ project folder has to be named $(PROJECTNAME) /!\ )
	cd .. && tar zcvf $(PROJECTNAME)/build/$(PROJECTNAME).tgz $(PROJECTNAME) >/dev/null

push :
	git push bbsrv
	git push gh

install : dist
	mv build/$(PROJECTNAME).tgz $$HOME/dev/opt/archive

info :
	$(info put what ever)
	@echo you want

# alias

r : run

t : test

c : check

p : push

d : debug

.PHONY : all test t alltest run r clean check c debug d dist push p install info
