#include <at.h>
#include <bc_atci.h>

static struct
{
    bc_led_t *led;
    bc_cmwx1zzabz_t *lora;
    char tmp[36];

} _at;

static bool _at_param_key_test(bc_atci_param_t *param);

void at_init(bc_led_t *led, bc_cmwx1zzabz_t *lora)
{
    _at.led = led;
    _at.lora = lora;
}

bool at_deveui_read(void)
{
    bc_cmwx1zzabz_get_deveui(_at.lora, _at.tmp);

    bc_atci_printf("$DEVEUI: %s", _at.tmp);

    return true;
}

bool at_devaddr_read(void)
{
    bc_cmwx1zzabz_get_devaddr(_at.lora, _at.tmp);

    bc_atci_printf("$DEVADDR: %s", _at.tmp);

    return true;
}

bool at_nwkskey_read(void)
{
    bc_cmwx1zzabz_get_nwkskey(_at.lora, _at.tmp);

    bc_atci_printf("$NWKSKEY: %s", _at.tmp);

    return true;
}

bool at_nwkskey_set(bc_atci_param_t *param)
{
    if (!_at_param_key_test(param))
    {
        return false;
    }

    bc_cmwx1zzabz_set_nwkskey(_at.lora, param->txt);

    return true;
}

bool at_appskey_read(void)
{
    bc_cmwx1zzabz_get_appskey(_at.lora, _at.tmp);

    bc_atci_printf("$APPSKEY: %s", _at.tmp);

    return true;
}

bool at_appskey_set(bc_atci_param_t *param)
{
    if (!_at_param_key_test(param))
    {
        return false;
    }

    bc_cmwx1zzabz_set_appskey(_at.lora, param->txt);

    return true;
}

bool at_blink(void)
{
    bc_led_blink(_at.led, 3);

    return true;
}

bool at_led_set(bc_atci_param_t *param)
{
    if (param->length != 1)
    {
        return false;
    }

    if (param->txt[0] == '1')
    {
        bc_led_set_mode(_at.led, BC_LED_MODE_ON);

        return true;
    }

    if (param->txt[0] == '0')
    {
        bc_led_set_mode(_at.led, BC_LED_MODE_OFF);

        return true;
    }

    return false;
}

bool at_led_help(void)
{
    bc_atci_printf("$LED: (0,1)");

    return true;
}

static bool _at_param_key_test(bc_atci_param_t *param)
{
    if (param->length != 32)
    {
        return false;
    }

    for (size_t i = 0; i < param->length; i++)
    {
        if (isdigit(param->txt[i]) || isupper(param->txt[i]))
        {
            continue;
        }

        return false;
    }

    return true;
}
