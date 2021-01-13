#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char buf[512];

// cat并不知道他是从文件，控制台，或者管道中读取数据，而是采用文件描述符，
// 一般情况下，0是标准输入， 1是标准输出。
//每个进程都有一张表，而xv6内核就以文件描述符作为这张表的索引，所以每个进程都有一个文件
//描述符空间，一般情况下，进程从文件描述符0读入， 从1读出。
//文件描述符指向的对象是“文件”，文件描述符的接口是对文件，管道，设备的抽象，使得他们看上去就像是字节流。这点特别关键，抽象出来的意义。


void
cat(int fd)
{
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0) { // 注意fd的int类型，抽象描述符
    if (write(1, buf, n) != n) {  //显然是写入，故抽象描述符为1
      printf("cat: write error\n");
      exit();
    }
  }
  if(n < 0){
    printf("cat: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0);
    exit();
  }

  for(i = 1; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      printf("cat: cannot open %s\n", argv[i]);
      exit();
    }
    cat(fd);
    close(fd);
  }
  exit();
}
