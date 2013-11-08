# all: sugar sugari sugarc clean_tmp
all: sugar sugari sugarc

.PHONY: all clean clear clean_tmp deps test tests cleartests test_sugar test_sugari test_sugarc

SRCS = $(shell find ./src -type f -name '*.cpp')
SRCS := $(filter-out ./src/parser/parser.cpp ./src/sugari.cpp ./src/sugarc.cpp ./src/sugar.cpp, $(SRCS))
SRCS += src/parser/parser.cpp

OBJS = $(SRCS:.cpp=.o)

TESTS = $(shell find ./test -type f -name '*.test' | sort )
TESTS := $(TESTS:.test=)

CPPFLAGS +=  -std=c++0x `llvm-config --cppflags` 
LDFLAGS += `llvm-config --ldflags`
LIBS += `llvm-config --libs` -ledit -lreadline -lcurses
CC = 'g++'
# CC = 'clang'
LINKER = 'g++'

DEPS := $(patsubst %.o,%.d,$(OBJS))

test: test_sugar	

clear: clean

clean: clean_tmp
	@$(RM) -rf sugar sugari sugarc
	@echo "all cleaned"
	
clean_tmp: cleartests
	@$(RM) -rf $(OBJS) $(DEPS) src/parser/parser.cpp src/parser/lexer.inter.hpp src/parser/lexer.batch.hpp src/parser/lexer.h src/parser/parser.hpp src/parser/parser.output src/parser/parser.dot test/results src/sugar.o src/sugari.o src/sugarc.o
	@echo "temporary files cleaned"

src/parser/parser.cpp: src/parser/parser.y
	bison -W --debug -v -d -o $@ $^
	
src/parser/parser.hpp: src/parser/parser.cpp

src/parser/InteractiveParser.o: src/parser/InteractiveParser.cpp src/parser/lexer.inter.hpp
	$(CC) -ggdb -c $(CPPFLAGS) -o $@ ./src/parser/InteractiveParser.cpp

src/parser/BatchParser.o: src/parser/BatchParser.cpp src/parser/lexer.batch.hpp
	$(CC) -ggdb -c $(CPPFLAGS) -o $@ ./src/parser/BatchParser.cpp
	
src/parser/lexer.inter.hpp: src/parser/lexer.l
	flex --header-file=src/parser/lexer.h --interactive -o $@ $^ 
	
src/parser/lexer.batch.hpp: src/parser/lexer.l
	flex --header-file=src/parser/lexer.h --batch --never-interactive --fast --perf-report -o $@ $^
	
src/parser/lexer.h: src/parser/lexer.inter.hpp

%.o: %.cpp src/config.h src/parser/parser.hpp
	$(CC) -ggdb -c $(CPPFLAGS) -o $@ $<
	
src/sugar.o: src/parser/lexer.batch.hpp src/parser/parser.hpp src/parser/lexer.batch.hpp

src/sugari.o: src/parser/lexer.inter.hpp src/parser/parser.hpp src/parser/lexer.inter.hpp

src/sugarc.o: src/parser/lexer.batch.hpp src/parser/parser.hpp src/parser/lexer.batch.hpp

sugar: src/sugar.o $(filter-out ./src/parser/InteractiveParser.o, $(OBJS))
	$(LINKER) -ggdb -o $@ $^ $(LIBS) $(LDFLAGS)

sugari: src/sugari.o $(filter-out ./src/parser/BatchParser.o, $(OBJS))
	$(LINKER) -ggdb -o $@ $^ $(LIBS) $(LDFLAGS)

sugarc: src/sugarc.o $(filter-out ./src/parser/InteractiveParser.o, $(OBJS))
	$(LINKER) -ggdb -o $@ $^ $(LIBS) $(LDFLAGS)

%.result: %.test sugar
	./sugar $< > $@ || echo "error"

cleartests:
	@$(RM) -rf test/results
	@mkdir test/results	
	@echo "old test results cleared"

