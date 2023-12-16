[lab官网](https://csapp.cs.cmu.edu/3e/labs.html)
# 1 环境配置
docker linux

试了centos和ubuntu 还是ubuntu更好一些(我也更熟悉一些)

[参考](https://blog.csdn.net/weixin_52693116/article/details/133149517)

相比这篇博客我把ubuntu版本换成了22.04

文件如果下载不了

右键 复制链接地址 在浏览器粘贴地址
# 2 gdb
```shell
#生成debug可执行文件test
g++ -g test.cpp -o test
#反汇编生成汇编文件test.asm
objdump -d test > test.asm
```
gdb模式下：
```shell
x 0x6044e4 打印0x6044e4这个地址的值
x/x 以16进制
x/d 以10进制
x/s 以字符串

p 0x123 打印0x123
p *0x123 打印0x123这个地址的值
p *(int*)0x123 打印0x123这个地方的指针指向的地址的值
p/x 16进制
p/d 10进制
p/s 字符串(不确定)
```
