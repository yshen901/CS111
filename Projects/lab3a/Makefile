#NAME: YUCI SHEN
#EMAIL: SHEN.YUCI11@GMAIL.COM
#ID: 604836772

CC = gcc
CFLAGS = -g -Wall -Wextra
ID = 604836772
ARCHIVE = lab3a-$(ID).tar.gz
SOURCES = lab3a.c Makefile ext2_fs.h README

build: 
	@$(CC) $(CFLAGS) -o lab3a lab3a.c

dist:
	tar -czvf $(ARCHIVE) $(SOURCES)

clean:
	rm -f lab3a $(ARCHIVE)
