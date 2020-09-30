#ifndef _CLOCK_HSPI_
#define _CLOCK_HSPI_

#include "common.h"

#define HSPI_MAX_LEN (256 * 2)

void hspi_init(void);

void hspi_write(void *buf, uint32_t len);

#endif  // _CLOCK_HSPI_
