/*Raw UDP Client*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <linux/sockios.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <asm/byteorder.h>

#ifndef MAC2STR
#define MAC2STR(a) (a)[0]&0xff,(a)[1]&0xff,(a)[2]&0xff,(a)[3]&0xff,(a)[4]&0xff,(a)[5]&0xff
#define MACSTR "%02X:%02X:%02X:%02X:%02X:%02X"
#endif

#define BROAD_IP "255.255.255.255"
//Define the interface name 
#define IFNAME "eth0"
#define BUFLEN 38
#define PORT 8999

typedef struct ip_hdr{//ipv4
#if defined(__LITTLE_ENDIAN_BITFIELD)
	unsigned int ip_length:4; /*little-endian*/
	unsigned int ip_version:4;
#elif defined(__BIG_ENDIAN_BITFIELD)
	unsigned int ip_version:4;
	unsigned int ip_length:4; /*little-endian*/
#else
#error	"Please fix <asm/byteorder.h>"
#endif
	unsigned char ip_tos;
	unsigned short ip_total_length;
	unsigned short ip_id;
	unsigned short ip_flags;
	unsigned char ip_ttl;
	unsigned char ip_protocol;
	unsigned short ip_cksum;
	unsigned int ip_source;
	unsigned int ip_dest;
}ip_hdr;
typedef struct udp_hdr{//UDP Hearder
	unsigned short s_port;
	unsigned short d_port;
	unsigned short length;
	unsigned short cksum;
}udp_hdr;

typedef struct psd_header{

	unsigned int s_ip;//source ip
	unsigned int d_ip;//dest ip
	unsigned char mbz;//0
	unsigned char proto;//proto type
	unsigned short plen;//length
}psd_header;

void swap(unsigned int *a, unsigned int *b)
{
	*a = (*a)^(*b);
	*b = (*a)^(*b);
	*a = (*a)^(*b);
}

unsigned short checksum(unsigned short* buffer, int size)
{
	unsigned long cksum = 0;
	while(size>1)
	{
		cksum += *buffer++;
		size -= sizeof(unsigned short);
	}
	if(size)
	{
		cksum += *(unsigned char*)buffer;
	}
	cksum = (cksum>>16) + (cksum&0xffff);
	cksum += (cksum>>16);

	return (unsigned short)(~cksum);
}

int main_TellIP(void)
{
	char buf[BUFLEN] = {0};
	int sockfd = -1;
	int sockfd2 = -1;
	int so_broadcast = 1;
	int i = 0;
	struct ifreq ifr;
	unsigned char macaddr[ETH_ALEN];
	struct sockaddr_in host_addr;
	struct sockaddr_in broadcast_addr;
	struct sockaddr_in local_addr;

	sockfd2 = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd2 < 0) {
		perror("Intenal Socket error");
		return -1;
	}
	strcpy(ifr.ifr_name, IFNAME);
	if(ioctl(sockfd2, SIOCGIFADDR, &ifr) == -1){
		perror("Intenal ioctl error");
		return -1;
	}

	memcpy(&local_addr, &ifr.ifr_addr, sizeof(struct sockaddr_in ));
	printf("Local IP address is: %s\n", inet_ntoa(local_addr.sin_addr));

	if(ioctl(sockfd2,SIOCGIFBRDADDR,&ifr) == -1){
		perror("Intenal ioctl error");
		return -1;
	}
	memcpy(&broadcast_addr, &ifr.ifr_broadaddr, sizeof(struct sockaddr_in ));
	printf("Local Broadcast address is: %s\n", inet_ntoa(broadcast_addr.sin_addr));

	if(ioctl(sockfd2,SIOCGIFHWADDR,&ifr) == -1){
		perror("Intenal ioctl error");
		return -1;
	}
	memcpy(macaddr, &ifr.ifr_hwaddr.sa_data, ETH_ALEN);
	printf("Local MAC is: "MACSTR"\n",MAC2STR(macaddr));

    if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP))<0)
    {
        printf("socket() error!\n");
	return -1;
    }

    memset(&host_addr, 0, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = inet_addr(inet_ntoa(broadcast_addr.sin_addr));

    const int on = 1;
    if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on))<0)
    {
        printf("setsockopt() error!\n");
	return -1;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof so_broadcast);
    int addr_len = sizeof(host_addr);

    i = 3 ;
    while(i--){
        ip_hdr *iphdr;
        iphdr				   = (ip_hdr*)buf;
        udp_hdr *udphdr;
        udphdr                 = (udp_hdr*)(buf + 20);
        iphdr->ip_length       = 5;
        iphdr->ip_version      = 4;
        iphdr->ip_tos          = 0;
        iphdr->ip_total_length = htons(sizeof(buf));
        iphdr->ip_id           = 0;
        iphdr->ip_flags        = htons(0x4000);
        iphdr->ip_ttl          = 0x40;
        iphdr->ip_protocol     = 0x11;
        iphdr->ip_cksum        = 0;
        iphdr->ip_source       = inet_addr(inet_ntoa(local_addr.sin_addr));

        iphdr->ip_dest  = inet_addr(BROAD_IP);
        iphdr->ip_cksum = checksum((unsigned short*)buf, 20);
        udphdr->s_port  = htons(80);
        udphdr->d_port  = htons(8999);
        udphdr->length  = htons(sizeof(buf) - 20);
        udphdr->cksum   = 0;
        psd_header psd;
        psd.s_ip  = iphdr->ip_source;
        psd.d_ip  = iphdr->ip_dest;
        psd.mbz   = 0;
        psd.proto = 0x11;
        psd.plen  = (udphdr->length);
        char tmp[sizeof(psd) + ntohs(udphdr->length)];
        memcpy(tmp, &psd, sizeof(psd));
        //Add the MAC to UDP data;
		memcpy(buf + 20 + sizeof(udphdr) + 4,macaddr, ETH_ALEN);
        //Add the IP to UDP data;
        memcpy(buf + 20 + sizeof(udphdr) + 4 + ETH_ALEN, &(broadcast_addr.sin_addr), 4);
        memcpy(tmp + sizeof(psd), buf + 20, sizeof(buf) - 20);
        udphdr->cksum = checksum((unsigned short*)tmp, sizeof(tmp));
        int res = sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&host_addr, sizeof(host_addr));
		printf("Send %d bytes.\n", res);
        sleep(1);
    }

    return 0;
}
