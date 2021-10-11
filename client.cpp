#include <stdio.h>		/* printf, sprintf */
#include <stdlib.h>		/* exit, atoi, malloc, free */
#include <unistd.h>		/* read, write, close */
#include <string.h>		/* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>		/* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.hpp"
#include "requests.hpp"
#include "json.hpp"
#include <iostream>

using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[])
{
	char *message; //mesajul trimis
	char *response; //raspunsul de la server
	int sockfd; //socket-ul pentru comunicarea cu server-ul
	char input[200]; //input de la tastatura
	char *cookie_saved; //salvare cookie
	char *token_saved; //salvare token library
	char host[15];
	sprintf(host, "34.118.48.238"); //salvare host

	cookie_saved = NULL;
	token_saved = NULL;

	// se asteapta input de la client
	while (1)
	{
		fgets(input, 200, stdin); //se citeste input-ul

		// pentru comanda de exit, se opreste programul
		if (!strncmp(input, "exit", 4))
		{
			break;
		}

		// pentru comanda de register, se citeste
		if (!strncmp(input, "register", 8))
		{
			char username[200];
			char pass[200];

			// se citesc datele de inregistrare
			// se elimina newline-ul din datele citite
			cout << "username=";
			fgets(username, 200, stdin);
			username[strlen(username) - 1] = '\0';
			cout << "password=";
			fgets(pass, 200, stdin);
			pass[strlen(pass) - 1] = '\0';

			// se deschide o conexiune cu server-ul
			sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

			// se creaza obiectul de tip json pentru trimiterea datelor
			json j = {
				{"username", username},
				{"password", pass}};

			char url[50];
			sprintf(url, "/api/v1/tema/auth/register");

			char content_type[50];
			sprintf(content_type, "application/json");

			// se compune mesajul pentru server
			message = compute_post_request(host, url, content_type, j, NULL);

			// se trimite mesajul la server
			// se primeste rasspunsul
			// raspunsul este afisat
			// se inchide conexiunea la server
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			puts(response);
			close_connection(sockfd);
		}

		// pentru comanda de login
		if (!strncmp(input, "login", 5))
		{
			char username[200];
			char pass[200];

			// se citesc datele de login
			// se elimina newline-ul de la final
			cout << "username=";
			fgets(username, 200, stdin);
			username[strlen(username) - 1] = '\0';
			cout << "password=";
			fgets(pass, 200, stdin);
			pass[strlen(pass) - 1] = '\0';

			// se deschide conexiune la server
			sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

			// se creazaz obiectul json pentru transmiterea datelor
			json j = {
				{"username", username},
				{"password", pass}};

			char url[50];
			sprintf(url, "/api/v1/tema/auth/login");

			char content_type[50];
			sprintf(content_type, "application/json");

			// se compune mesajul pentru server
			message = compute_post_request(host, url, content_type, j, NULL);
			// se trimite mesajul
			// se afiseazaz raspunsul
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			puts(response);

			// daca nu au aparut probleme, sunt retinute
			// cookie-urile pentru utilizator
			char *addr_cookie = strstr(response, "Set-Cookie: ");
			if (addr_cookie != NULL)
			{
				strtok(addr_cookie, " ;");
				char *token = strtok(NULL, " ;");

				cookie_saved = (char *)malloc(200 * sizeof(char));

				strcpy(cookie_saved, token);
			}

			// se inchide conexiunea la server
			close_connection(sockfd);
		}

		// pentru comanda de intrat in biblioteca
		if (!strncmp(input, "enter_library", 13))
		{
			// se deschide comunicatia cu server-ul
			sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

			char url[50];
			sprintf(url, "/api/v1/tema/library/access");

			// se creaza mesajul pentru server
			message = compute_get_request(host,url,NULL,&cookie_saved,1,NULL);

			// se trimite mesjaul
			// se afiseaza raspunsul de la server
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			puts(response);

			// daca nu au aparut probleme, este retinut token-ul
			// pentru utilizator
			char *addr_token = strstr(response, "token");
			if (addr_token != NULL)
			{
				strtok(addr_token, "\":");
				char *token = strtok(NULL, "\":");

				token_saved = (char *)malloc(500 * sizeof(char));

				strcpy(token_saved, token);
			}

			// se inchide conexiunea la server
			close_connection(sockfd);
		}

		// pentru comanda de vizualizare a cartilor
		if (!strncmp(input, "get_books", 9))
		{
			// se deschide conexiune la server
			sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

			char url[50];
			sprintf(url, "/api/v1/tema/library/books");

			// se creaza mesajul
			message = compute_get_request(host, url, NULL, NULL, 0, token_saved);

			// se trimite mesajul
			// se afiseaza raspunsul de la server
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			puts(response);

			// se inchide conexiunea la server
			close_connection(sockfd);
			continue;
		}

		// pentru comanda de vizualizare a unei carti
		if (!strncmp(input, "get_book", 8))
		{
			// se citeste id-ul cartii
			char id[10];
			cout << "id=";
			fgets(id, 200, stdin);
			id[strlen(id) - 1] = '\0';

			char url[50];
			sprintf(url, "/api/v1/tema/library/books/%s", id);

			// se deschide conexiunea cu server-ul
			sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

			// se creaza mesajul pentru server
			message = compute_get_request(host, url, NULL, NULL, 0, token_saved);

			// se trimite mesajul
			// se afiseaza raspunsul
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			puts(response);

			// se inchide conexiunea la server
			close_connection(sockfd);
		}

		// pentru comanda de adaugat o carte
		if (!strncmp(input, "add_book", 8))
		{
			char title[200];
			char author[200];
			char genre[200];
			char publisher[200];
			char page_count[200];

			// se citesc datele despre carte
			cout << "title=";
			fgets(title, 200, stdin);
			title[strlen(title) - 1] = '\0';
			cout << "author=";
			fgets(author, 200, stdin);
			author[strlen(author) - 1] = '\0';
			cout << "genre=";
			fgets(genre, 200, stdin);
			genre[strlen(genre) - 1] = '\0';
			cout << "publisher=";
			fgets(publisher, 200, stdin);
			publisher[strlen(publisher) - 1] = '\0';
			cout << "page_count=";
			fgets(page_count, 200, stdin);
			page_count[strlen(page_count) - 1] = '\0';

			// se deschide conexiunea la server
			sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

			// se creaza obiectul json pentru transmiterea datelor
			json j = {
				{"title", title},
				{"author", author},
				{"genre", genre},
				{"page_count", page_count},
				{"publisher", publisher}};

			char url[50];
			sprintf(url, "/api/v1/tema/library/books");

			char content_type[50];
			sprintf(content_type, "application/json");

			// se creaza mesajul pentru server
			message = compute_post_request(host,url,content_type,j,token_saved);

			// se trimite mesajul
			// se afiseaza raspunsul
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			puts(response);

			// se inchide conexiunea cu server-ul
			close_connection(sockfd);
		}

		// pentru comanda de sters o carte din biblioteca
		if (!strncmp(input, "delete_book", 11))
		{
			// se citeste id-ul cartii
			char id[10];
			cout << "id=";
			fgets(id, 200, stdin);
			id[strlen(id) - 1] = '\0';
			char url[50];
			sprintf(url, "/api/v1/tema/library/books/%s", id);

			// se deschide conexiunea la server
			sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

			// se creaza mesajul pentru server
			message = compute_delete_request(host, url, token_saved);

			// se trimie mesajul
			// se afiseaza raspunsul
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			puts(response);

			// se inchide conexiunea
			close_connection(sockfd);
		}

		// pentru comanda de logout
		if (!strncmp(input, "logout", 6))
		{
			// se deschide conexiunea la server
			sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

			char url[50];
			sprintf(url, "/api/v1/tema/auth/logout");

			// se creaza mesajul pentru server
			message = compute_get_request(host,url,NULL,&cookie_saved,1,NULL);

			// se sterg datele pentru clientul actual
			free(cookie_saved);
			cookie_saved = NULL;
			free(token_saved);
			token_saved = NULL;

			// se trimite mesajul
			// se afiseaza raspunsul
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			puts(response);

			// se inchide conexiunea
			close_connection(sockfd);
		}
	}

	return 0;
}
