
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <netdb.h>

#define BUFSIZE 1024
#define MAX_USER 100

struct ChatUser
{
  int socketNo;
  int port;
  char *userName;
  int removed;
};

struct ChatUser chatUsers[100] = {0};
int lastUserIndex = 10;


char *findUserbyPort(int port) // NOT REMOVED
{
  for (int i = 0; i < 10; i++)
  {
    struct ChatUser user = chatUsers[i];
    if (user.port == port)
      if (user.removed != 1)
        return user.userName;
  }
  printf("user not found");
  return NULL;
}



char *findUserbySocketNo(int socketNo) // NOT REMOVED
{
  for (int i = 0; i < 10; i++)
  {
   printf("the USR0 %s",chatUsers[0].userName);
  printf("the USR1 %s",chatUsers[1].userName);
printf("the USR2 %s",chatUsers[2].userName);
    struct ChatUser user = chatUsers[i];
    if (user.socketNo == socketNo)
    //  if (user.removed != 1)
        printf("the found user is  %s",user.userName);
        return user.userName;
  }
printf("user not found");
  return NULL;
}




void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void send_to_all(int j, int i, int sockfd, int nbytes_recvd, char *str, fd_set *master)
{
  nbytes_recvd = nbytes_recvd + 10;
  if (FD_ISSET(j, master))
  {
    if (j != sockfd && j != i)
    {
      if (send(j, str, nbytes_recvd, 0) == -1)
      {
        perror("send");
      }
    }
  }
}

void send_recv(int i, fd_set *master, int sockfd, int fdmax, char *usr)
{
  int nbytes_recvd, j;
  char recv_buf[BUFSIZE], buf[BUFSIZE];
  char str[2024];
  //        printf("the user is %s",usr);

  memset(recv_buf, 0, sizeof(recv_buf));
  if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0)
  {
    if (nbytes_recvd == 0)
    {
      printf("socket %d hung up\n", i);
    }
    else
    {
      perror("recv");
    }
    close(i);
    FD_CLR(i, master);
  }
  else
  {
   printf("the I is  %d\n",i);

printf("the USR0 %s\n",chatUsers[0].userName);
  printf("the USR1 %s\n",chatUsers[1].userName);
printf("the USR2 %s\n",chatUsers[2].userName);



//   char *nk  = findUserbySocketNo(i);
//   printf("the NK is %s, %d\n",nk,i);
   char respo[20] ={0};
  // strcpy (respo, nk);
//   strcat (recv_buf , nk);
    printf("%s\n", recv_buf);
    for (j = 0; j <= fdmax; j++)
    {
      send_to_all(j, i, sockfd, nbytes_recvd, recv_buf, master);
    }
  }
}

int main(int argc, char *argv[])
{
  fd_set master;
  fd_set read_fds;
  int fdmax, i,k=0;
  struct sockaddr_in my_addr;
  struct sockaddr_in *client_addr;
  struct sockaddr_in their_addr;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  int connfd;
  char recvBuff[1024];
  char sendBuff[1024];
  int sockfd; // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  socklen_t sin_size;
  int yes = 1;
  char s[INET6_ADDRSTRLEN];
  int rv;
  char const *reply = "ERROR\n";
  // Check number of argumnets
  if (argc != 2)
  {
    fprintf(stderr, "usage: showip hostname\n");
    return 1;
  }

  // Seperating IP and port
  char delim[] = ":";
  char *Desthost = strtok(argv[1], delim);
  char *Destport = strtok(NULL, delim);

  /* Do magic chage string to int*/
  int port = atoi(Destport);
  printf("Host %s, and port %d.\n", Desthost, port);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  // Getting address information
  if ((rv = getaddrinfo(Desthost, Destport, &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }


  // loop through all the results and bind to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
                         p->ai_protocol)) == -1)
    {
      perror("server: socket");
      continue;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1)
    {
      perror("setsockopt");
      exit(1);
    }
    // Binding start
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close(sockfd);
      perror("server: bind");
      exit(1);
    }

    break;
  }

  freeaddrinfo(servinfo); // all done with this structure

  if (p == NULL)
  {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  // Listening start
  if (listen(sockfd, 5) == -1)
  {
    perror("listen");
    close(sockfd);
    exit(1);
  }
  printf("\nCHAT Server Waiting for client on port %d\n", port);
  fflush(stdout);

  FD_SET(sockfd, &master);

  char usr[1024];
  fdmax = sockfd;
  while (1)
  {
    read_fds = master;
    if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
    {
      perror("select");
      exit(4);
    }

    for (i = 0; i <= fdmax; i++)
    {
      if (FD_ISSET(i, &read_fds))
      {
        if (i == sockfd)
        {

          socklen_t addrlen;
          int newsockfd, num;
          char sendBuff[1024];
          char recvBuff[1024];

          sin_size = sizeof(their_addr);

          addrlen = sizeof(struct sockaddr_in);
          if ((newsockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
          {
            perror("accept");
            exit(1);
          }
          else
          {

            printf("new connection from %s on port %d \n", inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port));

            memset(sendBuff, 0, sizeof(sendBuff));
            // Sending supported protocol
            strcpy(sendBuff, "HELLO 1\n");
            // Send Hello to client
            if (send(newsockfd, sendBuff, strlen(sendBuff), 0) < 0)
            {
              perror("send");
              /* close socket */
              exit(1);
            }
            // receive  Name from Client
            memset(recvBuff, 0, sizeof(recvBuff));
            if (recv(newsockfd, recvBuff, sizeof(recvBuff), 0) < 0)
            {
              perror("recv");
              /* close socket */
              exit(1);
            }

            snprintf(usr, sizeof(usr), "%s", recvBuff);

            char delim[] = "  ";
            char *nick = strtok(usr, delim);
            char *name = strtok(NULL, delim);

            printf("%s new user  is comming , k=  %d\n", name, k);

           chatUsers[k].userName = name ;
           chatUsers[k].socketNo = newsockfd;
           chatUsers[k].port = ntohs(their_addr.sin_port);
           chatUsers[k].removed = 0;
           
           printf("The username is %s %d %d %d\n ",chatUsers[k].userName, chatUsers[k].socketNo,chatUsers[k].port,k);
	   k++;
            memset(sendBuff, 0, sizeof(sendBuff));
            strcpy(sendBuff, "OK\n");


            if (send(newsockfd, sendBuff, strlen(sendBuff), 0) < 0)
            {
              perror("send");
              /* close socket */
              exit(1);
            }

            FD_SET(newsockfd, &master);
            if (newsockfd > fdmax)
            {
              fdmax = newsockfd;
            }
          }


        }
        else
        {
          //   printf("the I2 is %d\n",i);
          send_recv(i, &master, sockfd, fdmax, usr);
        }
      }
    }
  }
  return 0;
}
