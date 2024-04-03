#include <stdio.h>
#include <stdint.h>

// ADC FILE //////////////////////

#ifndef ADC_FUNCTIONS_H
#define ADC_FUNCTIONS_H

// Function prototypes
int readADCChannel(int channel);

#endif /* ADC_FUNCTIONS_H */

#define ADC_C0 0xFF204000       // 0xFF204000
#define ADC_C1 0xFF204004       // write 1 to bit 15 for auto-update
                                // read channel 0
#define mask_12_bits 0x00000FFF // 12 lsb to read channel input
#define LED_BASE 0xFF200060     // write led voltage to this
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
    float soil_volume;  // to determine how much water
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

////////////////////////////////////

#define HEX3_HEX0_BASE 0xFF200020
#define HEX3_HEX1_BASE 0xFF200030
#define BTN_BASE 0xFF200050

extern struct species_profile plant_profiles[];
#define NUM_SWITCHES 4

volatile struct plant_status plant_readings = {0, 0};
volatile struct species_profile current_plant;

volatile int *HEX_ptr = (int *)HEX3_HEX0_BASE; // hex address
volatile int *HEX_ptr2 = (int *)HEX3_HEX1_BASE;
volatile unsigned int *led_base_ptr = (unsigned int *)LED_BASE;
volatile unsigned int *led_dir_ptr = (unsigned int *)LED_DIR;

volatile int timer_done = 0; // 1 when its been x amount of time
volatile int max_light = 35; // watts per sqft

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

void delay(int seconds)
{
    volatile unsigned long i, j;
    for (i = 0; i < seconds; i++)
    {
        for (j = 0; j < 1000000; j++)
        {
            // do nothing
        }
    }
}

int round(float num)
{
    int intPart = (int)num;
    float fractionalPart = num - intPart;

    if (fractionalPart >= 0.5)
    {
        return intPart + 1;
    }
    else
    {
        return intPart;
    }
}

void displayHex(float value)
{
    int num = round(value);
    int thousands = num / 1000 % 10;
    int hundreds = num / 100 % 10;
    int tens = num / 10 % 10;
    int ones = num % 10;

    int hex_code[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0xFD, 0x07, 0x7F, 0x67};
    int first_2_digits = hex_code[hundreds] + (hex_code[thousands] << 8);
    int last_4_digits = (hex_code[tens] << 24) + (hex_code[ones] << 16);
    *(HEX_ptr2) = first_2_digits;
    *(HEX_ptr) = last_4_digits;
}

int getButtonInputs(void)
{
    // Declare a pointer to the button's memory-mapped I/O address
    volatile unsigned int *buttonPtr = (unsigned int *)BTN_BASE;

    unsigned int buttonState = *buttonPtr;

    // Read the state of the button

    // the button is pressed (buttonState is non-zero)
    if (buttonState == 1)
    {
        float soil_moisture = plant_readings.soil_moisture;
        displayHex(soil_moisture);
        printf("Current Soil Moisture: %f\n", soil_moisture / 100);
        printf("Ideal Soil Moisture Levels for %s: %f\n", current_plant.name, current_plant.soil_moisture);
    }
    else if (buttonState == 2)
    {
        float humidity = plant_readings.humidity;
        displayHex(humidity);
        printf("Current Surrounding Humidity Levels: %f\n", humidity / 100);
        printf("Ideal Surrounding Humidity Levels for %s: %f\n", current_plant.name, current_plant.humidity);
    }
    else
    {
        float soil_moisture = plant_readings.soil_moisture;
        displayHex(0000);
        printf("Displaying nothing \n");
    }

    // Return the state of the button
    return buttonState;
}

// this function reads the value from the switch memory-mapped register
void setCurrentPlant(void)
{
    volatile unsigned int *switchPtr = (unsigned int *)SW_BASE;
    unsigned int switchState = *switchPtr;

    // Ensure that the switchState is within the range of available profiles
    if (switchState < NUM_SWITCHES)
    {
        current_plant = plant_profiles[switchState];
        printf("\nCurrent plant set to: %s\n\n", current_plant.name);
    }
    else
    {
        printf("Invalid switch state: %u\n", switchState);
    }
}

int ceil_float(float x)
{
    int i = (int)x;
    return (x > i) ? (i + 1) : i;
}

void readSensors(void)
{
    plant_readings.soil_moisture = get_moisture(1); // adc channel 1
    plant_readings.humidity = get_humidity(0);      // adc channel 2
}

void displayLight(void)
{
    int light_ratio = (int)ceil_float(((float)current_plant.light / (float)max_light) * 10);
    unsigned int led_intensity[11] = {0x000, 0x200, 0x300, 0x380, 0x3C0, 0x3E0, 0x3F0, 0x3F8, 0x3FC, 0x3FE, 0x3FF};
    unsigned int bitmask = 0x3FF;
    *led_base_ptr = led_intensity[light_ratio] & bitmask;
}

int main(void)
{
    *c1_ptr |= 0b1;
    *led_dir_ptr = 0x000003FF;
    *led_base_ptr = 0x00000000;

    while (1)
    {
        setCurrentPlant();
        displayLight();
        readSensors();
        getButtonInputs();

        delay(5);
    }

    return 0;
}