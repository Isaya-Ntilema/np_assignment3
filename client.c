#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <errno.h>

#define DEBUG 1
#define MAX_TOKEN 3
#define MAX_RES_LEN 100
#define MAXDATASIZE 1024
#define BUFSIZE 1024

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void send_recv(int i, int sockfd)
{
	char send_buf[BUFSIZE];
	char recv_buf[BUFSIZE];
	int nbyte_recvd;
        char str[1024];

	if (i == 0){
		fgets(send_buf, BUFSIZE, stdin);
                strcpy (str,"MSG ");
                strcat (str, send_buf) ;
		if (strcmp(send_buf , "quit\n") == 0) {
			exit(0);
		}else
			send(sockfd, str, strlen(str), 0);
	}else {
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = 0;
		printf("%s\n" , recv_buf);
		fflush(stdout);
	}
}


int main(int argc,char * argv[])
{
	int  fdmax, i;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;
        int sockfd = 0, n = 0;
        char recvBuff[1024];
        struct addrinfo serv_addr;
        char const *supported_prot = "1";
        char result[MAX_RES_LEN];
        struct addrinfo hints, *res, *p;
        struct sockaddr_in sin;
        socklen_t len;
        int status;
        char myIP[16];
        char s[INET6_ADDRSTRLEN];
        char buf[MAXDATASIZE];
        int numbytes;

//   Checking the number of argument

  if (argc != 3)
  {
    fprintf(stderr, "usage: showip hostname\n");
    return 1;
  }


char const *name = argv[2];
printf("the name is %s\n",name);
int count;
for(int i = 0; i < strlen(name); i++) {
        if(name[i] != ' ')
            count++;
    }

//printf("Total number of characters in a string: %d", count); 
if (count > 12){ 
printf("The name  must be lower that 12\n");
exit(0);
}


  // Separating port and IP address
  char delim[] = ":";
  char *Desthost = strtok(argv[1], delim);
  char *Destport = strtok(NULL, delim);

  /* Converting string to int*/
  int port = atoi(Destport);
  printf("Host %s, and port %d.\n", Desthost, port);
  //Clearing hints
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
  hints.ai_socktype = SOCK_STREAM;


if ((status = getaddrinfo(Desthost, Destport, &hints, &res)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    printf("status %d", status);
    return 2;
  }
for (p = res; p != NULL; p = p->ai_next)
  {
    /* Create the socket */
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("socket");
      continue;
    }

   break;
  }

if (p == NULL) {
                fprintf(stderr, "talker: failed to create socket\n");
                return 2;
               }

  freeaddrinfo(res);

memcpy(&serv_addr, res->ai_addr, sizeof(serv_addr));

if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\n Error : Connect Failed \n");
    return 1;
  }

inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                  s, sizeof s);
//        printf("client: connecting to %s\n", s);

memset(recvBuff, 0, sizeof(recvBuff));

if ((numbytes = recv(sockfd, recvBuff, sizeof(recvBuff), 0)) == -1) {
          perror("recv");
          exit(1);
        }

printf("client: received %s\n",recvBuff);

char *found_substr = strstr(recvBuff, supported_prot);

//if (!strcmp(recvBuff, "Hello 1"))
if (found_substr != NULL)
{
printf("the protocol is supported\n");
char respo[1024];
strcpy (respo,"NICK ");
strcat (respo, argv[2]) ;

if (numbytes = send(sockfd,respo,sizeof(respo),0) == -1) {
            perror("sendto:");
            exit(1);
       }


memset(recvBuff, 0, sizeof(recvBuff));

if ((numbytes = recv(sockfd, recvBuff, sizeof(recvBuff), 0)) == -1) {
          perror("recv");
          exit(1);
        }

printf("client: received %s \n",recvBuff);


}else
{
printf("it is not ok\n");
}

	FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(0, &master);
        FD_SET(sockfd, &master);
	fdmax = sockfd;

	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}

		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds))
				send_recv(i, sockfd);
	}
	printf("client-quited\n");
	close(sockfd);
	return 0;
}
