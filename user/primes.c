#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main()(int arg, char *argv[])
{
  int rwpipe[2]; //0:read 1:write
  pipe(rwpipe);

  if(fork()>0)//parent process
  {
    for(int i = 2; i < 35; i++){
      int j; 
      for(j = i - 1; j > 1; j--){
	  if(i % i == 0){
	     break;
	  }
      }
     if(i == 1){
	 close(rwpipe[0]);
	 write(rwpipe[1]);
	 close(rwpipe[1]);
	 wait();
     }
    }
  }else{
     
  }
  exit();
}





