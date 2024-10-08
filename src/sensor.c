

#include "hanzi.h"
#include "sensor.h"
#include "lcd.h"

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

// 6818串口所对应的文件名
#define COM2 "/dev/ttySAC1"
#define COM3 "/dev/ttySAC2"
#define COM4 "/dev/ttySAC3"

void Sensor_Run()
{
    printf("add-in---sensorrun--\n");
    pthread_t task_gy39;
    if (pthread_create(&task_gy39, NULL, (void *)gy_39, NULL) == -1)
    {
        perror("fail to create pthread task_rfid\n");
        return;
    }
}

int serial_init(char *file, int speed)
{
    /*
        打开串口文件
    */
    int fd = open(file, O_RDWR);
    if (fd == -1)
    {
        perror("open serial error");
        return -1;
    }

    /*定义串口属性结构体*/
    struct termios myserial;
    memset(&myserial, 0, sizeof(myserial)); // 清零

    /*设置控制模式 本地连接  使能接收*/
    myserial.c_cflag |= (CLOCAL | CREAD);
    myserial.c_cflag &= ~CSIZE;   // 清空数据位
    myserial.c_cflag &= ~CRTSCTS; // 无硬件控制流
    myserial.c_cflag |= CS8;      // 数据位 8
    myserial.c_cflag &= ~CSTOPB;  // 停止位 1
    myserial.c_cflag &= ~PARENB;  // 不要校验

    /*设置波特率*/
    switch (speed)
    {
    case 9600:
        cfsetospeed(&myserial, B9600);
        cfsetispeed(&myserial, B9600);
        break;
    case 57600:
        cfsetospeed(&myserial, B57600);
        cfsetispeed(&myserial, B57600);
        break;
    case 115200:
        cfsetospeed(&myserial, B115200);
        cfsetispeed(&myserial, B115200);
        break;
    }

    /*刷新输出队列，清除正接收的数据*/
    tcflush(fd, TCIFLUSH);

    /*更改配置*/
    tcsetattr(fd, TCSANOW, &myserial);

    return fd;
}

void *gy_39(void *arg)
{

    pthread_detach(pthread_self());

    // char led_ctrl[2];
    // int fd1 = open("/dev/led_drv", O_RDWR);
    // if (fd1 < 0)
    // {
    //     printf("open led_drv failed\n");
    //     return (void *)-1;
    // }
    int fd = serial_init(COM3, 9600);
    int m = 2;
    char w_buf[][3] = {{0xa5, 0x81, 0x26}, {0xa5, 0x82, 0x27}, {0xa5, 0x83, 0x28}};
    int Lux = 0, T = 0, P = 0, Hum = 0, H = 0;
    int t, r, ret;
    char r_buf[24];

    while (1)
    {

        t = write(fd, w_buf[m], 3);
        printf("%d\n", t);
        usleep(1000);
        r = read(fd, r_buf, 24);

        if (r == 9 && r_buf[2] == 0x15)
        {
            Lux = (r_buf[4] << 24 | r_buf[5] << 16 | r_buf[6] << 8 | r_buf[7]) / 100;
        }
        else if (r == 15 && r_buf[2] == 0x45)
        {
            T = (r_buf[4] << 8 | r_buf[5]) / 100;
            P = (r_buf[6] << 24 | r_buf[7] << 16 | r_buf[8] << 8 | r_buf[9]) / 100;
            Hum = (r_buf[10] << 8 | r_buf[11]) / 100;
            H = (r_buf[12] << 8 | r_buf[13]) / 100;
        }
        else if (r == 24)
        {
            Lux = (r_buf[4] << 24 | r_buf[5] << 16 | r_buf[6] << 8 | r_buf[7]) / 100;
            T = (r_buf[13] << 8 | r_buf[14]) / 100;
            P = (r_buf[15] << 24 | r_buf[16] << 16 | r_buf[8] << 8 | r_buf[9]) / 100;
            Hum = (r_buf[10] << 8 | r_buf[11]) / 100;
            H = (r_buf[12] << 8 | r_buf[13]) / 100;
        }
        printf("Lux = %d, T = %d, P = %d, Hum = %d, H = %d\n", Lux, T, P, Hum, H);
        sleep(1);

        // if (Lux < 6)
        // {
        //     led_ctrl[1] = 9; // D9
        //     led_ctrl[0] = 1; // on
        //     ret = write(fd1, led_ctrl, sizeof(led_ctrl));

        //     if (ret != 2)
        //     {
        //         perror("write");
        //     }
        //     sleep(1);
        // }
        // else
        // {
        //     led_ctrl[1] = 9; // D9
        //     led_ctrl[0] = 0; // off

        //     ret = write(fd1, led_ctrl, sizeof(led_ctrl));

        //     if (ret != 2)
        //     {

        //         perror("write");
        //     }
        //     sleep(1);
        // }

        // display_word(black, 170, 100, guan, 16, 16);
        // display_word(black, 170, 116, qian, 16, 16);
        // display_word(black, 170, 132, maohao, 16, 16);
        // display_bg(170, 148, 186, 164, white);

        // display_digit(blue, 170, 148, Lux, 8, 16);
        // display_word(blue, 170, 169, L, 8, 16);
        // display_word(blue, 170, 177, U, 8, 16);
        // display_word(blue, 170, 185, X, 8, 16);

        // display_word(black, 196, 100, wen, 16, 16);
        // display_word(black, 196, 116, du, 16, 16);
        // display_word(black, 196, 132, maohao, 16, 16);
        // display_bg(196, 148, 218, 172, white);
        // display_digit(blue, 196, 148, T, 8, 16);
        printf("----test---if---open---gy39\n");
        lcd_draw_rectangle(150, 430, 90, 50, 0x000000);
        lcd_draw_rectangle(290, 430, 95, 50, 0x000000);
        digit_display(T, 290, 435);
        digit_display(Hum - 190, 150, 435);
        sleep(1);
        // display_word(blue, 196, 169, C, 16, 16);

        // display_word(black, 222, 100, shi, 16, 16);
        // display_word(black, 222, 116, du, 16, 16);
        // display_word(black, 222, 132, maohao, 16, 16);
        // display_bg(222, 148, 238, 182 + 100, white);
        // display_digit(blue, 222, 148, Hum, 8, 16);
        // display_word(blue, 222, 174, baifenhao, 8, 16);

        // display_word(black, 248, 100, qi, 16, 16);
        // display_word(black, 248, 116, ya, 16, 16);
        // display_word(black, 248, 132, maohao, 16, 16);
        // display_bg(248, 148, 264, 209 + 50, white);
        // display_digit(blue, 248, 148, P, 8, 16);
        // display_word(blue, 248, 193, _P, 8, 16);
        // display_word(blue, 248, 201, A, 8, 16);

        // display_word(black, 272, 100, hai, 16, 16);
        // display_word(black, 272, 116, ba, 16, 16);
        // display_word(black, 272, 132, maohao, 16, 16);
        // display_bg(272, 148, 288, 177 + 100, white);
        // display_digit(blue, 272, 148, H, 8, 16);
        // display_word(blue, 272, 169, M, 8, 16);
    }

    close(fd);
    // return 0;
}