
#ifndef PLANT_PROFILE_H
#define PLANT_PROFILE_H

struct species_profile
{
    char name[50];       // name of plant species/type
    float soil_moisture; // in %
    float humidity;      // in %
    int light;           // watts per sqft
    int light_duration;  // hours of exposure needed per day
};

struct plant_environment
{
    float sqft;         // Used for calculating light intensity (watts/sqft)
    int light_exposure; // 1: low, 2: medium, 3: high/direct (for simplicity)
    float soil_volume;  // to determine how much water to give
    float humidity;     // default air humidity
};

// to hold  the plant sensor readings in main
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