SchedAnalysis: ../bin/schedAnalysis.o ../bin/randomTask.o
	g++ -o ../bin/SchedAnalysis ../bin/schedAnalysis.o ../bin/randomTask.o

../bin/schedAnalysis.o: schedAnalysis.cpp
	g++ -I../include -c -o ../bin/schedAnalysis.o schedAnalysis.cpp

../bin/randomTask.o: randomTask.cpp
	g++ -I../include -c -o ../bin/randomTask.o randomTask.cpp
clean:
	rm -rf ../bin/*
