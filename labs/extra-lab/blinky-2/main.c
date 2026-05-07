/**
 * /brief blinky Led with protothreads
 */

#include <stdio.h>          //printf
#include "pico/stdlib.h"    //pico libraries
#include "pico/lock_core.h"
// Include protothreads
#include "pt_cornell_rp2040_v1_3.h"

#define LED_GPIO    25

// This thread runs on core 0
static PT_THREAD (pthread_led_core_0(struct pt *pt))
{
    // Indicate thread beginning
    PT_BEGIN(pt) ;
    while(1) {
        
        // Toggle on LED
        gpio_put(LED_GPIO, !gpio_get(LED_GPIO)) ;

        // Yield for 500 ms
        PT_YIELD_usec(500000) ;
    }
    // Indicate thread end
    PT_END(pt) ;
}

// This thread runs on core 0
static PT_THREAD (pthread_print_core_0(struct pt *pt))
{
    static int counter = 0;

    // Indicate thread beginning
    PT_BEGIN(pt) ;
    while(1) {
        
        printf("Blinky Led with Protothreads!\r\n");
        printf("Counter= %i\r\n", counter++);

        // Yield for 5000 ms
        PT_YIELD_usec(5000000) ;
    }
    // Indicate thread end
    PT_END(pt) ;
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

    // loop forever to blink led
    //while(1)
    // {
    //     // Example of Blink LED
    //     printf("Blinky Led with Protothreads!\r\n");
    //     sleep_ms(5000);

    //     // Put your code Here
    // }

    // Add core 0 threads
    pt_add_thread(pthread_led_core_0) ;

    // Add print core 0 threads
    pt_add_thread(pthread_print_core_0) ;

    // Start scheduling core 0 threads
    pt_schedule_start ;
}