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

typedef enum tagStatus
{
	E_ok = 0,
	E_args,
	E_long_exe,
	E_long_pwd,
	E_prio,
	E_file,
	E_param
} Status;

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

Status process_arguments(int argc, char *argv[], struct proc_msgbuf *request);
int file_exist(char *fname);

int main(int argc, char *argv[])
{
	Status ret;
	int msqid_snd, msqid_rcv;
	key_t key;
	struct proc_msgbuf request;
	struct answer_msgbuf answer;

	ret = process_arguments(argc, argv, &request);
	if (ret != E_ok)
	{
		return -1;
	}

	if ((msqid_snd = msgget(MATRICULA, 0666 )) == -1) {
		perror("msgget");
		fprintf(stderr, "\nDaemon exec_procd nao esta executando.\n");
		return -2;
	}

	key = ftok(request.process, 'm');
	if ((msqid_rcv = msgget(key, 0666 | IPC_CREAT)) == -1) {
		perror("msgget");
		return -3;
	}


	msgsnd(msqid_snd, &request,
			sizeof(struct proc_msgbuf) - sizeof(long), 0);

	msgrcv(msqid_rcv, &answer,
			sizeof(struct answer_msgbuf) - sizeof(long), 0, 0);

	printf("\nIdentificador do processo: %d\n", answer.pid);


	if (msgctl(msqid_rcv, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "\nFalha ao remover message queue.\n");
		return -4;
	}

	return 0;
}

Status process_arguments(int argc, char *argv[], struct proc_msgbuf *request)
{
	int len, i;

	if (argc < 3)
	{
		fprintf(stderr, "\nSolicita ao escalonador que execute um processo\n"
				"Modo de uso:\n"
				"%s <arquivo executavel> <prioridade={1,2,3}>"
				"[<param1> <param2> ... <paramn>]\n\n", argv[0]);
		return E_args;
	}


	len = strlen(argv[1]);
	if (len > EXE_MAX || len < 1)
	{
		fprintf(stderr, "Tamanho de executavel muito grande, abortando.\n");
		return E_long_exe;
	}
	if (!file_exist(argv[1]))
	{
		fprintf(stderr, "Arquivo executavel inacessivel, abortando.\n");
		return E_file;
	}
	strcpy(request->process, argv[1]);


	request->priority = atoi(argv[2]);
	if (request->priority > 3 || request->priority < 1)
	{
		fprintf(stderr, "Prioridade invalida, abortando.\n");
		return E_prio;
	}
	request->mtype = request->priority;


	len = 0;
	request->parameters[0] = '\0';
	for (i = 3; i < argc; i++)
	{
		len += strlen(argv[i]) + 1;
		if (len >= PARAM_MAX)
		{
			fprintf(stderr, "Lista de parametros muito longa, abortando.\n");
			return E_param;
		}
		strcat(request->parameters, " ");
		strcat(request->parameters, argv[i]);
	}


	if (getcwd(request->pwd, PWD_MAX) == NULL)
	{
		fprintf(stderr, "Caminho de pasta invalido ou muito grande, "
						"abortando.\n");
		return E_long_pwd;
	}

	return E_ok;
}


int file_exist(char *fname)
{
	if( access( fname, F_OK ) != -1 ) {
	    return 1;
	} else {
	    return 0;
	}
}

