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

int receive(int sockfd, int iteration)
{
    struct sockaddr_in 	sender;	
    socklen_t 			sender_len = sizeof(sender);
    u_int8_t 			buffer[IP_MAXPACKET];
    
    ssize_t packet_len;
    
    char sender_ip_addresses[3][20];
    sender_ip_addresses[0][0] = sender_ip_addresses[1][0] = sender_ip_addresses[2][0] = 0;
    
    char received[3] = {false, false, false};
    bool dest_reached = false;
    int times = 0;
    
    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET (sockfd, &descriptors);
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    
    while((tv.tv_sec != 0 || tv.tv_usec != 0) && !(received[0] && received[1] && received[2]))
    {
        if(select(sockfd + 1, &descriptors, NULL, NULL, &tv) == -1)
            return RECEIVE_ERROR;

        while ((packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len)) > 0) 
        {
            char sender_ip_str[20]; 
            inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
            struct icmphdr *icmp_header = (struct icmphdr *)(buffer + packet_len - 8);
                      
            if((int)icmp_header->un.echo.id == getpid() && (int)icmp_header->un.echo.sequence / 3 == iteration)
            {
                strcpy(sender_ip_addresses[(int)icmp_header->un.echo.sequence % 3], sender_ip_str);
                received[(int)icmp_header->un.echo.sequence % 3] = true;
                
                if(icmp_header->type == ICMP_ECHOREPLY)
                    dest_reached = true;
                times += 1000000 - tv.tv_usec;
            }
        }
       
        if(errno != EWOULDBLOCK)
            return RECEIVE_ERROR;
    }
    
    qsort(sender_ip_addresses, 3, 20, (int (*)(const void *, const void *))strcmp);
    
    if(received[0] || received[1] || received[2])
    {
        if (strcmp(sender_ip_addresses[1], sender_ip_addresses[2]) == 0) 
            sender_ip_addresses[2][0] = 0;
        if (strcmp(sender_ip_addresses[0], sender_ip_addresses[1]) == 0)
            sender_ip_addresses[1][0] = 0;
    
        qsort(sender_ip_addresses, 3, 20, (int (*)(const void *, const void *))strcmp);
        if(received[0] && received[1] && received[2])
            printf("%2.d. %16s %16s %16s %fms\n", iteration, sender_ip_addresses[2], sender_ip_addresses[1], sender_ip_addresses[0], times / 3000.0);
        else
            printf("%2.d. %16s %16s %16s ???\n", iteration, sender_ip_addresses[2], sender_ip_addresses[1], sender_ip_addresses[0]);
    }
    else
        printf("%2.d. *\n", iteration);
    
    return dest_reached ? DEST_REACHED : DEST_NOT_REACHED;
}
