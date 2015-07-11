CXX      = g++
CXXFLAGS = -Wall

EXECS  = lab2-persistent lab2-non-persistent lab2-p-persistent
SRC    = main.cc

all: ${EXECS}

lab2-persistent:
	${CXX} ${CXXFLAGS} -o $@ ${SRC} -DONE_PERSISTENT

lab2-non-persistent:
	${CXX} ${CXXFLAGS} -o $@ ${SRC} -DNON_PERSISTENT

lab2-p-persistent:
	${CXX} ${CXXFLAGS} -o $@ ${SRC} -DPRB_PERSISTENT

clean:
	rm -f *.o ${EXECS}
