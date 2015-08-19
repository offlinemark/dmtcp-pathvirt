PATHVIRT = $(HOME)/dmtcp/test/plugin/pathvirt

pv-test: pv-test.cpp
	g++ -g -o pv-test $<


check: clean pv-test
	DMTCP_PATH_PREFIX=$(PATHVIRT)/slot5/bin5:$(PATHVIRT)/slot5/doc5:$(PATHVIRT)/slot5/lib5 \
		dmtcp_launch -i2 --with-plugin ../dmtcp_pathvirthijack.so ./pv-test
		# dmtcp_launch -i2 --with-plugin ../dmtcp_pathvirthijack.so /usr/bin/python pv-test.py


check-python:  clean
	DMTCP_PATH_PREFIX=$(PATHVIRT)/slot5/bin5:$(PATHVIRT)/slot5/doc5:$(PATHVIRT)/slot5/lib5 \
		dmtcp_launch -i2 --with-plugin ../dmtcp_pathvirthijack.so /usr/bin/python pv-test.py

restart:
	DMTCP_PATH_PREFIX=$(HOME)/dmtcp/test/plugin/pathvirt/misc/slot7/bin7:$(HOME)/dmtcp/test/plugin/pathvirt/misc/slot7/doc7:$(HOME)/dmtcp/test/plugin/pathvirt/misc/slot7/lib7 \
		./dmtcp_restart_script.sh

clean:
	rm -f pv-test ckpt* dmtcp*
	> ../misc/slot7/pv-test.txt
	> ../misc/slot7/bin7/pv-test.txt
	> ../misc/slot7/doc7/pv-test.txt
	> ../misc/slot7/lib7/pv-test.txt
	> pv-test.txt
	> bin5/pv-test.txt
	> doc5/pv-test.txt
	> lib5/pv-test.txt
	
test:
	cat bin5/pv-test.txt
	cat doc5/pv-test.txt
	cat lib5/pv-test.txt
	cat ../misc/slot7/bin7/pv-test.txt
	cat ../misc/slot7/doc7/pv-test.txt
	cat ../misc/slot7/lib7/pv-test.txt
