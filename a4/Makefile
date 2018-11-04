LIMPL:=MC
OUTPUT:=OUTPUT

CXX = u++					# compiler
CXXFLAGS = -g -multi -O2 -std=c++11 -Wall -Wextra -MMD -D${LIMPL} -D${OUTPUT} # compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS = q1tallyVotes${LIMPL}.o # list of object files for question 1 prefixed with "q1"
EXEC = vote

DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"

#############################################################

.PHONY : all clean

all : ${EXEC}					# build all executables

-include LockImpl

ifeq (${LOCKIMPL},${LIMPL})			# same implementation type as last time ?
${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@
else
ifeq (${LIMPL},)				# no implementation type specified ?
# set type to previous type
LIMPL=${LOCKIMPL}
${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed
.PHONY : ${EXEC}
${EXEC} :
	rm -f LockImpl
	touch q1tallyVotes.h
	sleep 1
	${MAKE} ${EXEC} LIMPL="${LIMPL}"
endif
endif

LockImpl :
	echo "LOCKIMPL=${LIMPL}" > LockImpl
	sleep 1

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXEC} LockImpl
