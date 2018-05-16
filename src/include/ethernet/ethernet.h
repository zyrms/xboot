#ifndef __ETHERNET_H__
#define __ETHERNET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct ethernet_t
{
	char * name;
	char hwaddr[6];

	int (*tx)(struct ethernet_t * eth);
	int (*rx)(struct ethernet_t * eth);
	void * priv;
};

static inline void ethernet_fill_hwaddr(struct ethernet_t * eth, const char * s)
{
	if(!s)
		s = "00:04:a3:11:22:33";
	sscanf(s, "%02x:%02x:%02x:%02x:%02x:%02x", &eth->hwaddr[0], &eth->hwaddr[1], &eth->hwaddr[2], &eth->hwaddr[3], &eth->hwaddr[4], &eth->hwaddr[5]);
}

struct ethernet_t * search_ethernet(const char * name);
struct ethernet_t * search_first_ethernet(void);
bool_t register_ethernet(struct device_t ** device, struct ethernet_t * eth);
bool_t unregister_ethernet(struct ethernet_t * eth);

#ifdef __cplusplus
}
#endif

#endif /* __ETHERNET_H__ */
