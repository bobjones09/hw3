#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 32
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sd;
	FILE *fp;
    clock_t t;
	
	char file_name[BUF_SIZE];
	char buf[BUF_SIZE];
	int read_cnt;
	int read_size=0;
	struct sockaddr_in serv_adr;
    t = clock();
	if (argc != 4) {
		printf("Usage: %s <IP> <port> <file name> \n", argv[0]);
		exit(1);
	}

	strcpy(file_name, argv[3]);
	fp = fopen(argv[3], "rb");
	sd = socket(PF_INET, SOCK_STREAM, 0);   

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	connect(sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	
	//Send file name to server
	write(sd,file_name,BUF_SIZE);

	//Send file data
	while(1)
	{
		read_cnt = fread((void*)buf, 1, BUF_SIZE, fp);
		if (read_cnt < BUF_SIZE)
		{
			write(sd, buf, read_cnt);
			read_size += read_cnt;
			break;
		}
		read_size += BUF_SIZE; 
		write(sd, buf, BUF_SIZE);
	}
    t = clock() - t;
	double time_taken = ((double)t)/CLOCKS_PER_SEC; // calculate the elapsed time
    printf("Execution time: %f \n", time_taken);
    printf("Throughput: %f \n", read_size/time_taken);

	fclose(fp);
	close(sd);
	return 0;
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}