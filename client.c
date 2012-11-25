#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char** argv)
{
    int sock;
    char buf[1024];

    if (argc<2)
    {
	printf("./client Name_FILE\n");
        return;
    }

    
    struct sockaddr_in addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3436); 
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }

    memcpy(buf,argv[2],1024);
    write(sock,buf,sizeof(buf));
    while (1)
	{
		int bytes_read = recv(sock, buf , sizeof(buf),0);
		if(bytes_read<=0) break;
		printf("%s",buf);
	}
	printf("end\n");
	close(sock);
}
