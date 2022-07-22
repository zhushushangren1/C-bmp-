#include <stdio.h>
#include "jpegToBmp.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUFFER_LEN 1024			 //收发缓存
#define SERV_PORT 8888			 //服务器端口
#define SERV_ADDR "172.18.31.34" //服务器ip
char recv_buf[BUFFER_LEN];		 //定义接收缓存区
char send_buf[BUFFER_LEN];		 //定义发送缓存区

int main(int argc, char **argv)
{
	int ret = -1;
	int sockfd = -1; //定义网络文件描述符
	FILE *fp = NULL; //定义文件操作指针

	struct sockaddr_in servaddr = {0}; //服务器sockaddr_in定义成ipv4类型的服务器ip结构体（ipv6是sockaddr_inv6）

	// 1.首先使用socket函数创建网络文件描述符（类似于文件IO中的open函数）
	//函数原型：int socket(int domain, int type, int protocol);
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // ipv4,TCP,系统自动选择protocol
	if (sockfd < 0)
	{
		printf("error :can not create\n");
		_exit(-1);
	}
	printf("sockfd=%d\n", sockfd);
	// 2.使用connect函数连接服务器
	//函数原型：int connect(int socket, const struct sockaddr *address,socklen_t address_len);
	servaddr.sin_family = AF_INET;					 //定义servaddr的domain地址族为ipv4
	servaddr.sin_port = htons(SERV_PORT);			 //定义servaddr的portnum为SERV_PORT(8010),host to net short
	servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR); //定义servaddr的address为SERV_ADDR(192.168.1.23)  person----->u32

	ret = connect(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret < 0)
	{
		printf("error :client connect error\n");
		_exit(-1);
	}
	printf("client connect success\n");

	//下面客户端和服务器互相收发
	while (1)
	{
		// 3.使用send函数发生数据
		printf("input recv bmpFile name: \n");
		scanf("%s", send_buf);
		if (!strncmp(send_buf, "+++", 3))
			break;										   //输入+++客户端断开连接
		ret = send(sockfd, send_buf, strlen(send_buf), 0); //给服务端发送请求的文件名
														   //接收服务端返回信息（文件是否存在）
		char file_name[100] = "./image/";
		strcat(file_name, send_buf);
		char message[7];
		recv(sockfd, message, 7, 0);
		printf("message:%s\n", message);
		if (message[0] == 'e') //文件不存在则continue
		{
			printf("error :server can not found file\n");
			continue;
		}
		//如果文件存在下一步
		fp = fopen("temp.jpeg", "wb");
		if (fp == NULL)
		{
			printf("error: temp.jpeg can not open!\n");
			exit(0);
		}
		printf("temp.jpeg create success,write data...\n");
		while (1)
		{
			// 4.使用recv函数接收来自服务端的消息
			ret = recv(sockfd, recv_buf, BUFFER_LEN, 0);
			if (ret < 1)
			{
				printf("Server disconnected\n");
				exit(0);
			}
			// printf("recvlen:%d\n",ret);
			//调用fwrite函数将recv_buf缓存中的数据写入文件中
			int write_length = fwrite(recv_buf, sizeof(char), ret, fp);
			if (write_length < ret)
			{
				printf("error :can not wirte file!\n");
				break;
			}
			// printf("From Server[%s] 接收成功! %d\n", SERV_ADDR,write_length);
			if (ret >= 3 && strcmp(recv_buf + ret - 3, "+++") == 0)
			{
				printf("jpeg Success!\n");
				break;
			}
			memset(send_buf, 0, sizeof(send_buf)); //清空接收缓存区
			memset(recv_buf, 0, sizeof(recv_buf)); //清空接收缓存区
		}
		fclose(fp);
		printf("jpeg to bmp...\n");
		FILE *input_file = fopen("./temp.jpeg", "rb");
		FILE *output_file = fopen(file_name, "wb");
		//开始对jpeg文件解压缩
		analyse_jpeg(input_file, output_file);
		printf("Success！\n");
		fclose(input_file);
		fclose(output_file);
		remove("temp.jpeg"); //删除临时文件
	}
	printf("quick out\n");
	close(sockfd); //关闭socket文件描述符
	return 0;
}
