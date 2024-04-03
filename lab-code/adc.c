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
	// reading channel 0;
	if (channel == 0)
	{
		uint32_t c0_value = *c0_ptr;
		uint32_t bit_15 = (c0_value >> 15) & 0x1;
		/*
		while(!bit_15){
			c0_value = *c0_ptr;
			printf("c0%u\n",c0_value);
			bit_15 = (c0_value >> 15) & 0x1;
			printf("control bit: %u\n",bit_15);
		}*/
		uint32_t voltage_val = c0_value & mask_12_bits;
		// printf("voltage val: %u\n",voltage_val);
		return voltage_val;
	}
	else if (channel == 1)
	{
		uint32_t c1_value = *c1_ptr;
		uint32_t bit_15 = (c1_value >> 15) & 0x1;
		/*while(!bit_15){
			c1_value = *c1_ptr;
			printf("c0%u\n",c1_value);
			bit_15 = (c1_value >> 15) & 0x1;
			printf("control bit: %u\n",bit_15);
		}*/
		uint32_t voltage_val = c1_value & mask_12_bits;
		return voltage_val;
	}
}

int ceil_float(float x)
{
	int i = (int)x;
	return (x > i) ? (i + 1) : i;
}

int get_leds(uint32_t voltage)
{

	if (voltage > (uint32_t)max_voltage)
	{
		voltage = (uint32_t)max_voltage;
	}

	int v_ratio = (int)ceil_float((voltage / max_voltage) * 10);

	uint32_t led_mask = 0x3FF;
	uint32_t led_values[11] = {0x000, 0x001, 0x003, 0x007, 0x00F,
							   0x01F, 0x03F, 0x07F, 0x0FF, 0x1FF, 0x3FF};

	*led_base_ptr = led_values[v_ratio] & led_mask;

	printf("to led: %lu\n", led_values[v_ratio] & led_mask);

	return 0;
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
		uint32_t voltage = readChannel(v_channel);
		get_leds(voltage);
	}
}
