override CFLAGS := -Wall -Werror -std=gnu99 -O0 -g  $(CFLAGS) -I.
CC = gcc

# I generally make the first rule run all the tests
#all: check

# rule for making the parser.o  that is needed by all the test programs
myshell: myshell.c 	myshell_parser.c

# each of the test files depend on their own .c and myshell_parser.h
#  add another time for each test, e.g., test_simple_pipe.o line below
#test_simple_input.o: test_simple_input.c myshell_parser.h
#test_simple_pipe.o: test_simple_pipe.c myshell_parser.h
#test_both_redirects.o: test_both_redirects.c myshell_parser.h
#test_every_input.o: test_every_input.c myshell_parser.h
#test_leading_space.o: test_leading_space.c myshell_parser.h
#test_max_length.o: test_max_length.c myshell_parser.h
#test_no_input.o: test_no_input.c myshell_parser.h
#test_overflow.o: test_overflow.c myshell_parser.h
#test_trailing_space.o: test_trailing_space.c myshell_parser.h
#test_weird_spacing1.o: test_weird_spacing1.c myshell_parser.h
#test_weird_spacing2.o: test_weird_spacing2.c myshell_parser.h
#test_weird_spacing3.o: test_weird_spacing3.c myshell_parser.h
#test_weird_spacing4.o: test_weird_spacing4.c myshell_parser.h

# each of the test programs executables are generated by combining the generated .o with the parser.o
#test_simple_input : test_simple_input.o myshell_parser.o
#test_simple_pipe : test_simple_pipe.o myshell_parser.o
#test_both_redirects: test_both_redirects.o myshell_parser.o
#test_every_input: test_every_input.o myshell_parser.o
#test_leading_space: test_leading_space.o myshell_parser.o
#test_max_length: test_max_length.o myshell_parser.o
#test_no_input: test_no_input.o myshell_parser.o
#test_overflow: test_overflow.o myshell_parser.o
#test_trailing_space: test_trailing_space.o myshell_parser.o
#test_weird_spacing1: test_weird_spacing1.o myshell_parser.o
#test_weird_spacing2: test_weird_spacing2.o myshell_parser.o
#test_weird_spacing3: test_weird_spacing3.o myshell_parser.o
#test_weird_spacing4: test_weird_spacing4.o myshell_parser.o

# Add any additional tests here, e.g., the commented out test_simple_pipe
#test_files=./test_simple_input ./test_both_redirects ./test_every_input ./test_leading_space ./test_max_length ./test_no_input ./test_overflow ./test_trailing_space ./test_weird_spacing1 ./test_weird_spacing2 ./test_weird_spacing3 ./test_weird_spacing4
#./test_simple_pipe 

.PHONY: clean check checkprogs all

# Build all of the test programs
checkprogs: $(test_files)

check: checkprogs
	/bin/sh run_tests.sh $(test_files)
	./myshell
clean:
	rm -f *~ *.o $(test_files) $(test_o_files)
