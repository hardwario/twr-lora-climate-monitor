#ifndef _AT_H
#define _AT_H

#include <bc_atci.h>
#include <bc_cmwx1zzabz.h>
#include <bc_led.h>

#define AT_LORA_COMMANDS {"$DEVEUI", NULL, NULL, at_deveui_read, NULL, ""},\
                         {"$DEVADDR", NULL, NULL, at_devaddr_read, NULL, ""},\
                         {"$NWKSKEY", NULL, at_nwkskey_set, at_nwkskey_read, NULL, ""},\
                         {"$APPSKEY", NULL, at_appskey_set, at_appskey_read, NULL, ""}

#define AT_LED_COMMANDS {"$BLINK", at_blink, NULL, NULL, NULL, "LED blink 3 times"},\
                        {"$LED", NULL, at_led_set, NULL, at_led_help, "LED on/off"}

void at_init(bc_led_t *led, bc_cmwx1zzabz_t *lora);

bool at_deveui_read(void);
bool at_devaddr_read(void);
bool at_nwkskey_read(void);
bool at_nwkskey_set(bc_atci_param_t *param);
bool at_appskey_read(void);
bool at_appskey_set(bc_atci_param_t *param);
bool at_blink(void);
bool at_led_set(bc_atci_param_t *param);
bool at_led_help(void);

#endif // _AT_H
