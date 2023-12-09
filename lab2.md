每一个phase的cpp代码都是相似的，比如第一个
```cpp
    input = read_line();             /* Get input                   */
    phase_1(input);                  /* Run the phase               */
    phase_defused();                 /* Drat!  They figured it out!
				      * Let me know how they did it. */
    printf("Phase 1 defused. How about the next one?\n");

```
# phase1
```cpp
//汇编代码翻译 共8行
rsp -= 8;
esi = 0x402400;
call 401338<strings_not_equal>;
if（eax == 0)
goto 400ef7;
call 40143a<explode_bomb>;
    400ef7
*rsp += 8;
ret;
```
知识回顾：


<img width="556" alt="image" src="https://github.com/RaDsZ2z/CSAPP/assets/129292565/ba5b30ec-afe0-4913-8101-6112259ba412">

esi应该是被作为函数401338的传入参数了

0x402400应该是输入的字符串地址值

在函数调用结束后，根据寄存器eax值(也就是返回值)决定是否调用<explode_bomb>
```shell
gdb bomb
x/s 0x402400
#或者
print (char*)0x402400
#得到字符串
#Border relations with Canada have never been better.
```
# phase2
```cpp
//汇编代码翻译 共25行
push rbp;
push rbx;
rsp -= 0x28;
rsi = rsp;
call 40145c<read_six_numbers>;
if((%rsp) == 1)
goto 400f30<phase_2+0x34>;//值必须是1 否则爆炸
call 40143a<explode_bomb>;
goto 400f30<phase_2+0x34>;
    400f17:<not code,indicate address>
{
    //这个大括号是一个循环体
//现在rbx=4+rsp和 rbp=0x18+rsp
eax = (%rbx-4);//eax = (%rsp)
eax += eax;//eax = 2*(%rsp)
if(eax == (%rbx))
goto 400f25<phase_2+0x29>;
//值是rsp指向的上一个位置的值的两倍就goto 否则爆炸
call 40143a<explode_bomb>;
    400f25:<not code,indicate address>
rbx += 4;//偏移四个字节
if(rbx != rbp)//如果没有等于rsp+0x18就goto 400f17
goto 400f17<phase_2+0x1b>;
}
/*
    if( (%rsp!=1) ) explode)bomb();
    rbx = rsp + 4;
    rbp = rsp + 24;
    while(1){
        eax = (%rbx -4 ) * 2;
        if(eax != (%rbx)) explode_bomb();
        rbx += 4;
        if(rbx == rbp) break;

        rbx=4: 是上一个两倍否则bomb rbx=8
        rbx=8: 是上一个两倍否则bomb rbx=12
        rbx=12: 是上一个两倍否则bomb rbx=16
        rbx=16: 是上一个两倍否则bomb rbx=20
        rbx=20: 是上一个两倍否则bomb rbx=24
        break;
        /总共6个数 1 2 4 8 16 32
    }
*/
goto 400f3c<phase_2+0x40>;
    400f30:<not code,indicate address>
rbx = 4 + rsp;
rbp = 0x18 + rsp;
goto 400f17<phase_2+0x1b>;
    400f3c:<not code,indicate address>
rsp += 0x28;
pop rbx;
pop rbp;
return ;
```



