#include "plant_profile.h"
#include "adc_functions.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

/*
#define LED_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX3_HEX1_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define TIMER_BASE 0xFFFEC600
#define ADC_C0 0xFF204000
#define ADC_C1 0xFF204004       // write 1 to bit 15 for auto-update
#define mask_12_bits 0x00000FFF // 12 lsb to read channel input
#define LED_BASE 0xFF200060     // write led voltage to this
#define LED_DIR 0xFF200064      // write 1 to specify output
*/

#define HEX3_HEX0_BASE 0x00000000 // 0xFF200020
#define HEX3_HEX1_BASE 0x00000000 // 0xFF200030
#define LED_BASE 0x00000000
#define ADC_C1 0xFF204004 // write 1 to bit 15 for auto-update
#define BTN_BASE 0xFF200050
#define SW_BASE 0x00000000
#define LED_DIR 0xFF200064

extern struct species_profile plant_profiles[];
#define NUM_SWITCHES 4

volatile struct plant_status plant_readings = {0, 0};
volatile struct species_profile current_plant;

volatile int *HEX_ptr = (int *)HEX3_HEX0_BASE; // hex address
volatile int *HEX_ptr2 = (int *)HEX3_HEX1_BASE;
volatile unsigned int *led_base_ptr = (unsigned int *)LED_BASE;
volatile unsigned int *led_dir_ptr = (unsigned int *)LED_DIR;
volatile uint32_t *c1_ptr = (uint32_t *)ADC_C1;

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
    }
    else if (buttonState == 2)
    {
        float humidity = plant_readings.humidity;
        displayHex(humidity);
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
        printf("Current plant set to: %s\n", current_plant.name);
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
    plant_readings.humidity = get_humidity(2);      // adc channel 2
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