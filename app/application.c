#include <application.h>
#include <at.h>

#define SEND_DATA_INTERVAL        (15 * 60 * 1000)
#define MEASURE_INTERVAL               (30 * 1000)
#define MEASURE_INTERVAL_BAROMETER (5 * 60 * 1000)

// LED instance
bc_led_t led;
// Button instance
bc_button_t button;
// Lora instance
bc_cmwx1zzabz_t lora;
// Accelerometer instance
bc_lis2dh12_t lis2dh12;
bc_dice_t dice;

BC_DATA_STREAM_FLOAT_BUFFER(sm_voltage_buffer, 8)
BC_DATA_STREAM_FLOAT_BUFFER(sm_temperature_buffer, (SEND_DATA_INTERVAL / MEASURE_INTERVAL))
BC_DATA_STREAM_FLOAT_BUFFER(sm_humidity_buffer, (SEND_DATA_INTERVAL / MEASURE_INTERVAL))
BC_DATA_STREAM_FLOAT_BUFFER(sm_illuminance_buffer, (SEND_DATA_INTERVAL / MEASURE_INTERVAL))
BC_DATA_STREAM_FLOAT_BUFFER(sm_pressure_buffer, (SEND_DATA_INTERVAL / MEASURE_INTERVAL_BAROMETER))
BC_DATA_STREAM_INT_BUFFER(sm_orientation_buffer, 3)

bc_data_stream_t sm_voltage;
bc_data_stream_t sm_temperature;
bc_data_stream_t sm_humidity;
bc_data_stream_t sm_illuminance;
bc_data_stream_t sm_pressure;
bc_data_stream_t sm_orientation;

bc_scheduler_task_id_t battery_measure_task_id;

enum {
    HEADER_BOOT         = 0x00,
    HEADER_UPDATE       = 0x01,
    HEADER_BUTTON_CLICK = 0x02,
    HEADER_BUTTON_HOLD  = 0x03,

} header = HEADER_BOOT;

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_CLICK)
    {
        header = HEADER_BUTTON_CLICK;

        bc_scheduler_plan_now(0);
    }
    else if (event == BC_BUTTON_EVENT_HOLD)
    {
        header = HEADER_BUTTON_HOLD;

        bc_scheduler_plan_now(0);
    }
}

void climate_module_event_handler(bc_module_climate_event_t event, void *event_param)
{
    float value = NAN;

    if (event == BC_MODULE_CLIMATE_EVENT_UPDATE_THERMOMETER)
    {
        bc_module_climate_get_temperature_celsius(&value);

        bc_data_stream_feed(&sm_temperature, &value);
    }
    else if (event == BC_MODULE_CLIMATE_EVENT_UPDATE_HYGROMETER)
    {
        bc_module_climate_get_humidity_percentage(&value);

        bc_data_stream_feed(&sm_humidity, &value);
    }
    else if (event == BC_MODULE_CLIMATE_EVENT_UPDATE_LUX_METER)
    {
        bc_module_climate_get_illuminance_lux(&value);

        bc_data_stream_feed(&sm_illuminance, &value);
    }
    else if (event == BC_MODULE_CLIMATE_EVENT_UPDATE_BAROMETER)
    {
        bc_module_climate_get_pressure_pascal(&value);

        bc_data_stream_feed(&sm_pressure, &value);
    }
}

void battery_event_handler(bc_module_battery_event_t event, void *event_param)
{
    if (event == BC_MODULE_BATTERY_EVENT_UPDATE)
    {
        float voltage = NAN;

        bc_module_battery_get_voltage(&voltage);

        bc_data_stream_feed(&sm_voltage, &voltage);
    }
}

void battery_measure_task(void *param)
{
    if (!bc_module_battery_measure())
    {
        bc_scheduler_plan_current_now();
    }
}

