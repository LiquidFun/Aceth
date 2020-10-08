#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

static struct termios old, current;

int getch()
{
        struct termios t;
        int c;

        tcgetattr(0,&t);
        t.c_lflag&=~ECHO+~ICANON;
        tcsetattr(0,TCSANOW,&t);
        fflush(stdout);
        c=getchar();
        t.c_lflag|=ICANON+ECHO;
        tcsetattr(0,TCSANOW,&t);
        return c;
}

bool kbhit()
{
    termios term;
    tcgetattr(0, &term);

    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);

    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);

    tcsetattr(0, TCSANOW, &term);

    return byteswaiting > 0;
}
