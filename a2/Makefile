TYPE:=int

CXX = u++					# compiler
CXXFLAGS = -g -Wall -Wextra -MMD -DTYPE="${TYPE}"
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS1 = q1main.o	# object files forming 1st executable with prefix "q1"
EXEC1 = binsertsort

OBJECTS2 = q2main.o q2potato.o q2player.o # object files forming 2st executable with prefix "q2"
EXEC2 = hotpotato

OBJECTS = ${OBJECTS1} ${OBJECTS2}		# all object files
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2}			# all executables

.PHONY : all clean

all : ${EXECS}					# build all executables

#############################################################

-include ImplType

ifeq (${IMPLTYPE},${TYPE})			# same implementation type as last time ?
${EXEC1} : ${OBJECTS1}
	${CXX} ${CXXFLAGS} $^ -o $@
else
ifeq (${TYPE},)					# no implementation type specified ?
# set type to previous type
TYPE=${IMPLTYPE}
${EXEC1} : ${OBJECTS1}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed
.PHONY : ${EXEC1}
${EXEC1} :
	rm -f ImplType
	touch q1binsertsort.h
	sleep 1
	${MAKE} ${EXEC1} TYPE="${TYPE}"
endif
endif

ImplType :
	echo "IMPLTYPE=${TYPE}" > ImplType
	sleep 1

${EXEC2} : ${OBJECTS2}				# link step 2nd executable
	${CXX} ${CXXFLAGS} $^ -o $@

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXECS} ImplType
