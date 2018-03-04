/** @defgroup signal �źţ��¼�������ʵ��
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

/** �źŻص��������� \n
 *  �������ź�ʱ�����˺������롣���źű�����ʱ�������ô˺�����
 *  @param[in] signal_type  
 *  @param[in] param  һ��ͨ�ò���
 *  @param[in] signal_data  
 *  @return void
 
 *  @note 
 
 *  @see
 */
typedef void (CALLBACK* psignal_proc) (unsigned short, unsigned char, void *);
//������������ָ��������psignal_proc�൱��void(*)(unsigned char, unsigned char, void *)����
//psignal_proc proc���൱��void(*proc)(unsigned char, unsigned char, void *);proc��һ������ָ�������

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
	struct tagsignal_entry sigs[MAX_SIGNAL_COUNT];//�ڽṹ��tagsignal_queue����Ƕ����һ��tagsignal_entry sigs�Ľṹ��
};



/** �źŶ��г�ʼ������ \n
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

/** ����źŵ��źŶ����� \n
 *  
 *  @param[in] type ������ź�����, // no use
 *  @param[in] param  һ��ͨ�ò���
 *  @param[in] paramdata �źŴ����������
 *  @param[in] proc �źŴ���Ļص�����
 *  @param[in] priority �ź��ڶ����е����ȼ������������ֿ��ܡ�
 *  - FALSE �����ȼ��źţ������źŶ���β��
 *  - TRUE �����ȼ��źţ������źŶ���ͷ��

 *  @return void
 
 *  @note �źŶ������ʱ������̭β���źŵ�ԭ��
 
 *  @see timer_proc kill_timer
 */
void queue_callback(unsigned short type, unsigned char param, void* paramdata,
							   psignal_proc proc, BOOL priority);

/** ����źŶ��� \n
 *  ���ڴ����źţ�һ�������������á�
 *    @param[in] void
 *    @return void
 *	@return void
 *	@author:ding guopeng
 *	@date:2006~2007
 */
void poll_callbacks(void);



#endif

/** @}*/ // �źţ��¼�������ʵ��
