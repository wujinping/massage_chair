/*
*  @Copyright (c) 2007,
*  @All rights reserved.
*  @document name:signal.c
*  @function  description: 信号（事件）队列实现
*  @vertion:1.0
*  @author:fan yujin
*  @date:2006~2007

*/



#include "signal.h"


struct tagsignal_queue  g_signal_queue;

struct tagsignal_queue* get_signal_queue()
{
	return &g_signal_queue;
}

void init_signal_queue()
{
	struct tagsignal_queue* pqueue = NULL;//相当于初始化结构指针，指向一个空值，
	pqueue = get_signal_queue();//pqueue指向的地址是g_signal_queue

	pqueue->count = 0;
	pqueue->head = 1;
	pqueue->tail = 0;
//	pqueue->full = FALSE;
//	pqueue->empty = TRUE;
//	pqueue->Locked = FALSE;
}
/*
unsigned char get_signal_count()
{
	return g_signal_queue.count;
}
*/

void add_signal(unsigned short type, unsigned short param, void* paramdata,psignal_proc proc, BOOL priority) 
{
	struct tagsignal_entry* psig = NULL;
	struct tagsignal_queue* pqueue = NULL;
	pqueue = get_signal_queue();//pqueue指向的地址是g_signal_queue

//	DEBUG_1_16BITHEX_STRING("Into add_signal, the signal's proc is :", (unsigned int*)proc);

	ENTER_CRITIC();//这个是什么？
	if(priority) // priority是优先级的意思在此如果优先级不为零即执行if语句   
	{
		//优先级高的信号加在信号队列的头部，优先处理。
		if (pqueue->count != MAX_SIGNAL_COUNT) //MAX_SIGNAL_COUNT这个是什么
		{
			pqueue->head += MAX_SIGNAL_COUNT - 1;//head--
			pqueue->head %= MAX_SIGNAL_COUNT;
		}
		else
		{
				
			//淘汰掉最尾部的信号
			pqueue->head += MAX_SIGNAL_COUNT - 1;//head--
			pqueue->head %= MAX_SIGNAL_COUNT;
			pqueue->tail += MAX_SIGNAL_COUNT - 1;//tail--
			pqueue->tail %= MAX_SIGNAL_COUNT;
		}
		psig = &(pqueue->sigs[pqueue->head]);
//		DEBUG_1_8BITHEX_STRING("add_signal with position :", pqueue->head);
//		DEBUG_1_16BITHEX_STRING("sig address is :", (unsigned int*)&(pqueue->sigs[pqueue->head]));
	}
	else //优先级为零时执行下列语句         
	{
		//未设置优先级的信号直接加在信号队列尾部。
		if (pqueue->count != MAX_SIGNAL_COUNT) {
			pqueue->tail++;
			pqueue->tail %= MAX_SIGNAL_COUNT;
		}
		else
		{
//			add_log(SIGNAL_QUEUE_OVERFLOW);
			//同样使用淘汰最尾部的信号，即上一此添加到尾部的信号，因此
			// head、tail 均不变
		}
		psig = &(pqueue->sigs[pqueue->tail]);
//		DEBUG_1_8BITHEX_STRING("add_signal with position :", pqueue->tail);
//		DEBUG_1_16BITHEX_STRING("sig address is :", (unsigned int*)&(pqueue->sigs[pqueue->tail]));
	}
	if (pqueue->count != MAX_SIGNAL_COUNT)
	{
		pqueue->count++;
	}
	EXIT_CRITIC();

//	DEBUG_1_16BITHEX_STRING("sig address is :", (unsigned int*)psig);
	psig->type = type;
	psig->param = param;
	psig->proc = proc;
	psig->paramdata = paramdata;
//	DEBUG_1_8BITHEX_STRING("Out add_signal, the signal's type is :", psig->type);
//	DEBUG_1_8BITHEX_STRING("Out add_signal, the signal's param is :", psig->param);
//	DEBUG_1_16BITHEX_STRING("Out add_signal, the signal's proc is :", (unsigned int*)psig->proc);
//	DEBUG_1_16BITHEX_STRING("Out add_signal, the signal's paramdata is :", (unsigned int*)psig->paramdata);
//	DEBUG_1_8BITHEX_STRING("the signal count after add signal: ", pqueue->count);
}



void check_signal()
{	
	struct tagsignal_queue* pqueue = NULL;
	struct tagsignal_entry* psig = NULL;
	pqueue = get_signal_queue();

	if (pqueue->count > 0)
	{
//		DEBUG_1_8BITHEX_STRING("dispatch signal with position :", pqueue->head);
		ENTER_CRITIC();
		psig = &pqueue->sigs[pqueue->head];//获取队头的信号
//		DEBUG_1_16BITHEX_STRING("dispatch signal with sig address :", (unsigned int*)psig);

		pqueue->head++;//队头加一指向下一个信号
		pqueue->head %= MAX_SIGNAL_COUNT;//将这个信号放置在优先级高的位置处理
		pqueue->count--;//执行完一个操作将队列长度减一
		EXIT_CRITIC();

//		DEBUG_1_8BITHEX_STRING("the signal's type is :", psig->type);
//		DEBUG_1_8BITHEX_STRING("the signal's param is :", psig->param);
//		DEBUG_1_16BITHEX_STRING("psig->proc is :", (unsigned int*)psig->proc);
//		DEBUG_1_16BITHEX_STRING("the signal's paramdata is :", (unsigned int*)psig->paramdata);

		(*psig->proc)(psig->type, psig->param, psig->paramdata);//函数获取队列的信息
		
	}
}
/** @}*/ // 信号（事件）队列实现


