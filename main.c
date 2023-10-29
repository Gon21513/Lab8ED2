//Luis Pedro Gonzalez 21513

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#define XTAL 16000000 //reloj externo de 16MHz

// Definiciones para los LEDs
#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3
#define BUTTON_PIN GPIO_PIN_4 // Pin para el bot�n SW1

//-----------------------prototipos--------------------------------------------
void setupTimer0(void); //Funci�n del timer0
void Timer0IntHandler(void); // Manejador de interrupci�n para Timer0

int main(void)
{
    //---------------------------reloj y puertos-------------------------------
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED);

    //---------------------------tmr0----------------------------------------------
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler); // Configura el manejador de interrupci�n para Timer0
    setupTimer0(); //setup del timer0

    while(1)
    {
        // ...
    }
}

void setupTimer0(void){
    IntMasterEnable(); // Habilitar interrupciones
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Habilitar Timer0
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)); // Esperar a que inicialice
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // Configurar como peri�dico de 32 bits
    TimerLoadSet(TIMER0_BASE, TIMER_BOTH, 20000000 - 1); // Configurar cada 0.5s
    IntEnable(INT_TIMER0A); // Habilitar interrupci�n del timer0 bloque A
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // La interrupci�n ser� cuando se de un timeout (0.5s)
    TimerEnable(TIMER0_BASE, TIMER_A); // Iniciar Timer0
}

void Timer0IntHandler(void)
{
    // Limpia la interrupci�n del Timer 0
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Cambia el estado del LED rojo
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, ~GPIOPinRead(GPIO_PORTF_BASE, RED_LED));
}

