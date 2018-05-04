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

    char message[100] = {0};
    //用户写入的字符串

    if(sonPidA == 0)
    //子线程
    {
        sonPidB = fork();
        if(sonPidB == 0)
        //子线程B
        {
            cout << "B: " << getpid() << endl;
        }
        else
        //子线程A
        {
            cout << "A: " << getpid() << endl;
        }

    }
    else
    //父线程
    {
        cout << "father: " << getpid() << endl;

    }



}