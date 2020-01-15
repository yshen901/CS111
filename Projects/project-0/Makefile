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

check:
	@echo "Test input" > input.txt;

	@./lab0 < input.txt > output.txt
	@diff input.txt output.txt
	@if [ $$? -ne 0 ] ; \
	then \
		echo "SUCCESS: read/write"; \
	else \
		echo "FAILED : read/write"; \
	fi
	@rm -f output.txt

	@./lab0 --input=input.txt > output.txt
	@diff input.txt output.txt
	@if [ $$? -eq 0 ] ; \
	then \
		echo "SUCCESS: Input redirection"; \
	else \
		echo "FAILED : Input redirection"; \
	fi
	@rm -f output.txt

	@./lab0 --output=output.txt < input.txt
	@diff input.txt output.txt
	@if [ $$? -eq 0 ] ; \
	then \
		echo "SUCCESS: Output redirection"; \
	else \
		echo "FAILED : Output redirection"; \
	fi
	@rm -f output.txt

	@./lab0 --input=input.txt --output=output.txt
	@diff input.txt output.txt
	@if [ $$? -eq 0 ] ; \
	then \
		echo "SUCCESS: Input+Output redirection"; \
	else \
		echo "FAILED : Input+Output redirection"; \
	fi
	@rm -f output.txt

	@./lab0 --segfault
	@if [ $$? -eq 139 ] ; \
	then \
		echo "SUCCESS: Segfault trigger"; \
	else \
		echo "FAILED : Segfault trigger"; \
	fi

	@./lab0 --segfault --catch
	@if [ $$? -ne 4 ] ; \
	then \
		echo "SUCCESS: Segfault catch"; \
	else \
		echo "FAILED : Segfault catch"; \
	fi

	@./lab0 --fakeOption
	@if [ $$? -eq 1 ] ; \
	then \
		echo "CAUGHT  : Incorrect argument error"; \
	else \
		echo "UNCAUGHT: Incorrect argument error"; \
	fi

	@./lab0 --input=fakeInput.txt --output=output.txt
	@if [ $$? -eq 2 ] ; \
	then \
		echo "CAUGHT  : Inaccessible input error"; \
	else \
		echo "UNCAUGHT: Inaccessible input error"; \
	fi

	@echo "Inaccessible file" > fakeOutput.txt
	@chod -w fakeOutput.txt
	@./lab0 --output=fakeOutput.txt < input.txt
	@if [ $$? -eq 3 ] ; \
	then \
		echo "CAUGHT  : Inaccessible output error"; \
	else \
		echo "UNCAUGHT: Inaccessible output error"; \
	fi

dist:
	@tar -czf $(ARCHIVE) $(sources)

clean:
	@rm -f lab0 output.txt $(ARCHIVE)