test_sugar: cleartests sugar
	@echo "Testing sugar:"
	@for t in $(TESTS); do \
		test=$$(echo "$$t" | sed "s/\\.\\/test\\///"); \
		echo -n "  $$test:"; \
		./sugar "test/$$test.test" > "test/results/$$test.sugar.output" 2> /dev/null; \
		sugar_result=$$?; \
		if [ -e "test/$$test.compil.error" ]; then  \
			if [ "$$sugar_result" -eq 0 ]; then \
				/bin/echo -e "\e[1;31m fail \033[m"; \
			else \
				diff --strip-trailing-cr "test/results/$$test.sugar.output" "test/$$test.compil.error" > /dev/null && /bin/echo -e "\e[0;32m ok \033[m" || /bin/echo -e "\e[1;31m fail \033[m"; \
			fi; \
		else \
			if [ "$$sugar_result" -ne 0 ]; then \
				/bin/echo -e "\e[1;31m fail \033[m"; \
			else \
				diff --strip-trailing-cr "test/results/$$test.sugar.output" "test/$$test.expected" > /dev/null && /bin/echo -e "\e[0;32m ok \033[m" || /bin/echo -e "\e[1;31m fail \033[m"; \
			fi; \
		fi; \
	done \
	
test_sugari: cleartests sugari
	@echo "Testing sugari:"
	@for t in $(TESTS); do \
		test=$$(echo "$$t" | sed "s/\\.\\/test\\///"); \
		echo -n "  $$test:"; \
		./sugari "test/$$test.test" > "test/results/$$test.sugari.output" 2> /dev/null; \
		sugari_result=$$?; \
		if [ -e "test/$$test.interpret.error" ]; then  \
			if [ "$$sugari_result" -eq 0 ]; then \
				/bin/echo -e "\e[1;31m fail \033[m"; \
			else \
				diff --strip-trailing-cr "test/results/$$test.sugari.output" "test/$$test.interpret.error" > /dev/null && /bin/echo -e "\e[0;32m ok \033[m" || /bin/echo -e "\e[1;31m fail \033[m"; \
			fi; \
		else \
			if [ "$$sugari_result" -ne 0 ]; then \
				/bin/echo -e "\e[1;31m fail \033[m"; \
			else \
				diff --strip-trailing-cr "test/results/$$test.sugari.output" "test/$$test.expected" > /dev/null && /bin/echo -e "\e[0;32m ok \033[m" || /bin/echo -e "\e[1;31m fail \033[m"; \
			fi; \
		fi; \
	done \

test_sugarc: cleartests sugarc
	@echo "Testing sugarc:"
	@for t in $(TESTS); do \
		test=$$(echo "$$t" | sed "s/\\.\\/test\\///"); \
		echo -n "  $$test:"; \
		if [ -e "test/$$test.compil.error" ]; then  \
			./sugarc "test/$$test.test" "test/results/$$test.sugarc.exec" > "test/results/$$test.sugarc.output" 2> /dev/null; \
			sugarc_result=$$?; \
			if [ "$$sugarc_result" -eq 0 ]; then \
				/bin/echo -e "\e[1;31m fail \033[m"; \
			else \
				diff --strip-trailing-cr "test/results/$$test.sugarc.output" "test/$$test.compil.error" > /dev/null && /bin/echo -e "\e[0;32m ok \033[m" || /bin/echo -e "\e[1;31m fail \033[m"; \
			fi; \
		else \
			./sugarc "test/$$test.test" "test/results/$$test.sugarc.exec" > /dev/null 2> /dev/null; \
			sugarc_result=$$?; \
			if [ "$$sugarc_result" -ne 0 ]; then \
				/bin/echo -e "\e[1;31m fail \033[m"; \
			else \
				test/results/$$test.sugarc.exec > "test/results/$$test.sugarc.output"; \
				sugarc_result=$$?; \
				if [ "$$sugarc_result" -ne 0 ]; then \
					/bin/echo -e "\e[1;31m fail \033[m"; \
				else \
					diff --strip-trailing-cr "test/results/$$test.sugarc.output" "test/$$test.expected" > /dev/null && /bin/echo -e "\e[0;32m ok \033[m" || /bin/echo -e "\e[1;31m fail \033[m"; \
				fi; \
			fi; \
		fi; \
	done \

tests: test_sugar test_sugari test_sugarc	

deps: $(SOURCES)
	$(CC) -MD -E $(SOURCES) > /dev/null

-include $(DEPS)
