#include "plant_profile.h"
#include "adc_functions.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define ADC_C0 0xFF204000       // 0xFF204000
#define ADC_C1 0xFF204004       // write 1 to bit 15 for auto-update
                                // read channel 0
#define mask_12_bits 0x00000FFF // 12 lsb to read channel input
#define LED_BASE 0xFF200060     // write led voltage to this
#define LED_DIR 0xFF200064      // write 1 to specify output
#define SW_BASE 0xFF200040      // to select which volatge

#define HEX3_HEX0_BASE 0xFF200020
#define HEX3_HEX1_BASE 0xFF200030
#define BTN_BASE 0xFF200050

extern struct species_profile plant_profiles[];
#define NUM_SWITCHES 4

volatile struct plant_status plant_readings = {0, 0};
volatile struct species_profile current_plant;

volatile int *HEX_ptr = (int *)HEX3_HEX0_BASE;
volatile int *HEX_ptr2 = (int *)HEX3_HEX1_BASE;
volatile unsigned int *led_base_ptr = (unsigned int *)LED_BASE;
volatile unsigned int *led_dir_ptr = (unsigned int *)LED_DIR;
volatile uint32_t *c1_ptr = (uint32_t *)ADC_C1;
volatile int timer_done = 0; // 1 when its been x amount of time
volatile int max_light = 35; // watts per sqft

// check timer to decide when to measure values
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

// delay in between measurements and prints for better usability
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

// rounding function for displaying the rounded values on hex
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

// displays moisture and humidity on board
void displayHex(float value)
{
    int num = round(value); // rounded num without decimals
    int thousands = num / 1000 % 10;
    int hundreds = num / 100 % 10;
    int tens = num / 10 % 10;
    int ones = num % 10;
    int hex_code[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0xFD, 0x07, 0x7F, 0x67};
    // hex 2 ptr values
    int first_2_digits = hex_code[hundreds] + (hex_code[thousands] << 8);
    // hex 1 ptr values
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
        // get current moisture value
        float soil_moisture = plant_readings.soil_moisture;
        // display value
        displayHex(soil_moisture);
        printf("Current Soil Moisture: %f\n", soil_moisture / 100);
        printf("Ideal Soil Moisture Levels for %s: %f\n", current_plant.name, current_plant.soil_moisture);
    }
    else if (buttonState == 2)
    {
        // get current humidity value
        float humidity = plant_readings.humidity;
        // display value
        displayHex(humidity);
        printf("Current Surrounding Humidity Levels: %f\n", humidity / 100);
        // comparing actual vs ideal
        printf("Ideal Surrounding Humidity Levels for %s: %f\n", current_plant.name, current_plant.humidity);
    }
    else
    {
        // for invalid button inputs
        displayHex(0000);
        printf("Displaying nothing \n");
    }

    // Return the state of the button
    return buttonState;
}

// this function reads the selected plant species (for prototyping purposes)
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

// constantly update the humidiy and moisture values by reading adc channels
void readSensors(void)
{
    plant_readings.soil_moisture = get_moisture(1); // adc channel 1
    plant_readings.humidity = get_humidity(0);      // adc channel 2
}

// displays light proportional to plant needs
void displayLight(void)
{
    int light_ratio = (int)ceil_float(((float)current_plant.light / (float)max_light) * 10);
    // more or less leds depending on strength
    unsigned int led_intensity[11] = {0x000, 0x200, 0x300, 0x380, 0x3C0, 0x3E0, 0x3F0, 0x3F8, 0x3FC, 0x3FE, 0x3FF};
    unsigned int bitmask = 0x3FF;
    *led_base_ptr = led_intensity[light_ratio] & bitmask;
}

int main(void)
{
    // auto update adc
    *c1_ptr |= 0b1;
    // specift output for led peripheral
    *led_dir_ptr = 0x000003FF;
    // start led at 0
    *led_base_ptr = 0x00000000;

    // continuously read button, switch, and sensor inputs
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