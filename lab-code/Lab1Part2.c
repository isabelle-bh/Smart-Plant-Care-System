#define LED_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define SW_BASE 0xFF200040

volatile int DELAY_LENGTH;                     // volatile so it survives compilation
volatile int *LED_ptr = (int *)LED_BASE;       // LED address
volatile int *SW_ptr = (int *)SW_BASE;         // switch address
volatile int *HEX_ptr = (int *)HEX3_HEX0_BASE; // hex address
volatile int delay_count;                      // volatile so the C compiler doesn't remove the loop
volatile int status;

void DisplayHex(int value)
{
    char hex_code[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0xFD, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    *(HEX_ptr) = hex_code[value & 0xF];
}

int ReadSwitches(void)
{
    printf("Switches...\n");
    printf("%d\n", *(SW_ptr));

    volatile unsigned int *switchBankPtr = (unsigned int *)SW_BASE;

    // Read the state of the slide switch bank
    int switchBankState = *switchBankPtr;

    return switchBankState;
}

int main(void)
{

    *(HEX_ptr) = 0x0;

    DELAY_LENGTH = 700000;
    status = 0;

    while (1)
    {

        if (status == 0)
        {
            status = 1;
            DisplayHex(ReadSwitches());
        }
        else
        {
            status = 0;
            *(HEX_ptr) = 0x00;
        }

        for (delay_count = DELAY_LENGTH; delay_count != 0; --delay_count)
            ;
    }
}