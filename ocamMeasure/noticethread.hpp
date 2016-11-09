#ifndef NOTICETHREAD_HPP
#define NOTICETHREAD_HPP
#include <QtCore>
#include <QSemaphore>
#include <string>
class NoticeThread : public QThread
{

public:
    std::string cmd[3];
    int index;
    NoticeThread()
        : index(2)
    {
//        cmd[0]="mplayer \"/home/a/Music/Windows Shutdown.wav\"";
//        cmd[1]="mplayer \"/home/a/Music/Windows Error.wav\"";


        cmd[0]="mplayer \"/home/a/Music/slowdown.mp3\"";
//        cmd[1]="mplayer \"/home/a/Music/targetlost.mp3\"";
        cmd[1]="";
        cmd[2]="";

    }

    void run()
    {
        while(index>=0)
        {
            std::cout<<"nt:"<<index<<std::flush;
            system(cmd[index].c_str());
            msleep(100);
        }
    }
};
#endif // NOTICETHREAD_HPP

