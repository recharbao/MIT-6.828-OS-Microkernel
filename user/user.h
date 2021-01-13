struct stat;
struct rtcdate;

// system calls
int fork(void);
// 一个进程可以通过fork来创建一个新的进程，fork创建的进程被称为子进程
// 子进程的内存内容同创建它的进程一样。这点要注意，内存内容一样，理解后面的代码设计部分有用。
//父进程返回子进程的pid，子进程返回0.

int exit(void) __attribute__((noreturn));
int wait(void);
// int pid;
// pid = fork();
// if(pid > 0){
//     printf("parent: child=%d\n", pid);
//     pid = wait();    返回当前一个已经退出的进程，若没有则等待
//     printf("child %d is done\n", pid);
// } else if(pid == 0){
//     printf("child: exiting\n");
//     exit();   系统调用exit()会导致调用它的进程停止运行，并释放内存和打开文件在内的资源
// } else {
//     printf("fork error\n");
// }
//父进程与子进程拥有不同的内存空间和寄存器，改变一个进程中的变量不会影响另外一个程序

int pipe(int*);
// 管道是一个小的内核缓冲区，它以文件描述符对的形式提供给进程，一个用于写， 一个用于读；
// int p[2];
// char *argv[2];
// argv[0] = "wc";
// argv[1] = 0;
// pipe(p);
// if(fork() == 0) {
//     close(0);
//     dup(p[0]);
//     close(p[0]); 
//     close(p[1]);
        // 这里为什么要关闭管道呢？
        // 首先，fork， 与 exec Unix操作系统将他们分为创建进程与加载程序两个过程设计。exec部分有解释。
        // 何为标准输入？通过文件描述符0输入。
        // fork 会复制父进程文件描述符，而exec会保留原来的文件描述符。
        // exec 也相当于读入
        // fork后父进程与子进程有相同的管道文件描述符，
        // 也就是说这个wc执行后是通过管道读取的，
        // 如果数据没有准备好，那么对管道执行的read会一直等待，
        // 直到有数据了或者其他绑定在这个管道写端口的描述符都已经关闭了，
        // 系统调用 close 会释放一个文件描述符，使得它未来可以被 open等调用重用，
        // 一个新分配的文件描述符永远都是当前进程的最小的未被使用的文件描述符，
        // 你想想，如果我没有关闭0，如果有其他进程占据1， 他就得从1开始读取，可是那是管道的写端口，
        // 他就会已知占据写端口，一直等待写端口关闭。
//     exec("/bin/wc", argv);
// } else {
//     write(p[1], "hello world\n", 12);
//     close(p[0]);
//     close(p[1]);
// }

// 管道与临时文件的不同：
// 1、管道可以自我清扫， shell重定向的话我们必须在任务完成之后删除
// 2、管道可以传输任意字节长度的数据
// 3、管道允许同步，一端读操作后会阻塞进程，直到另一端完成写操作

int write(int, const void*, int);
int read(int, void*, int);
// read(fd, buf, n) 从fd读取最多n个字节将他们拷贝到buf中， 详细见cat

int close(int);
// 文件描述符是一个强大的抽象，因为他们将他们所连接的细节隐藏起来了：
// 一个进程向描述符1写出，它有可能是写到一份文件，一个设备（如控制台），或一个管道。
// 系统调用close会释放一个文件描述符，使得未来他可以被open， pipe， dup 等调用，
// 一个新分配的文件描述符永远都是当前进程的最小的未被使用的文件描述符。
//在这里我说的是文件描述符的作用之一就是为了IO从定向方便实现
// fork 会复制父进程的文件描述符和内存，所以子进程和父进程的文件描述符一模一样。
// exec 会替换调用它的进程的内存但是会保留它的文件描述符表。
//虽然fork复制了文件描述符，但是没一个文件当前的偏移量仍然是父子进城之间共享的，看下面:
// if(fork() == 0) {
//     write(1, "hello ", 6);
//     exit();
// } else {
//     wait();
//     write(1, "world\n", 6);  父进程会从子进程结束的地方继续写
// }


