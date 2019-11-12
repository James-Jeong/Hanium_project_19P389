#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "Processing.h"

#define MAXCLIENT	10
#define PORT        8000
#define SERVER_PORT	5000

void* ClientHandler(void* arg){
	pthread_detach(pthread_self());
	
	FILE* fp;
	char buffer[BUFSIZE];
	char buffer_from_database[BUFSIZE];
	int client_sock = *((int*)arg);
	int recv_len;

	struct sockaddr_in server_addr;
	char buf_from_server[BUFSIZE];
	const char* saddr = "15.164.228.68";
	int s = 0; int n = 0;

	bzero((char*)(&server_addr), sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(saddr);
	server_addr.sin_port = htons(SERVER_PORT);

	while((recv_len = recv(client_sock, buffer, BUFSIZE - 1, 0)) > 0){
		buffer[recv_len] = 0;
		
		if((fp = fopen("server_log.out", "a")) == NULL){
			perror("{ Fail to open server_log.out }"); exit(0);
		}
		fputs("[ Order : TM | UV | CO2 | Air | ADR | AM ]\n", fp);
		fprintf(fp, "[ Server received data : %s ]\n", buffer);
		fclose(fp);
		

		if(buffer[0] == 'q'){
			if((fp = fopen("server_log.out", "a")) == NULL){
				perror("{ Fail to open server_log.out }"); exit(0);
			}
			fputs("[ Server receives QUIT message ]\n", fp);
			fclose(fp);
			if(send(client_sock, "quit_ack", strlen("quit_ack"), 0) < 0){
				perror("send error");
				continue;
			}
			close(client_sock);
			return (void*)(-1);
		}
		else{
			//char* tp = (char*)malloc(sizeof(char) * 100);
			//tp = (char*)Processing(buffer);

			// Send data to Database
			if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
				if((fp = fopen("server_log.out", "a")) == NULL){
					perror("{ Fail to open server_log.out }"); exit(0);
				}
				fputs("{ Rasp socket error! }\n", fp);
				fclose(fp);
				perror("{ Rasp socket error! }");
				continue;
			}
			
			if(connect(s, (struct sockaddr*)(&server_addr), sizeof(server_addr))){
				if((fp = fopen("server_log.out", "a")) == NULL){
					perror("{ Fail to open server_log.out }"); exit(0);
				}
				fputs("{ Rasp connection error with Database! }\n", fp);
				fclose(fp);
				perror("{ Rasp connection error with Database! }");
				continue;
			}
			
			if((fp = fopen("server_log.out", "a")) == NULL){
				perror("{ Fail to open server_log.out }"); exit(0);
			}
			fprintf(fp, "[ Server send data to Database : %s ]\n", buffer);
			fclose(fp);
			write(s, buffer, strlen(buffer));
			
			if((recv(s, buffer_from_database, BUFSIZE - 1, 0)) > 0){
				if((fp = fopen("server_log.out", "a")) == NULL){
				perror("{ Fail to open server_log.out }"); exit(0);
				}
				fprintf(fp, "[ Database send data to Server : %s ]\n", buffer_from_database);
				fclose(fp);
			}
			
			if (send(client_sock, buffer, strlen(buffer), 0) < 0){
				printf("send error\n");
				perror("send error ");
				continue;
			}
			close(s);
		}
	}

	close(client_sock);
	return (void*)NULL;
}



int main(){
	int serv_sock;
	int clnt_sock;
	int result;
	FILE* fp;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size = sizeof(clnt_addr);

	pthread_t t_id;
	
	if ((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket ");
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
		perror("bind ");
		return -1;
	}
	
	if (listen(serv_sock, MAXCLIENT) == -1){
		perror("listen ");
		return -1;
	}

	fprintf(stderr,"[ Waiting for Clients ]\n");
	if((fp = fopen("server_log.out", "a")) == NULL){
		perror("{ Fail to open server_log.out }"); exit(0);
	}
	fputs("[ Waiting for Clients ]\n", fp);
	fclose(fp);

	while(1){
		if ((clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size)) == -1){
			perror("accept ");
			return -1;
		}
		
		if (pthread_create(&t_id, NULL, ClientHandler, (void*)&clnt_sock) < 0){
			perror("pthread_create ");
			return -1;
		}

		pthread_join(t_id, (void*)&result);
		if((int)result == -1)
			break;
	}
	close(serv_sock);

	return 0;
}

