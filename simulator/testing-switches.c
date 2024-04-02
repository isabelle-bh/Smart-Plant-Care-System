#include <stdio.h>
#include <stdint.h>

// ADC FILE //////////////////////

#ifndef ADC_FUNCTIONS_H
#define ADC_FUNCTIONS_H

// Function prototypes
int readADCChannel(int channel);

#endif /* ADC_FUNCTIONS_H */

#define ADC_C0 0x00000000       // 0xFF204000       // read channel 0
#define ADC_C1 0x00000000       // 0xFF204004       // write 1 to bit 15 for auto-update
#define mask_12_bits 0x00000FFF // 12 lsb to read channel input
#define LED_BASE 0xFF200020     // write led voltage to this
#define LED_DIR 0xFF200064      // write 1 to specify output
#define SW_BASE 0xFF200040      // to select which volatge
#define NUM_ADC_CHANNELS 2      // Number of ADC channels
volatile uint32_t *c0_ptr = (uint32_t *)ADC_C0;
volatile uint32_t *c1_ptr = (uint32_t *)ADC_C1;
// volatile uint32_t *sw0_ptr = (uint32_t *)SW_BASE;
// volatile uint32_t *led_dir_ptr = (uint32_t *)LED_DIR;
// volatile uint32_t *led_base_ptr = (uint32_t *)LED_BASE;
volatile uint32_t *adc_channels[NUM_ADC_CHANNELS] = {(uint32_t *)ADC_C0, (uint32_t *)ADC_C1};

int readADCChannel(int channel)
{
    *c1_ptr |= 0b1;

    if (channel >= 0 && channel < NUM_ADC_CHANNELS)
    {
        uint32_t channel_value = *adc_channels[channel];
        uint32_t bit_15 = (channel_value >> 15) & 0x1;
        uint32_t converted_value = channel_value & mask_12_bits;
        return (int)converted_value;
    }
    else
    {
        return -1;
    }
}

/////////////////////////////////////

//// PLANT PROFILE FILE //////////////

#ifndef PLANT_PROFILE_H
#define PLANT_PROFILE_H

struct species_profile
{
    char name[50]; // name of plant species/type
    float soil_moisture;
    float humidity;     //
    int light;          // watts per sqft
    int light_duration; // hours of exposure needed per day
};

struct plant_environment
{
    float sqft;         // Between 0.5 and 3
    int light_exposure; // 1: low, 2: medium, 3: high/direct (for simplicity)
    int soil_volume;    // to determine how much water
    float humidity;
};

struct plant_status
{
    float soil_moisture;
    float humidity;
};

void print_plant_status(struct plant_status plant, struct species_profile profile);
float get_moisture(int moisture_channel);
float get_humidity(int humidity_channel);

extern struct species_profile plant_profiles[];

#endif /* PLANT_PROFILE_H */

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

/*
void print_plant_info(struct plant_profile plant)
{
    printf("Plant Name: %s\n", plant.name);
    printf("Height: %d cm\n", plant.height_cm);
    printf("Light Intensity: %.2f lumens\n", plant.light_intensity);
}
*/

////////////////////////////////////

#define BTN_BASE 0xFF200050
#define HEX3_HEX0_BASE 0xFF200020

extern struct species_profile plant_profiles[];
#define NUM_SWITCHES 4

volatile struct species_profile current_plant;

volatile int timer_done = 0; // 1 when it's been x amount of time

int checkTimer(void)
{
    if (timer_done)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void setCurrentPlant(void)
{
    volatile unsigned int *switchPtr = (volatile unsigned int *)SW_BASE;
    unsigned int switchState = *switchPtr;

    if (switchState < NUM_SWITCHES)
    {
        current_plant = plant_profiles[switchState];
        printf("Current plant set to: %s\n", current_plant.name);
        displayHex(switchState); // displaying plant number on LEDs
    }
    else
    {
        printf("Invalid switch state: %u\n", switchState);
    }
}

void displayHex(unsigned int number)
{
    volatile unsigned int *hexDisplayPtr = (volatile unsigned int *)LED_BASE;
    unsigned int patterns[5] = {
        0x3F, // 0
        0x06, // 1
        0x5B, // 2
        0x4F, // 3
        0x66  // 4
    };

    if (number >= 0 && number < 4)
    {
        *hexDisplayPtr = patterns[number]; // send pattern to display
    }
    else
    {
        *hexDisplayPtr = 0x00; // turn off display for invalid numbers
    }
}

int main(void)
{
    while (1)
    {
        setCurrentPlant();
        // int moisture = get_moisture(1);
    }

    return 0;
}