void lis2dh12_event_handler(bc_lis2dh12_t *self, bc_lis2dh12_event_t event, void *event_param)
{
    if (event == BC_LIS2DH12_EVENT_UPDATE)
    {
        bc_lis2dh12_result_g_t g;

        if (bc_lis2dh12_get_result_g(self, &g))
        {
            bc_dice_feed_vectors(&dice, g.x_axis, g.y_axis, g.z_axis);

            int orientation = (int) bc_dice_get_face(&dice);

            bc_data_stream_feed(&sm_orientation, &orientation);
        }
    }
}

void lora_callback(bc_cmwx1zzabz_t *self, bc_cmwx1zzabz_event_t event, void *event_param)
{
    if (event == BC_CMWX1ZZABZ_EVENT_ERROR)
    {
        bc_led_set_mode(&led, BC_LED_MODE_BLINK_FAST);
    }
    else if (event == BC_CMWX1ZZABZ_EVENT_SEND_MESSAGE_START)
    {
        bc_led_set_mode(&led, BC_LED_MODE_ON);

        bc_scheduler_plan_relative(battery_measure_task_id, 20);
    }
    else if (event == BC_CMWX1ZZABZ_EVENT_SEND_MESSAGE_DONE)
    {
        bc_led_set_mode(&led, BC_LED_MODE_OFF);
    }
    else if (event == BC_CMWX1ZZABZ_EVENT_READY)
    {
        bc_led_set_mode(&led, BC_LED_MODE_OFF);
    }
}

bool at_send(void)
{
    bc_scheduler_plan_now(0); 

    return true;
}

bool at_status(void)
{
    float value_avg = NAN;

    static const struct {
        bc_data_stream_t *stream;
        const char *name;
        int precision;
    } values[] = {
            {&sm_voltage, "Voltage", 1},
            {&sm_temperature, "Temperature", 1},
            {&sm_humidity, "Humidity", 1},
            {&sm_illuminance, "Illuminance", 1},
            {&sm_pressure, "Pressure", 0},
    };

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++)
    {
        value_avg = NAN;

        if (bc_data_stream_get_average(values[i].stream, &value_avg))
        {
            bc_atci_printf("$STATUS: \"%s\",%.*f", values[i].name, values[i].precision, value_avg);
        }
        else
        {
            bc_atci_printf("$STATUS: \"%s\",", values[i].name);
        }
    }

    int orientation;

    if (bc_data_stream_get_median(&sm_orientation, &orientation))
    {
        bc_atci_printf("$STATUS: \"Orientation\",%d", orientation);
    }
    else
    {
        bc_atci_printf("$STATUS: \"Orientation\",", orientation);
    }

    return true;
}

