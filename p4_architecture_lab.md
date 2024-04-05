# 1.简介
在这个实验室中，您将了解管道Y86-64处理器的设计和实现，优化它和基准程序以最大限度地提高性能。您可以对基准程序进行任何语义保留转换，或者对流水线处理器进行增强，或者两者兼而有之。当你完成实验室后，你会对影响程序性能的代码和硬件之间的交互产生强烈的欣赏。实验室分为三个部分，每个部分都有自己的手。在A部分中，您将编写一些简单的Y86-64程序，并熟悉Y86-64工具。在B部分中，您将使用新的指令扩展SEQ模拟器。这两个部分将为您准备C部分，即实验室的核心，在那里您将优化Y86-64基准程序和处理器设计。
# 2.Logistics
你将独立完成工作，对作业的任何澄清和修订都将被发布在课程网页上
# 3.Handout instructions
SITE-SPECIFIC：在这里插入一段话，解释学生应该如何下载archlab-handout.tar文件。

1.首先将文件archlab-handout.tar复制到您计划在其中进行工作的（受保护的）目录中。

2.然后给出命令：tar xvf archlab-handout.tar。这将导致以下文件被解压缩到目录中：README、Makefile、sim.tar、archlab.pdf和simguide.pdf。

3.接下来，给出命令tar xvf sim.tar。这将创建目录sim，其中包含Y86-64工具的个人副本。您将在该目录中完成所有工作。

4.最后，切换到sim目录，构建Y86-64工具：
```shell
unix> cd sim
unix> make clean; make
```
# 4.Part A
在这一部分中，您将在sim/misc目录中工作。您的任务是编写和模拟以下三个Y86-64程序。这些程序所需的行为是由examples.C中的示例C函数定义的。请确保在每个程序开头的注释中输入您的姓名和ID。您可以测试您的程序，方法是首先将它们与程序YAS进行汇编，然后使用指令集模拟器YIS运行它们。在所有Y86-64函数中，应该遵循x86-64惯例来传递函数参数、使用寄存器和使用堆栈。这包括保存和恢复您使用的任何被调用方保存寄存器。

**sum.ys：迭代求和链表元素**

编写一个Y86-64程序sum.ys，对链表的元素进行迭代求和。您的程序应该由一些代码组成，这些代码设置堆栈结构，调用函数，然后停止。在这种情况下，函数应该是函数（sum-list）的Y86-64代码，该函数在功能上等效于图1中的C sum-list函数。使用以下三个元素列表测试您的程序：
```s
# Sample linked list
.align 8
ele1:
    .quad 0x00a
    .quad ele2
ele2:
    .quad 0x0b0
    .quad ele3
ele3:
    .quad 0xc00
    .quad 0
```

```C
/* 
 * Architecture Lab: Part A 
 * 
 * High level specs for the functions that the students will rewrite
 * in Y86-64 assembly language
 */

/* $begin examples */
/* linked list element */
typedef struct ELE {
    long val;
    struct ELE *next;
} *list_ptr;

/* sum_list - Sum the elements of a linked list */
long sum_list(list_ptr ls)
{
    long val = 0;
    while (ls) {
	    val += ls->val;
	    ls = ls->next;
    }
    return val;
}

/* rsum_list - Recursive version of sum_list */
long rsum_list(list_ptr ls)
{
    if (!ls)
	    return 0;
    else {
	    long val = ls->val;
	    long rest = rsum_list(ls->next);
	    return val + rest;
    }
}

/* copy_block - Copy src to dest and return xor checksum of src */
long copy_block(long *src, long *dest, long len)
{
    long result = 0;
    while (len > 0) {
	    long val = *src++;
	    *dest++ = val;
	    result ^= val;
	    len--;
    }
    return result;
}
/* $end examples */

```
图1:Y86-64解决方案函数的C版本。参见sim/misc/examples.c

**rsum.ys：递归求和链表元素**

编写一个Y86-64程序rsum.ys，递归地对链表的元素求和。这个代码应该类似于sum.ys中的代码，只是它应该使用一个递归求和数字列表的函数rsum list，如图1中的C函数rsum列表所示。使用与测试list.ys相同的三元素列表来测试程序。

**copy.ys：将源块复制到目标块**

