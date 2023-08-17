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
#MACROS=
FLAGS=-Wall -Wextra $(foreach F,$(INCDIRS),-I$(F)) $(OPT) $(DEPFLAGS) $(foreach M,$(MACROS),-D$(M))
LDFLAGS=lib/libglfw3.a $(FLAGS) -Llib -lX11

SRC=$(shell find . -name "*.$(EXT)" -path "./src/*")
OBJ=$(subst ./src/,./build/,$(SRC:.$(EXT)=.o))
TEST=$(shell find . -name "*.$(EXT)" -path "./test/*")
TESTO=$(subst ./test/,./build/,$(TEST:.$(EXT)=.t))

$(shell mkdir -p build)


all : $(BIN)

$(BIN) : $(OBJ) build/glad.o
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(OBJ:.o=.d) $(LIBO:.o=.d)

build/glad.o : lib/glad.c
	$(CC) $(FLAGS) -o $@ -c $<

build/%.o : src/%.$(EXT)
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

c : clean

p : push

d : debug

.PHONY : all test t alltest run r clean c debug d dist push p install info
