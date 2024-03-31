#include "plant_profile.h"
#include "adc_functions.h"
#include <stdio.h>
#include <stdint.h>
/*
#define LED_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX3_HEX1_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define BTN_BASE 0xFF200050
#define TIMER_BASE 0xFFFEC600
#define ADC_C0 0xFF204000
#define ADC_C1 0xFF204004       // write 1 to bit 15 for auto-update
#define mask_12_bits 0x00000FFF // 12 lsb to read channel input
#define LED_BASE 0xFF200060     // write led voltage to this
#define LED_DIR 0xFF200064      // write 1 to specify output
#define SW_BASE 0xFF200040      // to select which volatge
*/

#define SW_BASE 0x00000000

volatile int timer_done = 0; // 1 when its been x amount of time
volatile struct species_profile current_plant;

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

void setCurrentPlant(void)
{
    /*
    volatile unsigned int *switchPtr = (unsigned int *)SW_BASE;
    int switchState = *switchPtr;
    if (switchState < sizeof(*plant_profiles))
    {
        // current_plant = plant_profiles[switchState];
        current_plant = plant_profiles[2];
        printf("%s\n", current_plant.name);
    }*/

    current_plant = plant_profiles[2];
    printf("%s\n", current_plant.name);
}

void displayHex()
{
}

int main(void)
{
    while (1)
    {
        struct plant_status p1 = {2.0, 3.0};
        // int moisture = get_moisture(1);
        setCurrentPlant();

        return 0;
    }
}