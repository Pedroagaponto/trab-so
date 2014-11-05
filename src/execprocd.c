#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>


#define NFDS 3
#define EXE_MAX 250
#define PWD_MAX 1000
#define PARAM_MAX 250
#define KEY_EXEC 100119191
#define KEY_CANCEL 100113788
#define TIMEOUT 1000

struct proc_msgbuf
{
	long mtype;
	int priority;
	char pwd[PWD_MAX];
	char process[EXE_MAX];
	char parameters[PARAM_MAX];
};

struct pid_msgbuf
{
	long mtype;
	int pid;
};

int handle_signal (int signal_fd);
int handle_cancel (int msqid_cancel);
int handle_exec (int msqid_exec);
int write_pid ();

int main ()
{
	int msqid_exec, msqid_cancel, signal_fd;
	int nfds = NFDS, daemon_running = 1, npoll, i;
	struct pollfd fds[NFDS];
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGTERM);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGCHLD);
	if ((sigprocmask(SIG_BLOCK, &mask, NULL)) == -1)
		return -1;
	if ((signal_fd = signalfd (-1, &mask, 0)) == -1)
		return -2;


	if ((msqid_exec = msgget(KEY_EXEC, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
	{
		perror("msgget_exec");
		fprintf(stderr, "\nNao foi possivel criar nova fila.\n"
				"Execute: \"ipcrm -Q %d\" para remover a fila ja existente\n",
				KEY_EXEC);
		return -3;
	}

	if ((msqid_cancel = msgget(KEY_CANCEL, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
	{
		perror("msgget_cancel");
		fprintf(stderr, "\nNao foi possivel criar nova fila.\n"
				"Execute: \"ipcrm -Q %d\" para remover a fila ja existente\n",
				KEY_EXEC);
		return -3;
	}


	fds[0].fd = signal_fd;
	fds[0].events = POLLIN;
	fds[1].fd = msqid_cancel;
	fds[1].events = POLLIN;
	fds[2].fd = msqid_exec;
	fds[2].events = POLLIN;



	while (daemon_running)
	{
		npoll = poll (fds, nfds, TIMEOUT);
		if (npoll == -1)
		{
			perror("ppoll");
			break;
		}
		
		for (i = 0; (i < nfds) && (npoll); i++)
		{
			if (!(fds[i].revents))
				continue;
			npoll--;

			if (fds[i].fd == signal_fd)
			{
				if (handle_signal(signal_fd) == -1)
					return -1;
			}

			if (fds[i].fd == msqid_cancel)
			{
				if (handle_cancel(msqid_cancel) == -1)
					return -1;
			}

			if (fds[i].fd == msqid_exec)
			{
				if (handle_exec(msqid_exec) == -1)
					return -1;
			}

		}

		//handle quantum
	}	

	
	//print_stats();
	//kill_child();
	if (msgctl(msqid_exec, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "\nFalha ao remover message exec queue.\n");
		return -4;
	}

	if (msgctl(msqid_cancel, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "\nFalha ao remover message cancel queue.\n");
		return -4;
	}
	

	return 0;
}

int handle_signal (int signal_fd)
{
	return signal_fd;
}

int handle_cancel (int msqid_cancel)
{
	return msqid_cancel;
}

int handle_exec (int msqid_exec)
{
	/*
	key_t key;
	struct proc_msgbuf request;
	struct pid_msgbuf answer;
	msgrcv(msqid_rcv, &request,
			sizeof(struct proc_msgbuf) - sizeof(long), 0, 0);
	msgsnd(msqid_snd, &answer,
			sizeof(struct answer_msgbuf) - sizeof(long), 0);

	key = ftok(request.process, 'm');
	if ((msqid_snd = msgget(key, 0666)) == -1)
	{
		perror("msgget");
		fprintf(stderr, "\nFalha ao conectar na fila de resposta: %d.\n",
				key);
		return -4;
	}

	*/
	return msqid_exec;
}

int write_pid ()
{
	FILE *fp;
	pid_t pid = getpid();
	
	fp = fopen("execprocd.pid", "w");
	if(!fp) {
		printf("Erro ao abrir o arquivo execprocd.pid");
		return -1;
   }
	
	fprintf(fp, "%d", pid);
	fclose(fp);

	return 0; //Sucess
}

