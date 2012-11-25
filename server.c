#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
int main(int argc, char** argv)
{
    if(argc <2)
	{
	  printf("./server max_numb_of_client");
	  return;
	}
    int sock, listener;
    int numb_max_client = atoi(argv[1]);
    int numb_client=0;
    struct sockaddr_in addr;
    char buf[1024];
    int bytes_read;
    char* filename;
    FILE *file;

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
    
    while(1)
    {  
	if (numb_client< numb_max_client)
	{	
         sock = accept(listener, NULL, NULL);
         numb_client++;
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
            close(listener);
            while(1)
            { 
               
            char c;
            int i = 0;
            memset(buf,0,1024);
            filename = (char*)malloc(12 * sizeof(char));       
            read(sock, buf, 1024);
            printf("%s\n",buf);
            strcpy(filename,buf);
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
                printf("sock close\n");
            }
            }

            close(sock);
            exit(0);
            
        default:
            close(sock);
        }
    }
    
    close(listener);
    numb_client--;
}

    return 0;
}
