/*
* ex6a3.c
*
*  Created on: Dec 15, 2017
*      Author: eliyho tsuri
*       Login: eliyhots
*		    Id: 201610672
*
*  Client processes:
*
*  The client connects to the server through system when want to
*  do something that is a prime number checking Or check string if
*  Palindrome, he turns to the application server and waiting for
*  him to finally answer the customer print the answer.
*/
// ------------------- Include Section -----------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
// ----------------------- Define ----------------------------
#define  BUFLEN 50
#define  REG_PORT  "17171"
#define  APP_PORT  "17172"
// ---------------------- Prototype --------------------------
void get_command(char rbuf[], char *command, int *index);
// ------------------------ Main -----------------------------
int main(int argc, char *argv[])
{
	int reg_socket, app_socket;
	int rc_reg, rc_app;
	int id_answer;
	char buf[BUFLEN + 1], command;
	struct addrinfo con_kind_reg, *addr_info_reg;
	struct addrinfo con_kind_app, *addr_info_app;


	if (argc < 2)
	{
		fprintf(stderr, "Usage:%s <host name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}



	memset(&con_kind_reg, 0, sizeof(con_kind_reg));
	con_kind_reg.ai_family = AF_UNSPEC;
	con_kind_reg.ai_socktype = SOCK_STREAM;

	if ((rc_reg = getaddrinfo(argv[1], argv[2], &con_kind_reg, &addr_info_reg) != 0))
	{
		fprintf(stderr, "(getaddrinfo() failed %s\n", gai_strerror(rc_reg));
		exit(EXIT_FAILURE);
	}
	reg_socket = socket(addr_info_reg->ai_family,
		addr_info_reg->ai_socktype,
		addr_info_reg->ai_protocol);

	if (reg_socket < 0)
	{
		perror("socket: allocation failed");
		exit(EXIT_FAILURE);
	}

	rc_reg = connect(reg_socket, addr_info_reg->ai_addr,
		addr_info_reg->ai_addrlen);
	if (rc_reg)
	{
		perror("connect failed");
		exit(EXIT_FAILURE);
	}

	id_answer = getpid();

	sprintf(buf, "%d", id_answer);
	if ((write(reg_socket, buf, strlen(buf) + 1) < 0))
	{
		perror("write failed");
		exit(EXIT_FAILURE);
	}
	rc_reg = read(reg_socket, buf, BUFLEN + 1);
	id_answer = atoi(buf);

	if (rc_reg > 0)
		printf("%s\n", buf);
	else
	{
		perror("read() failed");
		exit(EXIT_FAILURE);
	}

	if (id_answer == 0)
	{
		memset(&con_kind_app, 0, sizeof(con_kind_app));
		con_kind_app.ai_family = AF_UNSPEC;
		con_kind_app.ai_socktype = SOCK_STREAM;

		if ((rc_app = getaddrinfo("10.3.10.26", APP_PORT, &con_kind_app, &addr_info_app) != 0))
		{
			fprintf(stderr, "(getaddrinfo() failed %s\n", gai_strerror(rc_app));
			exit(EXIT_FAILURE);
		}
		app_socket = socket(addr_info_app->ai_family,
			addr_info_app->ai_socktype,
			addr_info_app->ai_protocol);

		if (app_socket < 0)
		{
			perror("socket: allocation failed");
			exit(EXIT_FAILURE);
		}

		rc_app = connect(app_socket, addr_info_app->ai_addr,
			addr_info_app->ai_addrlen);
		if (rc_app)
		{
			perror("connect failed");
			exit(EXIT_FAILURE);
		}
		int index = 2;
		while (1)
		{
			get_command(buf, &command, &index);
			if (command == 'e')
				break;
			sprintf(buf, "%c", command);

			if (write(app_socket, buf, strlen(buf) + 1) < 0)
			{
				perror("write() failed");
				exit(EXIT_FAILURE);
			}
			if ((read(app_socket, buf, BUFLEN)) < 0)
			{
				perror("read() failed");
				exit(EXIT_FAILURE);
			}
			if (index % 2 == 0)
				printf("%s\n", buf);
			index++;
		}
	}
	else
	{
		puts("Unable to register on registration server\n");
		exit(EXIT_FAILURE);
	}
	close(reg_socket);
	close(app_socket);
	freeaddrinfo(addr_info_reg);
	freeaddrinfo(addr_info_app);
	return EXIT_SUCCESS;
}
//----------------------- get_command ------------------------
/*
* The function takes the structure with which the client
* communicates with the servers, The function reads the
* command the user wants to ask and the number or string
*/
void get_command(char rbuf[], char *command, int *index)
{
	int num;
	char text[BUFLEN];
	if (*index % 2 == 0)
	{
		printf("Enter Command: 'n', 's', 'e' to end\n");
		scanf("%c", &*command);

		if (*command == 'n')
		{
			puts("Enter number");
			scanf("%d", &num);
			sprintf(rbuf, "%d", num);
		}
		else if (*command == 's')
		{
			puts("Enter string");
			scanf("%s", &text[BUFLEN]);
			sprintf(rbuf, "%s", text);
		}
	}
}
