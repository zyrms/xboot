#ifndef __ARM64_SMP_H__
#define __ARM64_SMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xconfigs.h>
#include <arm64.h>

#if defined(CONFIG_MAX_SMP_CPUS) && (CONFIG_MAX_SMP_CPUS > 1)
static inline int smp_processor_id(void)
{
	return arm64_read_sysreg(tpidrro_el0);
}
#else
static inline int smp_processor_id(void)
{
	return 0;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ARM64_SMP_H__ */
