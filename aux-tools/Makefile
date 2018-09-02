CPP = g++
CPPFLAGS = -Ofast -march=native -mtune=native -std=c++11 -malign-double -w -fprofile-use -flto
# -fprofile-use -fprofile-generate
#CPPFLAGS = -Ofast -std=c++11 -align -w -m64

BOOST_INCLUDE=./include/
I= -I ${BOOST_INCLUDE} 

all: aux-band aux-dos

aux-band: aux-band.cpp
	${CPP} ${CPPFLAGS} ${I} aux-band.cpp -o $@
	mv aux-band bin/.

aux-dos: aux-dos.cpp
	${CPP} ${CPPFLAGS} ${I} aux-dos.cpp -o $@
	mv aux-dos bin/.

clean:
	rm -rf bin/*
