#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
int main(){
    WSADATA wsaData;
    //将两个byte型的"2"合并成一个word，分别在高8位和低8位
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &wsaData) != 0){
        return 0;
    }
    //创建套接字，指明协议族为AF_INET，socket类型为SOCK_DGRAM，type为IPPROTO_UDP
    SOCKET serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serSocket == INVALID_SOCKET){
        printf("socket error!");
        return 0;
    }
    SOCKADDR_IN serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(8888);
    serAddr.sin_addr.S_un.S_addr = inet_addr("192.168.83.193");
    //将ip地址和端口号绑定到套接字
    if (bind(serSocket, (SOCKADDR *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR){
        printf("bind error!");
        //关闭连接套接字
        closesocket(serSocket);
        return 0;
    }
    SOCKADDR_IN remoteAddr;
    int nAddrLen = sizeof(remoteAddr);
    int i = 1;
    while (1){
        char recData[255];
        //接受数据，指明要读取的套接口文件描述符，保存信息的缓冲区起始地址，缓冲区最大长度，标志和SOCKADDR_IN和其长度
        int ret = recvfrom(serSocket, recData, 255, 0, (SOCKADDR *)&remoteAddr, &nAddrLen);
        if (ret > 0){
            recData[ret] = 0x00;
            printf("NO%d：", i++);
            printf(recData);
        }
    }
    //关闭连接套接字
    closesocket(serSocket);
    //终止Winsock 2 DLL的使用
    WSACleanup();
    return 0;
}