# phase3
```cpp
//汇编代码翻译 共36行
rsp -= 0x18;//24
rcx = (rsp + 0xc);//12
rdc = (rsp + 0x8);//8
esi = 0x4025cf;
eax = 0;
call 400bf0 <isoc99_sscanf>
if(eax > 1)
goto 400f6a;//返回值>1则跳过爆炸
call 40143a<explode_bomb>
    400f6a:
if( (%rsp + 8) - 7 > 0 )
goto 400fad;//返回值>7则爆炸
eax = (%rsp + 8);//取(rsp+8)这个地方的值
//此时eax <= 7
goto *(0x402470+8*rax);//注意是二级跳转
// *(0x402470+8*(%rsp + 8))
/*
这里相当于是一个指针数组
在gbd中使用指令获得rax对应的地址的值
p/x *(int*)(0x402470+8*rax)
rax     goto
0       0x400f7c
1       0x400fb9
2       0x400f83
3       0x400f8a
4       0x400f91
5       0x400f98
6       0x400f9f
7       0x400fa6
8       0x7564616d
*/
    //rxa = 0:
eax = 0xcf;
goto 400fbe;
    //rax = 2:
eax = 0x2c3;
goto 400fbe;
    //rax = 3:
eax = 0x100;
goto 400fbe;
    //rax = 4:
eax = 0x185;
goto 400fbe;
    //rax = 5:
eax = 0xce;
goto 400fbe;
    //rax = 6:
eax = 0x2aa;
goto 400fbe;
    //rax = 7:(?)
eax = 0x147;
goto 400fbe;

    400fad:
call 40143a<explode_bomb>
eax = 0;
goto 400fbe;
    //rax = 1:
eax = 0x137;

    400fbe:
if(eax == (rsp+0xc))
goto 400fc9;
//eax是“这个值”就不会爆炸 但是不知道“这个值”
//但是好像只剩7 别的值走不到这里
call 40143a<explode_bomb>
    400fc9:
rsp += 0x18;
return ;
```
```shell
(gdb) x/s 0x4025cf #得知输入了两个数
```
两个数的位置分别应该是%rsp+8 和 %rsp+0xc

%eax = (%rsp+8)  它就是第一个数

要想炸弹不爆炸只能使最后一个判断成立

eax有8个值 对应第二个数也有八个值

## 3.1关于jmp
```
jmp %rsp
%rsp存储了地址值x，跳转到x

jmp *%rsp
%rsp存储了地址值x，取出x
再取出x处存储的地址值y，跳转到y
```
## 3.2关于cmp

cmp src1,src2

是根据src1-src2来设置条件码的，跟sub是相反的