编写一个程序（copy.ys），将一个字块从存储器的一个部分复制到存储器的另一个（非重叠区域）区域，计算复制的所有字的校验和（Xor）。您的程序应该由设置堆栈框架、调用函数复制块然后停止的代码组成。该函数在功能上应与图1所示的C函数复制块等效。使用以下三个元素的源块和目标块测试程序：
```s
.align 8
# Source block
src:
    .quad 0x00a
    .quad 0x0b0
    .quad 0xc00
# Destination block
dest:
    .quad 0x111
    .quad 0x222
    .quad 0x333
```


## 4.1.sum.ys
参考博客、书上的代码以及文档，确定的代码框架是
```s
# sum.ys
.pos 0
        irmovq stack, %rsp
        call main
        halt
.align 8 #8字节对齐
ele1:
        #.quad用于初始化并分配一个8字节空间
        #类似的还有     .byte .short .int .long .word
        #分配的空间分别为1      2       4   4     ？   字节
        .quad 0x00a 
        .quad ele2
ele2:
        .quad 0x0b0
        .quad ele3
ele3:
        .quad 0xc00
        .quad 0
# write your code here

# write your code above here

.pos 0x200
stack:
```

```C
long sum_list(list_ptr ls)
{
    long val = 0;
    while (ls) {
	    val += ls->val;
	    ls = ls->next;
    }
    return val;
}
```
已经给出了三个元素组成的列表，容易看出这是链表的三个节点

每个元素前半部分是值，后半部分指向下一个元素或为空

补齐代码如下
```s
# sum.ys
.pos 0
        irmovq stack, %rsp
        call main
        halt
.align 8
ele1:
        .quad 0x00a
        .quad ele2
ele2:
        .quad 0x0b0
        .quad ele3
ele3:
        .quad 0xc00
        .quad 0
# write your code here
main:
        #main函数以链表头为参数调用sum_list函数
        irmovq ele1,%rdi #irmovq:将立即数传给寄存器，大小8字节
        call sum_list
        ret
sum_list:
        #将会rbx寄存器，暂存其中的值以便还原
        pushq %rbx 
        xorq %rax,%rax # %rax = 0 
loop:
        andq %rdi,%rdi
        je here #if(rdi == 0) goto here;
        mrmovq (%rdi),%rbx #mrmovq:将地址处的值传给寄存器，大小8字节
        addq %rbx,%rdx
        mrmovq 8(%rdi),%rdi
        jmp loop
here:   
        popq %rbx
        ret
# write your code above here

.pos 0x200
stack:
```

## 4.2.rsum.ys
我认为这个函数的代码自己写得很好，可读性、简洁度完全超越参考博客

代码框架与上一个相同：
```s
# rsum.ys
.pos 0
        irmovq stack, %rsp
        call main
        halt
.align 8 #8字节对齐
ele1:
        #.quad用于初始化并分配一个8字节空间
        #类似的还有     .byte .short .int .long .word
        #分配的空间分别为1      2       4   4     ？   字节
        .quad 0x00a 
        .quad ele2
ele2:
        .quad 0x0b0
        .quad ele3
ele3:
        .quad 0xc00
        .quad 0
# write your code here

# write your code above here

.pos 0x200
stack:
```

```C
long rsum_list(list_ptr ls)
{
    if (!ls)
	    return 0;
    else {
	    long val = ls->val;
	    long rest = rsum_list(ls->next);
	    return val + rest;
    }
}
```


