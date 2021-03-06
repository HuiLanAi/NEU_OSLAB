/***************************************************************/
/*         PROGRAM NAME:         PRODUCER_CONSUMER             */
/*    This program simulates two processes, producer which     */
/* continues  to produce message and  put it into a buffer     */
/* [implemented by PIPE], and consumer which continues to get  */
/* message from the buffer and use it.                         */
/*    The program also demonstrates the synchronism between    */
/* processes and uses of PIPE.                                 */
/***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define PIPESIZE 8
#define PRODUCER 0
#define CONSUMER 1
#define RUN 0   /* statu of process */
#define WAIT 1  /* statu of process */
#define READY 2 /* statu of process */
#define NORMAL 0
#define SLEEP 1
#define AWAKE 2

#include <stdio.h>

struct pcb
{
  char *name;
  int statu;
  int time;
}; /* times of execution */


struct pipetype
{
  char type;
  int writeptr;
  int readptr;
  struct pcb *point_producer; /* write wait point */
  struct pcb *point_consumer;
}; /* read wait point  */


int pipe[PIPESIZE];
struct pipetype pipetb;
struct pcb process[2];
int wait_write = 0;
int wait_read = 0;

main()
{
  int output, ret, i;
  char in[2];
  int runp(), runc(), prn();

  pipetb.type = 'c';
  pipetb.writeptr = 0;
  pipetb.readptr = 0;
  pipetb.point_producer = pipetb.point_consumer = NULL;
  process[PRODUCER].name = "Producer\0";
  process[CONSUMER].name = "Consumer\0";
  process[PRODUCER].statu = process[CONSUMER].statu = READY;
  process[PRODUCER].time = process[CONSUMER].time = 0;
  //初始化

  output = 0;
  printf("Now starting the program!\n");
  printf("Press 'p' to run PRODUCER, press 'c' to run CONSUMER.\n");
  printf("Press 'e' to exit from the program.\n");

  for (i = 0; i < 1000; i++)
  {
    in[0] = 'N';

    while (in[0] == 'N')
    {
      scanf("%s", in);
      if (in[0] != 'e' && in[0] != 'p' && in[0] != 'c')
        in[0] = 'N';
    }
    //蜜汁输入

    if (in[0] == 'p' && process[PRODUCER].statu == READY)
    //在一开始初始化时都置READY
    {
      output = (output + 1) % 100;
      if ((ret = runp(output, process, pipe, &pipetb, PRODUCER)) == SLEEP)
      //缓冲区满时返回SLEEP
      {
        pipetb.point_producer = &process[PRODUCER];
      }

        //给生产者PCB指针赋值
      if (ret == AWAKE)
      //当前有消费者指针
      {
        pipetb.point_consumer = &process[CONSUMER];
        (pipetb.point_consumer)->statu = READY;//消费者就绪
        pipetb.point_consumer = NULL;
        // 给特么的消费者指针赋空值？？？？
        //runc里没用到消费者指针
        runc(process, pipe, &pipetb, CONSUMER);
        wait_read --;
      }
    }
    //调用了生产者进程且生产者进程为就绪态

    if (in[0] == 'c' && process[CONSUMER].statu == READY)
    {
      if ((ret = runc(process, pipe, &pipetb, CONSUMER)) == SLEEP)
      //只有在要读的数据区为空时才返回SLEEP
        pipetb.point_consumer = &process[CONSUMER];
        //给消费者PCB指针赋值
      if (ret == AWAKE)
      //队列被清空且生产者指针不为空
      {
        pipetb.point_producer = &process[PRODUCER];
        (pipetb.point_producer)->statu = READY;
        pipetb.point_producer = NULL;
        printf("\n waitline %d \n", wait_write);
      // int debug;
      // scanf("%d", &debug);
        runp(output, process, pipe, &pipetb, PRODUCER);
        wait_write --;
        if(wait_write > 0) output = (output + 1) % 100;
      }
      // else if(wait_write > 0)
      // {
      //   (pipetb.point_producer)->statu = READY;
      //   pipetb.point_producer = NULL;
      //   printf("\n waitline %d \n", wait_write);
      //   runp(output, process, pipe, &pipetb, PRODUCER);
      //   // if(wait_write == 0) break;     
      //   int debug;
      //   scanf("%d", &debug);
      //   wait_write --;
      // }

      // else ;
    }

    if (in[0] == 'p' && process[PRODUCER].statu == WAIT)
    {
      printf("PRODUCER is waiting, can't be scheduled.\n");
      wait_write++;      
    }
    if (in[0] == 'c' && process[CONSUMER].statu == WAIT)
    {
      printf("CONSUMER is waiting, can't be scheduled.\n");
      wait_read ++;      
    }
    if (in[0] == 'e')
      exit(1);

    printf("CTime: %d  PTime: %d \n", process[CONSUMER].time, process[PRODUCER].time);
    
    prn(process, pipe, pipetb);
    in[0] = 'N';
    printf("\n\n\n\n\n");
  }
}


