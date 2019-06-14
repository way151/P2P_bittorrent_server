/* TCPClient.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <math.h>
#include <process.h>
#include <windows.h>

#define	BUFLEN		2000                  // ��������С
#define WSVERS		MAKEWORD(2, 0)        // ָ���汾2.0 
#pragma comment(lib,"ws2_32.lib")         // ʹ��winsock 2.0 Llibrary

/*------------------------------------------------------------------------
 * main - TCP client for TIME service
 *------------------------------------------------------------------------
 */
struct struparam {
	int threadno;
	SOCKET sock;
};

//CRITICAL_SECTION cs;

unsigned __stdcall func(void *p)
{
	struct struparam *p1 = (struct struparam*)p;
	char buf_r[BUFLEN + 1];
	while (1)
	{
		int cc = recv(p1->sock, buf_r, BUFLEN, 0);                // ccΪ���յ����ַ��ĸ���(>0)��Է��ѹر�(=0)�����ӳ���(<0)
		if (cc == SOCKET_ERROR || cc == 0)
		{
			printf("Error: %d.\n", GetLastError());     //����������ر��׽���sock��
			break;
		}
		else if (cc > 0) {
			buf_r[cc] = '\0';                           // ensure null-termination
			printf("%s", buf_r);                         // ��ʾ�����յ��ַ���
		}
	}
	closesocket(p1->sock);
	return 0;
}

void main(int argc, char *argv[])
{
	//char *host = "172.18.146.224"; //ghm
	//char *service = "50500";       
	//char *host = "172.26.71.74";   //fy
	//char *service = "50500";
	//char *host = "172.18.146.199"; //gjn
	//char *service = "50500";

	//char *host = "172.18.187.9";	//tea
	//char *service = "50500";
	char	*host = "127.0.0.1";	    /* server IP to connect         */
	char	*service = "10086";  	    /* server port to connect       */
	struct sockaddr_in sin;	            /* an Internet endpoint address	*/
//	char	buf[BUFLEN + 1];   		        /* buffer for one line of text	*/
	SOCKET	sock;		  	            /* socket descriptor	    	*/

	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);						  //����winsock library��WSVERSΪ����İ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	  //�����׽��֣�������������Э���(family)�����׽��֣�TCPЭ��
														  //���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET

	memset(&sin, 0, sizeof(sin));						  // ��&sin��ʼ�ĳ���Ϊsizeof(sin)���ڴ���0
	sin.sin_family = AF_INET;							  // ��������ַ��
	sin.sin_addr.s_addr = inet_addr(host);                // ������IP��ַ(32λ)
	sin.sin_port = htons((u_short)atoi(service));         // �������˿ں�  
	connect(sock, (struct sockaddr *)&sin, sizeof(sin));  // ���ӵ�������

	HANDLE hThread;

	struct struparam p1;
	struct struparam *ptr1;
	p1.threadno = 1;
	p1.sock = sock;

	ptr1 = &p1;
	hThread = (HANDLE)_beginthreadex(NULL, 0, &func, (void *)ptr1, 0, NULL);



	char buf_s[BUFLEN + 1];
	//printf("input infomation you want to send: ");
	while (gets_s(buf_s, BUFLEN))
	{
		char ss[10] = "exit";
		//buf_s[strlen(buf_s)] = '\0';
		int cc = send(sock, buf_s, strlen(buf_s), 0);

		fflush(stdout);
		fflush(stdin);
		if (cc == SOCKET_ERROR || cc == 0)
		{
			printf("Error: %d.\n", GetLastError());     //����������ر��׽���sock��
			break;
		}
		/*else if (cc > 0) {
			printf("A message: %s\n", buf_s);                         // ��ʾ�����յ��ַ���
		}*/
		buf_s[0] = '\0';


		//printf("input infomation you want to send: ");

	}

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	getchar();

	WSACleanup();                                  // ж��winsock library

	printf("���س�������...");
	getchar();										// �ȴ����ⰴ��
}