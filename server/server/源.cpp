/* TCPServer.cpp - main */

#include <cstdlib>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include "conio.h"
#include <windows.h>
#include <process.h>
#include <math.h>
#include <string>
#include <sstream>
#include <map>

using std::vector;
using std::map;
using std::string;
using std::stringstream;

#define QLEN	   5
#define	WSVERS	MAKEWORD(2, 0)
#define MAXSOC 10
#define MAXLEN 20
#define BUFLEN 2000
#pragma comment(lib,"ws2_32.lib")  //使用winsock 2.2 library
/*------------------------------------------------------------------------
 * main - Iterative TCP server for TIME service
 *------------------------------------------------------------------------
 */



struct node {
	string ip;
	string port;
};
map<string, vector<node> > file_tab;


struct struparam {             /* 用于带入参数 */
	int threadno;            /* 线程编号 */
	SOCKET sock;
	string ip;
	char buf_ip[BUFLEN];

};
SOCKET ssock[MAXSOC];
char buf_r[BUFLEN + 1];
char buf_s[BUFLEN + 1];

CRITICAL_SECTION cs;		           // 临界区。每个时刻只有一个线程可以进入临界区

unsigned __stdcall func(void * p)    // 用结构变量或全局变量带入参数
{
	InitializeCriticalSection(&cs);

	EnterCriticalSection(&cs);

	struct struparam p1 = *((struct struparam *)p);  //why its wrong when using pointer??? 

	while (1)
	{
		int cc = recv(p1.sock, buf_r, BUFLEN, 0);
		if (cc == SOCKET_ERROR || cc == 0)
		{
			int keyy = GetLastError();
			printf("Erremor: %d.\n", keyy);     //出错。其后必须关闭套接字sock。
			break;
		}
		else if (cc > 0)
		{
			printf("%s", buf_r);
			if (!strcmp(buf_r, "0")) {
				char fname[MAXLEN];
				int ee = recv(p1.sock, fname, MAXLEN, 0);
				if (ee == SOCKET_ERROR || ee == 0) {
					int keyy = GetLastError();
					printf("Erremor: %d.\n", keyy);     //出错。其后必须关闭套接字sock。
					break;
				}
				else {
					char port[MAXLEN];
					int dd = recv(p1.sock, port, MAXLEN, 0);
					if (dd == SOCKET_ERROR || dd == 0) {
						int keyy = GetLastError();
						printf("Erremor: %d.\n", keyy);     //出错。其后必须关闭套接字sock。
						break;
					}
					else {
						string fn = fname;
						node Node;
						Node.port = port;
						Node.ip = p1.ip;

						if (file_tab.count(fn)) {
							file_tab[fn].push_back(Node);
						}
						else {
							vector<node> vv;
							vv.push_back(Node);
							file_tab[fn] = vv;
						}
					}

				}
			}
			else if (!strcmp(buf_r, "1")) {
				char fname[MAXLEN];
				int ee = recv(p1.sock, fname, MAXLEN, 0);
				if (ee == SOCKET_ERROR || ee == 0) {
					int keyy = GetLastError();
					printf("Erremor: %d.\n", keyy);     //出错。其后必须关闭套接字sock。
					break;
				}
				else {
					string str = fname;
					if (file_tab.count(str)) {
						vector<node> vnode = file_tab[str];
						for (int i = 0; i < vnode.size(); ++i) {
							strcat(buf_s, vnode[i].ip.c_str());
							buf_s[strlen(buf_s)] = ' ';
							strcat(buf_s, vnode[i].port.c_str());
							buf_s[strlen(buf_s)] = ' ';
						}
					}
					else {
						strcpy(buf_s, "cannot find the file.\n");
					}
					int sendc = send(p1.sock, buf_s, strlen(buf_s), 0);
					if (sendc == SOCKET_ERROR || sendc == 0) {
						int keyy = GetLastError();
						printf("Erremor: %d.\n", keyy);     //出错。其后必须关闭套接字sock。
						break;
					}
				}
			}
			else if (!strcmp(buf_r, "2")){
				map<string, vector<node> >::iterator it;
				it = file_tab.begin();
				while (it != file_tab.end())
				{
					vector<node>::iterator jt;
					for (jt = (it->second).begin; jt != (it->second).end; ) {
						if ((*jt).ip == p1.ip) {
							(it->second).erase(jt);
						}
						else
							jt++;
					}
					it++;
				}
			}
		}
		memset(buf_s, 0, sizeof(buf_s));
		memset(buf_r, 0, sizeof(buf_r));
	}
	(void)closesocket(p1.sock);
	LeaveCriticalSection(&cs);
	return 0;
}

void main(int argc, char *argv[])
/* argc: 命令行参数个数， 例如：C:\> TCPServer 8080
					 argc=2 argv[0]="TCPServer",argv[1]="8080" */
{
	struct	sockaddr_in fsin;	    /* the from address of a client	  */
	SOCKET	msock;		    /* master & slave sockets	      */
	WSADATA wsadata;
	char	*service = "10086";
	struct  sockaddr_in sin;	    /* an Internet endpoint address		*/
	int	    alen;			        /* from-address length		        */


	WSAStartup(WSVERS, &wsadata);						// 加载winsock library。WSVERS指明请求使用的版本。wsadata返回系统实际支持的最高版本
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// 创建套接字，参数：因特网协议簇(family)，流套接字，TCP协议
														// 返回：要监听套接字的描述符或INVALID_SOCKET

	memset(&sin, 0, sizeof(sin));						// 从&sin开始的长度为sizeof(sin)的内存清0
	sin.sin_family = AF_INET;							// 因特网地址簇(INET-Internet)
	sin.sin_addr.s_addr = INADDR_ANY;					// 监听所有(接口的)IP地址。
	sin.sin_port = htons((u_short)atoi(service));		// 监听的端口号。atoi--把ascii转化为int，htons--主机序到网络序(16位)
	bind(msock, (struct sockaddr *)&sin, sizeof(sin));  // 绑定监听的IP地址和端口号

	listen(msock, 5);                                   // 等待建立连接的队列长度为5

	printf("服务器，启动！\n");
	while (1) { 		                             // 检测是否有按键
		alen = sizeof(struct sockaddr);                   // 取到地址结构的长度
		int i = 0;
		for (i = 0; i < MAXSOC; ++i)
		{
			if (ssock[i] == 0)
				break;
		}
		ssock[i] = accept(msock, (struct sockaddr *)&fsin, &alen); // 如果有新的连接请求，返回连接套接字，否则，被阻塞。fsin包含客户端IP地址和端口号

		HANDLE hThread;
		unsigned threadID = NULL;
		struct struparam p1;

		struct in_addr ippre;
		ippre = fsin.sin_addr;
		//send2all();
		p1.threadno = i;
		p1.sock = ssock[i];
		p1.ip = std::to_string(ippre.S_un.S_addr);
		//ptr1 = &p1;
		//hThread = (HANDLE)_beginthreadex(NULL, 0,&func, (void *)ptr1, 0, &threadID);
		hThread = (HANDLE)_beginthreadex(NULL, 0, &func, (void *)&p1, 0, &threadID);

	}

	(void)closesocket(msock);                                 // 关闭监听套接字
	WSACleanup();                                             // 卸载winsock library

}
