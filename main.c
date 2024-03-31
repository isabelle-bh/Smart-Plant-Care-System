#include "plant_profile.h"
#include "adc_functions.h"
#include <stdio.h>
#include <stdint.h>

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

#define BTN_BASE 0xFF200050
#define SW_BASE 0x00000000

extern struct species_profile plant_profiles[];
#define NUM_SWITCHES 4

volatile struct species_profile current_plant;

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
}

// this function reads the value from the switch memory-mapped register
void setCurrentPlant(void) {
    volatile unsigned int *switchPtr = (unsigned int *)SW_BASE;
    unsigned int switchState = *switchPtr;

    // Ensure that the switchState is within the range of available profiles
    if (switchState < NUM_SWITCHES) {
        current_plant = plant_profiles[switchState];
        printf("Current plant set to: %s\n", current_plant.name);
    } else {
        printf("Invalid switch state: %u\n", switchState);
    }
}

void displayHex()
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