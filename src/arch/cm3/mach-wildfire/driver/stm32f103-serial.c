/*
 * driver/stm32f103-serial.c
 *
 * Copyright (c) 2007-2012  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <serial/serial.h>
#include "stm32f10x.h"

/*
 * default serial parameter.
 */
static struct serial_parameter serial_param[] = {
	[0] = {
		.baud_rate		= B115200,
		.data_bit		= DATA_BITS_8,
		.parity			= PARITY_NONE,
		.stop_bit		= STOP_BITS_1,
	},
	[1] = {
		.baud_rate		= B115200,
		.data_bit		= DATA_BITS_8,
		.parity			= PARITY_NONE,
		.stop_bit		= STOP_BITS_1,
	}
};

/*
 * serial information.
 */
static struct serial_info serial_info[] = {
	[0] = {
		.name			= "uart0",
		.desc			= "stm32f103 serial 0",
		.parameter		= &serial_param[0],
	},
	[1] = {
		.name			= "uart1",
		.desc			= "stm32f103 serial 1",
		.parameter		= &serial_param[1],
	}
};

/*
 * common function for ioctl.
 */
static int stm32f103_serial_ioctl(u32_t ch, int cmd, void * arg)
{
	u32_t baud, divider, fraction;
	u32_t temp, remainder;
	u8_t data_bit_reg, parity_reg, stop_bit_reg;
	u64_t uclk;
	struct serial_parameter param;

	if((ch < 0) || (ch > 3))
		return -1;

	memcpy(&param, &serial_param[ch], sizeof(struct serial_parameter));

	switch(cmd)
	{
	case IOCTL_WR_SERIAL_BAUD_RATE:
		param.baud_rate = *((enum SERIAL_BAUD_RATE *)arg);
		break;

	case IOCTL_WR_SERIAL_DATA_BITS:
		param.data_bit = *((enum SERIAL_DATA_BITS *)arg);
		break;

	case IOCTL_WR_SERIAL_PARITY_BIT:
		param.parity = *((enum SERIAL_PARITY_BIT *)arg);
		break;

	case IOCTL_WR_SERIAL_STOP_BITS:
		param.stop_bit = *((enum SERIAL_STOP_BITS *)arg);
		break;

	case IOCTL_RD_SERIAL_BAUD_RATE:
		*((enum SERIAL_BAUD_RATE *)arg) = param.baud_rate;
		return 0;

	case IOCTL_RD_SERIAL_DATA_BITS:
		*((enum SERIAL_DATA_BITS *)arg) = param.data_bit;
		return 0;

	case IOCTL_RD_SERIAL_PARITY_BIT:
		*((enum SERIAL_PARITY_BIT *)arg) = param.parity;
		return 0;

	case IOCTL_RD_SERIAL_STOP_BITS:
		*((enum SERIAL_STOP_BITS *)arg) = param.stop_bit;
		return 0;

	default:
		return -1;
	}

	switch(param.baud_rate)
	{
	case B50:
		baud = 50;				break;
	case B75:
		baud = 75;				break;
	case B110:
		baud = 110;				break;
	case B134:
		baud = 134;				break;
	case B200:
		baud = 200;				break;
	case B300:
		baud = 300;				break;
	case B600:
		baud = 600;				break;
	case B1200:
		baud = 1200;			break;
	case B1800:
		baud = 1800;			break;
	case B2400:
		baud = 2400;			break;
	case B4800:
		baud = 4800;			break;
	case B9600:
		baud = 9600;			break;
	case B19200:
		baud = 19200;			break;
	case B38400:
		baud = 38400;			break;
	case B57600:
		baud = 57600;			break;
	case B76800:
		baud = 76800;			break;
	case B115200:
		baud = 115200;			break;
	case B230400:
		baud = 230400;			break;
	case B380400:
		baud = 380400;			break;
	case B460800:
		baud = 460800;			break;
	case B921600:
		baud = 921600;			break;
	default:
		return -1;
	}

	switch(param.data_bit)
	{
	case DATA_BITS_5:
		data_bit_reg = 0x0;		break;
	case DATA_BITS_6:
		data_bit_reg = 0x1;		break;
	case DATA_BITS_7:
		data_bit_reg = 0x2;		break;
	case DATA_BITS_8:
		data_bit_reg = 0x3;		break;
	default:
		return -1;
	}

	switch(param.parity)
	{
	case PARITY_NONE:
		parity_reg = 0x0;		break;
	case PARITY_EVEN:
		parity_reg = 0x2;		break;
	case PARITY_ODD:
		parity_reg = 0x1;		break;
	default:
		return -1;
	}

	switch(param.stop_bit)
	{
	case STOP_BITS_1:
		stop_bit_reg = 0;		break;
	case STOP_BITS_1_5:
		return -1;
	case STOP_BITS_2:
		stop_bit_reg = 1;		break;
	default:
		return -1;
	}

	if(! clk_get_rate("uclk", &uclk))
		return -1;

	/*
	 * IBRD = UART_CLK / (16 * BAUD_RATE)
	 * FBRD = ROUND((64 * MOD(UART_CLK, (16 * BAUD_RATE))) / (16 * BAUD_RATE))
	 */
	temp = 16 * baud;
	divider = (u32_t)div64(uclk, temp);
	remainder = (u32_t)mod64(uclk, temp);
	temp = (8 * remainder) / baud;
	fraction = (temp >> 1) + (temp & 1);

	switch(ch)
	{
	case 0:
		break;
	case 1:
		break;
	default:
		return -1;
	}

	memcpy(&serial_param[ch], &param, sizeof(struct serial_parameter));

	return 0;
}

