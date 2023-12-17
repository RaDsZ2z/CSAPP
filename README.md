本机配置:m2 macOS Sonoma 14.1.2

[lab官网](https://csapp.cs.cmu.edu/3e/labs.html)
# 1 环境配置
## 23.12.09
docker linux

试了centos和ubuntu 还是ubuntu更好一些(我也更熟悉一些)

[参考](https://blog.csdn.net/weixin_52693116/article/details/133149517)

相比这篇博客我把ubuntu版本换成了22.04

文件如果下载不了

右键 复制链接地址 在浏览器粘贴地址

## 23.12.16

ubuntu始终没法正常使用gdb，但是今天试了一下centos可以...

决定把环境换成centos，两个参考

https://blog.csdn.net/weixin_43252521/article/details/124409151

https://zhuanlan.zhihu.com/p/339047608

## 23.12.17 12:00

之前ubuntu的参数是

docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -it --platform linux/amd64 -v "/Users/rads/study/csapp:/csapp" --name=csapp ubuntu:22.04 /bin/bash

简单换成

docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -it --platform linux/amd64 -v "/Users/rads/study/csapp:/csapp" --name=csapp centos:latest /bin/bash

还是不能正常使用gdb

这么看来可能不是ubuntu的问题而是参数问题？

可以使用gdb的参数：

docker container run -it -v /Users/rads/study/csapp:/csapp --name=csapp centos /bin/bash

## 23.12.17 17:00

上面的配置可以“进一步”使用gdb了，但是还不够，我在台式机的vmware ubuntu22.04上试了一下，什么东西全都能用，我要破防了

docker有各种问题可能因为他权限不足 还有跟这个草蛋的m2芯片的适配问题

总之我还是决定装vmware

https://blog.csdn.net/suixinfeixiangfei/article/details/133802925

## 23.12.17 19:36

最终选择了这个

https://blog.csdn.net/luzhengyang2077/article/details/129910484

共享文件夹

https://blog.csdn.net/weixin_45042601/article/details/131286353
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
