CFLAGS = -std=c99 -O0 -lm -Wall -MP -MD -o

MAKEFLAGS += --no-builtin-rules

all: bin 

bin: excercise1.c
	gcc $(CFLAGS) $@ $<

.PHONY: test clean diff

test: bin
	./bin

clean: bin
	rm -f bin *.d

# @ silences the printing of the command
# $(info ...) prints output 
diff:
	$(info The status of the repository, and the volume of per-file changes:)
	@git status
	@git diff --stat