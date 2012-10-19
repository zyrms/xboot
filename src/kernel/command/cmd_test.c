/*
 * xboot/kernel/command/cmd_test.c
 */

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <div64.h>
#include <fifo.h>
#include <byteorder.h>
#include <sha.h>
#include <xml.h>
#include <io.h>
#include <math.h>
#include <time/delay.h>
#include <time/timer.h>
#include <time/xtime.h>
#include <time/tick.h>
#include <xboot/machine.h>
#include <xboot/list.h>
#include <xboot/log.h>
#include <xboot/irq.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <xboot/chrdev.h>
#include <xboot/module.h>
#include <xboot/proc.h>
#include <shell/exec.h>
#include <fb/fb.h>
#include <fb/logo.h>
#include <rtc/rtc.h>
#include <input/input.h>
#include <input/keyboard/keyboard.h>
#include <console/console.h>
#include <loop/loop.h>
#include <command/command.h>
#include <fs/vfs/vfs.h>
#include <fs/fileio.h>
#include <mmc/mmc_host.h>
#include <mmc/mmc_card.h>
#include <graphic/surface.h>

#if	defined(CONFIG_COMMAND_TEST) && (CONFIG_COMMAND_TEST > 0)

#include <qrencode.h>

static int casesensitive = 1;
static int eightbit = 0;
static int version = 1;
static int size = 4;
static int margin = 4;
static int structured = 0;
static int micro = 0;
static QRecLevel level = QR_ECLEVEL_L;
static QRencodeMode hint = QR_MODE_8;

static void draw_QRcode(QRcode *qrcode, struct surface_t * screen, int ox, int oy)
{
	int x, y, width;
	unsigned char *p;
	struct rect_t rect;
	int size = 4;

	ox += margin * size;
	oy += margin * size;
	width = qrcode->width;
	p = qrcode->data;
	for (y = 0; y < width; y++)
	{
		for (x = 0; x < width; x++)
		{
			rect.x = ox + x * size;
			rect.y = oy + y * size;
			rect.w = size;
			rect.h = size;

			surface_fill(screen, &rect, (*p & 1) ? 0 : 0xffffff, BLEND_MODE_REPLACE);
			p++;
		}
	}
}

void draw_singleQRcode(QRinput *stream, int mask, struct surface_t * screen)
{
	u32_t c;
	QRcode *qrcode;
	int width;

	QRinput_setVersionAndErrorCorrectionLevel(stream, version, level);
	if(micro) {
		qrcode = QRcode_encodeMaskMQR(stream, mask);
	} else {
		qrcode = QRcode_encodeMask(stream, mask);
	}
	if(qrcode == NULL) {
		width = (11 + margin * 2) * size;
		fprintf(stderr, "Input data does not fit to this setting.\n");
	} else {
		version = qrcode->version;
		width = (qrcode->width + margin * 2) * size;
	}

	c = surface_map_color(screen, get_color_by_name("white"));
	surface_fill(screen, &screen->clip, c, BLEND_MODE_REPLACE);
	if(qrcode) {
		draw_QRcode(qrcode, screen, 0, 0);
	}
	QRcode_free(qrcode);
}

void view_simple(unsigned char *str, int length, struct surface_t * screen)
{
	QRinput *input;
	int ret;

	if(micro) {
		input = QRinput_newMQR(version, level);
	} else {
		input = QRinput_new2(version, level);
	}
	if(input == NULL) {
		fprintf(stderr, "Memory allocation error.\n");
	}
	if(eightbit) {
		ret = QRinput_append(input, QR_MODE_8, length, str);
	} else {
		ret = Split_splitStringToQRinput((char *)str, input, hint, casesensitive);
	}
	if(ret < 0) {
	}

	draw_singleQRcode(input, -1, screen);

	QRinput_free(input);
}

static int test(int argc, char ** argv)
{
	char buf[256];
	static int i = 0;

	struct fb * fb;
	struct surface_t * screen;

	fb = search_framebuffer("fb");
	screen = &fb->info->surface;

	struct console * con = get_console_stdout();
	console_setcursor(con, FALSE);
	console_cls(con);
	u32_t code;

	while(1)
	{
		if(console_stdin_getcode_with_timeout(&code, 250))
		{
			switch(code)
			{
			case 0x10:	/* up */

				break;

			case 0xe:	/* down */
				sprintf(buf, "mytest qrencode: val = %d", i++);
				view_simple(buf, strlen(buf), screen);
				break;

			case 0x2:	/* left */

				break;

			case 0x6:	/* right */

				break;

			default:

				break;
			}
		}
	}

	return 0;
}

static struct command test_cmd = {
	.name		= "test",
	.func		= test,
	.desc		= "test command for debug\r\n",
	.usage		= "test [arg ...]\r\n",
	.help		= "    test command for debug software by programmer.\r\n"
};

static __init void test_cmd_init(void)
{
	if(!command_register(&test_cmd))
		LOG_E("register 'test' command fail");
}

static __exit void test_cmd_exit(void)
{
	if(!command_unregister(&test_cmd))
		LOG_E("unregister 'test' command fail");
}

module_init(test_cmd_init, LEVEL_COMMAND);
module_exit(test_cmd_exit, LEVEL_COMMAND);

#endif
