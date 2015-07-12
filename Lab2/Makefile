CXX      = g++
CXXFLAGS = -Wall

EXECS  = lab2-persistent lab2-non-persistent lab2-p-persistent
SRC    = main.cc

all: ${EXECS}

lab2-persistent: ${SRC}
	${CXX} ${CXXFLAGS} -o $@ ${SRC} -DONE_PERSISTENT
	${CXX} ${CXXFLAGS} -DDEBUG -o $@-debug ${SRC} -DONE_PERSISTENT

lab2-non-persistent: ${SRC}
	${CXX} ${CXXFLAGS} -o $@ ${SRC} -DNON_PERSISTENT
	${CXX} ${CXXFLAGS} -DDEBUG -o $@-debug ${SRC} -DONE_PERSISTENT

lab2-p-persistent: ${SRC}
	${CXX} ${CXXFLAGS} -o $@ ${SRC} -DPRB_PERSISTENT
	${CXX} ${CXXFLAGS} -DDEBUG -o $@-debug ${SRC} -DONE_PERSISTENT

clean:
	rm -f ${EXECS} $(EXECS:=-debug)
