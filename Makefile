all: sugar

OBJS = parser.o  \
       codegen.o \
       sugar.o    \
       lexer.o  \
       corefn.o  \
       functionimpl.o \
       functionlist.o \

CPPFLAGS = `llvm-config --cppflags`
LDFLAGS = `llvm-config --ldflags`
LIBS = `llvm-config --libs`
CC='g++'

clean:
	$(RM) -rf parser.cpp parser.hpp sugar lexer.cpp $(OBJS)

parser.cpp: parser.y
	bison -W --debug -v -d -o $@ $^
	
parser.hpp: parser.cpp

lexer.cpp: lexer.l parser.hpp
	flex -o $@ $^

%.o: %.cpp
	$(CC) -ggdb -c $(CPPFLAGS) -o $@ $<

sugar: $(OBJS)
	$(CC) -ggdb -o $@ $(OBJS) $(LIBS) $(LDFLAGS)


