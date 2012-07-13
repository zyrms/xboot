/*
 * resource/res-led-heartbeat.c
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
#include <led/led.h>
#include <led/trigger.h>
#include <stm32f10x.h>

/*
 * led trigger for heartbeat using PC4 - LED2.
 */
static void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//GPIO_SetBits(GPIOC, GPIO_Pin_4);
	GPIO_ResetBits(GPIOC, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	debug("led init\r\n");
}

static void led_set(u8_t brightness)
{
	if(brightness)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		debug("led set\r\n");
	}
	else
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
		debug("led clr\r\n");
	}
}

static struct led led = {
	.name		= "led-pc4",
	.init		= led_init,
	.set		= led_set,
};

/*
 * the led-heartbeat resource.
 */
static struct resource led_heartbeat = {
	.name		= "led-heartbeat",
	.data		= &led,
};

static __init void dev_heartbeat_init(void)
{
	return;

	if(!register_resource(&led_heartbeat))
		LOG_E("failed to register resource '%s'", led_heartbeat.name);
}

static __exit void dev_heartbeat_exit(void)
{
	if(!unregister_resource(&led_heartbeat))
		LOG_E("failed to unregister resource '%s'", led_heartbeat.name);
}

module_init(dev_heartbeat_init, LEVEL_MACH_RES);
module_exit(dev_heartbeat_exit, LEVEL_MACH_RES);
