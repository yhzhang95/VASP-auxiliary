CPP = g++
CPPFLAGS = -Ofast -march=native -mtune=native -std=c++11 -malign-double -w -fprofile-use -flto 
#CPPFLAGS = -Ofast -std=c++11 -align -w -m64

BOOST_INCLUDE=./include/
I= -I ${BOOST_INCLUDE} 

all: cppotcar cppotcarall

cppotcar: cppotcar.cpp
	${CPP} ${CPPFLAGS} ${I} cppotcar.cpp -o $@
	mv cppotcar bin/.

cppotcarall: cppotcarall.cpp
	${CPP} ${CPPFLAGS} ${I} cppotcarall.cpp -o $@
	mv cppotcarall bin/.

clean:
	rm -rf bin/*