```s
# rsum.ys
.pos 0
        irmovq stack, %rsp
        call main
        halt
.align 8 #8字节对齐
ele1:
        #.quad用于初始化并分配一个8字节空间
        #类似的还有     .byte .short .int .long .word
        #分配的空间分别为1      2       4   4     ？   字节
        .quad 0x00a 
        .quad ele2
ele2:
        .quad 0x0b0
        .quad ele3
ele3:
        .quad 0xc00
        .quad 0
# write your code here
main:
        #同样地，传参调用rsum函数
        irmovq ele1,%rdi
        call rsum_list
        ret
rsum_list:
        andq %rdi,%rdi
        je ret_
        mrmovq (%rdi),%rbx      #val = ls->val
        rrmovq 0x8(%rdi),%rdi   #ls = ls -> next
        #递归之前保存寄存器
        pushq %rbx
        call rsum_list          #rest = rsum_list(ls)
        #递归之后恢复寄存器
        popq %rbx
        #递归回溯之后没有用到%rdi故无需恢复其值
        addq %rbx,%rax          #return val + rest
        ret
ret_
        xorq %rax, %rax
        ret
# write your code above here
.pos 0x200
stack:
#完美的代码  精妙绝伦
```
## 4.3.copy.ys
代码框架如下
```s
# rsum.ys
.pos 0
        irmovq stack, %rsp
        call main
        halt
.align 8 #8字节对齐
# Source block
src:
        .quad 0x00a
        .quad 0x0b0
        .quad 0xc00
# Destination block
dest:
        .quad 0x111
        .quad 0x222
        .quad 0x333
# write your code here
        
# write your code above here

.pos 0x200
stack:
```

```C
long copy_block(long *src, long *dest, long len)
{
    long result = 0;
    while (len > 0) {
	    long val = *src++;
	    *dest++ = val;
	    result ^= val;
	    len--;
    }
    return result;
}
```
```s
# rsum.ys
.pos 0
        irmovq stack, %rsp
        call main
        halt
.align 8 #8字节对齐
# Source block
src:
        .quad 0x00a
        .quad 0x0b0
        .quad 0xc00
# Destination block
dest:
        .quad 0x111
        .quad 0x222
        .quad 0x333
# write your code here
# 首先回顾传参的寄存器，前三个分别是rdi，rsi，rdx(见 杂项.md 2.)
main:
        irmovq src,%rdi
        irmovq dest,%rsi
        irmovq $3,%rdx
        call copy
        ret
copy:
        xorq %rax,%rax
loop:
        testq %rdx
        je ret_

        #记得没有内存到内存的指令，要用寄存器中转
        #下面这个写法是错误的(但是方便)
        mmmovq (%rdi),(%rsi) 
        #同样，也不能直接有立即数，下面的写法是错误的
        addq $0x8,%rdi
        addq $0x8,%rsi
        subq $0x1,%rdx
        jmp loop
ret_:
        ret
# write your code above here

.pos 0x200
stack:
```

# 5.Part B
在这一部分中，您将在sim/seq目录中工作。您在B部分的任务是扩展SEQ处理器以支持iaddq，如家庭作业问题4.51和4.52中所述。要添加此说明，您将修改文件seq-full.hcl，该文件实现了CS:APP3e教科书中描述的seq版本。此外，它还包含解决方案所需的一些常量的声明。您的HCL文件必须以包含以下信息的标头注释开头：

•您的姓名和身份证。

•iaddq指令所需计算的描述。使用CS:APP3e文本中图4.18中对irmovq和OPq的描述作为指南。

**构建和测试您的解决方案**

完成对seq-full.hcl文件的修改后，您将需要基于该hcl文件构建一个新的seq模拟器（ssim）实例，然后对其进行测试：

暂略


在重读了书的第四章之后我明白了要做的事情是什么...

seq-full.hcl是一个指令集文件，没有考虑iaddq这个指令。

现在指令集中加入了iaddq这个指令，检查seq-full.hcl文件中的每个变量，对应地修改它

