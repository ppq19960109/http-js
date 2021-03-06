#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <stdarg.h>

int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio;
    /*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        printf("tcgetattr( fd,&oldtio) -> %d\n", tcgetattr(fd, &oldtio));
        return -1;
    }
    bzero(&newtio, sizeof(newtio));
    /*步骤一，设置字符大小*/
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;
    /*设置停止位*/
    switch (nBits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }
    /*设置奇偶校验位*/
    switch (nEvent)
    {
    case 'o':
    case 'O': //奇数
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'e':
    case 'E': //偶数
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'n':
    case 'N': //无奇偶校验位
        newtio.c_cflag &= ~PARENB;
        break;
    default:
        break;
    }
    /*设置波特率*/
    switch (nSpeed)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    /*设置停止位*/
    if (nStop == 1)
        newtio.c_cflag &= ~CSTOPB;
    else if (nStop == 2)
        newtio.c_cflag |= CSTOPB;
    /*设置等待时间和最小接收字符*/
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    /*处理未接收字符*/
    tcflush(fd, TCIFLUSH);
    /*激活新配置*/
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        perror("com set error");
        return -1;
    }
    printf("set done!\n");
    return 0;
}
int uart_test()
{
    int fd, nread;
    char buff[256];
    char *dev = "/dev/ttyS1"; //串口二
    fd = open(dev, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        printf("open fail\n");
        perror(dev);
        return -1;
    }
    printf("open success\n");
    if (set_opt(fd, 115200, 8, 'N', 1) != 0)
    {
        printf("Set Parity Error\n");
        close(fd);
        exit(1);
    }
    while (1)
    {
        while ((nread = read(fd, buff, 256 - 1)) > 0)
        {
            printf("Len:%d\n", nread);
            buff[nread] = '\0';
            printf("buff:%s\n", buff);
            write(fd, buff, nread + 1);
        }
    }
    printf("Len %d\n", nread);
    close(fd);
    return 0;
}