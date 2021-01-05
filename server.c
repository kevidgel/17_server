#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>

int main();

static void sighandler(int signo)
{
    if (signo == SIGPIPE)
    {
        printf("Client disconnected.\n");
        main();
    }
    if (signo == SIGINT)
    {
        printf("\nExited\n");
        exit(0);   
    }
}

char *process(char *str)
{
    char *out = malloc((strlen(str)) * sizeof(char)); 
    int i = 0; 
    while(*str)
    {
        if(*str <= 'z' && *str >= 'a') out[i] = (((*str - 'a') + 13) % 26) + 'a';
        else if(*str <= 'Z' && *str >= 'A') out[i] = (((*str - 'A') + 13) % 26) + 'A';
        else out[i] = *str;  
        i++;
        str++;
    } 
    out[i] = '\0';
    return out;
}

int main()
{
    signal(SIGINT, sighandler);
    signal(SIGPIPE, sighandler);
    // creates well known pipe
    mkfifo("pringles", 0666); 

    // opens wkp
    int server = open("pringles", O_RDONLY); 
    if (server == -1) {
        printf("errno: %d\terror: %s\n", errno, strerror(errno));
        return -1; 
    }

    char pid[100];

    // finds handshakes
    int r = read(server, pid, sizeof(pid));
    if (r == -1) {
        printf("errno: %d\terror: char %s\n", errno, strerror(errno));
    }
    printf("Message received from %s\nSending message to client...\n", pid);

    // removes wk pipe
    remove("pringles");

    // sends message to client
    int client = open(pid, O_WRONLY);
    if (client == -1) {
        printf("errno: %d\terror: char %s\n", errno, strerror(errno));
        return -1;
    }
    char ack[] = "hello";
    int w = write(client, ack, sizeof(ack));
    if (w == -1) {
        printf("errno: %d\terror: char %s\n", errno, strerror(errno));
        return -1;
    } 

    char ack2[100];
    // receives message from server
    int r2 = read(server, ack2, sizeof(ack2));
    if (r2 == -1) {
        printf("errno: %d\terror: char %s\n", errno, strerror(errno));
        return -1;
    } 
    printf("Message received from client. Handshake complete.\n");

    char userinput[100];
    while(1)
    {
        // reads from console
        int r = read(server, userinput, sizeof(userinput));
        if (r == -1) {
            printf("errno: %d\terror: char %s\n", errno, strerror(errno));
            break;
        } 

        char *processed = process(userinput);

        // sends to console
        int w = write(client, processed, strlen(processed) + 1); 
        if (w == -1) {
            printf("errno: %d\terror: %s\n", errno, strerror(errno));
            break;
        }

        free(processed);
    }
}