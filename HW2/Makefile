CC ?= gcc
CFLAGS = -shared -ldl -fPIC -DLOG_FILE

.PHONY: all clean test

all: hijack.so

hijack.so: hijack.c
	${CC} -o $@ $< ${CFLAGS}

clean:
	@rm hijack.so

test: hijack.so
	LD_PRELOAD=./hijack.so curl http://rubygems.nos.ncu.edu.tw -so ./output.tmp
	@rm ./output.tmp

test_SSL: hijack.so
	LD_PRELOAD=./hijack.so curl https://rubygems.nos.ncu.edu.tw -so ./output.tmp
	@rm ./output.tmp
