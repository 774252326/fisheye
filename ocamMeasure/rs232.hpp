#ifndef RS232_HPP
#define RS232_HPP
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <errno.h>
namespace RS232
{

    const int RS232_PORTNR = 38;

    char *comports[RS232_PORTNR]={"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5",
                           "/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9","/dev/ttyS10","/dev/ttyS11",
                           "/dev/ttyS12","/dev/ttyS13","/dev/ttyS14","/dev/ttyS15","/dev/ttyUSB0",
                           "/dev/ttyUSB1","/dev/ttyUSB2","/dev/ttyUSB3","/dev/ttyUSB4","/dev/ttyUSB5",
                           "/dev/ttyAMA0","/dev/ttyAMA1","/dev/ttyACM0","/dev/ttyACM1",
                           "/dev/rfcomm0","/dev/rfcomm1","/dev/ircomm0","/dev/ircomm1",
                           "/dev/cuau0","/dev/cuau1","/dev/cuau2","/dev/cuau3",
                           "/dev/cuaU0","/dev/cuaU1","/dev/cuaU2","/dev/cuaU3"};

    int Cport[RS232_PORTNR],
        error;
    struct termios new_port_settings,
           old_port_settings[RS232_PORTNR];

    int OpenComport(int comport_number, int baudrate, const char *mode)
    {
      int baudr,
          status;

      if((comport_number>=RS232_PORTNR)||(comport_number<0))
      {
        printf("illegal comport number\n");
        return(1);
      }

      switch(baudrate)
      {
        case      50 : baudr = B50;
                       break;
        case      75 : baudr = B75;
                       break;
        case     110 : baudr = B110;
                       break;
        case     134 : baudr = B134;
                       break;
        case     150 : baudr = B150;
                       break;
        case     200 : baudr = B200;
                       break;
        case     300 : baudr = B300;
                       break;
        case     600 : baudr = B600;
                       break;
        case    1200 : baudr = B1200;
                       break;
        case    1800 : baudr = B1800;
                       break;
        case    2400 : baudr = B2400;
                       break;
        case    4800 : baudr = B4800;
                       break;
        case    9600 : baudr = B9600;
                       break;
        case   19200 : baudr = B19200;
                       break;
        case   38400 : baudr = B38400;
                       break;
        case   57600 : baudr = B57600;
                       break;
        case  115200 : baudr = B115200;
                       break;
        case  230400 : baudr = B230400;
                       break;
        case  460800 : baudr = B460800;
                       break;
        case  500000 : baudr = B500000;
                       break;
        case  576000 : baudr = B576000;
                       break;
        case  921600 : baudr = B921600;
                       break;
        case 1000000 : baudr = B1000000;
                       break;
        case 1152000 : baudr = B1152000;
                       break;
        case 1500000 : baudr = B1500000;
                       break;
        case 2000000 : baudr = B2000000;
                       break;
        case 2500000 : baudr = B2500000;
                       break;
        case 3000000 : baudr = B3000000;
                       break;
        case 3500000 : baudr = B3500000;
                       break;
        case 4000000 : baudr = B4000000;
                       break;
        default      : printf("invalid baudrate\n");
                       return(1);
                       break;
      }

      int cbits=CS8,
          cpar=0,
          ipar=IGNPAR,
          bstop=0;

      if(strlen(mode) != 3)
      {
        printf("invalid mode \"%s\"\n", mode);
        return(1);
      }

      switch(mode[0])
      {
        case '8': cbits = CS8;
                  break;
        case '7': cbits = CS7;
                  break;
        case '6': cbits = CS6;
                  break;
        case '5': cbits = CS5;
                  break;
        default : printf("invalid number of data-bits '%c'\n", mode[0]);
                  return(1);
                  break;
      }

      switch(mode[1])
      {
        case 'N':
        case 'n': cpar = 0;
                  ipar = IGNPAR;
                  break;
        case 'E':
        case 'e': cpar = PARENB;
                  ipar = INPCK;
                  break;
        case 'O':
        case 'o': cpar = (PARENB | PARODD);
                  ipar = INPCK;
                  break;
        default : printf("invalid parity '%c'\n", mode[1]);
                  return(1);
                  break;
      }

      switch(mode[2])
      {
        case '1': bstop = 0;
                  break;
        case '2': bstop = CSTOPB;
                  break;
        default : printf("invalid number of stop bits '%c'\n", mode[2]);
                  return(1);
                  break;
      }

    /*
    http://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html

    http://man7.org/linux/man-pages/man3/termios.3.html
    */

      Cport[comport_number] = open(comports[comport_number], O_RDWR | O_NOCTTY | O_NDELAY);
      if(Cport[comport_number]==-1)
      {
        perror("unable to open comport ");
        return(1);
      }

      /* lock access so that another process can't also use the port */
      if(flock(Cport[comport_number], LOCK_EX | LOCK_NB) != 0)
      {
        close(Cport[comport_number]);
        perror("Another process has locked the comport.");
        return(1);
      }

      error = tcgetattr(Cport[comport_number], old_port_settings + comport_number);
      if(error==-1)
      {
        close(Cport[comport_number]);
        flock(Cport[comport_number], LOCK_UN);  /* free the port so that others can use it. */
        perror("unable to read portsettings ");
        return(1);
      }
      memset(&new_port_settings, 0, sizeof(new_port_settings));  /* clear the new struct */

      new_port_settings.c_cflag = cbits | cpar | bstop | CLOCAL | CREAD;
      new_port_settings.c_iflag = ipar;
      new_port_settings.c_oflag = 0;
      new_port_settings.c_lflag = 0;
      new_port_settings.c_cc[VMIN] = 0;      /* block untill n bytes are received */
      new_port_settings.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */

      cfsetispeed(&new_port_settings, baudr);
      cfsetospeed(&new_port_settings, baudr);

      error = tcsetattr(Cport[comport_number], TCSANOW, &new_port_settings);
      if(error==-1)
      {
        tcsetattr(Cport[comport_number], TCSANOW, old_port_settings + comport_number);
        close(Cport[comport_number]);
        flock(Cport[comport_number], LOCK_UN);  /* free the port so that others can use it. */
        perror("unable to adjust portsettings ");
        return(1);
      }

    /* http://man7.org/linux/man-pages/man4/tty_ioctl.4.html */

      if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
      {
        tcsetattr(Cport[comport_number], TCSANOW, old_port_settings + comport_number);
        flock(Cport[comport_number], LOCK_UN);  /* free the port so that others can use it. */
        perror("unable to get portstatus");
        return(1);
      }

      status |= TIOCM_DTR;    /* turn on DTR */
      status |= TIOCM_RTS;    /* turn on RTS */

      if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
      {
        tcsetattr(Cport[comport_number], TCSANOW, old_port_settings + comport_number);
        flock(Cport[comport_number], LOCK_UN);  /* free the port so that others can use it. */
        perror("unable to set portstatus");
        return(1);
      }

      return(0);
    }

