#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#define out_redirect 1
#define in_redirect 2
#define have_pip 3

#define READ_END 0
#define WRITE_END 1
char recent[256];
int call_recent = 0;
void getInput(char*);
void instructionSegment(char*, int*, char a[][256]);
void executeCmd(int, char a[][256]);

int main(void) {
	int should_run = 1; /* flag to determine when to exit program */
	int argcount = 0;
	int i = 0;
	char arglist[100][256];
	char** arg = NULL;
	char* buf = NULL;
	buf = (char*)malloc(256);

	while (should_run) {
		printf("osh>");
		fflush(stdout);
		/*
		* After reading user input, the steps are:
		* (1) fork a child process using fork()
		* (2) the child process will invoke execvp()
		* (3) parent will invoke wait() unless command included &
		*/
		memset(buf, 0, 256);
		getInput(buf);
		/*exit*/
		if (strcmp(buf, "exit\n") == 0) {
			break;
		}
		for (i = 0; i < 100; i++) {
			arglist[i][0] = '\0';
		}
		argcount = 0;
		/*Pass the argcount address -> function reason*/
		instructionSegment(buf, &argcount, arglist);
		executeCmd(argcount, arglist);
	}
	
	if (buf != NULL) {
		free(buf);
		buf = NULL;
	}
	should_run=0;
	exit(0);
	

	return 0;
}

void getInput(char* buf) {
	int len = 0;
	int ch;
	ch = getchar();
	/*while loop getchar()*/
	while (len < 256 && ch != '\n') {
		buf[len++] = ch;
		ch = getchar();
	}
	/*check length*/
	if (len == 256) {
		printf("Command is too long \n");
		exit(-1);
	}
	/*check !!*/
	/*from recent copy to buffer*/
	if (buf[0] == '!' && buf[1] == '!') {
		if (call_recent == 0) {
			printf("No command in history.\n");
		}
		memset(buf, 0, sizeof(buf));
		int i = 0;
		for (; i < strlen(recent); i++) {
			buf[i] = recent[i];
		}
		buf[i++] = '\n';
		buf[i] = '\0';
		call_recent = 1;
	}
	/*nomal case*/
	/*deal with string in buffer*/
	else {
		memset(recent, 0, sizeof(recent));
		for (int i = 0; i < strlen(buf); i++) {
			recent[i] = buf[i];
		}
		buf[len++] = '\n';
		buf[len] = '\0';
	}
}

void instructionSegment(char* buf, int* argcount, char arglist[100][256]) {
	char* p = buf;
	char* q;
	int number = 0;
	while (1) {
		if (p[0] == '\n') { break; }
		if (p[0] == ' ') { p++; }
		/*p->head;q-move->end;p=q;*/
		else {
			q = p;
			number = 0;
			/*segment*/
			while ((q[0] != ' ') && (q[0] != '\n')) {
				number++;
				q++;
			}
			/*number+1 -> for \0*/
			/*arg have no \n*/
			strncpy(arglist[*argcount], p, number + 1);
			arglist[*argcount][number] = '\0';
			*argcount = *argcount + 1;
			p = q;
		}
	}
}

void executeCmd(int argcount, char arglist[100][256]) {
	int flag = 0;//judge command 
	int background = 0;//seperate process
	int op = 0;
	int status;
	int i;
	int fd;
	char* arg[argcount + 1];
	char* argnext[argcount + 1];
	char* file;
	pid_t pid;

	/* Copy Command. */
	for (i = 0; i < argcount; i++) {
		arg[i] = (char*)arglist[i];
	}
	arg[argcount] = NULL;

	/* Find & -> seperate process */
	for (i = 0; i < argcount; i++) {
		if (strncmp(arg[i], "&", 1) == 0) {
			/* last arg */
			if (i == argcount - 1) {
				background = 1;
				arg[argcount - 1] = NULL;
				break;
			}
			else {
				printf("Wrong Command\n");
			}
		}
	}

	/* Different Operations. */
	for (i = 0; arg[i] != NULL; i++) {
		if (strcmp(arg[i], ">") == 0) {
			flag++;
			op = out_redirect;
			if (arg[i + 1] == NULL) {
				flag++;//no output files
			}
			file = arg[i + 1];
			arg[i] = NULL;
		}
		if (strcmp(arg[i], "<") == 0) {
			flag++;
			op = in_redirect;
			if (i == 0) {
				flag++;//no input files
			}
			file = arg[i + 1];
			arg[i] = NULL;
		}
		if (strcmp(arg[i], "|") == 0) {
			flag++;
			op = have_pip;
			if (arg[i + 1] == NULL) { flag++; }
			if (i == 0) { flag++; }//wrong position
		}
	}
	/* Flag -> Wrong Command. */
	if (flag > 1) { printf("Wrong Command\n"); return; }

	/* Find the output file */
	/*if (op == out_redirect) {
		for (i = 0; arg[i] != NULL; i++) {
			if (strcmp(arg[i], ">") == 0) {
				file = arg[i + 1];
				arg[i] = NULL;
			}
		}
	}*/

	/* Find the input file */
	/*if (op == in_redirect) {
		for (i = 0; arg[i] != NULL; i++) {
			if (strcmp(arg[i], "<") == 0) {
				file = arg[i + 1];
				arg[i] = NULL;
			}
		}
	}*/

	/* pipe & copy instruction. */
	if (op == have_pip) {
		for (i = 0; arg[i] != NULL; i++) {
			if (strcmp(arg[i], "|") == 0) {
				arg[i] = NULL;
				int j;
				for (j = i + 1; arg[j] != NULL; j++) {
					argnext[j - i - 1] = arg[j];
				}
				argnext[j - i - 1] = arg[j];
				break;
			}
		}
	}

	/* Classic Fork Error.*/
	if ((pid = fork()) < 0) {
		printf("Fork Error\n");
		return;
	}

	switch (op) {
	case 0:
		if (pid == 0) {
			execvp(arg[0], arg);
			exit(0);
		}
		break;
	case 1:
		if (pid == 0) {
			fd = open(file, O_RDWR | O_CREAT | O_TRUNC, 0644);
			dup2(fd, 1);
			execvp(arg[0], arg);
			exit(0);
		}
		break;
	case 2:
		if (pid == 0) {
			fd = open(file, O_RDONLY);
			dup2(fd, 0);
			execvp(arg[0], arg);
			exit(0);
		}
		break;
	case 3:
		if(pid==0){
			pid_t pid2;
			int fd2[2];
			pipe(fd2);

			/*if((pipe(fd2))==-1){
				fprintf(stderr,"Pipe Failed");
			}*/
			
			pid2=fork();
			if(pid2<0){
				fprintf(stderr, "Fork Failed");
			}

			if(pid2==0){//child 
					
				dup2(fd2[WRITE_END],1);
				execvp(arg[0], arg);
				close(fd2[READ_END]);
				close(fd2[WRITE_END]);
				exit(0);

							
			}
			else{//parent

				dup2(fd2[READ_END],0);
				
				execvp (argnext[0],argnext);
				close(fd2[WRITE_END]);
				close(fd2[READ_END]);
				exit(0);
			}
		}
		break;
	default:
		break;
	}
	
	
	/*Parent Process*/
	if(background){
		printf("[Concurrency process id %d]\n", pid);
		return;
		}
	
	/*background==0(no &)*/
	/*wait*/
	if (waitpid(pid, &status, 0) == -1){
		printf("Wait for child process error.\n");
	}
}
