/*
* ex6a1.c
*
*  Created on: Dec 15, 2017
*      Author: eliyho tsuri
*       Login: eliyhots
*		  Id: 201610672
*
*  Registration server:
*  In shared memory that processes numbers plugged into the system.
*  The server produces queue for customers. Allocates memory to be
*  used by him and his partner application server, The server allows
*  two things add new process ,removes the existing process,
*  Now he is informed through Message Queuing to add process
*  checks whether the number of the process which exists in shared,
*  memory, If the process does not exist and there is space in the
*  data structure he adds the process, Answer is 0 or 1 when you
*  receive a prompt to uninstall process removes it.
*
*/
// ------------------- Include Section -----------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
// ----------------------- Define ----------------------------
#define  MY_PORT  "17171"
#define  APP_PORT  "17172"
#define  MAX_PROCESS 5
#define  MAX_LEN 50
#define  PROCESS_JOIN 0
#define  PROCESS_ON_ARR 1
#define  NOT_ENOUGH_PLACE 2
// ---------------------- Prototype --------------------------
int client_not_arr(pid_t arr_proc[], int indx_proc, pid_t pid);
// ------------------------ Main -----------------------------
int main()
{
	pid_t process[MAX_PROCESS], pid;
	int index = 0, answer;
	int   rc;
	int   main_socket;
	int   serving_socket;
	struct addrinfo con_kind, *addr_info_res;
	char buf[MAX_LEN];


	memset(&con_kind, 0, sizeof con_kind);
	con_kind.ai_family = AF_UNSPEC;
	con_kind.ai_socktype = SOCK_STREAM;
	con_kind.ai_flags = AI_PASSIVE;

	if ((rc = getaddrinfo("10.3.10.26", MY_PORT, &con_kind, &addr_info_res)) != 0)
	{
		fprintf(stderr, "(getaddrinfo() failed %s\n", gai_strerror(rc));
		exit(EXIT_FAILURE);
	}
	main_socket = socket(addr_info_res->ai_family,
		addr_info_res->ai_socktype,
		addr_info_res->ai_protocol);

	if (main_socket < 0)
	{
		perror("socket: allocation failed");
		exit(EXIT_FAILURE);
	}
	rc = bind(main_socket, addr_info_res->ai_addr, addr_info_res->ai_addrlen);
	if (rc)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	rc = listen(main_socket, 5);
	if (rc)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		serving_socket = accept(main_socket, NULL, NULL);
		if (serving_socket < 0)
		{
			perror("accept() failde\n");
			exit(EXIT_FAILURE);
		}

		while ((rc = read(serving_socket, buf, MAX_LEN)) > 0)
		{
			pid = atoi(buf);
			answer = client_not_arr(process, index, pid);
			if (answer == 0)
			{
				process[index] = pid;
				index++;
			}
			sprintf(buf, "%d", answer);
			write(serving_socket, buf, strlen(buf) + 1);
		}
		close(serving_socket);
	}
	freeaddrinfo(addr_info_res);
	return(EXIT_SUCCESS);
}

// ------------------ client_not_arr -------------------------
/*
* The function checks whether the number of the
* customer's process on the shared memory
*/
int client_not_arr(pid_t arr_proc[], int indx_proc, pid_t pid)
{
	int index;

	if (indx_proc == MAX_PROCESS)
		return 2;
	for (index = 0; index < MAX_PROCESS; index++)
		if (arr_proc[index] == pid)
			return 1;
	return 0;
}

