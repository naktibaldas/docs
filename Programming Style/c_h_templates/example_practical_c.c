//-------------------------------------------------------
//  Zemo Lygio sprendimas
//-------------------------------------------------------
#include "includes.h"

static int32_t termo_room_temperature = 0;
static uint8_t termo_room_light_status = 0;
static uint8_t termo_door_status = 0;
static uint8_t termo_action_flag = 0;

void termo_main_program(void)
{
    const unsigned int max_temp_to_reach = 28;

    for(int x = 0; x < 4; x++)
    {
        termo_room_temperature += ADCvalue[x];
    }

    termo_room_temperature = termo_room_temperature / 4;

    if((termo_door_status == TERMO_DOORS_OPEN) ||
       ((termo_door_status == TERMO_DOORS_CLOSED) && (termo_room_light_status == TERMO_LIGHTS_OFF)))
    {
        if((termo_room_temperature > max_temp_to_reach) && (termo_room_light_status == TERMO_LIGHTS_ON))
        {
            termo_room_temperature += TERMO_TEMP_COMPENSATE_VALUE;
        }
        else
        {
            termo_room_temperature -= TERMO_TEMP_COMPENSATE_VALUE;
        }
    }

    if(termo_room_temperature < TERMO_ACTION_THRESHOLD)
    {
        termo_action_flag = 1;
    }
    else
    {
        termo_action_flag = 0;
    }

    printf("Result: %d\r\n", termo_room_temperature);

    return;
}



//-------------------------------------------------------
//  Auksto Lygio sprendimas
//-------------------------------------------------------
#include "includes.h"

static int termo_room_temperature = 0;
static uint8_t termo_room_light_status = 0;
static uint8_t termo_door_status = 0;
static uint8_t termo_action_flag = 0;

void termo_main_program(void)
{
    const unsigned int max_temp_to_reach = 28;

    termo_calc_room_temp();

    if((termo_is_door_opened()) || (termo_is_door_closed() && termo_is_lights_off()))
    {
        termo_do_temperature_compens();
    }

    termo_set_action(termo_get_room_temperature());

    Debug("Result: %d\r\n", termo_room_temperature);

    return;
}

unsigned char termo_set_action(int room_temp_value)
{
    if(room_temp_value < TERMO_ACTION_THRESHOLD)
    {
        termo_action_flag = 1;
    }
    else
    {
        termo_action_flag = 0;
    }

    return;
}

void termo_do_temperature_compens(void)
{
    if(termo_get_room_temperature() > max_temp_to_reach)
    {
        if(IslightsOn())
        {
            termo_room_temperature += TERMO_TEMP_COMPENSATE_VALUE;
        }
    }
    else
    {
        termo_room_temperature -= TERMO_TEMP_COMPENSATE_VALUE;
    }

    return;
}

void termo_refresh_room_temp(int update_value)
{
    termo_room_temperature = update_value;

    return;
}

int termo_get_room_temperature(void)
{
    return termo_room_temperature;

    return;
}

int termo_calc_room_temp(void)
{
    #define TEMP_AVG_CONSTANT 4
    uint8_t x = 0;
    int32_t room_temp = 0;
    for(x = 0; x < TEMP_AVG_CONSTANT; x++)
    {
        room_temp += ADCvalue[x];
    }
    //Vidurkis is 4 semplu
    room_temp = room_temp / TEMP_AVG_CONSTANT;

    termo_refresh_room_temp(room_temp);

    return;
}

unsigned char termo_is_door_opened(void)
{
    if(termo_door_status == TERMO_DOORS_OPEN)
    {
        return 1;
    }

    return 0;
}

unsigned char termo_is_door_closed(void)
{
    if(termo_door_status == TERMO_DOORS_CLOSED)
    {
        return 1;
    }

    return 0;
}

unsigned char termo_is_lights_on(void)
{
    if(termo_room_light_status == TERMO_LIGHTS_ON)
    {
        return 1;
    }

    return 0;
}

unsigned char termo_is_lights_off(void)
{
    if(termo_room_light_status == TERMO__LIGHTS_OFF)
    {
        return 1;
    }

    return 0;
}
