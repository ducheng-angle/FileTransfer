XX=g++
FLAGS=-g -Wall -D_REENTRANT
TARGET1=Server
TARGET2=Client
LIB=-pthread

SOURCEDIR=src
BINDIR=bin

all: ${BINDIR}/${TARGET1} ${BINDIR}/${TARGET2}

${BINDIR}/${TARGET1}: 
	${shell mkdir -p ${BINDIR}}
	${XX} ${LIB} -o ${BINDIR}/${TARGET1} ${SOURCEDIR}/Server.cpp

${BINDIR}/${TARGET2}:
	${shell mkdir -p ${BINDIR}}
	${XX}  -o ${BINDIR}/${TARGET2} ${SOURCEDIR}/Client.cpp 
 
clean:
	@rm -rf  ${BINDIR}
.PHONY : clean