    int PollComport(int comport_number, unsigned char *buf, int size)
    {
      int n;

      n = read(Cport[comport_number], buf, size);

      if(n < 0)
      {
        if(errno == EAGAIN)  return 0;
      }

      return(n);
    }


    int SendByte(int comport_number, unsigned char byte)
    {
      int n = write(Cport[comport_number], &byte, 1);
      if(n < 0)
      {
        if(errno == EAGAIN)
        {
          return 0;
        }
        else
        {
          return 1;
        }
      }

      return(0);
    }


    int SendBuf(int comport_number, unsigned char *buf, int size)
    {
      int n = write(Cport[comport_number], buf, size);
      if(n < 0)
      {
        if(errno == EAGAIN)
        {
          return 0;
        }
        else
        {
          return 1;
        }
      }

      return(0);
    }


    void CloseComport(int comport_number)
    {
      int status;

      if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
      {
        perror("unable to get portstatus");
      }

      status &= ~TIOCM_DTR;    /* turn off DTR */
      status &= ~TIOCM_RTS;    /* turn off RTS */

      if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
      {
        perror("unable to set portstatus");
      }

      tcsetattr(Cport[comport_number], TCSANOW, old_port_settings + comport_number);
      close(Cport[comport_number]);

      flock(Cport[comport_number], LOCK_UN);  /* free the port so that others can use it. */
    }


    void cputs(int comport_number, const char *text)  /* sends a string to serial port */
    {
      while(*text != 0)   SendByte(comport_number, *(text++));
    }

}
#endif // RS232_HPP
