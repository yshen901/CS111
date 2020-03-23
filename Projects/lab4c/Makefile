# NAME: YUCI SHEN
# ID: 604836772
# EMAIL: SHEN.YUCI11@GMAIL.COM

CC = gcc
CFLAGS = -g -Wall -Wextra
ID = 604836772
ARCHIVE = lab4c-$(ID).tar.gz
SOURCES = lab4c_tcp.c lab4c_tls.c Makefile README smoketest.sh

build:
	@$(CC) $(CFLAGS) -o lab4c_tcp lab4c_tcp.c -lm -lmraa
	@$(CC) $(CFLAGS) -o lab4c_tls lab4c_tls.c -lm -lmraa -lssl -lcrypto

dist:
	@tar -czvf $(ARCHIVE) $(SOURCES)

check:
	@chmod +x smoketest.sh
	./smoketest.sh
clean:
	@rm -f lab4c_tls lab4c_tcp log.txt $(ARCHIVE)
