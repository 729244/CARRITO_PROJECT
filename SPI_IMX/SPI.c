#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/ioctl.h>

#include <stdint.h>
#include <string.h>

#include <linux/gpio.h>
#include <sys/stat.h>


#define LEN 2
#define PIPE7_NAME "/tmp/pipe7"


int main(int argc, char * argv[]) {


    int  fd;
    uint32_t mode = SPI_MODE_0;
    uint8_t  bits = 8;
    uint32_t speed = 1000000;
    uint16_t delay = 0;
    uint8_t tx[LEN] = {0x57, 0x92}; 
    uint8_t rx[LEN];
    //PIPES
    int pipe7;
    int ob_num = 0;
    char buff[51];
    char buff2[50];
  
    pipe7 = open(PIPE7_NAME, O_RDONLY);
    printf("Esperando a recibir en pipe7\n");

    fd = open("/dev/spidev1.0", O_RDWR);

    // spi mode
    int ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);

    // bits per word
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);

    // max speed hz
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    while(1){
        ret = read(pipe7,buff,51);
        ob_num = buff[0] - 48;
        printf("OBJ ESPERADOS DESDE SPI: %s\n",buff);
        if(ob_num != 0){
            printf("Info de objetos desde SPI: \n");
            printf("%s\n",buff);
        }
        
        //Mandar a SPI
        //MANDAR PRIMER BYTE
        struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long)buff,
            .rx_buf = 0,
            .len = 1,
            .delay_usecs = delay,
            .speed_hz = speed,
            .bits_per_word = bits,
        };

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
            perror("can't send init spi message");

        if(ob_num != 0){
            strcpy(buff2, buff + 1);
            //Mandar segunda parte
            struct spi_ioc_transfer tr2 = {
                .tx_buf = (unsigned long)buff2,
                .rx_buf = 0,
                .len = 5*ob_num,
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
            };

            ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr2);
            if (ret < 1)
                perror("can't send other spi message");
        }

        

        printf("SPI SENDING SUCCESS\n");
        memset(buff, 0, sizeof(buff));
        printf("//////////////////////\n");
    }
    close(pipe7);
    
    

	

	

}