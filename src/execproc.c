#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define EXE_MAX 250
#define PWD_MAX 1000
#define PARAM_MAX 250

typedef enum tagStatus
{
	E_ok = 0,
	E_args,
	E_long_exe,
	E_long_pwd,
	E_prio
}Status;

struct proc_msgbuf
{
	long mtype;
	int priority;
	char pwd[PWD_MAX];
	char process[EXE_MAX];
	char parameters[PARAM_MAX];
};

Status process_arguments(int argc, char *argv[], struct proc_msgbuf *request);

int main(int argc, char *argv[])
{
	Status ret;
	struct proc_msgbuf request;

	ret = process_arguments(argc, argv, &request);
	if (ret != E_ok)
	{
		return -1;
	}

	return 0;
}

Status process_arguments(int argc, char *argv[], struct proc_msgbuf *request)
{
	int len, i;

	if (argc < 3)
	{
		printf("\nSolicita ao escalonador que execute um processo\n"
			   "Modo de uso:\n"
			   "%s <arquivo executavel> <prioridade={1,2,3}>"
			   "[<param1> <param2> ... <paramn>]\n\n", argv[0]);
		return E_args;
	}

	len = strlen(argv[1]);
	if (len > EXE_MAX || len < 1)
	{
		printf("Tamanho de executavel muito grande, abortando");
		return E_long_exe;
	}
	strcpy(request->process, argv[1]);

	request->priority = atoi(argv[2]);
	if (request->priority > 3 || request->priority < 1)
	{
		printf("Prioridade invalida, abortando");
		return E_prio;
	}
	request->mtype = request->priority;

	request->parameters[0] = '\0';
	for (i = 3; i < argc; i++)
	{
		strcat(request->parameters, " ");
		strcat(request->parameters, argv[i]);
	}

	if (getcwd(request->pwd, PWD_MAX) == NULL)
	{
		printf ("Caminho de pasta invalido ou muito grande");
		return E_long_pwd;
	}

	return E_ok;
}

