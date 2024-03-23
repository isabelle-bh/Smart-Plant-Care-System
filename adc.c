#include <stdint.h>
#include <stdio.h>

#define ADC_C0 0xFF204000       // read channel 0
#define ADC_C1 0xFF204004       // write 1 to bit 15 for auto-update
#define mask_12_bits 0x00000FFF // 12 lsb to read channel input
#define LED_BASE 0xFF200060     // write led voltage to this
#define LED_DIR 0xFF200064      // write 1 to specify output
#define SW_BASE 0xFF200040      // to select which voltage

volatile uint32_t *c0_ptr = (uint32_t *)ADC_C0;
volatile uint32_t *c1_ptr = (uint32_t *)ADC_C1;

struct ADCConverter
{
    volatile uint32_t *c0_ptr;
    volatile uint32_t *c1_ptr;
};

struct ADCConverter createConverter()
{
    struct ADCConverter adc;
    adc.c0_ptr = (uint32_t *)ADC_C0;
    adc.c1_ptr = (uint32_t *)ADC_C1;
    return adc;
}

int readChannel(volatile uint32_t *channel_ptr)
{
    uint32_t cVal = *channel_ptr;
    uint32_t ctrl = (*channel_ptr >> 15) & 0x1;

    while (!ctrl)
    {
        ctrl = (cVal >> 15) & 0x1;
        cVal = *channel_ptr;
    }

    uint32_t voltage_val = cVal & mask_12_bits;
    return voltage_val;
}

int analyzeValue(volatile uint32_t digitalValue)
{
    // do something by analyzing the current plant needs
}
