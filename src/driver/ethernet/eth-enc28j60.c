/*
 * driver/eth-enc28j60.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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
#include <spi/spi.h>
#include <gpio/gpio.h>
#include <ethernet/ethernet.h>

struct eth_enc28j60_pdata_t {
	struct spi_device_t * dev;
	int irq;
	int rst;
	int rstcfg;
};

static struct device_t * eth_enc28j60_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct eth_enc28j60_pdata_t * pdat;
	struct ethernet_t * eth;
	struct device_t * dev;
	struct spi_device_t * spidev;
	int gpio = dt_read_int(n, "interrupt-gpio", -1);
	int irq = gpio_to_irq(gpio);

	spidev = spi_device_alloc(dt_read_string(n, "spi-bus", NULL), dt_read_int(n, "chip-select", 0), dt_read_int(n, "mode", 0), 8, dt_read_int(n, "speed", 0));
	if(!spidev)
		return NULL;

	pdat = malloc(sizeof(struct eth_enc28j60_pdata_t));
	if(!pdat)
	{
		spi_device_free(spidev);
		return NULL;
	}

	eth = malloc(sizeof(struct ethernet_t));
	if(!eth)
	{
		spi_device_free(spidev);
		free(pdat);
		return NULL;
	}

	pdat->dev = spidev;
	pdat->irq = irq;
	pdat->rst = dt_read_int(n, "reset-gpio", -1);
	pdat->rstcfg = dt_read_int(n, "reset-gpio-config", -1);

	eth->name = alloc_device_name(dt_read_name(n), -1);
	eth->priv = pdat;
	ethernet_fill_hwaddr(eth, dt_read_string(n, "hardware-address", NULL));

	if(pdat->rst >= 0)
	{
		if(pdat->rst >= 0)
			gpio_set_cfg(pdat->rst, pdat->rstcfg);
		gpio_set_pull(pdat->rst, GPIO_PULL_UP);
		gpio_direction_output(pdat->rst, 0);
		udelay(1);
		gpio_direction_output(pdat->rst, 1);
		udelay(1);
	}

	if(!register_ethernet(&dev, eth))
	{
		free_device_name(eth->name);
		free(eth->priv);
		free(eth);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void eth_enc28j60_remove(struct device_t * dev)
{
	struct ethernet_t * eth = (struct ethernet_t *)dev->priv;

	if(eth && unregister_ethernet(eth))
	{
		free_device_name(eth->name);
		free(eth->priv);
		free(eth);
	}
}

static void eth_enc28j60_suspend(struct device_t * dev)
{
}

static void eth_enc28j60_resume(struct device_t * dev)
{
}

static struct driver_t eth_enc28j60 = {
	.name		= "eth-enc28j60",
	.probe		= eth_enc28j60_probe,
	.remove		= eth_enc28j60_remove,
	.suspend	= eth_enc28j60_suspend,
	.resume		= eth_enc28j60_resume,
};

static __init void eth_enc28j60_driver_init(void)
{
	register_driver(&eth_enc28j60);
}

static __exit void eth_enc28j60_driver_exit(void)
{
	unregister_driver(&eth_enc28j60);
}

driver_initcall(eth_enc28j60_driver_init);
driver_exitcall(eth_enc28j60_driver_exit);
