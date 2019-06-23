CXX = u++
CXXFLAGS = -std=c++11 -Wall -g -MMD -Wextra -multi
EXEC = soda
OBJECTS = main.o configparms.o bank.o bottlingplant.o	\
		  groupoff.o nameserver.o parent.o printer.o	\
		  student.o truck.o vendingmachine.o watcard.o	\
		  watcardoffice.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}
