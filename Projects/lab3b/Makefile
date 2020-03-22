#NAME: YUCI SHEN
#EMAIL: SHEN.YUCI11@GMAIL.COM
#ID: 604836772

ID = 604836772
ARCHIVE = lab3b-$(ID).tar.gz
SOURCES = lab3b.py lab3b.sh Makefile README

default: 
	cp lab3b.sh lab3b
	chmod +x lab3b
dist:
	tar -czvf $(ARCHIVE) $(SOURCES)

clean:
	rm -f lab3b $(ARCHIVE)
