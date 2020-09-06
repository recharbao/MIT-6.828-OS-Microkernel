#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int first,others;

  //用做交换
  int *read_fd;
  int *write_fd;

  int a[2];
  int b[2];

  pipe(a);
  //feed 进程

  if(fork()>0)
  {
    close(a[0]);
    for(int i = 2; i <= 35; i++){
       write(a[1],&i,sizeof(i));
    }
    close(a[1]);
    wait();
  }else{

      read_fd = a;
      write_fd = b;
      while(1){
        //create pipe write to child process
	pipe(write_fd);

	close(read_fd[1]);
	if(read(read_fd[0], &first, sizeof(first)))
	{
          printf("prime: %d\n",first);
	}else{
	     close(read_fd[0]);
	     break;
	}
	
	//create child process
	if(fork() > 0)
	{
	  close(write_fd[0]);
	  while(read(read_fd[0],&others,sizeof(others)))
	  {
	    if(others % first == 0)
	    {
		 continue;
	    }
	    write(write_fd[1],&others,sizeof(others));
	  }
	
	  close(read_fd[0]);
	  close(write_fd[1]);
	  wait();
	  break;
      }else{
	     close(read_fd[0]);
	     int *tmp = read_fd;
	     read_fd = write_fd;
	     write_fd = tmp;
      }
     }
  }
  exit();
}



































/*#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int first, v;

  int* read_fd;
  int* write_fd;

  int first_fd[2];
  int second_fd[2];
  pipe(first_fd);

  if(fork() > 0){
    // feed process
    close(first_fd[0]);
    for(int i = 2; i <= 35; i++){
        write(first_fd[1], &i, sizeof(i));
    }
    close(first_fd[1]);
    wait();
  }else{
    read_fd = first_fd;
    write_fd = second_fd;

    while(1){
      // need a pipe writing to child
      pipe(write_fd);

      close(read_fd[1]);
      if(read(read_fd[0], &first, sizeof(first))){
        printf("prime %d\n", first);
      }else{
        break;
      }

      if(fork() > 0){
        close(write_fd[0]);
        while(read(read_fd[0], &v, sizeof(v))){
          if (v % first == 0)
            continue;
          write(write_fd[1], &v, sizeof(v));
        }
        // r&w finish, close them
        close(read_fd[0]);
        close(write_fd[1]);
        wait();
        break;
      }else{
        // do not need parent read pipe
        close(read_fd[0]);
        // parent write pipe is my read pipe
        int *tmp = read_fd;
        read_fd = write_fd;
        write_fd = tmp;
      }
    }
  }
  exit();
}
*/


