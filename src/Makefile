server: server.cpp ../include/NLPIR.h
	g++ -g server.cpp -L../lib/ ../lib/libNLPIR.so -lNLPIR -lpthread -m64  -O3 --std=c++0x -DOS_LINUX -o server
clean:
	rm server
