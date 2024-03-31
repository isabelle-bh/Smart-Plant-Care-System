#include <stdint.h>
#include <stdio.h>
#define ADC_C0 0xFF204000		// read channel 0
#define ADC_C1 0xFF204004		// write 1 to bit 15 for auto-update
#define mask_12_bits 0x00000FFF // 12 lsb to read channel input
#define LED_BASE 0xFF200060		// write led voltage to this
#define LED_DIR 0xFF200064		// write 1 to specify output
#define SW_BASE 0xFF200040		// to select which volatge

volatile uint32_t *c0_ptr = (uint32_t *)ADC_C0;
volatile uint32_t *c1_ptr = (uint32_t *)ADC_C1;
volatile uint32_t *sw0_ptr = (uint32_t *)SW_BASE;
volatile uint32_t *led_dir_ptr = (uint32_t *)LED_DIR;
volatile uint32_t *led_base_ptr = (uint32_t *)LED_BASE;
float max_voltage = 4095;

int readChannel(uint32_t channel)
{

	uint32_t c0 = *c0_ptr;
	uint32_t ctrl = (*c0_ptr >> 15) & 0x1;

	while (!ctrl)
	{
		ctrl = (c0 >> 15) & 0x1;
		c0 = *c0_ptr;
	}

	uint32_t voltage_val = c0 & mask_12_bits;
	return voltage_val;

	if (channel == 1)
	{
		uint32_t c1_value = *c1_ptr;
		uint32_t bit_15 = (c1_value >> 15) & 0x1;

		while (!bit_15)
		{
			c1_value = *c1_ptr;
			bit_15 = (c1_value >> 15) & 0x1;
		}
		uint32_t voltage_val = c1_value & mask_12_bits;
		return voltage_val;
	}
}

int main(void)
{
	*c1_ptr |= 0b1;
	*led_dir_ptr = 0x000003FF;
	uint32_t v_channel = *sw0_ptr & 0x1;
	*led_base_ptr = 0x00000000;

	while (1)
	{
		v_channel = *sw0_ptr & 0x1;
		uint32_t v = readChannel(v_channel);
		display(v);
	}
}
