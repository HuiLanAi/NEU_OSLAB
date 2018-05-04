// #include "apue.h"
#include "unistd.h"
#include "sys/types.h"
#include "stdlib.h"
#include "sys/wait.h"
#include "iostream"
using namespace std;

int main()
{
    pid_t pid1, pid2;
    pid1 = fork();
    if(pid1 == 0)
    {
        cout << "子线程 " << getpid() << endl;
        sleep(3);
    }
    else
    {
        pid2 = wait(0);
        cout << "父进程等待中 " << pid2 << endl;
    }
    exit(0);

}