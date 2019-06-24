#include "dy_barrage_client/def.h"
#include "dy_barrage_client/dy_barrage_client.h"
#include <iostream>
#include <cstdlib>
#include <thread>
#include <csignal>

using namespace std;

void thr_print (void *args)
{
    dy_barrage_client *dy_bclient = (dy_barrage_client *)args;

#ifdef _MSC_VER
    HANDLE  hConsole;
    hConsole = GetStdHandle (STD_OUTPUT_HANDLE);
#else
#endif

    while (true)
    {
        if (dy_bclient->get_barr_queue().is_empty ()) {
#ifdef _MSC_VER
            Sleep(100);
#else
            usleep(100000);
#endif
            continue;
        }

        // 从队列获取弹幕
        queue_msg barr = dy_bclient->get_barr_queue().pop ();

        // 设置弹幕颜色
#ifdef _MSC_VER
        if (barr.color == BARR_COLOR_WHITE)
            SetConsoleTextAttribute (hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // 白
        else if (barr.color == BARR_COLOR_BLUE)
            SetConsoleTextAttribute (hConsole, FOREGROUND_INTENSITY | FOREGROUND_BLUE); //蓝
        else if (barr.color == BARR_COLOR_GREEN)
            SetConsoleTextAttribute (hConsole, FOREGROUND_INTENSITY | FOREGROUND_BLUE); //绿
        else if (barr.color == BARR_COLOR_PINK)
            SetConsoleTextAttribute (hConsole, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN); //黄
        else if (barr.color == BARR_COLOR_ORANGE)
            SetConsoleTextAttribute (hConsole, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE); // 青
        else if (barr.color == BARR_COLOR_PURPLE)
            SetConsoleTextAttribute (hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE); // 品红
        else if (barr.color == BARR_COLOR_RED)
            SetConsoleTextAttribute (hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED); // 红

        cout << barr.msg_str << endl;
#else
        if (barr.color == BARR_COLOR_WHITE)
            cout << "\033[37m"; // white
        else if (barr.color == BARR_COLOR_BLUE)
            cout << "\033[34m"; // blue
        else if (barr.color == BARR_COLOR_GREEN)
            cout << "\033[32m"; // green
        else if (barr.color == BARR_COLOR_PINK)
            cout << "\033[36m"; // cyanine
        else if (barr.color == BARR_COLOR_ORANGE)
            cout << "\033[33m"; // yellow
        else if (barr.color == BARR_COLOR_PURPLE)
            cout << "\033[35m"; // purple
        else if (barr.color == BARR_COLOR_RED)
            cout << "\033[31m"; // red
        else
            cout << "\033[30m"; // black

        cout <<barr.msg_str << endl;
        cout << "\033[0m";
#endif

        // 当发生错误时关闭当前连接并新建新lp接
        if (barr.msg_type == MSG_TYPE_ERR) {
            dy_bclient->stop();
            dy_bclient->start("openbarrage.douyutv.com", 8601, 6274052);
        }
    }
}

void sig_int (int sig)
{
    if (sig == SIGINT)
    {
        cout << endl << endl;
        cout << "======================" << endl;
        cout << "process has be killed!" << endl;
        cout << endl;
        exit(1);
    }
}

int main (int argc, char **argv)
{
    dy_barrage_client client;

    // start
    client.start("openbarrage.douyutv.com", 8601, 6274052);

    // create a print thread;
    std::thread thr1(thr_print, &client);
    thr1.detach();

    // set signal
    if (signal(SIGINT, sig_int))
    {
        cout << "Set signal func error!" << endl;
        exit(1);
    }

    while (true)
#ifdef _MSC_VER
        Sleep(3600000);
#else
        sleep(3600);
#endif
    return 0;
}