/* serial 0 */
static void stm32f103_serial0_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);

	/* Configure the NVIC Preemption Priority Bits */
/*	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);*/
}

static void stm32f103_serial0_exit(void)
{
	return;
}

static ssize_t stm32f103_serial0_read(u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
			buf[i] = USART_ReceiveData(USART1);
		else
			break;
	}
	return i;
}

static ssize_t stm32f103_serial0_write(const u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		USART_SendData(USART1, buf[i]);
		while(!(USART1->SR & USART_FLAG_TXE));
	}

	return i;
}

static int stm32f103_serial0_ioctl(int cmd, void * arg)
{
	return (stm32f103_serial_ioctl(0, cmd, arg));
}

/* serial 1 */
static void stm32f103_serial1_init(void)
{
	/* disable everything */
//	writel(REALVIEW_SERIAL1_CR, 0x0);

	/* configure uart parameter */

/*
	stm32f103_serial_ioctl( 1, IOCTL_WR_SERIAL_BAUD_RATE, (void *)(&(serial_param[1].baud_rate)) );
	stm32f103_serial_ioctl( 1, IOCTL_WR_SERIAL_DATA_BITS, (void *)(&(serial_param[1].data_bit)) );
	stm32f103_serial_ioctl( 1, IOCTL_WR_SERIAL_PARITY_BIT, (void *)(&(serial_param[1].parity)) );
	stm32f103_serial_ioctl( 1, IOCTL_WR_SERIAL_STOP_BITS, (void *)(&(serial_param[1].stop_bit)) );
*/

	/* enable the serial */
//	writel(REALVIEW_SERIAL1_CR, REALVIEW_SERIAL_CR_UARTEN |	REALVIEW_SERIAL_CR_TXE | REALVIEW_SERIAL_CR_RXE);
}

static void stm32f103_serial1_exit(void)
{
	return;
}

static ssize_t stm32f103_serial1_read(u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
/*		if( !(readb(REALVIEW_SERIAL1_FR) & REALVIEW_SERIAL_FR_RXFE) )
			buf[i] = readb(REALVIEW_SERIAL1_DATA);
		else
			break;*/
	}
	return i;
}

static ssize_t stm32f103_serial1_write(const u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
/*		 wait until there is space in the fifo
		while( (readb(REALVIEW_SERIAL1_FR) & REALVIEW_SERIAL_FR_TXFF) );

		 transmit a character
		writeb(REALVIEW_SERIAL1_DATA, buf[i]);*/
	}
	return i;
}

static int stm32f103_serial1_ioctl(int cmd, void * arg)
{
	return (stm32f103_serial_ioctl(1, cmd, arg));
}

static struct serial_driver stm32f103_serial_driver[] = {
	[0] = {
		.info	= &serial_info[0],
		.init	= stm32f103_serial0_init,
		.exit	= stm32f103_serial0_exit,
		.read	= stm32f103_serial0_read,
		.write	= stm32f103_serial0_write,
		.ioctl	= stm32f103_serial0_ioctl,
	},
	[1] = {
		.info	= &serial_info[1],
		.init	= stm32f103_serial1_init,
		.exit	= stm32f103_serial1_exit,
		.read	= stm32f103_serial1_read,
		.write	= stm32f103_serial1_write,
		.ioctl	= stm32f103_serial1_ioctl,
	}
};

static __init void stm32f103_serial_dev_init(void)
{
	struct serial_parameter * param;
	u32_t i;

	/* register serial driver */
	for(i = 0; i < ARRAY_SIZE(stm32f103_serial_driver); i++)
	{
		param = (struct serial_parameter *)resource_get_data(stm32f103_serial_driver[i].info->name);
		if(param)
			memcpy(stm32f103_serial_driver[i].info->parameter, param, sizeof(struct serial_parameter));
		else
			LOG_W("can't get the resource of \'%s\', use default parameter", stm32f103_serial_driver[i].info->name);

		if(!register_serial(&stm32f103_serial_driver[i]))
			LOG_E("failed to register serial driver '%s'", stm32f103_serial_driver[i].info->name);
	}
}

static __exit void stm32f103_serial_dev_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(stm32f103_serial_driver); i++)
	{
		if(!unregister_serial(&stm32f103_serial_driver[i]))
			LOG_E("failed to unregister serial driver '%s'", stm32f103_serial_driver[i].info->name);
	}
}

module_init(stm32f103_serial_dev_init, LEVEL_DRIVER);
module_exit(stm32f103_serial_dev_exit, LEVEL_DRIVER);
