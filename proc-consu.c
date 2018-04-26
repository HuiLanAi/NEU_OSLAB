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
        output--;        
      }
      printf("\n[][][][][][][][][][][][][][][][][][][][][][]] %d\n", process[PRODUCER].time);

        //给生产者PCB指针赋值
      if (ret == AWAKE)
      //当前有消费者指针
      {
        (pipetb.point_consumer)->statu = READY;//消费者就绪
        pipetb.point_consumer = NULL;
        // 给特么的消费者指针赋空值？？？？
        //runc里没用到消费者指针
        runc(process, pipe, &pipetb, CONSUMER);
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
        (pipetb.point_producer)->statu = READY;
        pipetb.point_producer = NULL;
        runp(output, process, pipe, &pipetb, PRODUCER);
      }
    }

    printf("CTime: %d  PTime: %d \n", process[CONSUMER].time, process[PRODUCER].time);

    if (in[0] == 'p' && process[PRODUCER].statu == WAIT)
      printf("PRODUCER is waiting, can't be scheduled.\n");
    if (in[0] == 'c' && process[CONSUMER].statu == WAIT)
      printf("CONSUMER is waiting, can't be scheduled.\n");
    if (in[0] == 'e')
      exit(1);
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

  if ((process[PRODUCER].time) - (process[CONSUMER].time) >= 8)
  {
    // printf("\n出现溢出CTime: %d\n", process[CONSUMER].time);
    p[PRODUCER].statu = WAIT;
    return (SLEEP);
  }
  //查看缓冲区是否已满

  pipe[tb->writeptr % 8] = out;
  tb->writeptr++;
  //写入和移动指针
  if(process[CONSUMER].statu == WAIT) process[CONSUMER].statu = READY;

  p[PRODUCER].time++;//time似乎没什么用

  p[PRODUCER].statu = READY;//把状态置位READY
  if ((tb->point_consumer) != NULL)
    return (AWAKE);
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

  c = pipe[tb->readptr % 8];
  tb->readptr++;
  printf(" use %d      ", c);
  //读数据和指针移动
  //一次读取一个数据

  // if (tb->readptr >= tb->writeptr)
  //   tb->readptr = tb->writeptr = 0;
  process[t].time++;
  process[t].statu = READY;

  if(process[PRODUCER].statu == WAIT) process[PRODUCER].statu = READY;


  if ((tb->readptr) == 0 && (tb->point_producer) != NULL)
  //队列被清空且生产者指针不为空
    return (AWAKE);
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
    if ((process[PRODUCER].time - 1) / 8 > (process[CONSUMER].time ) / 8)
    {
      if ((i < tb.writeptr % 8) || (i >= tb.readptr % 8))
        printf("  %2d  |", pipe[i]);
      else
        printf("      |");
    }

    else
    {
      if ((i >= (tb.readptr % 8)) && (i < (tb.writeptr % 8)))
        printf("  %2d  |", pipe[i]);
      //输出没读的数据
      else
        printf("      |");
    }
  }
  printf("\n         ");
  for (i = 0; i < PIPESIZE; i++)
    printf("------ ");
  printf("\nwriteptr = %d, readptr = %d,  ", tb.writeptr, tb.readptr);
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
