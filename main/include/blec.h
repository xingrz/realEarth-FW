#ifndef __REALEARTH_BLEC__
#define __REALEARTH_BLEC__

#include <stdint.h>

typedef void (*blec_recv_cb)(void *buf, uint16_t len);

int blec_init(blec_recv_cb recv_cb);

void blec_adv_start();
void blec_adv_stop();

void blec_send(void *buf, uint16_t len);

#endif  // __REALEARTH_BLEC__
