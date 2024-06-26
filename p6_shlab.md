# 1.介绍
本任务的目的是更加熟悉过程控制和信号的概念。您将通过编写一个支持作业控制的简单Unix外壳程序来实现这一点。

# 2.Logistics
您可以以最多两人一组的方式解决此作业的问题。唯一的“handin”将是电子的。对作业的任何澄清和修改都将发布在课程网页上。

# 3.Hand Out Instructions
首先，将文件shlab-handout.tar复制到计划进行工作的受保护目录（实验室目录）中。然后执行以下操作：

•键入命令tar xvf shlab-handout.tar以展开tar文件。

•键入命令make以编译和链接一些测试例程。

•在tsh.c顶部的标题注释中键入您的团队成员姓名和Andrew ID。

查看tsh.c（tiny shell）文件，您会发现它包含一个简单Unix外壳的功能框架。为了帮助您入门，我们已经实现了不太有趣的功能。您的任务是完成下面列出的剩余空函数。作为对您的健全性检查，我们在参考解决方案中列出了每个函数的大致代码行数（其中包括大量注释）。

•eval：解析和解释命令行的主例程。[70行]

•内置cmd：识别并解释内置命令：quit、fg、bg和jobs。[25行]

•do_bgfg：实现bg和fg内置命令。[50行]

•waitfg：等待前台作业完成。[20行]

•sigchld处理程序：捕获SIGCHILD信号。[80行]

•sigint处理程序：捕获sigint（ctrl-c）信号。[15行]

•sigtstp处理程序：捕获sigtstp（ctrl-z）信号。[15行]

每次修改tsh.c文件时，键入make重新编译它。要运行shell，请在命令行中键入tsh：
```shell
unix> ./tsh
tsh> [type commands to your shell here]
```

# 4.Unix Shell概述
shell是一个交互式命令行解释器，代表用户运行程序。shell重复打印提示，等待stdin上的命令行，然后根据命令行内容执行一些操作。

命令行是一个由空格分隔的ASCII文本单词序列。命令行中的第一个单词是内置命令的名称或可执行文件的路径名。剩下的单词是命令行参数。如果第一个单词是内置命令，则shell会立即在当前进程中执行该命令。否则，该单词被假定为可执行程序的路径名。在这种情况下，shell派生一个子进程，然后在该子进程的上下文中加载并运行程序。由于解释单个命令行而创建的子进程统称为作业。通常，一个作业可以由通过Unix管道连接的多个子进程组成。

如果命令行以“&”号结尾，则作业将在后台运行，这意味着shell不会在打印提示并等待下一个命令行之前等待作业终止。否则，作业将在前台运行，这意味着shell将在等待下一个命令行之前等待作业终止。因此，在任何时间点，最多可以在前台运行一个作业。但是，可以在后台运行任意数量的作业。
例如，键入命令行
```shell
tsh> jobs
```
导致shell执行内置的jobs命令。键入命令行
```shell
tsh> /bin/ls -l -d
```
在前台运行ls程序。按照惯例，shell确保当程序开始执行其主例程时
```C
int main(int argc, char *argv[])
```
argc和argv参数具有以下值：

• argc == 3,

• argv[0] == ‘‘/bin/ls’’,

• argv[1]== ‘‘-l’’,

• argv[2]== ‘‘-d’’.

或者，键入命令行
```shell
tsh> /bin/ls -l -d &
```
在后台运行ls程序。

Unix shell支持作业控制的概念，它允许用户在后台和前台之间来回移动作业，并更改作业中进程的进程状态（运行、停止或终止）。键入ctrl-c会导致将SIGINT信号传递到前台作业中的每个进程。SIGINT的默认操作是终止进程。类似地，键入ctrl-z会将SIGTSTP信号传递到前台作业中的每个进程。SIGTSTP的默认操作是将进程置于停止状态，在接收到SIGCONT信号之前，进程一直保持停止状态。Unix shell还提供各种支持作业控制的内置命令。例如

•jobs：列出正在运行和停止的后台作业。

•bg＜job＞：将停止的后台作业更改为正在运行的后台作业。

•fg＜job＞：将停止或正在运行的后台作业更改为在前台运行的作业。

•kill＜job＞：终止作业。

# 5.tsh规范
您的tsh-shell应该具有以下功能：

•提示应为字符串“tsh>”。

•用户键入的命令行应由一个名称和零个或多个参数组成，所有参数都由一个或多个子空格分隔。如果name是一个内置命令，那么tsh应该立即处理它，并等待下一个命令行。否则，tsh应该假设name是一个可执行文件的路径，它在初始子进程的上下文中加载并运行该文件（在这种上下文中，术语job指的是这个初始子进程）。

•tsh不需要支持管道（|）或I/O重定向（< 和 >）

（不需要实现｜<>这三个运算符）

•键入ctrl-c（ctrl-z）应导致将SIGINT（SIGTSTP）信号发送到当前前台作业以及该作业的任何子进程（例如，它分叉的任何子过程）。如果没有前台作业，那么信号应该没有效果。

•如果命令行以&结尾，则tsh应在后台运行作业。否则，它应该在前台运行作业。

•每个作业都可以通过进程ID（PID）或作业ID（JID）来识别，后者是由tsh分配的正整数。JID应在命令行上用前缀“%”表示。例如，“%5”表示JID 5，“5”表示PID 5。（我们已经为您提供了操作作业列表所需的所有例程。）

•tsh应支持以下内置命令：

–quit命令终止shell。

–jobs命令列出所有后台作业。

–bg< job>命令通过向其发送SIGCONT信号来重新启动< job>，然后在后台运行它。< job>参数可以是PID或JID。

–fg＜job＞命令通过向其发送SIGCONT信号来重新启动＜job>，然后在前台运行它。< job>参数可以是PID或JID。

•tsh应该收获所有的僵尸孩子。如果任何作业因接收到未捕获的信号而终止，则tsh应识别此事件，并打印一条带有作业PID和违规信号描述的消息。

书上给出过一个[shellex.c代码](https://github.com/RaDsZ2z/CSAPP/blob/main/shellex.cpp) 我还抄下来了 先重读一下

我决定先自己实现所有功能再去改lab给出的代码


# 6.Checking Your Work
```shell
make test01
make test02
...
make test16
```

