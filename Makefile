CFLAGS = -std=c99 -O0 -lm -Wall -MP -MD -o

all: bin 

bin: excercise1.c
	gcc $(CFLAGS) $@ $<

.PHONY: test push clean

test: bin
	./bin
         
push: main.c
	curl -F 'f:1=<-' ix.io < $<

clean: bin
	rm -f bin *.d
