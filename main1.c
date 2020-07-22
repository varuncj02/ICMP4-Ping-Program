// C program to Implement Ping 

// compile as -o ping 
// run as sudo ./ping <hostname> 

#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h> 
#include <netinet/ip_icmp.h> 
#include <time.h> 
#include <fcntl.h> 
#include <signal.h> 
#include <time.h> 
#include <netinet/ip.h>

// Define the Packet Constants 
// ping packet size 
#define PING_PKT_S 64 

// Automatic port number 
#define PORT_NO 0 

// Automatic port number 
#define PING_SLEEP_RATE 100000 

// Gives the timeout delay for receiving packets 
// in seconds 
#define RECV_TIMEOUT 1 

// Define the Ping Loop 
int pingloop = 1;


// ping packet structure 
struct ping_pkt
{
	struct icmphdr hdr;
	char msg[PING_PKT_S - sizeof(struct icmphdr)];
};

// Calculating the Check Sum 
unsigned short checksum(void* b, int len)
{
	unsigned short* buf = b;
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}


// Interrupt handler 
void intHandler(int dummy)
{
	pingloop = 0;
}

// Performs a DNS lookup 
char* dns_lookup(char* addr_host, struct sockaddr_in* addr_con)
{
	printf("\nResolving DNS..\n");
	struct hostent* host_entity;
	char* ip = (char*)malloc(NI_MAXHOST * sizeof(char));
	int i;

	if ((host_entity = gethostbyname(addr_host)) == NULL)
	{
		// No ip found for hostname 
		return NULL;
	}

	//filling up address structure 
	strcpy(ip, inet_ntoa(*(struct in_addr*)
		host_entity->h_addr));

	(*addr_con).sin_family = host_entity->h_addrtype;
	(*addr_con).sin_port = htons(PORT_NO);
	(*addr_con).sin_addr.s_addr = *(long*)host_entity->h_addr;

	return ip;

}

