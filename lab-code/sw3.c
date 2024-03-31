#define LED_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX3_HEX1_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define BTN_BASE 0xFF200050
#define TIMER_BASE 0xFFFEC600

// volatile so it survives compilation
volatile int *LED_ptr = (int *)LED_BASE;       // LED address
volatile int *SW_ptr = (int *)SW_BASE;         // switch address
volatile int *HEX_ptr = (int *)HEX3_HEX0_BASE; // hex address
volatile int *HEX_ptr2 = (int *)HEX3_HEX1_BASE;
volatile int delay_count; // volatile so the C compiler doesn't remove the loop
volatile int paused = 0;
volatile int hs = 0;
volatile int currLap[3] = {0, 0, 0};
volatile int viewingLap = 0;
volatile int *load_register = (int *)(TIMER_BASE);
volatile int *timer_status = (int *)(0xFFFEC60C);
volatile int *control_register = (int *)(0xFFFEC608);

void ShowHex(int minutes, int seconds, int hundredths)
{
    int hex_code[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0xFD, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

    // display hundreths of seconds and seconds
    int first_4_digits = hex_code[hundredths % 10] + (hex_code[(hundredths / 10) % 10] << 8) + (hex_code[seconds % 10] << 16) + (hex_code[(seconds / 10) % 10] << 24);
    int last_2_digits = hex_code[minutes % 10] + (hex_code[(minutes / 10) % 10] << 8);

    *(HEX_ptr) = first_4_digits;
    *(HEX_ptr2) = last_2_digits;
}

int ReadSwitches(void)
{
    volatile unsigned int *switchBankPtr = (unsigned int *)SW_BASE;

    // Read the state of the slide switch bank
    int switchBankState = *switchBankPtr;
    return switchBankState;
}

int GetInputs(int mins, int secs, int hss)
{
    volatile unsigned int *btnPtr = (unsigned int *)BTN_BASE;
    int btnState = *btnPtr;
    volatile unsigned int *swPtr = (unsigned int *)SW_BASE;
    int swState = *swPtr;

    if (swState % 2 != 0)
    {
        viewingLap = 1;
    }
    else
    {
        viewingLap = 0;
    }

    if (btnState == 1)
    {
        paused = 0;
    }
    else if (btnState == 2)
    {
        paused = 1;
    }
    else if (btnState == 4)
    {
        currLap[0] = mins;
        currLap[1] = secs;
        currLap[2] = hss;
    }
    else if (btnState == 8)
    {
        currLap[0] = 0;
        currLap[1] = 0;
        currLap[2] = 0;
        hs = 0;
        ShowHex(0, 0, 0);
    }
}

void wait_for_timer() // loops until timer is finished counting
{
    while (*timer_status == 0)
        ;
    *timer_status = 1;
    hs++;

    // make sure that check_timer() returns 0 if the timer isn't running
    // or make sure that wait_for_timer() is never called unless timer is started
    // otherwise endless loop!
}

int main(void)
{

    *(HEX_ptr) = 0x0;
    *(HEX_ptr2) = 0x0;
    int minutes = 0;
    int seconds = 0;
    int hundredths = 0;
    *load_register = 2000000;
    *control_register = 0b0011;

    while (1)
    {
        if (paused == 0)
        {

            wait_for_timer();

            if (hs >= 360000)
            {
                hs = 0;
            }

            minutes = ((hs / 100) / 60) % 60;
            seconds = (hs / 100) % 60;
            hundredths = hs;

            if (viewingLap == 1)
            {
                ShowHex(currLap[0], currLap[1], currLap[2]);
            }
            else
            {
                ShowHex(minutes, seconds, hundredths);
            }
        }

        GetInputs(minutes, seconds, hundredths);
    }
}
