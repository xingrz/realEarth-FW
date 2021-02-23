#ifndef __REALEARTH_HSPI__
#define __REALEARTH_HSPI__

#include <stdint.h>

#define HSPI_MAX_LEN (256 * 2)

void hspi_init(void);

void hspi_write(void *buf, uint32_t len);

#endif  // __REALEARTH_HSPI__
