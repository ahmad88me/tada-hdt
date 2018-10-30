HEADERS = filter_num.h logger.h features.h
#headers = 
NOMOBJS =  filter_num.o logger.o features.o # no email objects
OBJS =  $(NOMOBJS) main.o 
TOBJS = $(NOMOBJS) tests.o
NOMSOURCES = filter_num.cpp logger.cpp features.cpp # sources excluding main.cpp
SOURCES = $(NOMSOURCES) main.cpp
TSOURCES = $(NOMSOURCES) tests.cpp # test sources
CXXFLAGS = -std=c++11 
CC = g++
OBJ_DIR = build
SRC_DIR = src
BIN_DIR = bin
LIBS = -lhdt -pthread
TLIBS = $(LIBS) -lgtest
TESTAPP = bin/testtadanum
COVAPP = bin/covtadanum
COVCLEANFILES = gcov.css snow.png ruby.png *.gcov  *.gcda *.gcno index-sort-f.html index-sort-l.html index.html \
				amber.png glass.png updown.png coverage.info emerald.png Users usr v1\

OBJS_ABS = $(patsubst %,$(OBJ_DIR)/%,$(OBJS))
TOBJS_ABS = $(patsubst %,$(OBJ_DIR)/%,$(TOBJS))
SOURCES_ABS = $(patsubst %,$(SRC_DIR)/%,$(SOURCES))
TSOURCES_ABS = $(patsubst %,$(SRC_DIR)/%,$(TSOURCES))


$(BIN_DIR)/tadanum : $(OBJS_ABS)
	$(CC) -o $@ $? $(LIBS)

$(OBJS_ABS): $(SOURCES_ABS)
	$(CC) $(CXXFLAGS) -c $(SOURCES_ABS)
	mv *.o build/

.PHONY: clean run debug test cov cleancov gcov codecov gcovr

debug: 
	$(CC) $(CXXFLAGS) -g -c $(SOURCES_ABS)
	mv *.o build/
	$(CC) -g  -o bin/tadanum $(OBJS_ABS) $(LIBS)
	valgrind --tool=massif bin/tadanum --time-unit=B --stacks=yes

test:
	$(CC) -c $(TSOURCES_ABS)
	mv *.o build/
	$(CC) -o $(TESTAPP) $(TOBJS_ABS) $(TLIBS)
	$(TESTAPP)

cov:
	$(CC) -c -fprofile-arcs -ftest-coverage -fPIC  $(TSOURCES_ABS)
	mv *.o build/
	$(CC) -o $(COVAPP) -fprofile-arcs -ftest-coverage $(TOBJS_ABS) $(TLIBS) 
	$(COVAPP)
	lcov --directory . --capture --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage.info
	lcov --list coverage.info

gcovr:
	gcovr -r .

codecov:
	bash <(curl -s https://codecov.io/bash) || echo "Codecov did not collect coverage reports"

gcov:
	$(CC) -c -fprofile-arcs -ftest-coverage $(TSOURCES_ABS)
	mv *.o build/
	$(CC) -o $(COVAPP) -fprofile-arcs -ftest-coverage $(TOBJS_ABS) $(TLIBS)
	$(COVAPP)


cleancov:
	-rm -Rf $(COVCLEANFILES)
	
run: 
	bin/tadanum

clean:
	-rm $(BIN_DIR)/* 
	-rm $(OBJ_DIR)/* 
	$(MAKE) cleancov
