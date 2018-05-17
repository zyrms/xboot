#ifndef __ETHERNET_H__
#define __ETHERNET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct ethernet_t
{
	char * name;

	void (*open)(struct ethernet_t * eth);
	void (*close)(struct ethernet_t * eth);
	int (*send)(struct ethernet_t * eth, u8_t * buf, int len);
	int (*recv)(struct ethernet_t * eth, u8_t * buf, int len);
	void (*getaddr)(struct ethernet_t * eth, u8_t * addr);
	void (*setaddr)(struct ethernet_t * eth, u8_t * addr);
	void * priv;
};

struct ethernet_t * search_ethernet(const char * name);
struct ethernet_t * search_first_ethernet(void);
bool_t register_ethernet(struct device_t ** device, struct ethernet_t * eth);
bool_t unregister_ethernet(struct ethernet_t * eth);

void ethernet_open(struct ethernet_t * eth);
void ethernet_close(struct ethernet_t * eth);
int ethernet_send(struct ethernet_t * eth, u8_t * buf, int len);
int ethernet_recv(struct ethernet_t * eth, u8_t * buf, int len);
void ethernet_getaddr(struct ethernet_t * eth, u8_t * addr);
void ethernet_setaddr(struct ethernet_t * eth, u8_t * addr);
void ethernet_genaddr(u8_t * addr, const char * s);

#ifdef __cplusplus
}
#endif

#endif /* __ETHERNET_H__ */
