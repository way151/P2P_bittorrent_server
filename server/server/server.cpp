/* TCPServer.cpp - main */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include <direct.h>
#include <queue>
#include "conio.h"
//#include "filepak.h"
#define	WSVERS	MAKEWORD(2, 0)
#define	BUFLEN		8000000
#define PATHLEN 50
#define MAXQUEUE 10
#pragma comment(lib,"ws2_32.lib")  //ʹ��winsock 2.2 library
/*------------------------------------------------------------------------
 * main - Iterative TCP server for TIME service
 *------------------------------------------------------------------------
 */
char buf_r[BUFLEN + 1];
char buf_s[BUFLEN + 1];
char tmp[21];
void main(int argc, char *argv[])
/* argc: �����в��������� ���磺C:\> TCPServer 8080
					 argc=2 argv[0]="TCPServer",argv[1]="8080" */
{
	struct	sockaddr_in fsin;	    /* the from address of a client	  */
	SOCKET	msock, ssock;		    /* master & slave sockets	      */
	WSADATA wsadata;
	char	*service = "10086";
	struct  sockaddr_in sin;	    /* an Internet endpoint address		*/
	int	    alen;			        /* from-address length		        */
	char	*pts;			        /* pointer to time string	        */
	time_t	now;			        /* current time			            */

	std::queue<SOCKET> node_queue[MAXQUEUE];

	WSAStartup(WSVERS, &wsadata);						// ����winsock library��WSVERSָ������ʹ�õİ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// �����׽��֣�������������Э���(family)�����׽��֣�TCPЭ��
														// ���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET

	memset(&sin, 0, sizeof(sin));						// ��&sin��ʼ�ĳ���Ϊsizeof(sin)���ڴ���0
	sin.sin_family = AF_INET;							// ��������ַ��(INET-Internet)
	sin.sin_addr.s_addr = INADDR_ANY;					// ��������(�ӿڵ�)IP��ַ��
	sin.sin_port = htons((u_short)atoi(service));		// �����Ķ˿ںš�atoi--��asciiת��Ϊint��htons--������������(host to network��s-short 16λ)
	bind(msock, (struct sockaddr *)&sin, sizeof(sin));  // �󶨼�����IP��ַ�Ͷ˿ں�

	listen(msock, 5);                                   // ��������Ϊ5������������У����ѵ������������������еȴ�����
	char filepath[PATHLEN + 1];
	printf("Input the received path: ");
	scanf("%s", filepath);

	if (_access(filepath, 0) == -1)
		_mkdir(filepath);

	printf("\nConnecting...\n");
	alen = sizeof(struct sockaddr);                         // ȡ����ַ�ṹ�ĳ���
	ssock = accept(msock, (struct sockaddr *)&fsin, &alen); // ����������������������������������������󲢽������ӣ����ظ����ӵ��׽��֣����򣬱���䱻����ֱ�����зǿա�fsin�����ͻ���IP��ַ�Ͷ˿ں�
	printf("Successfully connected!\n\n");

	while (1) { 		                                   // ����Ƿ��а���,���û�������ѭ����ִ��
		int rr = recv(ssock, buf_r, BUFLEN, 0);
		if (rr == SOCKET_ERROR) {
			printf("Error: %d.\n", GetLastError());
			break;
		}
		else if (rr > 0) {
			int i = 0, j = 0;
			char parse[3][50];
			for (int k = 0; k < BUFLEN; ++k) {
				if (buf_r[k] == '\n' && i != 2) {
					parse[i][j] = '\0';
					j = 0;
					i++;
				}
				else if (i == 2)
					buf_s[j++] = buf_r[k];
				else
					parse[i][j++] = buf_r[k];
			}

			printf("Receiving: %s...\n", parse[0]);
			char savepath[PATHLEN + 1];
			strcpy(savepath, filepath);
			strcat(savepath, "/");
			strcat(savepath, parse[0]);
			strcpy(savepath, make_filename(savepath));
			FILE* f = fopen(savepath, "wb");
			int len = atoi(parse[1]);
			for (i = 0; i < len / 20; ++i)
				fwrite(buf_s + i * 20, 1, 20, f);
			fwrite(buf_s + i * 20, 1, len % 20, f);
			fclose(f);
			printf("Successfully received!\n\n");

			(void)time(&now);                                      // ȡ��ϵͳʱ��
			pts = ctime(&now);
			strcpy(buf_s, "Successfully transported!  ");
			strcat(buf_s, pts);
			int cc = send(ssock, buf_s, BUFLEN, 0);
			if (cc == SOCKET_ERROR) {
				printf("Error: %d.\n", GetLastError());
				break;
			}
		}
	}

	(void)closesocket(ssock);                              // �ر������׽���
	(void)closesocket(msock);                                 // �رռ����׽���
	WSACleanup();                                             // ж��winsock library
	printf("\nPress any key to continue...");
	getchar();										// �ȴ����ⰴ��
	getchar();


}