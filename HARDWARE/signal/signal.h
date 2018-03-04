/** @defgroup signal 信号（事件）队列实现
 *  @author  fan yujin
 *  @version 1.0
 *  @date    2006-2007
 *  @{
 */
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

/** 信号回调函数类型 \n
 *  当生成信号时，将此函数传入。当信号被处理时，将调用此函数。
 *  @param[in] signal_type  
 *  @param[in] param  一个通用参数
 *  @param[in] signal_data  
 *  @return void
 
 *  @note 
 
 *  @see
 */
typedef void (CALLBACK* psignal_proc) (unsigned short, unsigned char, void *);
//这里是做函数指针声明，psignal_proc相当于void(*)(unsigned char, unsigned char, void *)类型
//psignal_proc proc就相当于void(*proc)(unsigned char, unsigned char, void *);proc是一个函数指针变量；

struct tagsignal_entry
{
	unsigned short type; // no use
	psignal_proc proc;
	unsigned char param;
	void *paramdata;
};


struct tagsignal_queue
{
	unsigned char head;
	unsigned char tail;
//	BOOL full;
//	BOOL empty;
//	BOOL locked;
	unsigned char count; // the available count of signal
	struct tagsignal_entry sigs[MAX_SIGNAL_COUNT];//在结构体tagsignal_queue里面嵌套了一个tagsignal_entry sigs的结构体
};



/** 信号队列初始化函数 \n
 *    @param[in] void 
 *    @return void
 *	@return void
 *	@author:ding guopeng
 *	@date:2006~2007
 *	@mender:fan junxia 
 *	@date:2008.09.10
 */
void init_signal_queue(void);

unsigned char get_signal_count(void);

/** 添加信号到信号队列中 \n
 *  
 *  @param[in] type 输入的信号类型, // no use
 *  @param[in] param  一个通用参数
 *  @param[in] paramdata 信号处理相关数据
 *  @param[in] proc 信号处理的回调函数
 *  @param[in] priority 信号在队列中的优先级，有以下两种可能。
 *  - FALSE 低优先级信号，加入信号队列尾部
 *  - TRUE 高优先级信号，加入信号队列头部

 *  @return void
 
 *  @note 信号队列溢出时采用淘汰尾部信号的原则
 
 *  @see timer_proc kill_timer
 */
void queue_callback(unsigned short type, unsigned char param, void* paramdata,
							   psignal_proc proc, BOOL priority);

/** 检查信号队列 \n
 *  用于处理信号，一般由主函数调用。
 *    @param[in] void
 *    @return void
 *	@return void
 *	@author:ding guopeng
 *	@date:2006~2007
 */
void poll_callbacks(void);



#endif

/** @}*/ // 信号（事件）队列实现
