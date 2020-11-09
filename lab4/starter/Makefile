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
OBJS3  = findpng2.o
TARGETS= findpng2

all: ${TARGETS}

findpng2: $(OBJS3)
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

-include $(SRCS:.c=.d)

.PHONY: clean
clean:
	rm -f *~ *.d *.o $(TARGETS) *.png *.html
