//Konrad Werbliński 291878

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <assert.h>
#include <strings.h>
#include "receive.h"
#include "send.h"

bool is_valid_id(char *s)
{
    struct sockaddr_in recipient;
    return inet_pton(AF_INET, s, &recipient.sin_addr) != 0;
}

int main(int argc, char *argv[])
{  
    if(argc < 2)
    {
        puts("Provide ip address!");
        return EXIT_SUCCESS;
    }
    
    if(!is_valid_id(argv[1]))
    {
        puts("Provide valid ip address!");
        return EXIT_SUCCESS;
    }
    
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        fprintf(stderr, "socket error: %s\n", strerror(errno)); 
        return EXIT_FAILURE;
    }
    
    int status = DEST_NOT_REACHED;
    
    for(int i = 1; i <= 30 && status == DEST_NOT_REACHED; i++)
    {
        if(send_icmp(sockfd, i, argv[1]) == SEND_ERROR)
            fprintf(stderr, "error while sending: %s\n", strerror(errno)); 
            
        status = receive(sockfd, i);
        
        if(status == RECEIVE_ERROR)
            fprintf(stderr, "error while receiving: %s\n", strerror(errno)); 
    }
    
    return EXIT_SUCCESS;
}
