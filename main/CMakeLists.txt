set(COMPONENT_SRCS
    "main.c"
    "task_btn.c"
    "task_lcd.c"
    "task_wlan.c"
    "task_ntp.c"
    "task_earth.c"
    "task_ble.c"
    "wlan.c"
    "gc9a01.c"
    "hspi.c"
    "earth.c"
    "decode_image.c"
    "blec.c"
    "userdata.c"
)

set(COMPONENT_EMBED_FILES
    "pic_offline.jpg"
    "pic_loading.jpg"
    "pic_qrcode.jpg"
)

register_component()
