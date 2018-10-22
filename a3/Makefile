OPT:= # -multi -O2
MIMPL:=IMPLICIT
BIMPL:=NOBUSY
TYPE:=int
SENTINEL:=-1

CXX = u++					# compiler
CXXFLAGS = -g ${OPT} -Wall -Wextra -MMD -std=c++14 -D"${MIMPL}" -D"${BIMPL}" \
	   -DTYPE="${TYPE}" -DSENTINEL="${SENTINEL}" # compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS1 = q2main.o q2matrixmultiply.o q2multiplier.o	# object files forming 1st executable with prefix "q2"
EXEC1 = matrixmultiply				# 1st executable name

OBJECTS2 = q3main.o q3consumer.o q3producer.o	# object files forming 2nd executable with prefix "q3"
EXEC2 = buffer					# 2nd executable name

OBJECTS = ${OBJECTS1} ${OBJECTS2}		# all object files
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2}			# all executables

#############################################################

.PHONY : all clean

all : ${EXECS}					# build all executables

-include MatrixImpl

ifeq (${MATRIXIMPL},${MIMPL})			# same implementation type as last time ?
${EXEC1} : ${OBJECTS1}
	${CXX} ${CXXFLAGS} $^ -o $@
else
ifeq (${MIMPL},)				# no implementation type specified ?
# set type to previous type
MIMPL=${MATRIXIMPL}
${EXEC1} : ${OBJECTS1}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed
.PHONY : ${EXEC1}
${EXEC1} :
	rm -f MatrixImpl
	touch q2matrixmultiply.h
	${MAKE} ${EXEC1} MIMPL=${MIMPL}
endif
endif

MatrixImpl :
	echo "MATRIXIMPL=${MIMPL}" > MatrixImpl
	sleep 1

-include BufImpl

ifeq (${BUFIMPL},${BIMPL})			# same implementation type as last time ?
${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@
else
ifeq (${BIMPL},)				# no implementation type specified ?
# set type to previous type
BIMPL=${BUFIMPL}
${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed
.PHONY : ${EXEC2}
${EXEC2} :
	rm -f BufImpl
	touch q3buffer.h
	sleep 1
	${MAKE} ${EXEC2} BIMPL="${BIMPL}"
endif
endif

BufImpl :
	echo "BUFIMPL=${BIMPL}" > BufImpl
	sleep 1

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXECS} MatrixImpl BufImpl
