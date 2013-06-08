all: sugar sugari sugarc clean_tmp

.PHONY: all clean clear clean_tmp deps test

SRCS = $(shell find ./src -type f -name '*.cpp')
SRCS := $(filter-out ./src/parser.cpp ./src/sugari.cpp ./src/sugarc.cpp ./src/sugar.cpp, $(SRCS))
SRCS += src/parser.cpp

OBJS = $(SRCS:.cpp=.o)

TESTS = $(shell find ./test -type f -name '*.test' | sort )
TESTS := $(TESTS:.test=)

CPPFLAGS += `llvm-config --cppflags` 
LDFLAGS += `llvm-config --ldflags`
LIBS += `llvm-config --libs` -ledit -lreadline -lcurses
CC = 'g++'

DEPS := $(patsubst %.o,%.d,$(OBJS))

clear: clean

clean: clean_tmp
	@$(RM) -rf sugar sugari sugarc
	@echo "all cleaned"
	
clean_tmp:
	@$(RM) -rf $(OBJS) $(DEPS) src/parser.cpp src/lexer.hpp src/parser.hpp src/parser.output src/parser.dot test/results src/sugar.o src/sugari.o src/sugarc.o
	@echo "temporary files cleaned"

src/parser.cpp: src/parser.y
	bison -W --debug -v -d -o $@ $^
	
src/parser.hpp: src/parser.cpp
	
src/lexer.hpp: src/lexer.l src/parser.hpp
	flex -o $@ $^ 

%.o: %.cpp src/config.h
	$(CC) -ggdb -c $(CPPFLAGS) -o $@ $<
	
src/sugar.o: src/lexer.hpp

src/sugari.o: src/lexer.hpp

src/sugarc.o: src/lexer.hpp

sugar: src/sugar.o $(OBJS)
	$(CC) -ggdb -o $@ $^ $(LIBS) $(LDFLAGS)

sugari: src/sugari.o $(OBJS)
	$(CC) -ggdb -o $@ $^ $(LIBS) $(LDFLAGS)

sugarc: src/sugarc.o $(OBJS)
	$(CC) -ggdb -o $@ $^ $(LIBS) $(LDFLAGS)

%.result: %.test sugar
	./sugar $< > $@ || echo "error"

test: sugari 
	@rm -rf test/results
	@mkdir test/results	
	@for t in $(TESTS); do \
		test=$$(echo "$$t" | sed "s/\\.\\/test\\///"); \
		echo -n "  $$test:"; \
		./sugari "test/$$test.test" > "test/results/$$test.sugari.output" 2> /dev/null && \
		diff --strip-trailing-cr "test/results/$$test.sugari.output" "test/$$test.expected" > /dev/null && \
		/bin/echo -e "\e[0;32m ok \033[m" || /bin/echo -e "\e[1;31m fail \033[m"; \
	done

tests: sugar sugari sugarc
	@rm -rf test/results
	@mkdir test/results
	@echo "Testing sugar:"
	@for t in $(TESTS); do \
		test=$$(echo "$$t" | sed "s/\\.\\/test\\///"); \
		echo -n "  $$test:"; \
		./sugar "test/$$test.test" > "test/results/$$test.sugar.output" 2> /dev/null && \
		diff --strip-trailing-cr "test/results/$$test.sugar.output" "test/$$test.expected" > /dev/null && \
		/bin/echo -e "\e[0;32m ok \033[m" || /bin/echo -e "\e[1;31m fail \033[m"; \
	done
	@echo "Testing sugari:"
	@for t in $(TESTS); do \
		test=$$(echo "$$t" | sed "s/\\.\\/test\\///"); \
		echo -n "  $$test:"; \
		./sugari "test/$$test.test" > "test/results/$$test.sugari.output" 2> /dev/null && \
		diff --strip-trailing-cr "test/results/$$test.sugari.output" "test/$$test.expected" > /dev/null && \
		/bin/echo -e "\e[0;32m ok \033[m" || /bin/echo -e "\e[1;31m fail \033[m"; \
	done
	@echo "Testing sugarc:"
	@for t in $(TESTS); do \
		test=$$(echo "$$t" | sed "s/\\.\\/test\\///"); \
		echo -n "  $$test:"; \
		./sugarc "test/$$test.test" "test/results/$$test.sugarc.exec" > /dev/null 2> /dev/null && \
		test/results/$$test.sugarc.exec > "test/results/$$test.sugarc.output" && \
		diff --strip-trailing-cr "test/results/$$test.sugarc.output" "test/$$test.expected" > /dev/null && \
		/bin/echo -e "\e[0;32m ok \033[m" || /bin/echo -e "\e[1;31m fail \033[m"; \
	done

deps: $(SOURCES)
	$(CC) -MD -E $(SOURCES) > /dev/null

-include $(DEPS)
