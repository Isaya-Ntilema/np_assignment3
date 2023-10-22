#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <regex.h>

#include <netdb.h>
#include <strings.h> 

#define LENGTH 2048
#define LENGTH2 12
#define DEBUG

//Initializing global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];
char username[2048];
 
void Overwrite_STDOUT() 
{
  fflush(stdout);
}
 
//sending out messages of not more than 256 characters
void Send_Message() 
{
  char message[2048] = {};
	char buffer[LENGTH + 32] = {};

  while(1) 
  {
  Overwrite_STDOUT();
  fgets(message, LENGTH, stdin);
  //check the message content and count the characters
  int charnum = strlen(message)-1;
    //The message should not exceed 256 
    if (charnum>255) 
    {
      printf("Maximum charachters reached you have:%d. resent shorter message\n",charnum);
      fflush(stdout);
    }
    else
    {
        //check protocol and send protocol to the server
        char Protocol_Msg[LENGTH]="MSG ";
        strcat(Protocol_Msg,message);
        strcat(Protocol_Msg,"\n");
        send(sockfd, Protocol_Msg, strlen(Protocol_Msg), 0);
        //clearing buffer
        bzero(message, LENGTH);
        bzero(buffer, LENGTH + 32);
    }
  }

}

// Receiving messages from server and splitting message into lines

void Recv_Message()
{
    char message[LENGTH] = {};
    
    while (1) 
    {
        // Receiving message from the server
        int receive = recv(sockfd, message, LENGTH, 0);
              //working on the protocol, username and message 
        char get_Protocol[LENGTH];
        char getUserName[LENGTH];
        char getMessage[LENGTH];

        if (receive > 0) 
        {
            char *line = strtok(message, "\n"); // Split message into lines

            while (line != NULL) 
            {
                // Process each line
                int rv = sscanf(line, "%s %s %[^\n]", get_Protocol, getUserName, getMessage);
                char dest[LENGTH] = {0};
                memcpy(dest, getMessage, sizeof(getMessage));

                // Check the message prefix and compare it with the username
                if (strcmp(get_Protocol, "MSG") == 0) 
                {
                    if (strcmp(getUserName, username) != 0) 
                    {
                        printf("%s\n", dest); // Print each line
                        fflush(stdout);
                    }
                }

                // Get the next line
                line = strtok(NULL, "\n");
            }
        } else if (receive == 0) 
        {
            break;
        }

        memset(message, 0, sizeof(message));
    }
}

