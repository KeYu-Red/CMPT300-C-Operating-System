#include<stdio.h>
#include<unistd.h>/*#包含<unistd.h>*/
#include<sys/types.h>/*#包含<sys/types.h>*/
int main(void)
{
    int childpid;
	int i;

	if (fork() == 0){
		//child process
		for (i=1; i<=8; i++){
			printf("This is child process\n");
		}
	}else{
		//parent process
		for(i=1; i<=8; i++){
			printf("This is parent process\n");
		}
	}
    

	printf("step2 after fork() !!\n\n");
}