#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>

static void sighandler(int signo)
{
    if (signo == SIGINT)
    {
        printf("\nExited\n");
        exit(0);   
    }
}

int main()
{
    signal(SIGINT, sighandler);

    // creates private pipe
    char buffer[100]; 
    sprintf(buffer, "%d", getpid());
    mkfifo(buffer, 0666);

    // opens server, client
    int server = open("pringles", O_WRONLY);
    if (server == -1) {
        printf("errno: %d\terror: %s\n", errno, strerror(errno));
        return -1;
    }

    // sends message to server
    printf("Client sending message to server...\n");
    int message = write(server, buffer, strlen(buffer) + 1); 
    if (message == -1) {
        printf("errno: %d\terror: %s\n", errno, strerror(errno));
        return -1;
    }

    int client = open(buffer, O_RDONLY);
    if (client == -1) {
        printf("errno: %d\terror: %s\n", errno, strerror(errno));
        return -1;
    }

    char ack[100];
    // receives message from server
    int r = read(client, ack, sizeof(ack));
    if (r == -1) {
        printf("errno: %d\terror: char %s\n", errno, strerror(errno));
        return -1;
    } 
    printf("Message received from server. Handshake complete.\n");

    // sends message to server
    printf("Client sending message to server...\n");
    int message2 = write(server, buffer, strlen(buffer) + 1); 
    if (message2 == -1) {
        printf("errno: %d\terror: %s\n", errno, strerror(errno));
        return -1;
    }

    // removes private pipe
    remove(buffer);

    // in, out 
    char userinput[100], processed[100];
    while(1)
    {

        // gets user input
        printf("Input   : " );
        fgets(userinput, sizeof(userinput), stdin);
        if(isspace(userinput[strlen(userinput) - 1])) userinput[strlen(userinput) - 1] = '\0';

        // sends to server
        int w = write(server, &userinput, strlen(userinput) + 1); 
        if (w == -1) {
            printf("errno: %d\terror: %s\n", errno, strerror(errno));
            break;
        } 

        // reads from server
        int r = read(client, &processed, sizeof(processed));
        if (r == -1) {
            printf("errno: %d\terror: %s\n", errno, strerror(errno));
            break;
        } 

        // prints processed
        printf("Response: %s\n", processed);
        
    }

    

    return 0;
}