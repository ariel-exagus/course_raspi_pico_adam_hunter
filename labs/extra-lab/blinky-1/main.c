/**
 * /brief blinky Led with interrupt timer
 */


#include <stdio.h>          //printf
#include "pico/stdlib.h"    //pico libraries
#include "hardware/timer.h"
#include "hardware/irq.h"

// Low-level alarm infrastructure we'll be using
#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0

#define DELAY 500000 // 1/Fs (in microseconds)

#define LED_GPIO    25

// Alarm ISR
static void alarm_irq(void) {

    // Toggle on LED
    gpio_put(LED_GPIO, !gpio_get(LED_GPIO)) ;

    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);

    // Reset the alarm register
    timer_hw->alarm[ALARM_NUM] = timer_hw->timerawl + DELAY ;

}

int main()
{
    // Initialize Pin
    gpio_init(LED_GPIO);
    gpio_set_dir(LED_GPIO, GPIO_OUT);

    //Put your setup here
    //setup();

    // Initialize chosen serial port
    stdio_init_all();           //We need to initialized the serial port  
    
    // Enable the interrupt for the alarm (we're using Alarm 0)
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM) ;
    // Associate an interrupt handler with the ALARM_IRQ
    irq_set_exclusive_handler(ALARM_IRQ, alarm_irq) ;
    // Enable the alarm interrupt
    irq_set_enabled(ALARM_IRQ, true) ;
    // Write the lower 32 bits of the target time to the alarm register, arming it.
    timer_hw->alarm[ALARM_NUM] = timer_hw->timerawl + DELAY ;

    // loop forever to blink led
    while(1)
    {
        // Example of Blink LED
        printf("Blink Led with interrupts\r\n");
        sleep_ms(5000);
        // Put your code Here
    }
}