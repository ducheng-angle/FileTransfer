#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <errno.h>
#include <vector>
#include "Common.hpp"

void handle(int connfd)
{
	usage();
    for(;;){
		int ret = 0;
		int flags = 0; //read = 0, write=1;
		int start = 0;
		int end = 0;
		std::string str;
		std::string line="";
		int send_len = 0;
		std::cout << "please input cmd: "<< std::endl;

		getline(std::cin,line); 
		ret = Parse(line,flags,start,end,str);
		if(ret != 0)
		{
		 std::cout << "invalid input,please input again! "<< std::endl;
		 std::cout << "***********************************"<< std::endl;
		 continue;
		} 
		while((send_len=send(connfd, line.c_str(), line.length()+1, 0)) < 0)
		{
			 std::cout << "send msg error: " <<strerror(errno) << std::endl;
		 exit(0);
		}

		char resline[BUFSIZE]={};
		recv(connfd,resline,BUFSIZE, 0);
		std::cout << "result: " << std::endl 
		           << resline << std::endl;
		std::cout << "***********************************"<< std::endl;
	} 
}


int main(){
	int connfd = 0;
	struct sockaddr_in servaddr;
	connfd = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
	if(connfd < 0)  
	{  
	 perror("socket error");  
	 exit(-1);  
	}  
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if(inet_pton(AF_INET, SERVERIP, &servaddr.sin_addr) <= 0)
	{
	printf("inet_pton error for %s\n",SERVERIP);
	exit(0);
	}
	// 连接到server服务器
	connect(connfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	// 处理接收socket套接字
	handle(connfd); 
	//关闭套接字
	close(connfd); 
	return 0; 
}




