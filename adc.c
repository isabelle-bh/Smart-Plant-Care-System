#include <stdint.h>
#include <stdio.h>
#define ADC_C0 0xFF204000       // read channel 0
#define ADC_C1 0xFF204004       // write 1 to bit 15 for auto-update
#define mask_12_bits 0x00000FFF // 12 lsb to read channel input
#define LED_BASE 0xFF200060     // write led voltage to this
#define LED_DIR 0xFF200064      // write 1 to specify output
#define SW_BASE 0xFF200040      // to select which volatge
#define NUM_ADC_CHANNELS 2      // Number of ADC channels
volatile uint32_t *c0_ptr = (uint32_t *)ADC_C0;
volatile uint32_t *c1_ptr = (uint32_t *)ADC_C1;
// array of adc channels
volatile uint32_t *adc_channels[NUM_ADC_CHANNELS] = {(uint32_t *)ADC_C0, (uint32_t *)ADC_C1};

int readADCChannel(int channel)
{
    // write to autoupdate
    *c1_ptr |= 0b1;

    // if valid channel selection
    if (channel >= 0 && channel < NUM_ADC_CHANNELS)
    {
        uint32_t channel_value = *adc_channels[channel];
        // checking conversion done
        uint32_t bit_15 = (channel_value >> 15) & 0x1;
        uint32_t converted_value = channel_value & mask_12_bits;
        return (int)converted_value;
    }
    else
    {
        // invalid inputs
        return -1;
    }
}
