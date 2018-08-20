# # # # # # #
# Makefile for assignment 1
# student no 789931
# created 29 Jan 2017
# Matt Farrugia <matt.farrugia@unimelb.edu.au>
#

CC     = gcc
CFLAGS = -Wall
EXE    = a1
OBJ    = main.o traverse.o graph.o list.o stack.o queue.o
#									add any new files here ^

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

main.o: traverse.h graph.h
traverse.o: traverse.h graph.h list.h stack.h queue.h
graph.o: graph.h

stack.o: stack.h list.h
queue.o: queue.h list.h
list.o: list.h

clean:
	rm -f $(OBJ) $(EXE)

STUDENTNUM = 789931
SUBMISSION = Makefile report.pdf main.c graph.h graph.c traverse.h traverse.c \
			 list.h list.c stack.h stack.c queue.h queue.c
#									add any new files here ^
submission: $(SUBMISSION)
	tar -czvf $(STUDENTNUM).tar.gz $(SUBMISSION)
