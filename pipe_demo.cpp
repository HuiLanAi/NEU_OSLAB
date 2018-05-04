#include "iostream"
#include "unistd.h"
#include "cstring"
#include "stdlib.h"
#include "sys/wait.h"
#include "sys/types.h"
using namespace std;

int main()
{
    int end[2] = {0};
    pipe(end);

    pid_t subThread = fork();

    if(subThread == 0)
    //子线程
    {
        cout << "子进程" << endl;
        sleep(2);
        char message[50] = "Hello, World";
        close(end[0]);
        //关闭读端口

        for(int i = 0; i < 3; i++)
        {
            ssize_t size = write(end[1], message, strlen(message));
            //size这个返回值是什么玩意
            //写入
            sleep(5);
            cout << "子线程正在写入" << endl;
        }

        close(end[1]);//关闭写端口
        exit(0);

    }

    else
    //父线程
    {
        cout << "父线程" << endl;
        sleep(3);
        close(end[1]);
        //关闭读端口
        char cache[200] = {0};
        
        while(1)
        {
            ssize_t len = read(end[0], cache, 200);
            cout << "父线程正在读管道" << endl;
            if(len > 0)
            {
                cache[len] = 0;
                cout << "得到内容： "<< cache << endl;
            }
            else if(len == 0)
            {
                cout << "读到头了" << endl;
                break;
                //必须要读到头才能退出吗??

            }
        }

        close(end[0]);
        //关闭读端口
        int status = 0;
        pid_t wpid = waitpid(subThread, &status, 0);

        // if(wpid == subThread)
        // {
        //     cout << "子线程的waitpid?" << endl;
        // }

        exit(0);

    }

    return 0;

}