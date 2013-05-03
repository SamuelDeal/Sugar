all: sugar sugari sugarc

OBJS = parser.o  \
       codegen.o \
       corefn.o  \
       functionimpl.o \
       functionlist.o \

CPPFLAGS = `llvm-config --cppflags`
LDFLAGS = `llvm-config --ldflags`
LIBS = -lreadline -lcurses
CC='g++'

clean:
	$(RM) -rf sugar.o sugar sugari.o sugari parser.cpp parser.hpp lexer.cpp  $(OBJS)

parser.cpp: parser.y
	bison -W --debug -v -d -o $@ $^
	
parser.hpp: parser.cpp

lexer.cpp: lexer.l parser.hpp
	flex -o $@ $^ 

sugari.o: sugari.cpp parser.hpp lexer.cpp
	$(CC) -ggdb -c $(CPPFLAGS) -o $@ sugari.cpp
	
sugar.o: sugar.cpp parser.hpp lexer.cpp
	$(CC) -ggdb -c $(CPPFLAGS) -o $@ sugar.cpp
	
%.o: %.cpp
	$(CC) -ggdb -c $(CPPFLAGS) -o $@ $<
	
%.o: config.h condegen.h

sugar: $(OBJS) sugar.o
	$(CC) -ggdb -o $@ $(OBJS) sugar.o $(LIBS) $(LDFLAGS)
	
sugari: $(OBJS) sugari.o
	$(CC) -ggdb -o $@ $(OBJS) sugari.o $(LIBS) $(LDFLAGS)
	
sugarc: $(OBJS) sugarc.o
	$(CC) -ggdb -o $@ $(OBJS) sugarc.o $(LIBS) $(LDFLAGS)

test: test.o
	$(CC) -ggdb -o $@ test.o $(LIBS) $(LDFLAGS)