void application_init(void)
{
    bc_data_stream_init(&sm_voltage, 1, &sm_voltage_buffer);
    bc_data_stream_init(&sm_temperature, 1, &sm_temperature_buffer);
    bc_data_stream_init(&sm_humidity, 1, &sm_humidity_buffer);
    bc_data_stream_init(&sm_illuminance, 1, &sm_illuminance_buffer);
    bc_data_stream_init(&sm_pressure, 1, &sm_pressure_buffer);
    bc_data_stream_init(&sm_orientation, 1, &sm_orientation_buffer);
    
    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_set_mode(&led, BC_LED_MODE_ON);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    // Initialize climate module
    bc_module_climate_init();
    bc_module_climate_set_event_handler(climate_module_event_handler, NULL);
    bc_module_climate_set_update_interval_thermometer(MEASURE_INTERVAL);
    bc_module_climate_set_update_interval_hygrometer(MEASURE_INTERVAL);
    bc_module_climate_set_update_interval_lux_meter(MEASURE_INTERVAL);
    bc_module_climate_set_update_interval_barometer(MEASURE_INTERVAL_BAROMETER);

    // Initialize battery
    bc_module_battery_init();
    bc_module_battery_set_event_handler(battery_event_handler, NULL);
    battery_measure_task_id = bc_scheduler_register(battery_measure_task, NULL, 2020);

    bc_dice_init(&dice, BC_DICE_FACE_UNKNOWN);

    bc_lis2dh12_init(&lis2dh12, BC_I2C_I2C0, 0x19);
    bc_lis2dh12_set_resolution(&lis2dh12, BC_LIS2DH12_RESOLUTION_8BIT);

    bc_lis2dh12_set_event_handler(&lis2dh12, lis2dh12_event_handler, NULL);
    bc_lis2dh12_set_update_interval(&lis2dh12, MEASURE_INTERVAL);

    // Initialize lora module
    bc_cmwx1zzabz_init(&lora, BC_UART_UART1);
    bc_cmwx1zzabz_set_event_handler(&lora, lora_callback, NULL);
    bc_cmwx1zzabz_set_mode(&lora, BC_CMWX1ZZABZ_CONFIG_MODE_ABP);
    bc_cmwx1zzabz_set_class(&lora, BC_CMWX1ZZABZ_CONFIG_CLASS_A);

    // Initialize AT command interface
    at_init(&led, &lora);
    static const bc_atci_command_t commands[] = {
            AT_LORA_COMMANDS,
            {"$SEND", at_send, NULL, NULL, NULL, "Immediately send packet"},
            {"$STATUS", at_status, NULL, NULL, NULL, "Show status"},
            AT_LED_COMMANDS,
            BC_ATCI_COMMAND_CLAC,
            BC_ATCI_COMMAND_HELP
    };
    bc_atci_init(commands, BC_ATCI_COMMANDS_LENGTH(commands));

    bc_scheduler_plan_current_relative(10 * 1000);
}

void application_task(void)
{
    if (!bc_cmwx1zzabz_is_ready(&lora))
    {
        bc_scheduler_plan_current_relative(100);
        
        return;
    }

    static uint8_t buffer[10];

    memset(buffer, 0xff, sizeof(buffer));

    buffer[0] = header;

    float voltage_avg = NAN;

    bc_data_stream_get_average(&sm_voltage, &voltage_avg);

    if (!isnan(voltage_avg))
    {
        buffer[1] = ceil(voltage_avg * 10.f);
    }

    int orientation;

    if (bc_data_stream_get_median(&sm_orientation, &orientation))
    {
        buffer[2] = orientation;
    }
    
    float temperature_avg = NAN;

    bc_data_stream_get_average(&sm_temperature, &temperature_avg);

    if (!isnan(temperature_avg))
    {
        int16_t temperature_i16 = (int16_t) (temperature_avg * 10.f);

        buffer[3] = temperature_i16 >> 8;
        buffer[4] = temperature_i16;
    }

    float humidity_avg = NAN;

    bc_data_stream_get_average(&sm_humidity, &humidity_avg);

    if (!isnan(humidity_avg))
    {
        buffer[5] = humidity_avg * 2;
    }

    float illuminance_avg = NAN;

    bc_data_stream_get_average(&sm_illuminance, &illuminance_avg);

    if (!isnan(illuminance_avg))
    {
        if (illuminance_avg > 65534)
        {
            illuminance_avg = 65534;
        }

        uint16_t value = (uint16_t) illuminance_avg;
        buffer[6] = value >> 8;
        buffer[7] = value;
    }

    float pressure_avg = NAN;

    bc_data_stream_get_average(&sm_pressure, &pressure_avg);

    if (!isnan(pressure_avg))
    {
        uint16_t value = pressure_avg / 2.f;
        buffer[8] = value >> 8;
        buffer[9] = value;
    }

    bc_cmwx1zzabz_send_message(&lora, buffer, sizeof(buffer));

    static char tmp[sizeof(buffer) * 2 + 1];
    for (size_t i = 0; i < sizeof(buffer); i++) 
    {
        sprintf(tmp + i * 2, "%02x", buffer[i]);
    }

    bc_atci_printf("$SEND: %s", tmp);

    header = HEADER_UPDATE;

    bc_scheduler_plan_current_relative(SEND_DATA_INTERVAL);
}
