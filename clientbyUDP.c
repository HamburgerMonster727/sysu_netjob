#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
int main(){
    WORD socketVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(socketVersion, &wsaData) != 0){
        return 0;
    }
    //创建套接字，指明协议族为AF_INET，socket类型为SOCK_DGRAM，type为IPPROTO_UDP
    SOCKET cliSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (cliSocket == INVALID_SOCKET){
        printf("socket error!");
        return 0;
    }
    SOCKADDR_IN cliAddr;
    cliAddr.sin_family = AF_INET;
    cliAddr.sin_port = htons(8888);
    cliAddr.sin_addr.S_un.S_addr = inet_addr("192.168.83.193");
    int len = sizeof(cliAddr);
    for (int i = 1; i <= 100; i++){
        char *sendData = "Packets from the client.\n";
        printf("send the %d packets\n", i);
        //发送数据，指明套接口文件描述符，发送的数据的指针，字节长度，标志位，SOCKADDR_IN和其长度
        sendto(cliSocket, sendData, strlen(sendData), 0, (SOCKADDR *)&cliAddr, len);
        Sleep(250);
    }
    //关闭连接套接字
    closesocket(cliSocket);
    //终止Winsock 2 DLL的使用
    WSACleanup();
    return 0;
}