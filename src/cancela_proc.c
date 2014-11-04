#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define key_cancela 100113788

struct cancel_msgbuf {
		long mtype;
		int pid;
};

int main(int argc, char *argv[]) {

	int msqid_snd;
   struct cancel_msgbuf cancel;

	if(argc < 2) {
		fprintf(stderr, "\nInforme o pid do processo que deseja cancelar.\n");
		return -1;
	}

   if ((msqid_snd = msgget(key_cancela, 0666)) == -1) {
		perror("msgget");
      fprintf(stderr, "\nDaemon execprocd nao esta executando.\n");
		return -2;
	}

   cancel.mtype = 1;
	cancel.pid = atoi(argv[1]);
	if (msgsnd(msqid_snd, &cancel, sizeof(struct cancel_msgbuf) - sizeof(long), 0) == -1) {
		perror("msgsnd");
		fprintf(stderr, "\nNao foi possivel enviar cancelamento do processo %d.\n", cancel.pid);
	}
	
	return 0;
}
