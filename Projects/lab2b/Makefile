# Name: YUCI SHEN
# EMAIL: SHEN.YUCI11@GMAIL.COM
# ID: 604836772

CC = gcc
CFLAGS = -g -Wall -Wextra
ID = 604836772
ARCHIVE = lab2b-$(ID).tar.gz
SOURCES = lab2_list.c SortedList.h SortedList.c
DATA = lab2b_list.csv profile.out
OTHER = Makefile README lab2b_list.gp
GRAPHS = lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png

build:
	@$(CC) $(CFLAGS) -o lab2_list lab2_list.c SortedList.c -lpthread -lprofiler

# Runs all test cases once
tests: build
	@rm -f lab2b_list.csv
	@./lab2_list --threads=1  --iterations=1000		    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=2  --iterations=1000		    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=1000		    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=1000		    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000		    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=16 --iterations=1000		    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=24 --iterations=1000		    --sync=m	>> lab2b_list.csv

	@./lab2_list --threads=1  --iterations=1000		    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=2  --iterations=1000		    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=1000		    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=1000		    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000		    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=16 --iterations=1000		    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=24 --iterations=1000		    --sync=s	>> lab2b_list.csv



	@-./lab2_list --threads=1  --iterations=1   --lists=4   --yield=id   		>> lab2b_list.csv
	@-./lab2_list --threads=4  --iterations=1   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=8  --iterations=1   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=12 --iterations=1   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=16 --iterations=1   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=1  --iterations=2   --lists=4   --yield=id   		>> lab2b_list.csv
	@-./lab2_list --threads=4  --iterations=2   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=8  --iterations=2   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=12 --iterations=2   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=16 --iterations=2   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=1  --iterations=4   --lists=4   --yield=id   		>> lab2b_list.csv
	@-./lab2_list --threads=4  --iterations=4   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=8  --iterations=4   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=12 --iterations=4   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=16 --iterations=4   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=1  --iterations=8   --lists=4   --yield=id   		>> lab2b_list.csv
	@-./lab2_list --threads=4  --iterations=8   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=8  --iterations=8   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=12 --iterations=8   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=16 --iterations=8   --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=1  --iterations=16  --lists=4   --yield=id   		>> lab2b_list.csv
	@-./lab2_list --threads=4  --iterations=16  --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=8  --iterations=16  --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=12 --iterations=16  --lists=4   --yield=id    		>> lab2b_list.csv
	@-./lab2_list --threads=16 --iterations=16  --lists=4   --yield=id    		>> lab2b_list.csv

	@./lab2_list --threads=1  --iterations=10  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=10  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=10  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=10  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=16 --iterations=10  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=1  --iterations=20  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=20  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=20  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=20  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=16 --iterations=20  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=1  --iterations=40  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=40  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=40  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=40  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=16 --iterations=40  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=1  --iterations=80  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=80  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=80  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=80  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=16 --iterations=80  --lists=4   --yield=id    --sync=s	>> lab2b_list.csv

	@./lab2_list --threads=1  --iterations=10  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=10  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=10  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=10  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=16 --iterations=10  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=1  --iterations=20  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=20  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=20  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=20  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=16 --iterations=20  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=1  --iterations=40  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=40  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=40  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=40  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=16 --iterations=40  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=1  --iterations=80  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=80  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=80  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=80  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=16 --iterations=80  --lists=4   --yield=id    --sync=m	>> lab2b_list.csv



	@./lab2_list --threads=1  --iterations=1000  --lists=4 		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=1  --iterations=1000  --lists=8 		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=1  --iterations=1000  --lists=16		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=2  --iterations=1000  --lists=4 		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=2  --iterations=1000  --lists=8 		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=2  --iterations=1000  --lists=16		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=1000  --lists=4 		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=1000  --lists=8 		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=1000  --lists=16		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=1000  --lists=4 		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=1000  --lists=8 		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=1000  --lists=16		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000  --lists=4 		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000  --lists=8 		     --sync=m	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000  --lists=16		     --sync=m	>> lab2b_list.csv



	@./lab2_list --threads=1  --iterations=1000  --lists=4 		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=1  --iterations=1000  --lists=8 		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=1  --iterations=1000  --lists=16		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=2  --iterations=1000  --lists=4 		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=2  --iterations=1000  --lists=8 		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=2  --iterations=1000  --lists=16		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=1000  --lists=4 		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=1000  --lists=8 		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=1000  --lists=16		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=1000  --lists=4 		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=1000  --lists=8 		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=1000  --lists=16		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000  --lists=4 		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000  --lists=8 		     --sync=s	>> lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000  --lists=16		     --sync=s	>> lab2b_list.csv


graphs: tests
	./lab2b_list.gp

profile: build
	rm -f ./output.out profile.out
	LD_PRELOAD=/usr/local/cs/gperftools-2.7/lib/libprofiler.so.0
	CPUPROFILE=./output.out ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./output.out > profile.out
	pprof --list=thread_func ./lab2_list ./output.out >> profile.out
	pprof --list=lock ./lab2_list ./output.out >> profile.out
	rm -f ./output.out

dist: graphs
	@tar -czvf $(ARCHIVE) $(SOURCES) $(DATA) $(OTHER) $(GRAPHS)

clean:
	@rm -f lab2_list $(ARCHIVE)
