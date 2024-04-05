#include "plant_profile.h"
#include "adc_functions.h"
#include <stdio.h>

#define MOISTURE_SENSOR_BASE 0xFF200020
#define HUMIDITY_SENSOR_BASE 0xFF200020
volatile int *moisture_ptr = (int *)MOISTURE_SENSOR_BASE;
volatile int *humidity_ptr = (int *)HUMIDITY_SENSOR_BASE;

void print_plant_status(struct plant_status plant, struct species_profile profile)
{
    printf("Current Humidity:  %f [unit], Ideal Humidity:  %f [unit]", plant.humidity, profile.humidity);
    printf("Current Soil Moisture:  %f [unit], Ideal Soil Moisture:  %f [unit]", plant.soil_moisture, profile.soil_moisture);
}

float get_moisture(int moisture_channel)
{
    int moisture_value = readADCChannel(moisture_channel);
    // do some calculation
    float moisture_float = (float)moisture_value;
    return moisture_float;
}

float get_humidity(int humidity_channel)
{
    int humidity_value = readADCChannel(humidity_channel);
    // do some calculation
    float humidity_float = (float)humidity_value;

    return humidity_float;
}

struct species_profile *get_profiles(void)
{
    return plant_profiles;
}

struct species_profile plant_profiles[] = {
    {"Golden Barrel Cactus", 20.0, 40.0, 20, 14},
    {"Broccoli Microgreen", 60.0, 50.0, 30, 8},
    {"Orchid", 30.0, 60.0, 15, 7},
    {"Aloe Vera", 25.0, 40.0, 20, 12},
};
