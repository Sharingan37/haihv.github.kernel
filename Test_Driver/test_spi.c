#include "spi/spi.h"
#define CAMIP_S211 0
static uint32_t mode;
int main(int argc, char *argv[])
{
/*	int a[2];
	unsigned char mread_data=0;
	printf("ex testv \n");
	spi_init("/dev/spi_bkav_dev0.0",mode);
	RegWriteA(SPIMD3,0x0);
	RegWriteA(SPIMD3,0x0);
	RegReadA(SPIMD3,&mread_data);
	printf("spi mode: %d\n", mread_data);
*/
	 int file_fd[3] = {-1,-1, -1};
	 file_fd[0] = open("/dev/bkav_movezoom",O_RDWR);
	 printf("file %d \n",file_fd[0]);
	 ioctl(file_fd[0], 50, 200);
         file_fd[1] = open("/dev/bkav_moveiris",O_RDWR);
	 printf("file %d \n",file_fd[1]);
	 ioctl(file_fd[1], 10, 0);
 	// file_fd[2] = open("/dev/bkav_openircut",O_RDWR);
	// printf("file %d \n",file_fd[2]);
	// ioctl(file_fd[2], 1, 1);

	 close(file_fd[0]);
         close(file_fd[1]);
 	// close(file_fd[2]);


}
