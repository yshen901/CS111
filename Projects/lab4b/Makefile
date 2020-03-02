# NAME: YUCI SHEN
# ID: 604836772
# EMAIL: SHEN.YUCI11@GMAIL.COM

CC = gcc
CFLAGS = -g -Wall -Wextra
ID = 604836772
ARCHIVE = lab4b-$(ID).tar.gz
SOURCES = lab4b.c Makefile README

build:
	@$(CC) $(CFLAGS) -o lab4b lab4b.c -lm -lmraa

dist:
	@tar -czvf $(ARCHIVE) $(SOURCES)

check:
	{
		echo "START"
		echo "PERIOD=5"
		echo "SCALE=F"
		echo "STOP"
		echo "OFF"
	} | ./lab4b --log=log.txt --period=2 --scale=C

	if [ $? -ne 0 ]
	then
		echo "ERROR program has invalid exit code"
	else
		echo "OK passed smoketest"
	fi

	for command in START STOP PERIOD=5 SCALE=F OFF ; do \
		grep "$command" log.txt > /dev/null
		if [ $? -eq 0 ]
			echo "OK passed logging test"
		else
			echo "ERROR failed logging test"
		fi
	done

clean:
	@rm -f lab4b log.txt $(ARCHIVE)
