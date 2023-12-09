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
