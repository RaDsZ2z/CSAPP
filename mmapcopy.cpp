#include <stdio.h>
#include <iostream>

#include <fcntl.h>    //open
#include <sys/mman.h> //mmap
#include <unistd.h>   //write
#include <sys/stat.h> //stat
using namespace std;
void mmapcopy(int fd, int size)
{
    char *bufp;
    bufp = (char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    // mmap：
    // 创建一个新的虚拟内存区域，将文件描述符fd指定的对象的一个连续的片映射到这个新的区域
    // 参数具体含义见 csapp P586
    write(1, bufp, size);
    // write：
    // 返回值：写入文档的字节数（成功）；-1（失败）
    // 第一个参数是写入文件的文件描述符 1代表标准输出
    // 第二个参数是指定的缓冲区，即指针，指向一段内存单元
    // 第三个参数是要写入文件指定的字节数

    return;
}
int main(int argc, char *argv[])
{
    struct stat stat;
    int fd;
    if (argc != 2)
    {
        printf("ERROR: argc != 2\n");
        exit(0);
    }
    fd = open(argv[1], O_RDONLY);
    fstat(fd, &stat); // 由文件描述符取得文件的状态 将信息存入结构体stat中
    mmapcopy(fd, stat.st_size);
    close(fd);
    exit(0);
}
