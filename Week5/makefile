CXX = gcc
CXXFLAGS = -Wall
#CXXFLAGS += -g
#CXXFLAGS += -pedantic


SRC1 = main.c 
SRC2 = tokenizedcommand.c 
SRC3 = parser.c
SRC4 = prompt.c
SRC5 = executer.c
SRCS = ${SRC1} ${SRC2} ${SRC3} ${SRC4} ${SRC5}

OBJ1 = main.o 
OBJ2 = tokenizedcommand.o
OBJ3 = parser.o
OBJ4 = prompt.o
OBJ5 = executer.o
OBJS = ${OBJ1} ${OBJ2} ${OBJ3} ${OBJ4} ${OBJ5}

HDR1 = tokenizedcommand.h
HDR2 = parser.h
HDR3 = prompt.h
HDR4 = executer.h
HDR5 = definitions.h
HDRS = ${HDR1} ${HDR2} ${HDR3} ${HDR4} ${HDR5}

PROG = smallsh 

default: ${OBJS} ${PROG}
	rm -f *.o *.gch
${OBJS}: ${SRCS} ${HDRS}
	${CXX} ${CXXFLAGS} ${SRCS} ${HDRS} -c $(@.o=.c)

${PROG}: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${PROG}

clean:
	rm -f  *.o *~ *.gch  ${PROG}
