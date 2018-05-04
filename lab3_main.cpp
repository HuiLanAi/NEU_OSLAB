#include "iostream"
#include "unistd.h"
#include "cstring"
#include "stdlib.h"
#include "sys/wait.h"
#include "sys/types.h"
using namespace std;

int main()
{
    cout << "father: " << getpid() << endl;

    int pipeEnd[2] = {0};
    int pipeNum = pipe(pipeEnd);
    //初始化管道

    int rawPid = getpid();
    //原始父线程的pid
    pid_t sonPidA, sonPidB;

    sonPidA = fork();

    //11个字符 第12位是0
    int chooseThread = 0;
    //选择哪个子线程执行
    if (sonPidA == 0)
    //子线程
    {
        sonPidB = fork();
        if (sonPidB == 0)
        //子线程B
        {
            for(int i = 0; i < 3; i++)
            {
                cout << "B: " << getpid() << endl;
                lockf(pipeNum, F_LOCK, 0);
                //上锁

                cout << "B即将写入..." << endl;
                char message[50] = "我是线程B";
                // sleep(3);
                close(pipeEnd[0]);
                ssize_t size = write(pipeEnd[1], message, strlen(message));
                //size这个返回值是什么玩意
                //写入
                lockf(pipeNum, F_ULOCK, 0);
                //解锁

                sleep(3);
                cout << "B线程写入完毕" << endl;
            }

            close(pipeEnd[1]);
            //关闭写端口
            exit(0);
        }
        else
        //子线程A
        {
            for (int i = 0; i < 3; i++)
            {
                cout << "A: " << getpid() << endl;
                lockf(pipeNum, F_LOCK, 0);
                //上锁

                cout << "A即将写入..." << endl;
                char message[50] = "我是线程A";
                // sleep(3);
                close(pipeEnd[0]);
                ssize_t size = write(pipeEnd[1], message, strlen(message));
                //size这个返回值是什么玩意
                //写入
                lockf(pipeNum, F_ULOCK, 0);
                //解锁

                sleep(4);
                cout << "A线程写入完毕" << endl;
            }

            close(pipeEnd[1]);
            exit(0);
        }
    }
    else
    //父线程
    {
        cout << "father: " << getpid() << endl;
        close(pipeEnd[1]);
        //关闭读端口
        char cache[200] = {0};

        while (1)
        {
            ssize_t len = read(pipeEnd[0], cache, 200);
            cout << "父线程正在读管道" << endl;
            if (len > 0)
            {
                cache[len] = 0;
                cout << "得到内容： " << cache << endl;
            }
            else if (len == 0)
            {
                cout << "所有管道消息都已读完" << endl;
                break;
                //必须要读到头才能退出吗??
            }
        }

        close(pipeEnd[0]);
        int status = 0;
        pid_t wpid = waitpid(sonPidA, &status, 0);
        wpid = waitpid(sonPidB, &status, 0);
        
        exit(0);

    }

    return 0;
}