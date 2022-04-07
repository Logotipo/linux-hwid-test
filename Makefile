all:
	gcc hwid_test.c diskid.c sysfs.c nonroot.c sha256.c -o hwid_test
