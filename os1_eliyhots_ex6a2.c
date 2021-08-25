/*
*  ex6a2.c
*
*  Created on: Dec 15, 2017
*      Author: eliyho tsuri
*       Login: eliyhots
*		    Id: 201610672
*
*  Application server:
*
*  Application server enables testing of a number of preliminary
*  or test if a string is a Palindrome is the server accepts the
*  order from the client
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
#define REG_PORT "17171"
#define MY_PORT "17172"
#define MAX_TEXT 50
#define PRIME "The number is prime "
#define NOT_PRIME "The number is not prime "
#define PALINDROME "The string is Palindrome "
#define NOT_PALINDROME "The string is not Palindrome "
// ---------------------- Prototype --------------------------
int is_prime(int number);
int is_palindrome(char string[]);
// ------------------------ Main -----------------------------
int main(int argc, char *argv[])
{
	struct addrinfo con_kind, *addr_info_res;
	int rc, main_socket, serving_socket;
	char buf[MAX_TEXT];
	int num, answer;
	int for_write;

	if (argc < 2)
	{
		fprintf(stderr, "Usage:%s <host name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	memset(&con_kind, 0, sizeof con_kind);
	con_kind.ai_family = AF_UNSPEC;
	con_kind.ai_socktype = SOCK_STREAM;
	con_kind.ai_flags = AI_PASSIVE;

	if ((rc = getaddrinfo(argv[1], MY_PORT
		, &con_kind, &addr_info_res)) != 0)

	{
		fprintf(stderr, "getaddrinfo() failed %s\n", gai_strerror(rc));
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

	rc = bind(main_socket, addr_info_res->ai_addr,
		addr_info_res->ai_addrlen);
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
	serving_socket = accept(main_socket, NULL, NULL);
	if (serving_socket < 0)
	{
		perror("accept failed");
		exit(EXIT_FAILURE);
	}
	while (1)
	{
		while ((rc = read(serving_socket, buf, MAX_TEXT)) > 0)
		{
			char command = buf[0];
			if (command == 'n')
			{
				num = atoi(buf);
				answer = is_prime(num);
				if (answer == 1)
					for_write = (write(serving_socket, PRIME, strlen(PRIME) + 1));
				else
					for_write = (write(serving_socket, NOT_PRIME, strlen(NOT_PRIME) + 1));
			}
			else
			{
				answer = is_palindrome(buf);
				if (answer == 1)
					for_write = (write(serving_socket, PALINDROME, strlen(PALINDROME) + 1));
				else
					for_write = (write(serving_socket, NOT_PALINDROME, strlen(NOT_PALINDROME) + 1));
			}
			if (for_write < 0)
			{
				perror("read() failed");
				exit(EXIT_FAILURE);
			}
		}
	}
	if (rc < 0)
	{
		perror("read failed");
		exit(EXIT_FAILURE);
	}
	close(serving_socket);
	return EXIT_SUCCESS;
}
//----------------------- is_prime ---------------------------
/*
* The function takes a number and checks whether it is prime.
* returns 1 or 0 respectively to the number if it is preliminary
*/
int is_prime(int number)
{
	int div;
	if (number == 1)
		return 0;
	for (div = 2; div <= number / 2; div++)
		if (number % div == 0)
			return 0;
	return 1;
}
//------------------- check_palindrome -----------------------
/*
* The function takes a string and checks the string if
* is a palindrome, returns 1 or 0 if the string is palindrome
*/
int is_palindrome(char string[])
{
	int start = 0;
	int end = strlen(string);

	while (start < end)
	{
		if (string[start] != string[end])
			return 0;
		start++;
		end--;
	}
	return 1;
}
