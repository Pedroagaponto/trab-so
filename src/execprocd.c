#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define NFDS 3
#define EXE_MAX 250
#define PWD_MAX 1000
#define PARAM_MAX 250
#define KEY_EXEC 100119191

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

int main ()
{
	int msqid_snd, msqid_rcv, signal_fd;
	int nfds = NFDS, daemon_running = 1;
	key_t key;
	struct proc_msgbuf request;
	struct answer_msgbuf answer;
	struct pollfd fds[NFDS];
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGTERM);
	sigaddset(&mask, SIGUSR1);
	signal_fd = signalfd (-1, &mask, 0);


	if ((msqid_rcv = msgget(KEY_EXEC, 0666 | IPC_CREAT | IPC_EXCL)) == -1) {
		perror("msgget");
		fprintf(stderr, "\nNao foi possivel criar nova fila.\n"
				"Execute: \"ipcrm -Q %d\" para remover a fila ja existente\n",
				KEY_EXEC);
		return -1;
	}


	key = ftok(request.process, 'm');
	if ((msqid_snd = msgget(key, 0666)) == -1) {
		perror("msgget");
		fprintf(stderr, "\nFalha ao conectar na fila de resposta: %d.\n",
				key);
		return -3;
	}

	fds[0].fd = msqid_rcv;
	fds[0].events = POLLIN;
	fds[1].fd = msqid_snd;
	fds[1].events = POLLIN;
	fds[2].fd = signal_fd;
	fds[2].events = POLLIN;

	answer.mtype = 1;
	answer.pid = 10101;
	msgrcv(msqid_rcv, &request,
			sizeof(struct proc_msgbuf) - sizeof(long), 0, 0);
	msgsnd(msqid_snd, &answer,
			sizeof(struct answer_msgbuf) - sizeof(long), 0);


	while(daemon_running)
	{
		npoll = ppoll (fds, nfds, &timeout, NULL );
		
	}


	if (msgctl(msqid_rcv, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "\nFalha ao remover message queue.\n");
		return -4;
	}

	return 0;
}
