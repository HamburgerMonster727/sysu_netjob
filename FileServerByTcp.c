#include <stdio.h>
#include <time.h>
#include <winsock2.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#define BUFLEN 2000				   
#define FILELEN 100
#define PATHLEN 200				   
#pragma comment(lib, "ws2_32.lib") 		   

void refilename(const char* folder, char* filename){
	char pathname[PATHLEN];
	strcpy(pathname,folder);
	strcat(pathname,filename);
	char temp[300];
	char numstr[10];
	int num = 1;
	while(access(pathname, 0) == 0) {
		int dotpos = strrchr(filename, '.') - filename;	
		if(strrchr(filename, '(') != NULL && strrchr(filename, ')') != NULL) {
			int leftpos = strrchr(filename, '(') - filename;	
			strncpy(temp, filename+dotpos, 300);
			filename[leftpos] = '\0';
			strncat(filename, temp, 300);	
		}
		dotpos = strrchr(filename, '.') - filename;	
		num++;							
		numstr[0] = '(';				
		itoa(num, numstr+1, 10);		
		numstr[strlen(numstr)+1] = '\0';
		numstr[strlen(numstr)] = ')';	
		strncpy(temp, filename, dotpos);
		temp[dotpos] = '\0';
		strcat(temp, numstr);
		strcat(temp, filename + dotpos);
		strncpy(filename, temp, 300);	
		strcpy(pathname,folder);
		strcat(pathname,filename);
	}
}

int main(){
	struct sockaddr_in clsin; 
	struct sockaddr_in sin;   
	SOCKET msock, ssock;	  
	u_short port = 8888;	 
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 0), &wsadata); 
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); 
	memset(&sin, 0, sizeof(sin));					   
	sin.sin_family = AF_INET;						   
	sin.sin_addr.s_addr = INADDR_ANY;				   
	sin.sin_port = htons(port);	

	if(bind(msock, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR){
        printf("bind error !");
        closesocket(msock);
        return 0;
    } 
	listen(msock, 5);								   

	printf("服务器开始运行\n");
	char folder[100] = {0}; 
	do {
		printf("输入接收文件夹(格式:路径\\文件夹名): ");
		scanf("%s", folder);
		if(access(folder, F_OK) != 0) {  
			fprintf(stderr, "无效文件夹！\n");
		}
	} while(access(folder, F_OK) != 0);
	if(folder[strlen(folder)-1] != '\\') { // 保证目录以反斜杠结尾 
		folder[strlen(folder)+1] = '\0';
		folder[strlen(folder)] = '\\';  
	}
	printf("接收文件夹已设置为 %s\n", folder);

	char buf[BUFLEN + 1]; 
	while (!_kbhit()){															
		int alen = sizeof(struct sockaddr);						 
		ssock = accept(msock, (struct sockaddr *)&clsin, &alen); // 从socket，阻塞地从连接请求队列中创建连接
		char filename[FILELEN + 1] = {0};
		int recvlen = recv(ssock, filename, FILELEN + 1, 0); // 接收文件名
		buf[recvlen] = '\0';					   // 保证以空字符结尾
		printf("正在接收文件：%s\n", filename);
		refilename(folder, filename); //为重名文件添加序号

		char msg[BUFLEN + 1];
		recvlen = recv(ssock, msg, BUFLEN + 1, 0); //接收文件时间和大小
		msg[recvlen] = '\0';
		printf("%s\n",msg);

		char pathname[PATHLEN + 1];
		strcpy(pathname,folder);
		strcat(pathname,filename);
		FILE* fp = fopen(pathname, "wb");
		int length = 0;
		while((length = recv(ssock, buf, BUFLEN, 0)) > 0) {// 循环接收数据写入文件 
			fwrite(buf, sizeof(char), length, fp);
		}
		fclose(fp);
		printf("接收成功，已保存至 %s\n", pathname);

		strncpy(buf, "服务器已成功接收文件。\n", BUFLEN); //发送反聩信息
		send(ssock, buf, BUFLEN + 1, 0);  
		shutdown(ssock, SD_SEND);			  // 禁止收发数据
		closesocket(ssock);					  
	}
	closesocket(msock); 
	WSACleanup();		
	printf("服务器运行结束\n");
	return 0;
}