## 简介

- INSTITUTE OF INFORMATION ENGINEERING, CAS, CSTE1.1
- CSTE 框架包含各种漏洞程序的实现，并给出了针对各种漏洞程序的攻击方式。其中漏洞程序使用 C 语言编写，攻击程序以 Python (Python 2.7) 脚本的形式给出。攻击脚本的执行依赖 cle 库。
- CSTE 框架是以 Python (Python 2.7) 为开发语言的。
- CSTE 框架目前集成了 QEMU 分析工具和 BASP 分析工具。并能给出漏洞程序运行结果、攻击脚本运行结果、各个分析工具的分析报告作为参考。
- CSTE 目前版本为 VERSION 1.1 (x86-64, Ubuntu16.04)


## 环境

- Ubuntu 16.04
- Python 2.7
- QEMU
- BASP

## 部署

// 安装依赖包

sudo pip install cle

// BASP

其中 BASP 分析工具位于 CSTE 根目录下的 tools 文件夹下，类型为二进制可执行文件，不需要安装。

// QEMU

[参考链接](https://github.com/sonnylier/qemu_eclipse)


## CSTE 整体目录结构

```
├── README.md                   // help
├── cste.conf                   // CSTE 配置文件
├── src                         // CSTE 框架实现源代码
├── doc                         // 说明文档
├── tools                       // CSTE 集成工具集  
│   ├── basp                    // BASP 分析工具
├── samples                     // CSTE 样例集
│   ├── sample_name             // 样例
│   ├── ...                     // 其他攻击实例      
└── run.sh                      // 启动脚本
```

## CSTE 单个样例目录结构（以 ROP 样例为例）


```
├── basp                        // BASP 分析工具的分析结果
├── exploit                     // 攻击脚本
├── include                     // 漏洞程序使用的头文件
├── input                       // 漏洞程序使用的输入文件  
├── output                      // 漏洞程序产生的输出
├── obj                         // 编译漏洞程序得到的中间文件(.o)
├── report                      // 框架运行产生的报告
│   ├── attack.report           // 攻击脚本产生的输出
│   ├── vul.report              // 漏洞程序产生的输出
│   ├── qemu.report             // QEMU 分析报告
│   ├── compile.report          // 编译信息
├── my.conf                     // 样例配置文件
└── rop.c                       // 漏洞程序

```

## 配置文件说明

### cste.conf

#### defense

- on: 关于防御机制的配置（dep、aslr、stack_protector）以 cste.conf 里的配置为基准，所有的样例关于这三项的配置都将被设置为 cste.conf 里的配置。而每个样例的关于这三项的配置失效。
- off: 关于防御机制的配置（dep、aslr、stack_protector）以各个样例的配置为基准，每个样例都配置自己对应的防御机制。cste.conf 里的这三项配置将不起作用。

#### analyse ([enable] 片段下）

- on: 关于（analyse、basp、qemu、qemu_args）四项的配置以 cste.conf 里的配置为基准。 所有的样例关于这四项的配置都将被设置为 cste.conf 里的配置。而每个样例的关于这四项的配置失效。
- off: 关于（analyse、basp、qemu、qemu_args）四项的配置以各个样例的配置为基准。 每个样例都配置自己对应的分析方式，而 cste.conf 里这四项配置失效。

#### dep

- on: 开启 DEP 防御机制。
- off: 关闭 DEP 防御机制。

#### aslr

- on: 打开 ASLR 防御机制。
- off: 关闭 ASLR 防御机制。

#### stack_protector

- on: 打开 STACK PROTECTOR 防御机制。
- off: 关闭 STACK PROTECTOR 防御机制。

#### analyse （[analyse] 片段下）

- on: 运行漏洞程序的时候，使用 QEMU 和 BASP 对程序运行过程进行分析。
- off: 只运行漏洞程序，不使用 QEMU 和 BASP 进行分析。

#### basp

- on: 开启 BASP 分析工具对程序进行分析，得到分析结果，其中主要有 .call 文件和 .func 文件。
- off: 不开启 BASP 分析工具对程序进行分析。

#### qemu

- on: 使用 QEMU 对程序的运行过程进行动态分析。
- off: 不使用 QEMU 对程序的动态过程进行动态分析。

#### qemu_args

分析一个二进制文件的完整 QEMU 命令，具体的命令样例可以参考 cste.conf 配置文件。

### my.conf

#### [vul]: type
漏洞类型，填写漏洞对应的类型（主要有：栈溢出，堆溢出，use-after-free，double-free，格式化字符串等）

#### [vul]: args
漏洞程序运行需要的外部参数

#### [attack]: name

攻击名称，攻击采用的方式为（主要有：return-to-libc，rop，cop，jop，fop，dop 等）
#### [attack]: way

攻击要使用的脚本名称，只需要写文件名，并将文件放到 exploit 文件夹下。

#### [attack]: dep、aslr、stack_protector
该三项的配置说明与 cste.conf 里的配置说明相同，请参阅 cste.conf 里的相关配置项。

#### [attack]: others

对漏洞程序进行编译需要的额外的编译条件。例如“-masm=intel,-ldl”，如果有多个并列的额外的编译条件，各个条件之间用逗号进行分隔（中英文即可）。

#### [analyse]: analyse、basp、qemu、qemu_args
这四项的配置说明与 cste.conf 里的配置说明相同，请参阅 cste.conf 里的相关配置项。


## QUICK START

```
# 首先按**部署**部分的提示进行部署。

# 部署成功后将路径切换到 CSTE 根路径下。

# 启动程序
./run.sh

# help 命令
输入 help，直接按回车键，会显示 CSTE 当前版本号以及 CSTE 框架支持的命令。

# help + 命令名称
例如输入 help compile，按回车键，会显示 compile 命令的相关帮助信息。通常帮助信息都是以命令名称 + 命令参数的形式给出的，其中 = 后面的 [] 内的内容为该命令可以使用的参数。

# compile
例如以下帮助信息表示 compile 命令后可以使用的参数为 all、rop 或者 uaf。整体就是 compile all、compile rop、compile uaf。其中 compile all 可以一次编译所有已有的样例。rop 和 uaf 分别为单个样例的样例名称。
compile [args]
[args] = ['all', 'rop', 'uaf']

# run
输入 help run，然后按下回车键，会得到关于 run 命令的说明。其中 run all 可以一次执行所有的样例。当然前提是按照配置文件的给定格式进行配置。给的两个例子 uaf 和 rop 都是可供参考的例子。如果开启 BASP 工具进行分析，那么运行完成后会得到 BASP 的分析结果。共计 5 个文件。这 5 个文件的位置请参照单个样例目录结构。其中 QEMU 分析工具要用到的两个文件为 vul.func 和 vul.call。同时如果开启了 QEMU 分析工具，相应的会得到 QEMU 的分析报告。所有的报告都位于 report 文件夹下。
run [args]
[args] = ['all', 'rop', 'uaf']

# aslr
aslr 命令可以用于配置 ASLR 防御机制，当然该防御机制也可以在配置文件中进行配置。这里的主要用途是可以用该命令查看 ASLR 的状态。特别提示：修改 ASLR 的状态需要 ROOT 权限。 
aslr [args]
[args] = ['status', 'on', 'off']

# bye
bye 命令用于正常退出程序，后面不接任何参数。也可以使用 CTRL + C 强制中断 CSTE 程序的运行。
bye [args]
[args] = ['']

# show
show 命令主要用于列出当前例子的名称，目前只支持一个参数 samples。
show [args]
[args] = ['samples']

# ls, vim, cd
框架融合了 Linux 三个原生的命令，可以直接通过这三个命令进行一些操作，比如可以通过 vim 修改配置文件，如果在 CSTE 的环境中通过 vim 命令修改了配置文件，那么 CSTE 框架会自动重新加载。

# refresh
重新加载 CSTE 运行环境。特别是不是在 CSTE 的环境中修改了 CSTE 的配置文件，想让配置生效，可以执行该命令。
```

## 添加样例
这里只介绍添加样例所必须的遵守的几个规则。

### 漏洞程序部分
``` c
// 必须在指定端口号的那一行的后面加上“服务器端口号需要提取”的字样。
//这样 CSTE 才可以统一分配端口，调度所有样例执行，而不发生端口冲突
 my_addr.sin_port = htons(30001);//服务器端口号需提取
```

### 攻击程序部分
``` python
# 必须在指定端口号的那一行的后面加上“服务器端口号需要提取”的字样。
# 这样 CSTE 才可以统一分配端口，调度所有样例执行，而不发生端口冲突
sock.connect(('127.0.0.1',30001)) #服务器端口号需提取
```

### 攻击目标
目前的样例程序共有以下三个攻击目标。
1. 创建文件，在 my.conf 配置文件里将 [attack] 片段下的 target 配置为 create，文件创建路径为每个样例的 output 文件夹。
2. 输出指定的信息（traget = print）。
3. 调用系统命令 ls（target = ls）。
在 my.conf 配置文件中配置攻击目标，这样才能进行统计，并把统计的结果放到了 cste.log 文件中，统计的主要信息包括：各个样例攻击是否成功，攻击成功的个数，攻击失败的个数和攻击成功的比例。

### 统计信息
如果是在 CSTE 运行环境下执行 run all 命令，那么命令执行完成后，CSTE 会给出每个样例攻击是否成功的结果，该结果记录在 CSTE 根目录下的 cste.log 文件夹下。统计的信息有：每个样例攻击是否成功，攻击成功的个数，攻击失败的个数，攻击成功的比例，各项保护机制的开关情况。

## 样例介绍
目前漏洞库共有 42 个样例。下面将给出每类样例的基本信息。

### stack overflow

### heap overflow
存在堆溢出漏洞，采取堆溢出方式注入数据绕过 unlink 函数，最终成功结果为在 output 文件夹下创建 out.txt。

### rop
漏洞程序存在栈溢出点，攻击程序通过找溢出点然后构件配件链进行攻击，最终攻击成功的结果为打开和漏洞程序在同一文件夹下的 hello 程序，在屏幕上输出 hello world! 代表着程序攻击成功。

### dop
存在dop漏洞，最终成功结果为输出当前目录下所包含文件的名字(system(‘ls’))

### jop

### return to libc
漏洞程序存在栈溢出点，攻击程序通过溢出点进行 return to libc 攻击，最终成功的结果是输出当前目录下所包含文件的名字(system(‘ls’))，攻击成功以后漏洞程序会报段错误，这个是因为返回地址我没有管，攻击已经成功效果已经达到，报的段错误没有关系。

### use after free
漏洞程序存在 use after free 漏洞点，攻击程序利用漏洞进行攻击，最终成功结果为输出当前目录下所包含文件的名字(system(‘ls’))。

### double free
存在 double free 漏洞点，绕过 unlink 函数，最终成功结果为在 output 文件夹下创建 out.txt。

### format string
存在格式化字符串漏洞，通过格式化字符串改变变量的值，最终成功结果为输出当前目录下所包含文件的名字(system(‘ls’))。

## 后续计划
在开发过程中发现几个问题需要改进。会使框架的逻辑变得更加简单易于维护。

### 统一攻击目标
目前框架中的攻击脚本共有三个攻击目标。

第一个攻击目标是创建文件，这里统一只允许在 output 文件夹下创建 success.txt 文件（文件名不一定为 success.txt，但是必须统一）。
第二个攻击目标是输出信息，这里统一只允许输出 success 字符串（字符串名称统一即可）。
第三个攻击目标是调用系统命令，这里只允许统一调用 ls 命令。

### 漏洞程序的编写
漏洞程序目前存在一个问题，假设漏洞程序启动以后就是服务端，服务端程序不应当只接受一次连接就关闭服务，服务端程序应当具有迭代接收连接请求的能力。这里我给出了一个服务端的 demo。希望一起讨论解决这个
问题，否则框架中会存在一些不合理的痛点。比如，如果发一个连接检测服务端程序是否监听在了端口，如果服务器端监听在了指定端口，那么服务端程序就会立马结束。检测操作就不可行了。只能通过人为的指定延时来解决这个问题。
我给出的示例见 CSTE 根目录下的 server_sample 文件夹。

### 文档的完善
自动生成样例部分的操作文档需要补齐。
该 README.md 样例部分的说明需要补齐，简单的将现有样例进行分类，并介绍各个分类的基本原理、实现方式即可。

### 统计工作的完善
统计输出的结果不整洁，在解决了上述问题之后，考虑解决这个问题。