那就把所有指令拿出来一个个看
```shell
#参照书P277图4-26 iaddq的指令名称应该是 IIADDQ

# Determine instruction code
word icode = [
	imem_error: INOP;
	1: imem_icode;		# Default: get from instruction memory
];
#不需要修改


# Determine instruction function
word ifun = [
	imem_error: FNONE;
	1: imem_ifun;		# Default: get from instruction memory
];
#不需要修改


bool instr_valid = icode in 
	{ INOP, IHALT, IRRMOVQ, IIRMOVQ, IRMMOVQ, IMRMOVQ,
	IOPQ, IJXX, ICALL, IRET, IPUSHQ, IPOPQ, IIADDQ };
#IIADDQ是合法的可执行指令，加在后面


# Does fetched instruction require a regid byte?
bool need_regids =
	icode in { IRRMOVQ, IOPQ, IPUSHQ, IPOPQ, 
		     IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ };
#IIADDQ需要用到寄存器，加在后面


# Does fetched instruction require a constant word?
bool need_valC =
	icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IJXX, ICALL, IIADDQ };
#指令中有立即数就需要用到valC，加在后面


################ Decode Stage  译码

## What register should be used as the A source?
word srcA = [
	icode in { IRRMOVQ, IRMMOVQ, IOPQ, IPUSHQ  } : rA;
	icode in { IPOPQ, IRET } : RRSP;
	1 : RNONE; # Don't need register
];
#IIADDQ第一个参数是立即数而不是寄存器，不会用到rA
#p278 rA是第一个寄存器，rB是第二个寄存器(registerA registerB)
#p279 从rA读出的是valA 从rB读出的是valB
#顺便一提 valC是指令中的立即数

## What register should be used as the B source?
word srcB = [
	icode in { IOPQ, IRMMOVQ, IMRMOVQ, IIADDQ  } : rB;
	icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
	1 : RNONE;  # Don't need register
];
#IIADDQ第二个参数是寄存器，会用到rB

## What register should be used as the E destination?
word dstE = [
	icode in { IRRMOVQ } && Cnd : rB;
	icode in { IIRMOVQ, IOPQ, IIADDQ} : rB;
	icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
	1 : RNONE;  # Don't write any register
];
#写回阶段会用到rB
#P279 dstE和dstM是写回阶段的写端口地址输入



## What register should be used as the M destination?
word dstM = [
	icode in { IMRMOVQ, IPOPQ } : rA;
	1 : RNONE;  # Don't write any register
];
#用不到rA
################ Execute Stage   执行

## Select input A to ALU
word aluA = [
	icode in { IRRMOVQ, IOPQ } : valA;
	icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ } : valC;
	icode in { ICALL, IPUSHQ } : -8;
	icode in { IRET, IPOPQ } : 8;
	# Other instructions don't need ALU
];
#相加的第一个操作数来自指令中的立即数 即valC

## Select input B to ALU
word aluB = [
	icode in { IRMMOVQ, IMRMOVQ, IOPQ, ICALL, 
		      IPUSHQ, IRET, IPOPQ, IIADDQ } : valB;
	icode in { IRRMOVQ, IIRMOVQ } : 0;
	# Other instructions don't need ALU
];
#相加的第二个操作数来自从第二个参数所指的寄存器中的值 即valB

## Set the ALU function
word alufun = [
	icode == IOPQ : ifun;
	1 : ALUADD;
];
#这个是干嘛的？
#ALU需要两个操作数和一个alufun信号，alufun信号用于指明ALU对两个操作符执行怎样的逻辑运算（加、减、与、异或）
#仅当指令为IOPL指令（即运算指令）时，alufun由ifun决定，其它情况下ALU全部当做加法器来使用



## Should the condition codes be updated?
bool set_cc = icode in { IOPQ, IIADDQ };
#cc:condition code条件码  会修改条件码

################ Memory Stage    访存
#不涉及读写内存，访存阶段都没有要改的

## Set read control signal
bool mem_read = icode in { IMRMOVQ, IPOPQ, IRET };

## Set write control signal
bool mem_write = icode in { IRMMOVQ, IPUSHQ, ICALL };

## Select memory address
word mem_addr = [
	icode in { IRMMOVQ, IPUSHQ, ICALL, IMRMOVQ } : valE;
	icode in { IPOPQ, IRET } : valA;
	# Other instructions don't need address
];

## Select memory input data
word mem_data = [
	# Value from register
	icode in { IRMMOVQ, IPUSHQ } : valA;
	# Return PC
	icode == ICALL : valP;
	# Default: Don't write anything
];

## Determine instruction status
word Stat = [
	imem_error || dmem_error : SADR;
	!instr_valid: SINS;
	icode == IHALT : SHLT;
	1 : SAOK;
];

################ Program Counter Update 更新PC

## What address should instruction be fetched at

word new_pc = [
	# Call.  Use instruction constant
	icode == ICALL : valC;
	# Taken branch.  Use instruction constant
	icode == IJXX && Cnd : valC;
	# Completion of RET instruction.  Use value from stack
	icode == IRET : valM;
	# Default: Use incremented PC
	1 : valP;
];
#不需要修改
```
嗯 发现指令集没有写回阶段
# 5.Part C
似乎是一个优化任务

看起来很麻烦而且环境也有问题 先跳过
