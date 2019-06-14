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
#pragma comment(lib,"ws2_32.lib")  //ʹ��winsock 2.2 library
/*------------------------------------------------------------------------
 * main - Iterative TCP server for TIME service
 *------------------------------------------------------------------------
 */



struct node {
	string ip;
	string port;
};
map<string, vector<node> > file_tab;


struct struparam {             /* ���ڴ������ */
	int threadno;            /* �̱߳�� */
	SOCKET sock;
	string ip;
	char buf_ip[BUFLEN];

};
SOCKET ssock[MAXSOC];
char buf_r[BUFLEN + 1];
char buf_s[BUFLEN + 1];

CRITICAL_SECTION cs;		           // �ٽ�����ÿ��ʱ��ֻ��һ���߳̿��Խ����ٽ���

unsigned __stdcall func(void * p)    // �ýṹ������ȫ�ֱ����������
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
			printf("Erremor: %d.\n", keyy);     //����������ر��׽���sock��
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
					printf("Erremor: %d.\n", keyy);     //����������ر��׽���sock��
					break;
				}
				else {
					char port[MAXLEN];
					int dd = recv(p1.sock, port, MAXLEN, 0);
					if (dd == SOCKET_ERROR || dd == 0) {
						int keyy = GetLastError();
						printf("Erremor: %d.\n", keyy);     //����������ر��׽���sock��
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
					printf("Erremor: %d.\n", keyy);     //����������ر��׽���sock��
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
						printf("Erremor: %d.\n", keyy);     //����������ر��׽���sock��
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
/* argc: �����в��������� ���磺C:\> TCPServer 8080
					 argc=2 argv[0]="TCPServer",argv[1]="8080" */
{
	struct	sockaddr_in fsin;	    /* the from address of a client	  */
	SOCKET	msock;		    /* master & slave sockets	      */
	WSADATA wsadata;
	char	*service = "10086";
	struct  sockaddr_in sin;	    /* an Internet endpoint address		*/
	int	    alen;			        /* from-address length		        */


	WSAStartup(WSVERS, &wsadata);						// ����winsock library��WSVERSָ������ʹ�õİ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// �����׽��֣�������������Э���(family)�����׽��֣�TCPЭ��
														// ���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET

	memset(&sin, 0, sizeof(sin));						// ��&sin��ʼ�ĳ���Ϊsizeof(sin)���ڴ���0
	sin.sin_family = AF_INET;							// ��������ַ��(INET-Internet)
	sin.sin_addr.s_addr = INADDR_ANY;					// ��������(�ӿڵ�)IP��ַ��
	sin.sin_port = htons((u_short)atoi(service));		// �����Ķ˿ںš�atoi--��asciiת��Ϊint��htons--������������(16λ)
	bind(msock, (struct sockaddr *)&sin, sizeof(sin));  // �󶨼�����IP��ַ�Ͷ˿ں�

	listen(msock, 5);                                   // �ȴ��������ӵĶ��г���Ϊ5

	printf("��������������\n");
	while (1) { 		                             // ����Ƿ��а���
		alen = sizeof(struct sockaddr);                   // ȡ����ַ�ṹ�ĳ���
		int i = 0;
		for (i = 0; i < MAXSOC; ++i)
		{
			if (ssock[i] == 0)
				break;
		}
		ssock[i] = accept(msock, (struct sockaddr *)&fsin, &alen); // ������µ��������󣬷��������׽��֣����򣬱�������fsin�����ͻ���IP��ַ�Ͷ˿ں�

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

	(void)closesocket(msock);                                 // �رռ����׽���
	WSACleanup();                                             // ж��winsock library

}
