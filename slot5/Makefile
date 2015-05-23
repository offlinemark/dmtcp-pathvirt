pv-test: pv-test.c
	cc -g -o pv-test pv-test.c

check: pv-test
	DMTCP_PATH_PREFIX=$(HOME)/dmtcp/test/plugin/pathvirt/slot5/ \
		dmtcp_launch -i3 --with-plugin ../dmtcp_pathvirthijack.so ./pv-test

clean:
	rm -f pv-test ckpt* dmtcp*
	echo > pv-test.txt
	echo > ../misc/slot7/pv-test.txt
