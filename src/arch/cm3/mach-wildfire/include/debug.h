#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <xboot.h>
#include "stm32f10x.h"

void usart1_interrupt(void);
void usart1_initial(void);
u32_t usart1_write(const u8_t * buf, u32_t count);

int debug(const char * fmt, ...);

#endif /* __DEBUG_H__ */
