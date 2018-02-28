/*
*  @Copyright (c) 2007,
*  @All rights reserved.
*  @document name:signal.c
*  @function  description: �źţ��¼�������ʵ��
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
	struct tagsignal_queue* pqueue = NULL;//�൱�ڳ�ʼ���ṹָ�룬ָ��һ����ֵ��
	pqueue = get_signal_queue();//pqueueָ��ĵ�ַ��g_signal_queue

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
	pqueue = get_signal_queue();//pqueueָ��ĵ�ַ��g_signal_queue

//	DEBUG_1_16BITHEX_STRING("Into add_signal, the signal's proc is :", (unsigned int*)proc);

	ENTER_CRITIC();//�����ʲô��
	if(priority) // priority�����ȼ�����˼�ڴ�������ȼ���Ϊ�㼴ִ��if���   
	{
		//���ȼ��ߵ��źż����źŶ��е�ͷ�������ȴ���
		if (pqueue->count != MAX_SIGNAL_COUNT) //MAX_SIGNAL_COUNT�����ʲô
		{
			pqueue->head += MAX_SIGNAL_COUNT - 1;//head--
			pqueue->head %= MAX_SIGNAL_COUNT;
		}
		else
		{
				
			//��̭����β�����ź�
			pqueue->head += MAX_SIGNAL_COUNT - 1;//head--
			pqueue->head %= MAX_SIGNAL_COUNT;
			pqueue->tail += MAX_SIGNAL_COUNT - 1;//tail--
			pqueue->tail %= MAX_SIGNAL_COUNT;
		}
		psig = &(pqueue->sigs[pqueue->head]);
//		DEBUG_1_8BITHEX_STRING("add_signal with position :", pqueue->head);
//		DEBUG_1_16BITHEX_STRING("sig address is :", (unsigned int*)&(pqueue->sigs[pqueue->head]));
	}
	else //���ȼ�Ϊ��ʱִ���������         
	{
		//δ�������ȼ����ź�ֱ�Ӽ����źŶ���β����
		if (pqueue->count != MAX_SIGNAL_COUNT) {
			pqueue->tail++;
			pqueue->tail %= MAX_SIGNAL_COUNT;
		}
		else
		{
//			add_log(SIGNAL_QUEUE_OVERFLOW);
			//ͬ��ʹ����̭��β�����źţ�����һ����ӵ�β�����źţ����
			// head��tail ������
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
		psig = &pqueue->sigs[pqueue->head];//��ȡ��ͷ���ź�
//		DEBUG_1_16BITHEX_STRING("dispatch signal with sig address :", (unsigned int*)psig);

		pqueue->head++;//��ͷ��һָ����һ���ź�
		pqueue->head %= MAX_SIGNAL_COUNT;//������źŷ��������ȼ��ߵ�λ�ô���
		pqueue->count--;//ִ����һ�����������г��ȼ�һ
		EXIT_CRITIC();

//		DEBUG_1_8BITHEX_STRING("the signal's type is :", psig->type);
//		DEBUG_1_8BITHEX_STRING("the signal's param is :", psig->param);
//		DEBUG_1_16BITHEX_STRING("psig->proc is :", (unsigned int*)psig->proc);
//		DEBUG_1_16BITHEX_STRING("the signal's paramdata is :", (unsigned int*)psig->paramdata);

		(*psig->proc)(psig->type, psig->param, psig->paramdata);//������ȡ���е���Ϣ
		
	}
}
/** @}*/ // �źţ��¼�������ʵ��


