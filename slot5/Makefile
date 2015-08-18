pv-test: pv-test.c
	cc -g -o pv-test pv-test.c

# check: pv-test
check:
	# DMTCP_PATH_PREFIX=$(HOME)/dmtcp/test/plugin/pathvirt/misc/slot7/
	DMTCP_PATH_PREFIX=$(HOME)/dmtcp/test/plugin/pathvirt/slot5/bin5:$(HOME)/dmtcp/test/plugin/pathvirt/slot5/doc5:$(HOME)/dmtcp/test/plugin/pathvirt/slot5/lib5 \
		dmtcp_launch -i2 --with-plugin ../dmtcp_pathvirthijack.so /usr/bin/python pv-test.py -v
		# dmtcp_launch -i3 --with-plugin ../dmtcp_pathvirthijack.so /usr/bin/python
	# dmtcp_launch -i3 --with-plugin ../dmtcp_pathvirthijack.so ./pv-test

restart:
	DMTCP_PATH_PREFIX=$(HOME)/dmtcp/test/plugin/pathvirt/misc/slot7/bin7:$(HOME)/dmtcp/test/plugin/pathvirt/misc/slot7/doc7:$(HOME)/dmtcp/test/plugin/pathvirt/misc/slot7/lib7 \
		./dmtcp_restart_script.sh
	# DMTCP_PATH_PREFIX=$(HOME)/dmtcp/test/plugin/pathvirt/misc/slot7/ \
		./dmtcp_restart_script.sh
	# ./dmtcp_restart_script.sh

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
	
