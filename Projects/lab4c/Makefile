# NAME: YUCI SHEN
# ID: 604836772
# EMAIL: SHEN.YUCI11@GMAIL.COM

CC = gcc
CFLAGS = -g -Wall -Wextra
ID = 604836772
ARCHIVE = lab4c-$(ID).tar.gz
SOURCES = lab4c.c Makefile README smoketest.sh

build:
	@$(CC) $(CFLAGS) -o lab4c lab4c.c -lm -lmraa

dist:
	@tar -czvf $(ARCHIVE) $(SOURCES)

check:
	@chmod +x smoketest.sh
	./smoketest.sh
clean:
	@rm -f lab4c log.txt $(ARCHIVE)