但是，这来自[intel](https://www.felixcloutier.com/x86/)，它跟csapp书中的设置是相反的


<img width="555" alt="image" src="https://github.com/RaDsZ2z/CSAPP/assets/129292565/067f1a8b-5230-487c-8c52-604e70b3373b">


根据代码逻辑(一些合理猜测)，此处是用src2-src1来设置条件码的

## 3.3关于gdb
```
(gdb) p/x ()    #输出变量值
(gdb) x/s ()    #以字符流显示内存中的内容
```

# phase4
```cpp
//汇编代码翻译func4 共22行
//被传入的参数:edi=(rsp+0x8)(第一个值)    esi=0 
//edx=0xe rcx=rsp+0xc(此函数被调用前第二个值的位置)
rsp -= 0x8
eax = edx
eax -= esi
//eax = edx - esi
ecx = eax
ecx >>= 31 //逻辑右移 高位补0
//ecx = eax >> 31
eax += ecx
//eax += eax >> 31

eax >>= 1 //算数右移 补符号位 猜测位移量为1
//上面总结成三行
//eax = edx - esi;
//eax += eax >> 31;
//eax >>= 1;

//eax = (edx - esi + sign(edx-esi) )/2
ecx = rax + rsi
//ecx = eax + esi
//ecx = (edx - esi + sign(edx-esi)) / 2 + esi

//ecx = (edx - esi) / 2 + esi  */2向0取整

//ecx是根据输入的第二个参数和第三个参数得到的
//跟它进行比较的是第一个参数
if(ecx - edi <= 0)
goto 400ff2
edx = rcx - 1
call 400fce<func>
eax += eax
goto 401007

    400ff2:
eax = 0
if(ecx - edi >= 0)
goto 401007
esi = rcx + 1
call 400fce<func>
eax = 1 + 2 * rax
    401007:
rsp += 0x8
return
```
```cpp
//进一步翻译func
//edi    esi    edx    rcx
//int a, int b, int c, int* d
int t = (c - b) / 2 + b;// /2向0取整
if(t - a <=0){
    eax = 0;
    if(t - a >=0){
        return ;//t==a时 才能结束递归
    }
    else{
        //t < a
        b = d + 1;
        call 400fce<func>;
        eax = 1 + 2 * rax;
    }
}else{
    //t > a
    c = d - 1;
    call 400fce<func>;
    eax += eax;
}
//再翻译
int t = (c - b) / 2 + b;// /2向0取整
if( t == a ) {
    eax = 0;
}
else if( a < t ) {
    c = d - 1;
    func()
    eax = 2 * eax;
}
else if( a > t ) {
    b = d + 1;
    func();
    eax = 2 * rax +1;
}

//假设输入的两个值为x y
func(x,0,0xe,rsp+0xc){
    //a=x b=0 c=0xe d=rsp+0xc
    //1  由函数main调用
    int t = (c - b) / 2 + b;// /2向0取整
    t = 7;
    //假设x < 7
    c = rsp + 0xb;
    func(x,0,rsp+0xb,rsp+0xc);
    //假设x > 7
    b = rsp + 0xd;
    func(x,rsp + 0xd,0xe,rsp+0xc);
    //不等于7会陷入无限递归，因此x=7
    //返回值为0
}
func(x,0,rsp+0xb,rsp+0xc){
    //2 由1调用  已知x < 7
    int t = (c - b) / 2 + b;// /2向0取整
    t = (rsp+0xb)/2;
    //认为rsp很大 即 t很大
    //x既然小于7了 也小于现在的t
    //认为x < 7
    c = d - 1 = rsp + 0xb;
    func(x,0,rsp+0xb,rsp+0xc);//会陷入无限递归
    
}
func(x,rsp + 0xd,0xe,rsp+0xc){
    //3 由1调用 已知x > 7
    int t = (c - b) / 2 + b;// /2向0取整
    t = (0xe - 0xd - rsp)/2;
    //t < 0 且 x >7 
    //认为 x > 7
    b = rsp + 0xd;
    func(x,rsp+0xd,0xe,rsp+0xc);//会陷入无限递归
}

```

```cpp
//汇编代码翻译 共22行
rsp -= 0x18
rcx = rsp + 0xc//参数4
rdx = rsp + 0x8
esi = 0x4025cf
eax = 0
call 400bf0<isoc99_sscanf>
//到这里 读入了两个数 分别在0x8 0xc的位置
if(eax != 2)
goto 401035//输入的数不是2个就引爆

if( (rsp + 0x8) - 0xe <= 0 )
goto 40103a//第一个数 <= 0xe 否则引爆

    401035:
call 40143a<explode_bomb>
    40103a:
edx = 0xe//edx 参数3
esi = 0//esi 参数2
edi = (rsp + 8)//edi 参数1
//下面是调用函数
call 400fce<func4>
if(eax != 0)//返回值不为0则引爆
goto 401058

if( (rsp + 0xc) == 0 )
goto 40105d//这个值为0则安全 这个值正是输入的第二个值

    401058:
call 40143a<explode_bomb>
    40105d:
rsp += 0x18
return
/*
综上答案为7 0
*/
```
# phase5

```cpp
// 参数
// rdi  rsi rdx rcx
// edi  esi edx ecx
// di   si  dx  cx
// dil  sil dl  cl
//汇编代码翻译 共40行
push rbx
rsp -= 0x20
rbx = rdi//rdi是传进来的第一个参数
rax = %fs:0x28
(rsp + 0x18) = rax//把一个特殊值放入栈中
eax  = 0
call 40131b<string_length>
if(6 == eax)//rdi 指向的字符串长度为6
goto 4010d2 //字符串长度为6 否则爆炸
explode_bomb()
goto 4010d2
//下面这段循环了六次 每次取一个字符？
//把eax置0 跳回来往下执行
    40108b:
//rbx应该指向输入的字符串
ecx = (rbx + rax) //ecx是字符串其中一个字符？
//但是32位最多可以装4个字符？
(rsp) = cl
rdx = (rsp)
//rdx = cl  取ecx低8位
edx &= 0xf//取低ecx的4位
rdx =(0x4024b0 + rdx) //rdx属于[4024b0,4024bf]
(0x10 + rsp + rax) = dl
//((0x10 + rsp + rax))属于[b0,bf]
//应该是一个字符数组
//rsp+[10,11,12,13,14,15]
//    [b_,b_,b_,b_,b_,b_]
//1011_ _ _ _
//1011 = 16+32-128 = 48
rax++
if(6 != rax)
goto 40108b
(rsp+0x16) = 0 (movb 只影响一个字节)
//确实是一个字符数组了，0是字符串结束符
esi = 0x40245e //x/s 0x40245e -> "flyers"
rdi = rsp + 0x10
call 401338<strings_not_equal>
if(eax == 0)
goto 4010d9
/*
esi和rdi两个字符串要相等
esi已经知道了

0x4024b0为基址，偏移之后取字符
取到esi中的字符的偏移量作为低4位，0xb作为高四位
得到的字符就是rdi对应位置的字符

x/s 0x4024b0 查看字符串
可以知道得到六个字符对应是偏移量分别是
9,0xf,0xe,5,6,7
	int s[]={9,0xf,0xe,5,6,7};
	for(int i=0;i<6;i++){
		printf("%c",s[i]+48);
	}
得到字符串9?>567
*/
explode_bomb()
nopl 0x0(%rax,%rax,1) (?这是一句空指令吗)
goto 4010d9
    4010d2:
eax = 0
goto 40108b
    4010d9:
rax = (0x18 + rsp)
rax ^= %fs:0x28

if(rax == 0) goto 4010ee
call 400b30<stack_check_fail>
    4010ee
rsp += 0x20
pop rbx
return
```
# phase6
```cpp
//汇编代码翻译 共87行
push r14
push r13
push r12
push rbp
push rbx
rsp -= 0x50
r13 = rsp
rsi = rsp
call 40145c<read_six_numbers>//读入的是六个数字
r14 = rsp
r12d = 0
    401114:
    //r13 = rsp
    //r13 = rsp + 4
rbp = r13
eax = (r13)
//eax指向第一个值
//eax指向第二个值
eax -= 1
if( eax - 5 <= 0 )
goto 401128
//读入的第一个数字要<=6 否则bomb
//读入的第二个数字要<=6 否则bomb
explode_bomb()
    401128:
r12d += 1
//=1
//=2
if( r12d - 6 == 0 )
goto 401153
ebx = r12d//=1 =2
{
    //这里有一段循环
    //执行了五次

    401135:
rax = ebx//=1 =2
eax = (rsp + 4 * rax)
if((rbp) - eax != 0)
//第2～6个数 不能跟第一个数一样
//第3～6个数 不能跟第一个数一样
//最后这六个数要各不相同，并且都<=6
goto 401145
explode_bomb()
    401145:
ebx += 1
//ebx = 2 3 4 5 6
if(ebx - 5 <= 0)
goto 401135
}
r13 += 4
//=rsp+4(指向输入的第二个数)
goto 401114
    401153:
rsi = rsp + 0x18//紧挨着存放六个数字后面的位置
rax = r14//rax=r14=rsp
ecx = 0x7
{
    //这段循环使六个数字被7减
    //也就是x变成7-x
    401160:
edx = ecx
edx -= (rax)
(rax) = edx
rax += 0x4
if(rax - rsi != 0)
goto 401160
}
esi = 0
goto 401197


    401176:
    //跳转到这里时
    //eax=1 edx=0x6032d0
rdx = (rdx + 8)
eax++
if(eax - ecx != 0)
goto 401176
//链表上往后跳ecx-1次
//也就是 使rdx指向链表上第exc个值？
// $0 = 0x6032d0
// (gdb) p/x *(int*) 0x6032d8
// $1 = 0x6032e0
// (gdb) p/x *(int*) 0x6032e8
// $2 = 0x6032f0
// (gdb) p/x *(int*) 0x6032f8
// $3 = 0x603300
// (gdb) p/x *(int*) 0x603308
// $4 = 0x603310
// (gdb) p/x *(int*) 0x603318
// $5 = 0x603320
// (gdb) p/x *(int*) 0x603328
// $6 = 0x0
/*
v,p
v,p
v,p
v,p
v,p
v,p
null
*/
goto 401188
    401183:
edx = 0x6032d0
    401188:
    //此时rsi=0
(0x20 + rsp + 2 * rsi) = rdx
//把链表上的六个数以某种顺序排在这里
//间隔为8
//(0x20 + rsp) = 0x6032d0
rsi += 4//rsi=4
if( rsi - 0x18 == 0)
goto 4011ab//上面判断六次之后 执行这里的跳转
    401197:
ecx = (rsp + rsi)
//ecx=(rsp)=7-num1
//ecx=(rsp+4)=7-num2
{
    //若ecx为1
    //rdx=0x6032d0执行对应代码
    //否则使rdx为链表上第ecx个值
    //再执行对应代码
    //对应代码指 401188
    //最终效果是，把链表上的六个数重新排列了一下
    //间隔还是8字节
    //排列的顺序由数组决定
if(ecx - 1 <= 0)
goto 401183
eax = 1
edx = 0x6032d0
goto 401176
}

    4011ab:

//20 28 30 38 40 48
rbx = (rsp + 0x20)//rbx是重排后的第一个值
rax = rsp + 0x28//指向重排后的第二个值
rsi = rsp + 0x50//指向数组末尾
rcx = rbx//重排后第一个值
{
    4011bd:
rdx = (rax)
//rdx=重排后第二个值
//rdx=重排后第二个值

(rcx + 0x8) = rdx
//重排后第一个值的指针 指向 重排后第二个值

rax += 0x8
//指向重排后的第三个值

if(rax - rsi == 0)
goto 4011d2//rax指向数组尾就break

rcx = rdx
//rcx=重排后第二个值
goto 4011bd
}
    4011d2:
(rdx + 0x8) = 0 (movq)
//重排后最后一个值的指针 指向0

ebp = 5
{
    4011df:
rax = (8 + rbx)//重排后第一个值指向的值 即第二个值
eax = (rax)
//eax=第二个值指向的地址的值
//rax只修改了低32位？
if( (rbx) - eax >= 0)
goto 4011ee
//第一个值指向的地址的值 >= 第二个值指向的地址的值 则 跳过爆炸
explode_bomb()
    4011ee:
rbx = (8 + rbx)
//现在rbx是重排后的第二个值
ebp--
//ebp=4

//需要重排后的数组 各元素指向的地址的值是非升序
//重排前元素值如下
//[0x6032d0,0x6032e0,0x6032f0,0x603300,0x603310,0x603320]
//它们指向的地址的值如下
//[0x14c,0xa8,0x39c,0x2b3,0x1dd,0x1bb]
//[332,168,924,691,477,443]
//降序排序
//[924,691,477,443,332,168]
//变换数组
//[3,4,5,6,1,2]
//对7取补
//[4,3,2,1,6,5]
//答案即为4 3 2 1 6 5
if(ebp != 0) goto 4011df
}
rsp += 0x50
pop rbx
pop rbp
pop r12
pop r13
pop r14
return 
```


<img width="556" alt="image" src="https://github.com/RaDsZ2z/CSAPP/assets/129292565/7ed6a42e-532c-47e5-9878-c8163841c51d">

# answer
```
answer1:
Border relations with Canada have never been better.
answer2:
1 2 4 8 16 32
answer3:
0 207
answer4:
7 0
answer5:
9?>567
answer6:
4 3 2 1 6 5
```
