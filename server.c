#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/sem.h>
#include <sys/shm.h>

static struct sembuf sop_lock = {
	0, -1, 0
};


int main(int argc, char** argv)
{	
	if(argc < 2)
   	{
       		printf("./server max number_client\n");
       		return;
  	}	
	char* filename;
	FILE *file;
	int sock, listener;
	int max_number_client = atoi(argv[1]);
	struct sockaddr_in addr;
	char buf[1024];
	int bytes_read;
	int numb_client;
//if(mas_shmid=shmget(IPC_PRIVATE,sizeof(int *) * vsrt.size(), IPC_CREATE|0666))<0)	
	//{
	//	perror("Created error");
	//	exit(EXIT_FAILURE);
	//}

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener < 0)
	{
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3436);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		exit(2);
	}

	listen(listener, 1);
	int semid;
	semid = semget(IPC_PRIVATE, 1, 0666|IPC_CREAT);
	if(semid == -1)
	{
		perror("Semophore creation failed\n");
		exit(EXIT_FAILURE);
	}
	
	struct sembuf sop_unlock;
	sop_unlock.sem_num=0;
	sop_unlock.sem_flg=0;
	sop_unlock.sem_op=atoi(argv[1]);
	listen(listener, 1);
	semop(semid,&sop_unlock,1);
	sop_unlock.sem_op = 1;


	while(1)
	{
		sock = accept(listener, NULL, NULL);
		//numb_client++;
		if(sock < 0)
		{
			perror("accept");
			exit(3);
		}

		switch(fork())
		{
		case -1:
			perror("fork");
			break;

		case 0:
			semop(semid, &sop_lock, 1);

			close(listener);
			while(1)
			{
				char c;
				int i = 0;
				memset(buf,0,1024);
				filename = (char*)malloc(12 * sizeof(char));
				printf("memset filename\n");
				read(sock, buf, 1024);
				printf("%s\n",buf);
				strcpy(filename,buf);
				printf("\n%s\n",filename)	;
			        printf("before while\n");
				if((file = fopen(filename, "r")) == NULL)
				{
					memcpy(buf, "file not found\n",sizeof(buf));
					send(sock, buf, sizeof(buf),0);
					close(sock);
					free(filename);

				}
				else
				{
					while((c = fgetc(file)) != EOF)
				{
				buf[i] = c;
				i++;
				if((i == 1023))
				{
					printf("send\n");
					send(sock, buf, sizeof(buf), 0);
					i=0;
					memset(buf,0,sizeof(buf));		
				}

			}
			if(i!=0)
			{
				send(sock, buf, sizeof(buf), 0);
			}
			fclose(file);
			close(sock);
			semop(semid, &sop_unlock, 1); 

			//numb_client--;
			exit(0);
		}
	}

		default:
		close(sock);
		}
}

close(listener);
semctl (semid,0, IPC_RMID, 0);


return 0;
}

