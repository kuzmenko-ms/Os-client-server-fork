#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



int main(int argc, char** argv)
{
    
    char buf[1024];
    char* filename;
    FILE *file;
    int sock, listener;
    struct sockaddr_in addr;
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3436);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);
    
    while(1)
    {
        sock = accept(listener, NULL, NULL);
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
            { 	char c;
		int i;
		memset(buf,0,1024);
		filename=(char*)malloc(12*sizeof(char));
		read(sock, buf, 1024);
		strcpy(filename,buf);
		if((file = fopen(filename, "r")) == NULL)
            {
                memcpy(buf, "file not found\n",sizeof(buf));
                send(sock, buf, sizeof(buf),0);
                close(sock);
                free(filename);
                //exit(2);
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
                        //sleep(10);
                        //printf("%i\n",i);
                        i=0;
                        memset(buf,0,sizeof(buf));

                    }


                }
		if (i!=0)
		{
		send(sock,buf,sizeof(buf),0);
                
                if(bytes_read <= 0) break; 
		}
		fclose(file);

            close(sock);
            exit(0);
            }
        default:
            close(sock);
        }
    }
    
    close(listener);

    return 0;
}
}
