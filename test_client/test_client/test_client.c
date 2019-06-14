/* TCPClient.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <math.h>
#include <process.h>
#include <windows.h>

#define	BUFLEN		2000                  // 缓冲区大小
#define WSVERS		MAKEWORD(2, 0)        // 指明版本2.0 
#pragma comment(lib,"ws2_32.lib")         // 使用winsock 2.0 Llibrary

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
		int cc = recv(p1->sock, buf_r, BUFLEN, 0);                // cc为接收到的字符的个数(>0)或对方已关闭(=0)或连接出错(<0)
		if (cc == SOCKET_ERROR || cc == 0)
		{
			printf("Error: %d.\n", GetLastError());     //出错。其后必须关闭套接字sock。
			break;
		}
		else if (cc > 0) {
			buf_r[cc] = '\0';                           // ensure null-termination
			printf("%s", buf_r);                         // 显示所接收的字符串
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
	WSAStartup(WSVERS, &wsadata);						  //加载winsock library。WSVERS为请求的版本，wsadata返回系统实际支持的最高版本

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	  //创建套接字，参数：因特网协议簇(family)，流套接字，TCP协议
														  //返回：要监听套接字的描述符或INVALID_SOCKET

	memset(&sin, 0, sizeof(sin));						  // 从&sin开始的长度为sizeof(sin)的内存清0
	sin.sin_family = AF_INET;							  // 因特网地址簇
	sin.sin_addr.s_addr = inet_addr(host);                // 服务器IP地址(32位)
	sin.sin_port = htons((u_short)atoi(service));         // 服务器端口号  
	connect(sock, (struct sockaddr *)&sin, sizeof(sin));  // 连接到服务器

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
			printf("Error: %d.\n", GetLastError());     //出错。其后必须关闭套接字sock。
			break;
		}
		/*else if (cc > 0) {
			printf("A message: %s\n", buf_s);                         // 显示所接收的字符串
		}*/
		buf_s[0] = '\0';


		//printf("input infomation you want to send: ");

	}

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	getchar();

	WSACleanup();                                  // 卸载winsock library

	printf("按回车键继续...");
	getchar();										// 等待任意按键
}