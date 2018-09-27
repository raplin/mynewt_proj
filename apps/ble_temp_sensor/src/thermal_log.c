#include <os/mynewt.h>
#include <nrf_temp.h>
#include <bsp/bsp.h>
#include <hal/hal_gpio.h>

#include "thermal_log.h"

/* Define task stack and task object */
#define THERMAL_LOG_TASK_PRIORITY         (100) //127=main loop, lower=higher
#define MY_STACK_SIZE       (256) //64)

/* Globals.. I really don't like their names not indicating they're global in scope.
   I'm going with the 'house style' here but it's asking for trouble IMO. Why _wouldn't_ you?
   My personal preference, fwiw, is camelcase with e.g. "gThisIsGlobal"  and "thisIsLocal", but consistency > my preferences ;-)
*/
struct os_task thermal_log_task;
os_stack_t thermal_log_taskstack[MY_STACK_SIZE];

/* Ring buffer is one longer than the reported history length which allows us to avoid contention */
/* ..just using a system primative like a mutex or queue is a perfectly good idea as well of course */ 
volatile int16_t thermal_log_ringbuf[THERMAL_LOG_LENGTH+1]; 
volatile uint8_t thermal_log_writeptr;
volatile uint8_t thermal_log_fill_level;
#if (THERMAL_LOG_LENGTH+1)>=256
#error Please change "thermal_log_writeptr" and friends to uint16 format; this will require 16-bit writes to be atomic on your architecture.  
#endif


/* Returns the internal temperature of the nRF52 in degC (2 decimal places, scaled) */
int16_t
thermal_log_read_temp(void)
{
    int16_t temp;
    /* Start the temperature measurement. */
    NRF_TEMP->TASKS_START = 1;
    while(NRF_TEMP->EVENTS_DATARDY != TEMP_INTENSET_DATARDY_Set) {};
    /* Temp reading is in units of 0.25degC, so divide by 4 to get in units of degC
     * (scale by 100 to avoid representing as decimal). */
    temp = ((nrf_temp_read() * 100)+2) >> 2;  /* /4 and round it */

    return temp;
}



/* Task that just loops reading the temp into circular buffer */
void
thermal_log_taskloop(void *arg) {
    /* Set the led pin as an output */
    hal_gpio_init_out(LED_BLINK_PIN, 1);

    /* Sample temp every (~100ms) into circular buffer */
    while (1) {
        /* Wait fixed interval; - note if the temperature ADC sampling takes any appreciable amount of time this will impact the sampling rate slightly; assume nobody cares */
        os_time_delay(OS_TICKS_PER_SEC/THERMAL_LOG_SAMPLES_PER_SECOND);

        thermal_log_ringbuf[thermal_log_writeptr]=thermal_log_read_temp();
#if defined(__ARMCC_VERSION)
        __DMB(); /* ensure compiler doesn't reorder the writes to ringbuf/writeptr; it may do this anyway as they're volatile */
#endif        
        if (thermal_log_writeptr==THERMAL_LOG_LENGTH){ /*this is not an off-by-one; we want the log array to be one longer than the defined _LOG_LENGTH to avoid it "eating its own tail" (corrupting the oldest entry) when read & written without locking */
            thermal_log_writeptr=0;
            /* Toggle the LED every 10 samples  */
            hal_gpio_toggle(LED_BLINK_PIN);
            
        }else{
            thermal_log_writeptr++;
        }

        if (thermal_log_fill_level<THERMAL_LOG_LENGTH){
            thermal_log_fill_level++;
        }
    
    }
}

/*  Fetch a set of previous samples in reverse chronological order.
    May return less than requested if the buffer is still filling (typically the first second after booting)
    which will be reflected in the returned count
    */
uint8_t
thermal_log_read_history(uint8_t count,int16_t *dest)
{
    uint8_t src_offset=thermal_log_writeptr; 
    uint8_t t;
    assert(count<=THERMAL_LOG_LENGTH);
    if (count>thermal_log_fill_level){
        count=thermal_log_fill_level;
    }
    t=count;
    while(t--){ /* step backwards through the history buffer copying out samples, wrapping from start to end as required */
        if (src_offset==0){
            src_offset=THERMAL_LOG_LENGTH;
        }else{
            src_offset--;
        }
        *dest++=thermal_log_ringbuf[src_offset];
    }
    return count;
}



/* init, call once only */
void
thermal_log_init(void)
{
    /* Prepare the internal temperature module for measurement */
    nrf_temp_init();

    thermal_log_writeptr=0;
    thermal_log_fill_level=0;
    
     /* Initialize the task */
    os_task_init(&thermal_log_task, "thermal_log_task", thermal_log_taskloop, NULL, THERMAL_LOG_TASK_PRIORITY,
                 OS_WAIT_FOREVER, thermal_log_taskstack, MY_STACK_SIZE);
                 
}

