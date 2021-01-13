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
int write(int, const void*, int);
int read(int, void*, int);

// read(fd, buf, n) 从fd读取最多n个字节将他们拷贝到buf中， 详细见cat

int close(int);
int kill(int);
int exec(char*, char**);

// exec 从某个文件里读取内存镜像，并将其替换到调用他的进程内存空间
// 注意，是在装入可执行文件时分配内存空间， fork是在需要装入父进程的拷贝内存
//时分配空间。



int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
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
