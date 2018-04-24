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
    if (in[0] == 'p' && process[PRODUCER].statu == READY)
    {
      output = (output + 1) % 100;
      if ((ret = runp(output, process, pipe, &pipetb, PRODUCER)) == SLEEP)
        pipetb.point_producer = &process[PRODUCER];
      if (ret == AWAKE)
      {
        (pipetb.point_consumer)->statu = READY;
        pipetb.point_consumer = NULL;
        runc(process, pipe, &pipetb, CONSUMER);
      }
    }
    if (in[0] == 'c' && process[CONSUMER].statu == READY)
    {
      if ((ret = runc(process, pipe, &pipetb, CONSUMER)) == SLEEP)
        pipetb.point_consumer = &process[CONSUMER];
      if (ret == AWAKE)
      {
        (pipetb.point_producer)->statu = READY;
        pipetb.point_producer = NULL;
        runp(output, process, pipe, &pipetb, PRODUCER);
      }
    }
    if (in[0] == 'p' && process[PRODUCER].statu == WAIT)
      printf("PRODUCER is waiting, can't be scheduled.\n");
    if (in[0] == 'c' && process[CONSUMER].statu == WAIT)
      printf("CONSUMER is waiting, can't be scheduled.\n");
    if (in[0] == 'e')
      exit(1);
    prn(process, pipe, pipetb);
    in[0] = 'N';
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

  if (tb->writeptr >= PIPESIZE)
  {
    p[t].statu = WAIT;
    return (SLEEP);
  }
  //查看缓冲区是否已满

  pipe[tb->writeptr] = out;
  tb->writeptr++;
  //写入和移动指针
  读指针不移动吗？

  p[t].time++;
  ？？

  p[t].statu = READY;
  if ((tb->point_consumer) != NULL)
    return (AWAKE);
  return (NORMAL);
}


runc(p, pipe, tb, t) /* run consumer */
    int pipe[], t;
    struct pcb p[];
    struct pipetype *tb;

{
  int c;
  p[t].statu = RUN;
  printf("run CONSUMER. ");
  if (tb->readptr >= tb->writeptr)
  maybe是>？
  {
    p[t].statu = WAIT;
    return (SLEEP);
  }
  //判断是不是空

  
  c = pipe[tb->readptr];
  tb->readptr++;
  printf(" use %d      ", c);
  if (tb->readptr >= tb->writeptr)
    tb->readptr = tb->writeptr = 0;
  p[t].time++;
  p[t].statu = READY;
  if ((tb->readptr) == 0 && (tb->point_producer) != NULL)
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
    if ((i >= tb.readptr) && (i < tb.writeptr))
      printf("  %2d  |", pipe[i]);
    else
      printf("      |");
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
