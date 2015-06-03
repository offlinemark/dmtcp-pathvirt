pv-test: pv-test.c
	cc -g -o pv-test pv-test.c

check: pv-test
	DMTCP_PATH_PREFIX=$(HOME)/dmtcp/test/plugin/pathvirt/slot5/ \
		dmtcp_launch -i3 --with-plugin ../dmtcp_pathvirthijack.so ./pv-test

restart:
	DMTCP_PATH_PREFIX=$(HOME)/dmtcp/test/plugin/pathvirt/misc/slot7 \
		./dmtcp_restart_script.sh

clean:
	rm -f pv-test ckpt* dmtcp*
	> pv-test.txt
	> ../misc/slot7/pv-test.txt
