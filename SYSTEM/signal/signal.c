
#include "signal.h"

struct tagsignal_queue  g_signal_queue;

struct tagsignal_queue* get_signal_queue()
{
	return &g_signal_queue;
}

void init_signal_queue()
{
	struct tagsignal_queue* pqueue = NULL;
	pqueue = get_signal_queue();

	pqueue->count = 0;
	pqueue->head = 1;
	pqueue->tail = 0;

}

void queue_callback(unsigned short type, unsigned short param, void* paramdata,psignal_proc proc, BOOL priority) 
{
	struct tagsignal_entry* psig = NULL;
	struct tagsignal_queue* pqueue = NULL;
	pqueue = get_signal_queue();


	ENTER_CRITIC();
	if(priority) 
	{
		if (pqueue->count != MAX_SIGNAL_COUNT) 
		{
			pqueue->head += MAX_SIGNAL_COUNT - 1;
			pqueue->head %= MAX_SIGNAL_COUNT;
		}
		else
		{	
			pqueue->head += MAX_SIGNAL_COUNT - 1;//head--
			pqueue->head %= MAX_SIGNAL_COUNT;
			pqueue->tail += MAX_SIGNAL_COUNT - 1;//tail--
			pqueue->tail %= MAX_SIGNAL_COUNT;
		}
		psig = &(pqueue->sigs[pqueue->head]);
	}
	else 
	{
		if (pqueue->count != MAX_SIGNAL_COUNT) {
			pqueue->tail++;
			pqueue->tail %= MAX_SIGNAL_COUNT;
		}
		else
		{

		}
		psig = &(pqueue->sigs[pqueue->tail]);
	}
	if (pqueue->count != MAX_SIGNAL_COUNT)
	{
		pqueue->count++;
	}
	EXIT_CRITIC();

	psig->type = type;
	psig->param = param;
	psig->proc = proc;
	psig->paramdata = paramdata;
}



void poll_callbacks()
{	
	struct tagsignal_queue* pqueue = NULL;
	struct tagsignal_entry* psig = NULL;
	pqueue = get_signal_queue();

	if (pqueue->count > 0)
	{
		ENTER_CRITIC();
		psig = &pqueue->sigs[pqueue->head];

		pqueue->head++;
		pqueue->head %= MAX_SIGNAL_COUNT;
		pqueue->count--;
		EXIT_CRITIC();
		(*psig->proc)(psig->type, psig->param, psig->paramdata);
		
	}
}

