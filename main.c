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

#define BTN_BASE 0xFF200050
#define SW_BASE 0x00000000

extern struct species_profile plant_profiles[];
#define NUM_SWITCHES 4

volatile struct species_profile current_plant;
volatile int *HEX_ptr = (int *)HEX3_HEX0_BASE; // hex address
volatile int *HEX_ptr2 = (int *)HEX3_HEX1_BASE;

volatile int timer_done = 0; // 1 when its been x amount of time

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

int getButtonInputs(void)
{
    // Declare a pointer to the button's memory-mapped I/O address
    volatile unsigned int *buttonPtr = (unsigned int *)BTN_BASE;

    // Read the state of the button
    unsigned int buttonState = *buttonPtr;

    // the button is pressed (buttonState is non-zero)
    if (buttonState)
    {
        // Get the soil moisture of the current plant
        int soil_moisture = current_plant.soil_moisture;

        // Display the soil moisture on the HEX displays
        displayHex((float)soil_moisture);
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

void displayHex(float value)
{

    int num = round(value);

    int thousands = num / 1000 % 10;
    int hundreds = num / 100 % 10;
    int tens = num / 10 % 10;
    int ones = num % 10;

    printf("thousands %d\n", thousands);
    printf("hund %d\n", hundreds);

    int hex_code[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0xFD, 0x07, 0x7F, 0x67};
    int first_2_digits = hex_code[hundreds] + (hex_code[thousands] << 8);
    int last_4_digits = (hex_code[tens] << 24) + (hex_code[ones] << 16);
    *(HEX_ptr2) = first_2_digits;
    *(HEX_ptr) = last_4_digits;
}

float truncate(float num)
{
    return (int)(num * 100) / 100.0f;
}

void displayLight()
{
}

int main(void)
{
    while (1)
    {
        setCurrentPlant();
        int moisture = get_moisture(1);
    }

    return 0;
}