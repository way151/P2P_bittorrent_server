/* TCPClient.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#define	BUFLEN		8000000                  // ��������С
#define WSVERS		MAKEWORD(2, 0)        // ָ���汾2.0 
#define PATHLEN 50
#pragma comment(lib,"ws2_32.lib")         // ʹ��winsock 2.0 Llibrary

/*------------------------------------------------------------------------
 * main - TCP client for TIME service
 *------------------------------------------------------------------------
 */
char	buf_r[BUFLEN];   		    /* buffer for one line of text	*/
char	fname[PATHLEN + 1];
char	tmp[BUFLEN + 1];
char	buf_s[BUFLEN];

void main(int argc, char *argv[])
{
	char	*host = "127.0.0.1";	    /* server IP to connect         */
	char	*service = "50500";  	    /* server port to connect       */
	struct sockaddr_in sin;	            /* an Internet endpoint address	*/
	SOCKET	sock;		  	            /* socket descriptor	    	*/
	int	cc;			                    /* recv character count		    */
	int ss;
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);						  //����winsock library��WSVERSΪ����İ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	  //�����׽��֣�������������Э���(family)�����׽��֣�TCPЭ��
														  //���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET
	memset(&sin, 0, sizeof(sin));						  // ��&sin��ʼ�ĳ���Ϊsizeof(sin)���ڴ���0
	sin.sin_family = AF_INET;							  // ��������ַ��
	sin.sin_addr.s_addr = inet_addr(host);                // ���÷�����IP��ַ(32λ)
	sin.sin_port = htons((u_short)atoi(service));         // ���÷������˿ں�  
	printf("Connecting...\n");
	int ret = connect(sock, (struct sockaddr *)&sin, sizeof(sin));  // ���ӵ����������ڶ�������ָ���ŷ�������ַ�Ľṹ������������Ϊ�ýṹ�Ĵ�С������ֵΪ0ʱ��ʾ�޴�����������SOCKET_ERROR��ʾ����Ӧ�ó����ͨ��WSAGetLastError()��ȡ��Ӧ������롣
	if (ret == SOCKET_ERROR) {
		printf("Error:%d.\n", GetLastError());
		getchar();
		exit(1);
	}
	printf("Successfully connected!\n\n");
	while (1) {
		printf("Input the file path: ");
		scanf("%s", fname);
		if (!strcmp(fname, "exit"))
			break;
		strcpy(buf_s, fname);
		strcat(buf_s, "\n");

		FILE *f;
		f = fopen(fname, "rb");
		if (f == NULL) {
			printf("Cannot open the file.\n\n");
			getchar();
			continue;
		}
		fseek(f, 0, SEEK_END); //read the file in one time
		int len = ftell(f);
		if (len > BUFLEN) {
			printf("out of size!\n");
			getchar();
			continue;
		}
		sprintf(tmp, "%d\n", len);
		strcat(buf_s, tmp);

		rewind(f);
		int leng = strlen(buf_s);
		fread(buf_s + leng, 1, len, f);
		fclose(f);
		printf("Transporting...\n");
		ss = send(sock, buf_s, BUFLEN, 0);
		if (ss == SOCKET_ERROR) {
			printf("Error:%d.\n", GetLastError());
		}
		else if (ss == 0) {                             // �Է������ر�
			printf("Server closed!");
		}
		else {
			//for (int i = 0; i < 10000;i++);
			cc = recv(sock, buf_r, BUFLEN, 0);                // �ڶ�������ָ�򻺳���������������Ϊ��������С(�ֽ���)�����ĸ�����һ������Ϊ0������ֵ:(>0)���յ����ֽ���,(=0)�Է��ѹر�,(<0)���ӳ���
			if (cc == SOCKET_ERROR)                          // ����������ر��׽���sock
				printf("Error: %d.\n", GetLastError());
			else if (cc > 0) {
				//buf_r[cc] = '\0';	                       // ensure null-termination
				printf("%s\n", buf_r);                         // ��ʾ�����յ��ַ���
			}
		}
	}

	closesocket(sock);                             // �رռ����׽���
	WSACleanup();                                  // ж��winsock library

	printf("\nPress any key to continue...");
	getchar();										// �ȴ����ⰴ��
	getchar();
}