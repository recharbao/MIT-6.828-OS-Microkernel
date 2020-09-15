#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int
wanted_file(char *path, char *name)
{
  char *p;

  //拿到最后一个字符
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  if(strcmp(p, name) == 0) return 1; else return 0;
}


void find(char* path, char* name){
  char buf[64], *p;
  int fd;
  struct dirent de; //目录
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  //如果是文件，打开
  case T_FILE:
    if (wanted_file(path, name))
        printf("%s\n", path);
    break;

  //如果是目录，continue，后者递归
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      //按照题目要求，对输入 '.' 和 '..' continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      //将该目录的名字追加
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      //递归进入下一层目录;
      find(buf, name);
    }
    break;
  }
  close(fd);
}


int
main(int argc, char *argv[])
{
  if(argc < 3){
    exit();
  }
  find(argv[1], argv[2]);
  exit();
}