CC = g++

CFLAGS = -O3 -DNDEBUG -funroll-loops -fomit-frame-pointer 
#CFLAGS = -O0 -g 

LIBS = -lsqlite3 -lpthread

SRC = src/apply.cpp src/attack.cpp src/bitboards.cpp src/book.cpp src/draw.cpp src/drawboard.cpp \
	src/eval.cpp src/evalks.cpp src/evalp.cpp src/extend.cpp src/globals.cpp \
	src/hash.cpp src/init.cpp src/log.cpp src/magic.cpp src/main.cpp src/move.cpp src/movegen.cpp \
	src/mvvlva.cpp src/next.cpp src/perft.cpp src/pos.cpp src/prune.cpp src/search.cpp \
	src/see.cpp src/square.cpp src/suites.cpp src/think.cpp src/time.cpp src/utils.cpp \
	src/xboard.cpp 

TESTSRC = test/apply__test.cpp test/attack__test.cpp test/bitboards__test.cpp test/defs__test.cpp \
	test/draw__test.cpp test/eval__test.cpp test/evalp__test.cpp test/extend__test.cpp \
	test/hash__test.cpp test/init__test.cpp test/magic__test.cpp test/move__test.cpp test/movegen__test.cpp \
	test/mvvlva__test.cpp test/next__test.cpp test/pos__test.cpp test/prune__test.cpp \
	test/search__test.cpp test/see__test.cpp test/square__test.cpp test/time__test.cpp \
	test/utils__test.cpp

SOURCE = $(SRC) $(TESTSRC)
#$(info $(SOURCE))

OBJ=$(join $(addsuffix ../obj/, $(dir $(SOURCE))), $(notdir $(SOURCE:.cpp=.o))) 
#$(info $(OBJ))

all: $(OBJ)
	$(CC) $(CFLAGS) -o prophet3  $(OBJ) $(LIBS)

clean:
	rm -rf obj prophet3


## Rules for object files from cpp files
## Object file for each file is put in obj directory
## one level up from the actual source directory.
src/../obj/%.o : src/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling source file: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

test/../obj/%.o : test/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling test file: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

