readme:

At CMU we use this lab in place of Performance Lab.

用这个lab替换Performance Lab，不要再做Performance Lab了

# 1.Logistics
你需要在64位的x86-64机器上运行这个项目

# 2.Overview
这个实验室将帮助您了解缓存内存对C程序性能的影响。实验室由两部分组成。在第一部分中，您将编写一个小的C程序（大约200-300行），模拟缓存的行为。

在第二部分中，您将优化一个小型矩阵转置函数，目标是最大限度地减少缓存未命中的数量
# 3. Downloading the assignment
下载解压之后进入文件夹，修改csim.c和trans.c

```shell
make clean
make
```
警告：不要让Windows WinZip程序打开.tar文件（许多Web浏览器都设置为自动打开）。相反，将文件保存到Linux目录中，然后使用Linux tar程序提取文件。一般来说，对于这个类，您永远不应该使用Linux以外的任何平台来修改您的文件。这样做可能会导致数据丢失（以及重要工作！）
# 4.description
实验室分为两部分。在A部分中，您将实现一个缓存模拟器。在B部分中，您将编写一个针对缓存性能进行优化的矩阵转置函数。
## 4.1.引用trace文件
讲义目录的traces子目录包含一组参考跟踪文件，我们将使用这些文件来评估您在a部分中编写的缓存模拟器的正确性。跟踪文件由一个名为valgrind的Linux程序生成。例如，键入
```shell
linux> valgrind --log-fd=1 --tool=lackey -v --trace-mem=yes ls -l
```
在命令行上运行可执行程序“ls-l”，按照发生的顺序捕获每个内存访问的跟踪，并将其打印到stdout上。

Valgrind记忆痕迹具有以下形式：

```
I 0400d7d4,8
 M 0421c7f0,4
 L 04f6b868,8
 S 7ff0005c8,8
```

每一行表示一次或两次存储器访问。每行的格式为
```
[space]operation address,size
```
操作字段表示内存访问的类型：“I”表示指令加载，“L”表示数据加载，“S”表示数据存储，“M”表示数据修改（即数据加载后接数据存储）。

每个“I”前面从来没有空格。在每个“M”、“L”和“S”之前总是有一个空格。地址字段指定64位十六进制内存地址。大小字段指定操作访问的字节数
## 4.2.Part A:编写缓存模拟器
在A部分中，您将在csim.c中编写一个缓存模拟器，该模拟器以valgrind内存跟踪作为输入，模拟缓存在该跟踪上的命中/未命中行为，并输出命中、未命中和驱逐的总数

我们为您提供了一个名为csim-ref的引用缓存模拟器的二进制可执行文件，该模拟器模拟valgrind跟踪文件上具有任意大小和关联性的缓存的行为。在选择要收回的缓存线时，它使用LRU（最近最少使用）替换策略。引用模拟器采用以下命令行参数：
```shell
Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>
•-h：打印使用信息的可选帮助标志
•-v：显示跟踪信息的可选详细标志
•-s<s>：集合索引位数（S = 2^s为集合数）
•-E<E>：关联性（每个集合的行数）
•-b<b>：块位数（B = 2^b为块大小）
•-t＜tracefile＞：要回放的valgrind跟踪的名称
#cache有S个set，每个set有E行，每行是 有效位+t bits偏移位+B bit缓存位

```
命令行参数基于CS:APP2e教科书第597页中的符号（s、E和b）。例如
```shell
linux> ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace
hits:4 misses:5 evictions:3
```
详细模式下的相同示例：
```shell
linux> ./csim-ref -v -s 4 -E 1 -b 4 -t traces/yi.trace
L 10,1 miss
M 20,1 miss hit
L 22,1 hit
S 18,1 hit
L 110,1 miss eviction
L 210,1 miss eviction
M 12,1 miss eviction hit
hits:4 misses:5 evictions:3
```
A部分的工作是填充csim.c文件，以便它采用与参考模拟器相同的命令行参数并产生相同的输出。请注意，此文件几乎完全为空。你需要从头开始写。

**Part A的编程规则**
1.在csim.c的标题注释中包括您的姓名和登录ID

2.您的csim.c文件必须在没有警告的情况下编译才能获得信用。

3.您的模拟器必须对任意的s、E和b正确工作。这意味着您需要使用malloc函数为模拟器的数据结构分配存储。键入“man malloc”以获取有关此函数的信息。

4.对于这个实验室，我们只对数据缓存性能感兴趣，所以您的模拟器应该忽略所有指令缓存访问（以“I”开头的行）。回想一下，valgrind总是把“I”放在第一列（前面没有空格），把“M”、“L”和“S”放在第二列（前面有空格）。这可能有助于您解析跟踪。

5.要获得A部分的积分，您必须在主函数printSummary（hit_count、miss_count、ejection_count）末尾调用函数printSummary，其中包含命中、未命中和驱逐的总数；

6.在这个实验室中，您应该假设内存访问是正确对齐的，这样单个内存访问就不会跨越块边界。通过进行此假设，您可以忽略valgrind跟踪中的请求大小。

## 4.3.Part B:优化矩阵转置
关于任务B，你将在trancs.c中写一个转置函数，尽可能少地导致 cache misses。
定义一个矩阵 A $A_{ij}$是其第i行第j列的元素，A的转置$A^T$满足$A_{ij}=A^T_{ji}$

trans.c中已经有一个写好的转置函数trans，它是对的，但是会导致很多cache misses。请你自己实现transpose_submit尽可能少地导致cache missess。

不要修改函数名

**Programming Rules for Part B**
•在trans.c的标题注释中包括您的姓名和登录ID。

•您在trans.c中的代码必须在没有警告的情况下编译才能获得信用。

•每个转置函数最多允许定义12个int类型的局部变量。

•您不允许通过使用任何类型为long的变量或使用任何位技巧将多个值存储到单个变量来绕过先前的规则。

•转置函数可能不使用递归。

•如果选择使用辅助函数，则辅助函数和顶级转置函数之间的堆栈上一次的局部变量可能不超过12个。例如，如果你的转置声明了8个变量，然后你调用了一个使用4个变量的函数，该函数调用了另一个使用2个的函数，那么堆栈上就会有14个变量，这就违反了规则。

•您的转置函数可能不会修改数组A。但是，您可以对数组B的内容执行任何操作。

•您不允许在代码中定义任何数组或使用malloc的任何变体。

把5.2的信息提前：

转置会测试三个矩阵，大小分别是
``` cpp
32 × 32 (M = 32, N = 32)
64 × 64 (M = 64, N = 64)
61 × 67 (M = 61, N = 67)
// M N的含义见文件trans.c中的函数定义
```
模拟器的缓存参数是 s=5 E=1 b=5

也就是32个组 每个组1行 每行32个字节

可以在转置函数中判断M和N的值，对应写特定的代码

两部分的解决代码以及部分解析见csim.c和trans.c

[参考博客](https://zhuanlan.zhihu.com/p/410662053)

关于partB 64*64的处理方式示意图如下
<img width="509" alt="image" src="https://github.com/RaDsZ2z/CSAPP/assets/129292565/7bcdfacc-51c7-4af3-9692-378877ed3f14">


需要注意的是，在将B的8*8块变为第二个状态时(也就是上图第二行右边那个状态)，取A中的元素要一列一列取而不能一行一行地取

具体的代码和差异在trans.c中写出来了
