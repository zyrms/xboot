/*
 * stm32f103-tick.c
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
#include <stm32f10x.h>

/*
 * tick timer interrupt.
 */
void SysTick_Handler(void)
{
	tick_interrupt();
}

static bool_t tick_timer_init(void)
{
	SysTick_Config(SystemCoreClock / 100);

	return TRUE;
}

static struct tick stm32f103_tick = {
	.hz			= 100,
	.init		= tick_timer_init,
};

static __init void stm32f103_tick_init(void)
{
	return;

	if(!register_tick(&stm32f103_tick))
		LOG_E("failed to register tick");
}

module_init(stm32f103_tick_init, LEVEL_MACH_RES);
