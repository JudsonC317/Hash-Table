# makefile for MP7
#
# -lm is used to link in the math library
# -Wall turns on all warning messages 
#
comp = gcc
comp_flags = -g -Wall 
comp_libs = -lm  

lab7 : table.o lab7.o
	$(comp) $(comp_flags)  table.o lab7.o -o lab7 $(comp_libs)

table.o : table.c table.h
	$(comp) $(comp_flags) -c table.c

lab7.o : lab7.c table.h
	$(comp) $(comp_flags) -c lab7.c

clean :
	rm -f *.o lab7 core

