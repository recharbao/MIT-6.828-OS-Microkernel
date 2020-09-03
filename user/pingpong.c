//0:read
//1:write
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



/*
 *pipe 在一端只能读或写
 *
*/
int
main(int argc, char *argv)
{
  int parent_fd[2];
  int child_fd[2];

  //create pipe
  pipe(parent_fd);
  pipe(child_fd);

  
  char v = 0;
  if(fork()>0)//if parent process
  {
     close(parent_fd[0]);
     close(child_fd[1]);
     
     write(parent_fd[1],&v,1);
     read(child_fd[0],&v,1);

     if(v==1)
        printf("%d:received pong\n", getpid());
     close(parent_fd[1]);
     close(child_fd[0]);
     wait();
  }else{  //if child process
      close(child_fd[0]);
      close(parent_fd[1]);
      read(parent_fd[0],&v,1);
      if(v==0)
	printf("%d:received ping\n", getpid());
      v=1;
      write(child_fd[1],&v,1);
      close(child_fd[1]);
      close(parent_fd[0]);
  }
  exit();
}





