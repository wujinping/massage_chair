
#ifndef _SIGNAL_H_
#define _SIGNAL_H_


#define MAX_SIGNAL_COUNT 200
#define CALLBACK
#define ENTER_CRITIC()
#define EXIT_CRITIC()

typedef char               BOOL;
#ifndef NULL
#define NULL    ((void *)0)
#endif


typedef void (CALLBACK* psignal_proc) (unsigned short, unsigned char, void *);

struct tagsignal_entry
{
	unsigned short type; 
	psignal_proc proc;
	unsigned char param;
	void *paramdata;
};


struct tagsignal_queue
{
	unsigned char head;
	unsigned char tail;
	unsigned char count; 
	struct tagsignal_entry sigs[MAX_SIGNAL_COUNT];
};

void init_signal_queue(void);

unsigned char get_signal_count(void);

void queue_callback(unsigned short type, unsigned short param, void* paramdata,
							   psignal_proc proc, BOOL priority);
void poll_callbacks(void);

#endif