// make a ping request 
void send_ping(int ping_sockfd, struct sockaddr_in* ping_addr,
	 char* ping_ip, char* rev_host)
{
	int ttl_val = 64, msg_count = 0, i, addr_len, flag = 1,
		msg_received_count = 0;

	struct ping_pkt pckt;
	struct sockaddr_in r_addr;
	struct timespec time_start, time_end, tfs, tfe;
	long double rtt_msec = 0, total_msec = 0;
	struct timeval tv_out;
	int failure_cnt = 0;
	int cnt;

	tv_out.tv_sec = RECV_TIMEOUT;
	tv_out.tv_usec = 0;

	clock_gettime(CLOCK_MONOTONIC, &tfs);


	// set socket options at ip to TTL and value to 64, 
	// change to what you want by setting ttl_val 
	if (setsockopt(ping_sockfd, SOL_IP, IP_TTL,
		&ttl_val, sizeof(ttl_val)) != 0)
	{
		printf("\nSetting socket options to TTL failed!\n");
		return;
	}

	
	else
	{
		printf("\nSocket set to TTL..\n");
	}

	// setting timeout of recv setting 
	setsockopt(ping_sockfd, SOL_SOCKET, SO_RCVTIMEO,
		(const char*)& tv_out, sizeof tv_out);

	printf("\nVarun1..\n");

	int counter = 0;
	// send icmp packet in an infinite loop 
	while (pingloop)
	{
		while (counter < 3) {
			// flag is whether packet was sent or not 
			flag = 1;

			//filling packet 
			bzero(&pckt, sizeof(pckt));

			pckt.hdr.type = ICMP_ECHO;
			pckt.hdr.un.echo.id = getpid();

			for (i = 0; i < sizeof(pckt.msg) - 1; i++)
				pckt.msg[i] = i + '0';

			pckt.msg[i] = 0;
			pckt.hdr.un.echo.sequence = msg_count++;
			pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
			printf("\nVarun2..\n");
			printf("Pcktheader %s", pckt.msg);
			printf("PING %s : %d data bytes\n", ping_ip, sizeof(pckt.msg));


			usleep(PING_SLEEP_RATE);

			//send packet 
			clock_gettime(CLOCK_MONOTONIC, &time_start);

	//		printf("\nVarun3..\n");

			if (sendto(ping_sockfd, &pckt, sizeof(pckt), 0,
				(struct sockaddr*) ping_addr,
				sizeof(*ping_addr)) <= 0)
			{
				printf("\nPacket Sending Failed!\n");
				flag = 0;
			}
		//	printf("\nVarun4..\n");

			//receive packet 
			addr_len = sizeof(r_addr);
			//printf("\nVarun5.1..\n");

			printf("Recv len is %d \n", addr_len);
		REC:
			cnt = recvfrom(ping_sockfd, &pckt, sizeof(pckt), 0,
				(struct sockaddr*) & r_addr, &addr_len);
			//printf("\nVarun5.2..\n");

			if (cnt <= 0)
			{
				printf("\nPacket receive failed!\n");
				failure_cnt++;
				if (failure_cnt > 5) {
					break;
				}
			}

			else
			{
				//printf("\nVarun7..\n");

				clock_gettime(CLOCK_MONOTONIC, &time_end);

				double timeElapsed = ((double)(time_end.tv_nsec -
					time_start.tv_nsec)) / 1000000.0;
				rtt_msec = (time_end.tv_sec -
					time_start.tv_sec) * 1000.0
					+ timeElapsed;
				//printf("\nVarun8..\n");
				// if packet was not sent, don't receive 
				if (flag)
				{
					struct icmp* icmp_hdr;
					printf(" count %d", cnt);

					//printf("\nVarun8.1..\n");


					if (cnt >= 60) { //76
						//printf("\nVarun9..\n");
						struct iphdr* iphdr = (struct iphdr*) & pckt;
						//printf("\nVarun8.2..\n");
						/* skip ip hdr */
						icmp_hdr = (struct icmp*) (((char*)& pckt) + (iphdr->ihl << 2));
						//printf("\nVarun8.3..\n");
					}
					//printf("\nVarun8.9..\n");

					printf("icmp type %d ", icmp_hdr->icmp_type);


					printf("ICMP_ECHO %d ", ICMP_ECHO);


					if (icmp_hdr->icmp_type == ICMP_ECHO) {
						//printf("\nVarun8.4..\n");
						goto REC;

					}
					//printf("\nVarun9..\n");

					printf("icmp type %d ", icmp_hdr->icmp_type);

					if ((icmp_hdr->icmp_type != ICMP_ECHOREPLY))
					{
						printf("Error..Packet received with ICMP"
							"type %d code %d\n",
							icmp_hdr->icmp_type, icmp_hdr->icmp_code);
					}
					else
					{
						printf("%d bytes from (h: %s)"
							"(%s) msg_seq=%d ttl=%d "
							"rtt = %Lf ms.\n",
							PING_PKT_S, rev_host,
							ping_ip, msg_count,
							ttl_val, rtt_msec);

						msg_received_count++;
					}
				}
			}
			counter++;
		}
	}
	//printf("\nVarun5..\n");

	clock_gettime(CLOCK_MONOTONIC, &tfe);
	double timeElapsed = ((double)(tfe.tv_nsec -
		tfs.tv_nsec)) / 1000000.0;

	total_msec = (tfe.tv_sec - tfs.tv_sec) * 1000.0 +
		timeElapsed;

	printf("\n===%s ping statistics===\n", ping_ip);
	printf("\n%d packets sent, %d packets received, %f percent "
		"packet loss. Total time: %Lf ms.\n\n",
		msg_count, msg_received_count,
		((msg_count - msg_received_count) / msg_count) * 100.0,
		total_msec);
}

// Driver Code 
int main(int argc, char* argv[])
{
	int sockfd;
	char* resolved_addr;
	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con);
	//char net_buf[NI_MAXHOST];

	if (argc != 2)
	{
		printf("\nFormat %s <address>\n", argv[0]);
		return 0;
	}

	resolved_addr = dns_lookup(argv[1], &addr_con);
	if (resolved_addr == NULL)
	{
		printf("\nDNS lookup failed! Could "
			"not resolve hostname!\n");
		return 0;
	}

	//reverse_hostname = reverse_dns_lookup(ip_addr);
	printf("\nTrying to connect to '%s' IP: %s\n",
		argv[1], resolved_addr);
/*	printf("\nReverse Lookup domain: %s",
		reverse_hostname);*/

	//socket() 
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		printf("\nSocket file descriptor not received!!\n");
		return 0;
	}
	else
		printf("\nSocket file descriptor %d received\n", sockfd);

	signal(SIGINT, intHandler);//catching interrupt 

	//send pings continuously 
	send_ping(sockfd, &addr_con, ip_addr, argv[1]);

	return 0;
}