int kill(int);
int exec(char*, char**);  //加载程序，其实就是一个输入过程，而且是标准输入

// exec函数可以用来替换进程映像。
// 执行exec系列函数后，原来的进程将不再执行，新的进程的PID、PPID和nice值与原先的完全一样。
// 其实执行exec系列函数所发生的一切就是，运行中的程序开始执行exec调用中指定的新的可执行文件中的代码。

// exec函数的特点：

// 当进程调用一种exec函数时，源进程完全由新程序代换，而新程序则从其main函数开始执行。
// 因为调用exec并不创建新进程，所以前后的进程ID并未改变。
// exec只是用另一个新程序替换了当前进程的正文、数据、堆和栈段。
// 特别地，在原进程中已经打开的文件描述符，在新进程中仍将保持打开，除非它们的“执行时关闭标志”（close on exec flag）被置位。
// 任何在原进程中已打开的目录流都将在新进程中被关闭。

// exec 从某个文件里读取内存镜像，并将其替换到调用他的进程内存空间
// 注意，是在装入可执行文件时分配内存空间， fork是在需要装入父进程的拷贝内存
//时分配空间。




// 首先比较一下exec()函数和fork()。这两个函数一个是换药不换汤（execl函数），另一个是换汤不换药（fork函数）。
// 那么什么是汤、什么又是药呢？我们知道进程是个很复杂的东西。
// 从task_struct 结构体的代码量上就可以看出来（task_struct是Linux内核中用来描述进程的一个结构体，这个结构体光代码貌似就有好几屏）。
// 我们可以把进程的PID、PPID和nice值等看作是汤，而把进程空间（简单理解就是正文段、数据段、堆、栈等）看作是药。

// exec()函数是换药不换汤，就是说执行exec函数后，并没有产生新的进程，也就是汤还是那些汤，进程的PID、PPID和nice值等没有发生变化。
// 但是exec()函数却将药换了，也就是将进程空间换掉了，新的进程空间是为了执行新的程序所准备的，所以新的进程空间与原进程空间并没有什么关系。

// fork()函数是换汤不换药，意思是执行fork()函数后，产生了新的进程，新的进程的PID、PPID与原来原来的进程不同，说明父子进程是两个不同的进程，但是fork并没有把药换掉，而是将药复制了一份给子进程。
// fork刚执行后的一段时间内，父子进程有着相同的状态（进程空间中的东西都一样，因为fork采用“写时复制”，一开始父子进程共享物理内存空间）。但是一旦父子进程中有一个进程试图修改进程空间，这时父子进程就各自拥有了各自的进程空间，简单地理解，从这一时刻器，父子进程就是两个独立的进程，谁都不会影响谁（实际上还是有一定影响的，在这里可以忽略），父子进程之间的关联仅剩下它们共享的代码段了。



int open(const char*, int);
int mknod(const char*, short, short);
//创建文件

int unlink(const char*);
int fstat(int fd, struct stat*);
//获得文件描述符指向的文件信息


int link(const char*, const char*);
// 文件名和这个文件本身是有很大的区别。同一个文件（称为 inode）可能有多个名字，叫连接。
// 系统调用 link 创建另一个文件系统的名称，它指向同一个 inode
// open("a", O_CREATE|O_WRONGLY);
// link("a", "b");

int mkdir(const char*);
//创建目录

int chdir(const char*);
//切换目录

int dup(int);
//复制文件描述符，指向同一个文件，公用一个文件偏移
// fd = dup(1);
// write(1, "hello", 6);
// write(fd, "world\n", 6);


int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int ntas();
int crash(const char*, int);
int mount(char*, char *);
int umount(char*);

// ulib.c
//printf.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...);
void printf(const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