runp(out, p, pipe, tb, t) /* run producer */
/* out为生产者写入的整数 */
    int out, pipe[], t;
    struct pcb p[];
    struct pipetype *tb;

{
  p[t].statu = RUN;//更改PCB状态
  printf("run PRODUCER. product %d     ", out);

  if ((process[PRODUCER].time) - (process[CONSUMER].time) >= PIPESIZE)
  {
    p[PRODUCER].statu = WAIT;
    return (SLEEP);
  }
  //查看缓冲区是否已满

  pipe[tb->writeptr % PIPESIZE] = out;
  tb->writeptr++;
  //写入和移动指针

  if(process[CONSUMER].statu == WAIT) process[CONSUMER].statu = READY;

  p[PRODUCER].time++;//time似乎没什么用

  p[PRODUCER].statu = READY;//把状态置位READY

  // if ((process[PRODUCER].time) - (process[CONSUMER].time) >= PIPESIZE)
  // {
  //   p[PRODUCER].statu = WAIT;
  //   return (SLEEP);
  // }

  if ((tb->point_consumer) != NULL || wait_read > 0)
  {
    process[CONSUMER].statu = READY;
    return (AWAKE);
  }
  return (NORMAL);
}


runc(process, pipe, tb, t) /* run consumer */
    int pipe[], t;//pipe是缓冲区
    struct pcb process[];
    struct pipetype *tb;

{
  int c;//读出来的数据
  process[CONSUMER].statu = RUN;
  printf("run CONSUMER. ");
  if (process[CONSUMER].time >= process[PRODUCER].time)
  {
    process[CONSUMER].statu = WAIT;
    return (SLEEP);
  }
  //判断是不是空

  c = pipe[tb->readptr % PIPESIZE];
  tb->readptr++;
  printf(" use %d      ", c);
  //读数据和指针移动
  //一次读取一个数据

  // if (tb->readptr >= tb->writeptr)
  //   tb->readptr = tb->writeptr = 0;
  process[t].time++;
  process[t].statu = READY;

  if(process[PRODUCER].statu == WAIT || wait_write > 0) 
  {
    process[PRODUCER].statu = READY;
    return (AWAKE);
  }
  return (NORMAL);
}

prn(p, pipe, tb)
    int pipe[];
    struct pipetype tb;
    struct pcb p[];

{
  int i;
  printf("\n         ");
  for (i = 0; i < PIPESIZE; i++)
    printf("------ ");
  printf("\n        |");
  for (i = 0; i < PIPESIZE; i++)
  {
    if ((process[PRODUCER].time - 1) / PIPESIZE > (process[CONSUMER].time ) / PIPESIZE)
    {
      if ((i < tb.writeptr % PIPESIZE) || (i >= tb.readptr % PIPESIZE))
        printf("  %2d  |", pipe[i]);
      else
        printf("      |");
    }

    else
    {
      if((tb.writeptr % PIPESIZE == 0) && (tb.writeptr != 0) && 
        (i >= tb.readptr % PIPESIZE) && (tb.readptr != tb.writeptr))
        printf("  %2d  |", pipe[i]);

      else if((tb.readptr % PIPESIZE == 0) && (tb.writeptr % PIPESIZE == 0) 
              && (tb.readptr != 0) && (tb.writeptr != 0))
        printf("      |");
        
      else if ((i >= (tb.readptr % PIPESIZE)) && (i < (tb.writeptr % PIPESIZE)))
        printf("  %2d  |", pipe[i]);
      //输出没读的数据
      else
        printf("      |");
    }
  }
  printf("\n         ");
  for (i = 0; i < PIPESIZE; i++)
    printf("------ ");
  printf("\nwriteptr = %d, readptr = %d,  ", tb.writeptr % 8, tb.readptr % 8);
  if (p[PRODUCER].statu == WAIT)
    printf("PRODUCER wait ");
  else
    printf("PRODUCER ready ");
  if (p[CONSUMER].statu == WAIT)
    printf("CONSUMER wait ");
  else
    printf("CONSUMER ready ");
  printf("\n");
}
