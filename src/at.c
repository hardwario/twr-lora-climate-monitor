#include "at.h"
#include <twr.h>

static struct
{
    twr_led_t *led;
    twr_cmwx1zzabz_t *lora;
    char tmp[36];

} _at;

static bool _at_param_format_and_test(twr_atci_param_t *param, uint8_t length);

void at_init(twr_led_t *led, twr_cmwx1zzabz_t *lora)
{
    _at.led = led;
    _at.lora = lora;
}

bool at_deveui_read(void)
{
    twr_cmwx1zzabz_get_deveui(_at.lora, _at.tmp);

    twr_atci_printfln("$DEVEUI: %s", _at.tmp);

    return true;
}

bool at_deveui_set(twr_atci_param_t *param)
{
    if (!_at_param_format_and_test(param, 16))
    {
        return false;
    }

    twr_cmwx1zzabz_set_deveui(_at.lora, param->txt);

    return true;
}

bool at_devaddr_read(void)
{
    twr_cmwx1zzabz_get_devaddr(_at.lora, _at.tmp);

    twr_atci_printfln("$DEVADDR: %s", _at.tmp);

    return true;
}

bool at_devaddr_set(twr_atci_param_t *param)
{

    twr_cmwx1zzabz_set_devaddr(_at.lora, param->txt);

    return true;
}

bool at_nwkskey_read(void)
{
    twr_cmwx1zzabz_get_nwkskey(_at.lora, _at.tmp);

    twr_atci_printfln("$NWKSKEY: %s", _at.tmp);

    return true;
}

bool at_nwkskey_set(twr_atci_param_t *param)
{
    if (!_at_param_format_and_test(param, 32))
    {
        return false;
    }

    twr_cmwx1zzabz_set_nwkskey(_at.lora, param->txt);

    return true;
}

bool at_appkey_read(void)
{
    twr_cmwx1zzabz_get_appkey(_at.lora, _at.tmp);

    twr_atci_printfln("$APPKEY: %s", _at.tmp);

    return true;
}

bool at_appkey_set(twr_atci_param_t *param)
{
    if (!_at_param_format_and_test(param, 32))
    {
        return false;
    }

    twr_cmwx1zzabz_set_appkey(_at.lora, param->txt);

    return true;
}

bool at_appeui_read(void)
{
    twr_cmwx1zzabz_get_appeui(_at.lora, _at.tmp);

    twr_atci_printfln("$APPEUI: %s", _at.tmp);

    return true;
}

bool at_appeui_set(twr_atci_param_t *param)
{
    if (!_at_param_format_and_test(param, 16))
    {
        return false;
    }

    twr_cmwx1zzabz_set_appeui(_at.lora, param->txt);

    return true;
}

bool at_appskey_read(void)
{
    twr_cmwx1zzabz_get_appskey(_at.lora, _at.tmp);

    twr_atci_printfln("$APPSKEY: %s", _at.tmp);

    return true;
}

bool at_appskey_set(twr_atci_param_t *param)
{
    if (!_at_param_format_and_test(param, 32))
    {
        return false;
    }

    twr_cmwx1zzabz_set_appskey(_at.lora, param->txt);

    return true;
}

bool at_band_read(void)
{
    twr_cmwx1zzabz_config_band_t band = twr_cmwx1zzabz_get_band(_at.lora);

    twr_atci_printfln("$BAND: %d", band);

    return true;
}

bool at_band_set(twr_atci_param_t *param)
{
    uint8_t band = atoi(param->txt);

    if (band > 8)
    {
        return false;
    }

    twr_cmwx1zzabz_set_band(_at.lora, band);

    return true;
}

bool at_mode_read(void)
{
    twr_cmwx1zzabz_config_mode_t mode = twr_cmwx1zzabz_get_mode(_at.lora);

    twr_atci_printfln("$MODE: %d", mode);

    return true;
}

bool at_mode_set(twr_atci_param_t *param)
{
    uint8_t mode = atoi(param->txt);

    if (mode > 1)
    {
        return false;
    }

    twr_cmwx1zzabz_set_mode(_at.lora, mode);

    return true;
}

bool at_join(void)
{
    twr_cmwx1zzabz_join(_at.lora);

    return true;
}

bool at_nwk_read(void)
{
    uint8_t nwk_public = twr_cmwx1zzabz_get_nwk_public(_at.lora);

    twr_atci_printfln("$NWK: %d", nwk_public);

    return true;
}

bool at_nwk_set(twr_atci_param_t *param)
{
    uint8_t nwk_public = atoi(param->txt);

    if (nwk_public > 1)
    {
        return false;
    }

    twr_cmwx1zzabz_set_nwk_public(_at.lora, nwk_public);

    return true;
}

bool at_blink(void)
{
    twr_led_blink(_at.led, 3);

    return true;
}

bool at_led_set(twr_atci_param_t *param)
{
    if (param->length != 1)
    {
        return false;
    }

    if (param->txt[0] == '1')
    {
        twr_led_set_mode(_at.led, TWR_LED_MODE_ON);

        return true;
    }

    if (param->txt[0] == '0')
    {
        twr_led_set_mode(_at.led, TWR_LED_MODE_OFF);

        return true;
    }

    return false;
}

bool at_led_help(void)
{
    twr_atci_printfln("$LED: (0,1)");

    return true;
}

static bool _at_param_format_and_test(twr_atci_param_t *param, uint8_t length)
{

    // Capitalize letters
    for (uint32_t i = 0; param->txt[i] != '\0'; i++) {
        if (param->txt[i] >= 'a' && param->txt[i] <= 'z') {
            param->txt[i] = param->txt[i] - 32;
        }
    }

    // Skip spaces
    for (uint32_t i = 0; i < strlen(param->txt); i++)
    {
        while (param->txt[i] == ' ')
        {
            for (uint32_t q = 0; q < strlen(param->txt); q++)
            {
                param->txt[i + q] = param->txt[i + q + 1];
            }
        }
    }

    // Correct new string length
    param->length = strlen(param->txt);

    if (param->length != length)
    {
        return false;
    }

    // Check the string is HEX
    for (size_t i = 0; i < strlen(param->txt); i++)
    {
        if ((param->txt[i] >= '0' && param->txt[i] <= '9') || (param->txt[i] >= 'A' && param->txt[i] <= 'F'))
        {
            continue;
        }

        return false;
    }

    return true;
}
