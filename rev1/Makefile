SYSTEM     = x86-64_sles10_4.1
LIBFORMAT  = static_pic

# ---------------------------------------------------------------------         
# Compiler selection                                                            
# ---------------------------------------------------------------------         

# CCC = g++
CCC = gcc

# ---------------------------------------------------------------------         
# Compiler options                                                              
# ---------------------------------------------------------------------         

CCOPT = -m64 -O2 -fPIC -fexceptions -DNDEBUG -DIL_STD -g -Wall

# ---------------------------------------------------------------------         
# Link options and libraries                                                    
# ---------------------------------------------------------------------         

CCFLAGS = $(CCOPT) 
CCLNFLAGS = -lm -pthread 

#------------------------------------------------------------                   
#  make all      : to compile.                                     
#  make execute  : to compile and execute.                         
#------------------------------------------------------------    

ROUTE.exe: main.o ece556.o 
	/bin/rm -f ROUTE.exe
	$(CCC) $(LINKFLAGS) $(CCFLAGS) main.o ece556.o $(CCLNFLAGS) -o ROUTE.exe

main.o: main.c ece556.h
	/bin/rm -f main.o
	$(CCC) $(CCFLAGS) main.c -c

ece556.o: ece556.c ece556.h
	/bin/rm -f ece556.o
	$(CCC) $(CCFLAGS) ece556.c -c

clean:
	/bin/rm -f *~ *.o ROUTE.exe 
