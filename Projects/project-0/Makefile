#NAME : YUCI SHEN
#EMAIL: SHEN.YUCI11@GMAIL.COM
#ID   : 604836772

CC = gcc
CFLAGS = -g -Wall -Wextra
ID = 604836772
ARCHIVE = lab0-$(ID).tar.gz
sources = lab0.c input.txt backtrace.png breakpoint.png Makefile README

build:
	@$(CC) $(CFLAGS) -o lab0 lab0.c


#$? refers to the most recent exit code
#2> redirects everything from STDERR (file descriptor 2) into a file
#Adding a ; \ after the call allows the Makefile to not crash at the error
check:
	@echo "Test input" > input.txt;

	@./lab0 < input.txt > output.txt 2> dump.txt ; \
	diff input.txt output.txt
	@if [ $$? -eq 0 ] ; \
	then \
		echo "SUCCESS: read/write"; \
	else \
		echo "FAILED : read/write"; \
	fi
	@rm -f output.txt

	@./lab0 --input=input.txt > output.txt 2> dump.txt ; \
	diff input.txt output.txt
	@if [ $$? -eq 0 ] ; \
	then \
		echo "SUCCESS: Input redirection"; \
	else \
		echo "FAILED : Input redirection"; \
	fi
	@rm -f output.txt

	@./lab0 --output=output.txt < input.txt 2> dump.txt ; \
	diff input.txt output.txt
	@if [ $$? -eq 0 ] ; \
	then \
		echo "SUCCESS: Output redirection"; \
	else \
		echo "FAILED : Output redirection"; \
	fi
	@rm -f output.txt

	@./lab0 --input=input.txt --output=output.txt 2> dump.txt ; \
	diff input.txt output.txt
	@if [ $$? -eq 0 ] ; \
	then \
		echo "SUCCESS: Input+Output redirection"; \
	else \
		echo "FAILED : Input+Output redirection"; \
	fi
	@rm -f output.txt

	@./lab0 --segfault 2> dump.txt ; \
	if [ $$? -eq 139 ] ; \
	then \
		echo "SUCCESS: Segfault trigger"; \
	else \
		echo "FAILED : Segfault trigger"; \
	fi

	@./lab0 --segfault --catch 2> dump.txt ; \
	if [ $$? -eq 4 ] ; \
	then \
		echo "SUCCESS: Segfault catch"; \
	else \
		echo "FAILED : Segfault catch"; \
	fi

	@./lab0 --fakeOption 2> dump.txt ; \
	if [ $$? -eq 1 ] ; \
	then \
		echo "CAUGHT  : Incorrect argument error"; \
	else \
		echo "UNCAUGHT: Incorrect argument error"; \
	fi

	@./lab0 --input=fakeInput.txt --output=output.txt 2> dump.txt ; \
	if [ $$? -eq 2 ] ; \
	then \
		echo "CAUGHT  : Inaccessible input error"; \
	else \
		echo "UNCAUGHT: Inaccessible input error"; \
	fi

	@echo "Inaccessible file" > fakeOutput.txt
	@chmod -w fakeOutput.txt
	@./lab0 --output=fakeOutput.txt < input.txt 2> dump.txt ; \
	if [ $$? -eq 3 ] ; \
	then \
		echo "CAUGHT  : Inaccessible output error"; \
	else \
		echo "UNCAUGHT: Inaccessible output error"; \
	fi
	@rm -f fakeOutput.txt

dist:
	@tar -czf $(ARCHIVE) $(sources)

clean:
	@rm -f lab0 output.txt fakeOutput.txt dump.txt $(ARCHIVE)
