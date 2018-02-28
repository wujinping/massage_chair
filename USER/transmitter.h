#pragma once
#include "def.h"
#include "nrf24xx.h"
#include "stm32f10x.h"
enum controller_type {
	CTRLER_TRANSMITTER,
	CTRLER_RECEIVER
};
struct xmit_list {
	uint8_t *packet;
	uint8_t packet_len;
	struct xmit_list *next;
};

struct controller {
	enum controller_type controller_type;
	struct nrf24xx *nrf;
	struct ds311x_device *ds311x_h;
	struct ds311x_device *ds311x_v;	
};
struct controller_init_para {
	enum controller_type type;
	struct nrf24xx_init_para *nrf_para;
	struct ds311x_init_para	*ds311x_para_h;
	struct ds311x_init_para	*ds311x_para_v;
};
void xmit_list_push(u8 *packet, u16 packet_len);
struct xmit_list * xmit_list_pop(void);

int controller_init(struct controller **pctrler, struct controller_init_para *para);
void transmitter_handle_upstream(uint8_t *pup, uint8_t len);
void transmitter_handle_downstream(uint8_t *pdown, uint8_t len);
