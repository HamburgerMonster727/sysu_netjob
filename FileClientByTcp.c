#include <stdio.h>
#include <winsock2.h>
#include <io.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#define BUFLEN 2000				   
#define FILELEN 100
#define PATHLEN 200				   
#pragma comment(lib, "ws2_32.lib") 

int sendfilebytcp(){
	char host[80] = "127.0.0.1";
	u_short serverport = 8888;
	struct sockaddr_in sin;
	char buf[BUFLEN + 1];
	SOCKET sock;
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 0), &wsadata);
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(host);
	sin.sin_port = htons(serverport);

	int ret = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
	if (ret == -1){
		printf("连接到服务器失败！\n");
		closesocket(sock);
		WSACleanup();
		return 0;
	}
	
	char filename[FILELEN] = {0};
	do {
		printf("输入要传送的文件名(格式:路径/文件名): ");
		fgets(filename, FILELEN, stdin);
		filename[strlen(filename) - 1] = '\0'; // 把换行符替换为空字符
		if (strcmp("exit", filename) == 0){  // 用户退出
			closesocket(sock);
			WSACleanup();
			return 0;
		} 
		if(access(filename, R_OK) != 0) {
			fprintf(stderr, "无效文件名！\n");
		}
	} while(access(filename, R_OK) != 0);

	char *ptr = strrchr(filename,'/');
	for(int i = 0;i <= strlen(ptr)-1;i++){
		ptr[i] = ptr[i+1];
	}
	send(sock, ptr, FILELEN + 1, 0);  //发送文件名

	struct stat statbuf;
    stat(filename,&statbuf);
    int filesize = statbuf.st_size;//获取文件大小
	struct tm *t;
	t = localtime(&statbuf.st_ctime);//获取文件创建时间
	sprintf(buf,"文件创建日期:%04d-%02d-%02d %02d:%02d:%02d 文件大小:%d字节",t->tm_year+1900, t->tm_mon+1, t->tm_mday,t->tm_hour, t->tm_min, t->tm_sec, filesize);
	send(sock, buf, BUFLEN, 0);   //发送文件时间和大小

	FILE* fp = fopen(filename, "rb");
	int length;
	while((length = (int)fread(buf, sizeof(char), BUFLEN, fp)) > 0) {// 循环读取文件发送 
		send(sock, buf, length, 0);
	}
	fclose(fp);
	shutdown(sock, SD_SEND);  // 发送FIN包，表示发送完毕 
	
	int recvlen = recv(sock, buf, BUFLEN + 1, 0); // 接收反馈信息
	if (recvlen == SOCKET_ERROR){
		printf("Error: %ld.\n", GetLastError());
	}
	else if (recvlen == 0){
		printf("服务器关闭！\n");
	}
	else if (recvlen > 0){
		buf[recvlen] = '\0';
		printf("%s\n", buf);
	}
	closesocket(sock);
	WSACleanup();
	return 1;
}

int main(){
	printf("客户端开始运行,退出请输入exit\n");
	while (sendfilebytcp());
	printf("客户端运行结束。\n");
	return 0;
}