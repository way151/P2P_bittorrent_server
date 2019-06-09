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
#pragma comment(lib,"ws2_32.lib")  //使用winsock 2.2 library
/*------------------------------------------------------------------------
 * main - Iterative TCP server for TIME service
 *------------------------------------------------------------------------
 */
char buf_r[BUFLEN + 1];
char buf_s[BUFLEN + 1];
char tmp[21];
void main(int argc, char *argv[])
/* argc: 命令行参数个数， 例如：C:\> TCPServer 8080
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

	WSAStartup(WSVERS, &wsadata);						// 加载winsock library。WSVERS指明请求使用的版本。wsadata返回系统实际支持的最高版本
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// 创建套接字，参数：因特网协议簇(family)，流套接字，TCP协议
														// 返回：要监听套接字的描述符或INVALID_SOCKET

	memset(&sin, 0, sizeof(sin));						// 从&sin开始的长度为sizeof(sin)的内存清0
	sin.sin_family = AF_INET;							// 因特网地址簇(INET-Internet)
	sin.sin_addr.s_addr = INADDR_ANY;					// 监听所有(接口的)IP地址。
	sin.sin_port = htons((u_short)atoi(service));		// 监听的端口号。atoi--把ascii转化为int，htons--主机序到网络序(host to network，s-short 16位)
	bind(msock, (struct sockaddr *)&sin, sizeof(sin));  // 绑定监听的IP地址和端口号

	listen(msock, 5);                                   // 建立长度为5的连接请求队列，并把到来的连接请求加入队列等待处理。
	char filepath[PATHLEN + 1];
	printf("Input the received path: ");
	scanf("%s", filepath);

	if (_access(filepath, 0) == -1)
		_mkdir(filepath);

	printf("\nConnecting...\n");
	alen = sizeof(struct sockaddr);                         // 取到地址结构的长度
	ssock = accept(msock, (struct sockaddr *)&fsin, &alen); // 如果在连接请求队列中有连接请求，则接受连接请求并建立连接，返回该连接的套接字，否则，本语句被阻塞直到队列非空。fsin包含客户端IP地址和端口号
	printf("Successfully connected!\n\n");

	while (1) { 		                                   // 检测是否有按键,如果没有则进入循环体执行
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

			(void)time(&now);                                      // 取得系统时间
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

	(void)closesocket(ssock);                              // 关闭连接套接字
	(void)closesocket(msock);                                 // 关闭监听套接字
	WSACleanup();                                             // 卸载winsock library
	printf("\nPress any key to continue...");
	getchar();										// 等待任意按键
	getchar();


}