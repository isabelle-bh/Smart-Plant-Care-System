#include "plant_profile.h"
#include "adc_functions.h"
#include <stdio.h>

int main()
{
    struct plant_status p1 = {2.0, 3.0};
    printf("%f", p1.humidity);
    // int moisture = get_moisture(1);
    return 0;
}