//main function 
int main(int argc, char *argv[])
{
//check CLI arguments 
	if (argc != 3) 
    {
        printf("Please provide exactly three arguments: IP:Port and a nickname/number.\n");
        fflush(stdout);
        exit(1); // Exit with a non-zero status code to indicate an error.
    } 
    else 
    {
        char *ipPortArg = argv[1];
        char *colon = strchr(ipPortArg, ':');
        if (colon == NULL) 
        {
            printf("Error: IP and Port must be separated by a colon (e.g., 127.0.0.1:8080).\n");
            fflush(stdout);
            exit(1); // Exit with an error status code.
        }
    }

// checking the  nicknames
  char *expression="^[A-Za-z0-9_]+$", *org ;
   regex_t regularexpression;
  int reti;
  int matches;
  regmatch_t items;

  reti=regcomp(&regularexpression, expression,REG_EXTENDED);
  if(reti)
  {
    printf("Could not compile regex.\n");
    fflush(stdout);
    exit(0);
  }

for (int i = 2; i < argc; i++) 
{
    if (strlen(argv[i]) <= 12) 
    { 
        reti = regexec(&regularexpression, argv[i], matches, &items, 0);
        if (!reti) 
        {
            // Clearing username, check for validity using regex
            bzero(username, LENGTH);
            strncpy(username, argv[i], LENGTH);
            #ifdef DEBUG
            printf("%s is valid User Name.\n", argv[i]);
            fflush(stdout);
            #endif
        } else 
        {
            // Error if the username is not valid
            printf("ERROR, %s is NOT valid User Name.\n", argv[i]);
            fflush(stdout);
            exit(1); // Exit with a non-zero status code to indicate an error
        }
    } else 
    {
        // The nickname should be less than or equal to 12 characters
        printf(" ERROR %s is too long (%zu vs 12 chars).\n", argv[i], strlen(argv[i]));
        fflush(stdout);
        exit(1); // Exit with a non-zero status code to indicate an error
    }
}

  regfree(&regularexpression);
  free(org);

//Declaration of variables 
  char *hoststring,*portstring, *rest;
  struct addrinfo hints;
  struct addrinfo* res=0;

//GET name and port from arguments
  org=strdup(argv[1]);
  rest=argv[1];
  hoststring=strtok_r(rest,":",&rest);
  portstring=strtok_r(rest,":",&rest);
  int port=atoi(portstring);
  printf("Connected to %s:%d \n",hoststring,port);
  fflush(stdout);

// get the address information from the server 
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_protocol=0;
	hints.ai_flags=AI_ADDRCONFIG;
	int err=getaddrinfo(hoststring,portstring,&hints,&res);
	if (err!=0) 
	{
    printf("ERROR in getaddrinfo\n");
    fflush(stdout);
    exit(0);
	}
// Creating the socket
	sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if (sockfd==-1) 
	{
		printf("ERROR in socket creation\n");
    fflush(stdout);
		exit(0);
	}
// Connecting to server
	if (connect(sockfd,res->ai_addr,res->ai_addrlen)==-1) 
	{
    printf("ERROR in SOCKET Connetion\n");
    fflush(stdout);
    exit(0);
	}
//Get the server Protocol
  char Protocol[LENGTH];
  bzero(Protocol, LENGTH);
  int n = read(sockfd, Protocol, sizeof(Protocol));
  if (n <= 0) 
  {
    perror("ERROR reading Server Protocol\n");
    fflush(stderr);	
    exit(0);	
  }
  printf("Serve Protocol: %s", Protocol);
  fflush(stdout);
//check the server protocol and proceed 
  if (strcmp(Protocol,"HELLO 1\n")==0)
  {
    char NICProtocolMsg[LENGTH]="NICK ";
    strcat(NICProtocolMsg,username);
    strcat(NICProtocolMsg,"\n");
    printf("sending NICK name to server:%s\n",NICProtocolMsg);
    fflush(stdout);

    #ifdef DEBUG
    printf("Protocol Supported, Sending Nick Name to server\n");
    fflush(stdout);
    #endif

    if(send(sockfd, NICProtocolMsg, strlen(NICProtocolMsg), 0)<=0)
    {
      perror("ERROR Sending UserName to server\n");
      fflush(stderr);
      exit(0);
    }
    else
    {
      char response[LENGTH]={0};
      int n = read(sockfd, response, sizeof(response));
      if (n <= 0) 
      {
        printf("ERROR reading Server response\n");	
        fflush(stdout);
        exit(0);	
      }
      else
      {
        #ifdef DEBUG
	printf("Server message : %s\n",response);
        fflush(stdout);
			  #endif

        if(strcmp(response,"OK\n")==0)
        {
            printf("NICKName Accepted \n",username);
            fflush(stdout);
        }
        else
        {
            printf("ERROR, NICKName NOT Accepted  \n",username);
            fflush(stdout);
            exit(0);
        }
      }
    }
  }
  else
  {
  printf("ERROR Server Protocl NOT support\n");
  fflush(stdout);
  exit(0);
  }
  
  printf("WELCOME THE CHAT \n");
  fflush(stdout);

   pthread_t send_thread;
  if(pthread_create(&send_thread, NULL, (void *) Send_Message, NULL) != 0)
  {
    printf("ERROR: pthread\n");
    fflush(stdout);
    return 1;
	}

   pthread_t receive_thread;
  if(pthread_create(&receive_thread, NULL, (void *) Recv_Message, NULL) != 0)
  {
    printf("ERROR: check on the pthread\n");
    fflush(stdout);
    return 1;
	}

while (1)
  {
		if(flag)
    {
			printf("\n BYE BYE, EXITING \n");
      fflush(stdout);
			break;
    }
	}
	    close(sockfd);
  return 0;
}
