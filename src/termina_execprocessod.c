#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


int main () {

	FILE *fp;
	float pid;
	pid_t p;
	
	if ( (fp = fopen ("execprocd.pid", "r")) == NULL){
		printf("Arquivo execprocd.pid nao existe.\n");
		return -1;
	}

	fscanf(fp, "%f", &pid);

	p = pid;
	kill(p, 15); //15 = SIGTERM
	return 0;
}
