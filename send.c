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
#include "send.h"

static u_int16_t compute_icmp_checksum (const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

int send_icmp(int sockfd, int ttl, char *address)
{
    for (int i = 0; i < 3; i++) 
    {
        struct icmphdr icmp_header;
        
        icmp_header.type = ICMP_ECHO;
        icmp_header.code = 0;
        icmp_header.un.echo.id = getpid();
        icmp_header.un.echo.sequence = ttl * 3 + i;
        icmp_header.checksum = 0;
        icmp_header.checksum = compute_icmp_checksum((u_int16_t*) &icmp_header, sizeof(icmp_header));

        struct sockaddr_in recipient;
        bzero(&recipient, sizeof(recipient));
        recipient.sin_family = AF_INET;
        
        inet_pton(AF_INET, address, &recipient.sin_addr);

        if(setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) != 0)
            return SEND_ERROR;
            
        if(sendto(sockfd, &icmp_header, sizeof(icmp_header), 0, (struct sockaddr *)&recipient, sizeof(recipient)) != sizeof(icmp_header))
            return SEND_ERROR;
    }
    
    return SEND_SUCCESS;
}   
