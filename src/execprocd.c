#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define EXE_MAX 250
#define PWD_MAX 1000
#define PARAM_MAX 250
#define MATRICULA 100119191

struct proc_msgbuf
{
	long mtype;
	int priority;
	char pwd[PWD_MAX];
	char process[EXE_MAX];
	char parameters[PARAM_MAX];
};

struct answer_msgbuf
{
	long mtype;
	int pid;
};

int main (/*int argc, char *argv[]*/)
{
	int msqid_snd, msqid_rcv;
	key_t key;
	struct proc_msgbuf request;
	struct answer_msgbuf answer;

	if ((msqid_rcv = msgget(MATRICULA, 0666 | IPC_CREAT | IPC_EXCL)) == -1) {
		perror("msgget");
		fprintf(stderr, "\nNao foi possivel criar nova fila.\n"
				"Execute: \"ipcrm -Q %d\" para remover a fila ja existente\n",
				MATRICULA);
		return -1;
	}

	msgrcv(msqid_rcv, &request,
			sizeof(struct proc_msgbuf) - sizeof(long), 0, 0);


	key = ftok(request.process, 'm');
	if ((msqid_snd = msgget(key, 0666)) == -1) {
		perror("msgget");
		fprintf(stderr, "\nFalha ao conectar na fila de resposta: %d.\n",
				key);
		return -3;
	}

	answer.mtype = 1;
	answer.pid = 10101;
	msgsnd(msqid_snd, &answer,
			sizeof(struct answer_msgbuf) - sizeof(long), 0);

	if (msgctl(msqid_rcv, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "\nFalha ao remover message queue.\n");
		return -4;
	}

	while(1)
	{
		sleep(1);
	}

	return 0;
}
