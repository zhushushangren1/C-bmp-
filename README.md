功能：
主要是实现两台 Linux 机器之间的 bmp 文件传输，建立连接之后，客户端向服务端发送请求的文件
服务端收到请求后查找本地是否有此文件，如果发现本地没有此文件返回一个 notfound 如果有
先将 bmp 文件压缩为 jpeg 格式，然后将 jpeg 格式的数据发送给客户端
客户端收到数据后，解压缩为 bmp 格式
到此一个流程结束

## 运行之前先把服务端和客户端的 ip 和端口都改为服务端对应的

## 共两个部分 server 和 client

## 分别部署在两台 Linux 虚拟机上

## 先运行 server 在运行 client

## 每个都写了 makefile 文件，直接执行 make 命令即可