# Makefile, ECE252
# Yiqing Huang <yqhuang@uwaterloo.ca>

CC = gcc
CFLAGS_XML2 = $(shell xml2-config --cflags)
CFLAGS_CURL = $(shell curl-config --cflags)
CFLAGS = -Wall $(CFLAGS_XML2) $(CFLAGS_CURL) -std=gnu99 -g -DDEBUG1_
LD = gcc
LDFLAGS = -std=gnu99 -g
LDLIBS_XML2 = $(shell xml2-config --libs)
LDLIBS_CURL = $(shell curl-config --libs)
LDLIBS = $(LDLIBS_XML2) $(LDLIBS_CURL)

SRCS   = curl_xml.c findpng2.c
HEADERS = curl_xml.h
OBJS3  = findpng2.o
TARGETS= findpng2

all: ${TARGETS}

findpng2: $(HEADERS) $(SRCS)
	$(LD) -o findpng2 $(SRCS) -pthread $(CFLAGS) $(LDLIBS) $(LDFLAGS)

empty: findpng2
	./findpng2 http://ece252-1.uwaterloo.ca/lab4

m10t1: findpng2
	./findpng2 -m 10 http://ece252-1.uwaterloo.ca/lab4

m30t1: findpng2
	./findpng2 -m 30 http://ece252-1.uwaterloo.ca/lab4

m10t5: findpng2
	./findpng2 -m 10 -t 5 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

hm10t5: findpng2
	valgrind --tool=helgrind ./findpng2 -m 10 -t 5 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

vm10t5: findpng2
	valgrind ./findpng2 -m 10 -t 5 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

m10t15: findpng2
	./findpng2 -m 10 -t 15 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

m30t5: findpng2
	./findpng2 -m 30 -t 5 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

m30t10: findpng2
	./findpng2 -m 30 -t 10 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

hm30t10: findpng2
	valgrind --tool=helgrind ./findpng2 -m 30 -t 10 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

vm30t10: findpng2
	valgrind ./findpng2 -m 30 -t 10 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

m50t5: findpng2
	./findpng2 -m 50 -t 5 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

m50t15: findpng2
	./findpng2 -m 50 -t 15 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

m55t20: findpng2
	./findpng2 -m 55 -t 20 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

m100t20: findpng2
	./findpng2 -m 100 -t 20 -v logfile.txt http://ece252-1.uwaterloo.ca/lab4

.PHONY: clean
clean:
	rm -f *~ *.d *.o *.gch $(TARGETS) *.png *.html *.txt output*
