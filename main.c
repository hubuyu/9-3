#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>
#include <linux/input.h> 

#define KEY_DEVICE  "/dev/input/event0"

int fd;
static int cnt = 0 ;

static void async_notification(signum)
{	int err ;
	struct input_event ev_data ;
	err = read( fd, &ev_data , sizeof(struct input_event));
    if(err < 0)
    {
        perror("Read event error \n");
        exit(1);
    }
    printf("Loop-%d \n" , cnt);
    if( EV_KEY == ev_data.type )
    {
        printf("    Key type = %d , Key code = %d , Key Value = %d \n ",
                ev_data.type , ev_data.code , ev_data.value);
    }

}

int main(int argc, char **argv)
{
    int flag;
    int cnt = 0 ;

    fd = open( KEY_DEVICE , O_RDONLY);
    if(fd < 0)
    {
         perror("cannot open device button");
         exit(1);
    }

	signal(SIGIO , async_notification);	
	/* F_SETOWN:  Set the process ID 
    *  鍛婅瘔鍐呮牳锛屽彂缁欒皝 
    */  
	fcntl(fd, F_SETOWN , getpid());
 
	/*  F_GETFL :Read the file status flags 
	*  璇诲嚭褰撳墠鏂囦欢鐨勭姸鎬?
    */  
	flag = fcntl(fd,F_GETFL);  
 
   /* F_SETFL: Set the file status flags to the value specified by arg 
    * int fcntl(int fd, int cmd, long arg); 
    * 淇敼褰撳墠鏂囦欢鐨勭姸鎬侊紝娣诲姞寮傛閫氱煡鍔熻兘 
    */  
   fcntl(fd,F_SETFL,flag | FASYNC);  
    
	do{
		sleep(1);
		printf("ZZZZzzzzzz....\n");
    }while(cnt++<=20);

    close(fd);
    return 0;
}

