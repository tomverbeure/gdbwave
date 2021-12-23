#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "riscv.h"
#include "reg.h"
#include "top_defines.h"
#include "lib.h"
#include "semihosting.h"
#include "printf.h"

void wait_led_cycle(int ms)
{
    if (REG_RD_FIELD(STATUS, SIMULATION) == 1){
        // Wait for a much shorter time when simulation...
        wait_cycles(100);
    }
    else{
        wait_ms(ms);
    }
}

int global_cntr = 0;

int main() 
{
    REG_WR(LED_CONFIG, 0x07);
    wait_led_cycle(1000);
    REG_WR(LED_CONFIG, 0x00);

    printf("Hello World!\n");

    while(1){
        int wait_time = REG_RD_FIELD(STATUS, BUTTON) ? 200 : 100;
        sh_writec('.');
        REG_WR(LED_CONFIG, 0x01);
        wait_led_cycle(wait_time);

        REG_WR(LED_CONFIG, 0x02);
        wait_led_cycle(wait_time);

        REG_WR(LED_CONFIG, 0x04);
        wait_led_cycle(wait_time);

        int c = getchar();
        printf("char: %d", c);

        global_cntr++;
    }
}
