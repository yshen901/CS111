# Name: YUCI SHEN
# EMAIL: SHEN.YUCI11@GMAIL.COM
# UID: 604836772

CC = gcc
CFLAGS = -g -Wall -Wextra
ID = 604836772
ARCHIVE = lab2a-$(ID).tar.gz
SOURCES = lab2_add.c Makefile README

build:
	@$(CC) $(CFLAGS) -o lab2_add lab2_add.c -lpthread

# Runs all lab2_add test cases once
tests-add-all:
	@./lab2_add --threads=1   --iterations=10           			>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=20        			>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=40         			>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=80           			>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=100    				>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=1000   				>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=10000 				>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=100000 				>> lab2_add.csv

	@./lab2_add --threads=2   --iterations=100    				>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=1000   				>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=10000 				>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=100000 				>> lab2_add.csv

	@./lab2_add --threads=4   --iterations=100    				>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=1000   				>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=10000 				>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=100000 				>> lab2_add.csv

	@./lab2_add --threads=8   --iterations=100    				>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=1000   				>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=10000 				>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=100000 				>> lab2_add.csv

	@./lab2_add --threads=12  --iterations=100    				>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=1000   				>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=10000 				>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=100000 				>> lab2_add.csv

	@./lab2_add --threads=1   --iterations=10      	--yield   		>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=20      	--yield 		>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=40      	--yield 		>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=80      	--yield   		>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=100     	--yield			>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=1000    	--yield			>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=10000   	--yield			>> lab2_add.csv
	@./lab2_add --threads=1   --iterations=100000  	--yield			>> lab2_add.csv

	@./lab2_add --threads=2   --iterations=10      	--yield   		>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=20      	--yield 		>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=40      	--yield 		>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=80      	--yield   		>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=100     	--yield			>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=1000    	--yield			>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=10000   	--yield			>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=100000  	--yield			>> lab2_add.csv

	@./lab2_add --threads=4   --iterations=10      	--yield   		>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=20      	--yield 		>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=40      	--yield 		>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=80      	--yield   		>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=100     	--yield			>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=1000    	--yield			>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=10000   	--yield			>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=100000  	--yield			>> lab2_add.csv

	@./lab2_add --threads=8   --iterations=10      	--yield   		>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=20      	--yield 		>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=40      	--yield 		>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=80      	--yield   		>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=100     	--yield			>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=1000    	--yield			>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=10000   	--yield			>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=100000  	--yield			>> lab2_add.csv

	@./lab2_add --threads=12  --iterations=10      	--yield		   	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=20      	--yield 		>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=40      	--yield 		>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=80      	--yield	  		>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=100     	--yield			>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=1000    	--yield			>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=10000   	--yield			>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=100000  	--yield	  		>> lab2_add.csv

	@./lab2_add --threads=2   --iterations=100    	--yield   --sync=m  	>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=1000  	--yield   --sync=m	>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=10000   	--yield   --sync=m	>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=100  	--yield   --sync=m	>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=1000  	--yield   --sync=m	>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=10000  	--yield   --sync=m	>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=100  	--yield   --sync=m	>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=1000  	--yield   --sync=m	>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=10000  	--yield   --sync=m	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=100  	--yield   --sync=m	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=1000  	--yield   --sync=m	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=10000  	--yield   --sync=m	>> lab2_add.csv

	@./lab2_add --threads=2   --iterations=100    	--yield   --sync=c  	>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=1000  	--yield   --sync=c	>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=10000   	--yield   --sync=c	>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=100  	--yield   --sync=c	>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=1000  	--yield   --sync=c	>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=10000  	--yield   --sync=c	>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=100  	--yield   --sync=c	>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=1000  	--yield   --sync=c	>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=10000  	--yield   --sync=c	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=100  	--yield   --sync=c	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=1000  	--yield   --sync=c	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=10000  	--yield   --sync=c	>> lab2_add.csv

	@./lab2_add --threads=2   --iterations=100    	--yield   --sync=s  	>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=1000  	--yield   --sync=s	>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=100  	--yield   --sync=s	>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=1000  	--yield   --sync=s	>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=100  	--yield   --sync=s	>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=1000  	--yield   --sync=s	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=100  	--yield   --sync=s	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=1000  	--yield   --sync=s	>> lab2_add.csv

	@./lab2_add --threads=1   --iterations=10000		  --sync=m	>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=10000		  --sync=m	>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=10000		  --sync=m	>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=10000		  --sync=m	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=10000		  --sync=m	>> lab2_add.csv

	@./lab2_add --threads=1   --iterations=10000		  --sync=c	>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=10000		  --sync=c	>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=10000		  --sync=c	>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=10000		  --sync=c	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=10000		  --sync=c	>> lab2_add.csv

	@./lab2_add --threads=1   --iterations=10000		  --sync=s	>> lab2_add.csv
	@./lab2_add --threads=2   --iterations=10000		  --sync=s	>> lab2_add.csv
	@./lab2_add --threads=4   --iterations=10000		  --sync=s	>> lab2_add.csv
	@./lab2_add --threads=8   --iterations=10000		  --sync=s	>> lab2_add.csv
	@./lab2_add --threads=12  --iterations=10000		  --sync=s	>> lab2_add.csv

# Runs all test cases once
tests: clean-csv tests-add-all

graphs: tests
	./lab2_add.gp

dist: graphs
	@tar -czf $(ARCHIVE) $(SOURCES)

clean-csv:
	@rm -f lab2_add.csv lab2_list.csv

clean: clean-csv
	@rm -f lab2_add $(ARCHIVE)
