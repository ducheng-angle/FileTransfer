#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>                         
#include <unistd.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>                      
#include <pthread.h>
#include <iostream> 
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "Common.hpp"
/************************************************************************ 
 * 函数名称：   void *Thread_fun(void *arg) 
 * 函数功能：   线程函数,处理客户信息 
 * 函数参数：   已连接套接字 
 * 函数返回：   无 
 * ************************************************************************/  
void *Thread_fun(void *arg)  
{  
	int recv_len = 0;  
    char recv_buf[BUFSIZE] = "";   // 接收缓冲区  
    int connfd =*((int*)arg);// 传过来的已连接套
    // 接收数据  
    while((recv_len = recv(connfd, recv_buf, sizeof(recv_buf), 0)) > 0)  
    { 
    	std::cout << recv_buf << std::endl; 
        int fd =-1;
        int flags = 0; //read = 0, write=1;
        int start = 0;
        int end = 0;
        std::string str; 
        Parse(recv_buf,flags,start,end,str);
        ssize_t ret = 0;
        off_t offset = start*BLOCKSIZE;
        size_t count = (end-start)*BLOCKSIZE;
        size_t rw = 0;
        char *ptr=NULL;
        if(flags==1){
        	fd = open(PATH,O_WRONLY);
            if(fd < 0){
            	std::cout << "open file failed" << std::endl;
            }
           
            ptr=const_cast<char*>(str.c_str());
            while(count>0){
            	ret=pwrite(fd,ptr+rw,count,offset);
                if(ret<0){
					std::cout <<"pwrite failed, error: " << errno
                        << ", errstr: " << strerror(errno) << std::endl;
                	send(connfd, "server write failed",SIZE, 0);
                	break;
              	}
                count-=ret;
                offset+=ret;
                rw+=ret;
           	}
           	send(connfd, "server write success!!!",SIZE, 0);
           	close(fd);
        }
        else if(flags==0){
        	struct stat st;
           	stat(PATH, &st);
           	if(st.st_size < end*BLOCKSIZE){
            	send(connfd, "server read over file size",SIZE, 0);
                continue; 
            }
            fd = open(PATH,O_RDONLY);
            if(fd < 0){
            	std::cout << "open file failed" << std::endl;
            }
           
            ptr=new char[count+1];
            while(count>0){
              	ret=pread(fd,ptr+rw,count,offset);
              	if(ret<0){
                	std::cout <<"pread failed, error: " << errno
                        << ", errstr: " << strerror(errno) << std::endl;
                	send(connfd, "server read failed",SIZE, 0);
                	break;
              	}
              	count-=ret;
              	offset+=ret;
              	rw+=ret;
          	}
           
           	std::string temp = ptr;
           	std::string buf = "read success!!!,context:" +temp;
           	ret = send(connfd,buf.c_str(),(end-start)*BLOCKSIZE+SIZE, 0);
           	std::cout <<"contextlen: " << ret <<",context: "  <<ptr <<  std::endl;
           	close(fd);
        }
        if(fd!=-1){
        	close(fd);
        }
        
    }  
      
    std::cout<< "client closed!"<<std::endl;  
    close(connfd);  //关闭已连接套接字  
}  

/************************************************************************ 
*语法格式：    void main(void)  
*实现功能：    主函数，建立一个TCP并发服务器  
*入口参数：    无  
*出口参数：    无  
************************************************************************/

int main(int argc, char *argv[])  
{  
	int sockfd = 0;             // 套接字  
    int connfd = 0;  
    int err = 0;  
    struct sockaddr_in my_addr; // 服务器地址结构体  
    pthread_t thread_id;  
    int fd = open("test",O_CREAT|O_RDWR,0644);
    if(fd < 0){
    	std::cout <<"creat file failed" <<std::endl;
    }
    close(fd);
    std::cout <<"TCP Server Started at port: "<< PORT << std::endl;  
      
    sockfd = socket(AF_INET, SOCK_STREAM, 0);   // 创建TCP套接字  
    if(sockfd < 0)  
    {  
        perror("socket error");  
        exit(-1);  
    }  
      
    bzero(&my_addr, sizeof(my_addr));      // 初始化服务器地址  
    my_addr.sin_family = AF_INET;  
    my_addr.sin_port   = htons(PORT);  
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
      
    // 绑定
    err = bind(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr));  
    if(err != 0)  
    {  
        perror("bind");  
        close(sockfd);        
        exit(-1);  
    }  
     // 监听，套接字变被动 
    err = listen(sockfd, 10);  
    if( err != 0)  
    {  
        perror("listen");  
        close(sockfd);        
        exit(-1);  
    }  
      
    std::cout << "Waiting client..." <<std::endl;  
      
    while(1)  
    {  
        char cli_ip[INET_ADDRSTRLEN] = "";     // 用于保存客户端IP地址  
        struct sockaddr_in client_addr;        // 用于保存客户端地址  
        socklen_t cliaddr_len = sizeof(client_addr);   // 必须初始化!!!  
        //获得一个已经建立的连接  
        connfd = accept(sockfd, (struct sockaddr*)&client_addr, &cliaddr_len);                                
        if(connfd < 0)  
        {  
            perror("accept ");  
            continue;  
        }  
        // 打印客户端的 ip 和端口
        inet_ntop(AF_INET, &client_addr.sin_addr, cli_ip, INET_ADDRSTRLEN);  
        std::cout<<"----------------------------------------------"<<std::endl;  
        std::cout<<"client ip=" << cli_ip <<",port=" << ntohs(client_addr.sin_port) << std::endl;
        int *conn = &connfd ; 
        if(connfd > 0)  
        {  
	    	//由于同一个进程内的所有线程共享内存和变量，因此在传递参数时需作特殊处理，值传递。
	    	pthread_create(&thread_id, NULL, Thread_fun, (void *)conn);  //创建线程  
            pthread_detach(thread_id); // 线程分离，结束时自动回收资源  
        }
    }  
      
    close(sockfd);  
      
    return 0;  
}  
