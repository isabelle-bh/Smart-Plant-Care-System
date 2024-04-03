// PLANTTTT

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
    int soil_volume;    // to determine how much water
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

////PLANT DONE

#define HEX3_HEX0_BASE 0xFF200020
#define HEX3_HEX1_BASE 0xFF200030
#define LED_BASE 0xFF200000 // ITS SOMETHING ELSE

#define BTN_BASE 0xFF200050
#define SW_BASE 0x00000000
#define LED_DIR 0xFF200064
#define NUM_SWITCHES 4

extern struct species_profile plant_profiles[];

volatile struct plant_status plant_readings;
volatile struct species_profile current_plant;

volatile int *HEX_ptr = (int *)HEX3_HEX0_BASE; // hex address
volatile int *HEX_ptr2 = (int *)HEX3_HEX1_BASE;
volatile unsigned int *led_base_ptr = (unsigned int *)LED_BASE;
volatile unsigned int *led_dir_ptr = (unsigned int *)LED_DIR;

volatile int max_light = 35; // watts per sqft

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

void displayLight(void)
{
    int light_ratio = (int)ceil_float((current_plant.light / max_light) * 10);
    unsigned int led_intensity[11] = {0x000, 0x001, 0x003, 0x007, 0x00F, 0x01F, 0x03F, 0x07F, 0x0FF, 0x1FF, 0x3FF};
    *led_base_ptr = led_intensity[light_ratio] & 0x3FF;
}

int main(void)
{
    while (1)
    {
        displayLight();
        setCurrentPlant();
    }

    return 0;
}