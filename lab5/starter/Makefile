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

SRCS   = curl_xml.c findpng3.c
HEADERS = curl_xml.h
OBJS3  = findpng3.o
TARGETS= findpng3

all: ${TARGETS}

findpng3: $(HEADERS) $(SRCS)
	$(LD) -o findpng3 $(SRCS) -pthread $(CFLAGS) $(LDLIBS) $(LDFLAGS)

clean:
	rm -f *~ *.d *.o *.gch $(TARGETS) *.png *.html *.txt output*
