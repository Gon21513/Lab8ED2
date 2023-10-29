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

#define RED_LED   GPIO_PIN_1 // Definici�n del pin del LED rojo

// Prototipo de la funci�n handler del Timer0
void Timer0IntHandler(void);

// Prototipo de la funci�n setupTimer0
void setupTimer0(void);

int main(void)
{
    // Configura el reloj del sistema a 40 MHz.
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Habilita el reloj para el puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Espera a que el perif�rico est� listo
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    // Configura el pin del LED rojo como salida
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED);

    // Configura el Timer0
    setupTimer0();

    // Habilita las interrupciones globales
    IntMasterEnable();

    // Bucle infinito
    while(1)
    {
        // El c�digo dentro de esta secci�n se ejecuta continuamente en el bucle principal
        // ...
    }
}

// Funci�n para configurar el Timer0
void setupTimer0(void){
    // Habilita el reloj para el Timer0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Espera a que el perif�rico del Timer0 est� listo
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    // Configura el Timer0 como peri�dico de 32 bits
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // Establece el valor de carga del Timer0 para una interrupci�n cada 2 segundos (0.5Hz)
    TimerLoadSet(TIMER0_BASE, TIMER_BOTH, SysCtlClockGet() * 2 - 1);

    // Habilita la interrupci�n del Timer0A
    IntEnable(INT_TIMER0A);

    // Habilita la interrupci�n por timeout en el Timer0A
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Habilita el Timer0A
    TimerEnable(TIMER0_BASE, TIMER_A);
}

// Funci�n handler para la interrupci�n del Timer0
void Timer0IntHandler(void)
{
    // Limpia la interrupci�n del Timer0A para evitar que se vuelva a activar de inmediato
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Cambia el estado del LED rojo
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, GPIOPinRead(GPIO_PORTF_BASE, RED_LED) ^ RED_LED);
}


