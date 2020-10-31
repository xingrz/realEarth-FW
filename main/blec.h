#ifndef _CLOCK_BLEC_
#define _CLOCK_BLEC_

#include "common.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

typedef void (*blec_recv_cb)(void *buf, uint16_t len);

int blec_init(blec_recv_cb recv_cb);

void blec_adv_start();
void blec_adv_stop();

void blec_send(void *buf, uint16_t len);

#endif  // _CLOCK_BLEC_
