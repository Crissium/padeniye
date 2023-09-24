P=padeniye
CFLAGS = -g -Wall -O3 `pkg-config --cflags sfml-all`
LDLIBS= `pkg-config --libs sfml-all`
OBJS=main.o common.o game.o ui.o

$(P): $(OBJS)
	$(CXX) $(CFLAGS) -o $(P) $(OBJS) $(LDLIBS)

archive: main.cc common.h common.cc game.h game.cc ui.h ui.cc
	zip padeniye *.cc *.h Makefile

main.o: main.cc common.h game.h ui.h
	$(CXX) $(CFLAGS) -c main.cc -o main.o

common.o: common.cc common.h
	$(CXX) $(CFLAGS) -c common.cc -o common.o

game.o: game.cc game.h common.h
	$(CXX) $(CFLAGS) -c game.cc -o game.o

ui.o: ui.cc game.h common.h ui.h
	$(CXX) $(CFLAGS) -c ui.cc -o ui.o
