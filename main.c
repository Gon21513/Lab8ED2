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
#define BUTTON_PIN GPIO_PIN_4 // Pin para el botón SW1

bool ledState = false; // Variable de estado del LED

//-----------------------prototipos--------------------------------------------
void setupTimer0(void); //Función del timer0
void Timer0IntHandler(void); // Manejador de interrupción para Timer0

int main(void)
{
    //---------------------------reloj y puertos-------------------------------
    // Configura el reloj del sistema a 40 MHz.
    // SYSCTL_XTAL_16MHZ  frecuencia del oscilador de 16 MHz.
    // SYSCTL_OSC_MAIN  oscilador principal.
    // SYSCTL_USE_PLL utilizar el PLL.
    // SYSCTL_SYSDIV_5 divide la frecuencia del oscilador por 5 para obtener 40 MHz (200 MHz / 5 = 40 MHz)
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Habilita el reloj para el puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Espera a que el periférico esté listo
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    // Configura los pines de los LEDs como salidas
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED);

    //---------------------------tmr0----------------------------------------------
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler); // Configura el manejador de interrupción para Timer0
    setupTimer0(); //setup del timer0

    while(1)
    {
        // Código principal (si lo hay)
    }
}

void setupTimer0(void){
    IntMasterEnable(); // Habilitar interrupciones
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Habilitar Timer0
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)); // Esperar a que inicialice
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // Configurar como periódico de 32 bits
    TimerLoadSet(TIMER0_BASE, TIMER_BOTH, 80000000 - 1); // Configurar cada 2s
    IntEnable(INT_TIMER0A); // Habilitar interrupción del timer0 bloque A
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // La interrupción será cuando se de un timeout (2s)
    TimerEnable(TIMER0_BASE, TIMER_A); // Iniciar Timer0
}

void Timer0IntHandler(void){
    // Limpia la bandera de interrupción del Timer 0 para evitar que se llame continuamente
    // a esta función manejadora de interrupciones.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Toggling del estado de ledState.
    // Si ledState es true, se cambia a false, y viceversa.
    if (ledState == true) {
        ledState = false;
    } else {
        ledState = true;
    }

    // Cambio del estado del LED rojo en función del valor de ledState.
    // Si ledState es true (1), el LED rojo se enciende.
    // Si ledState es false (0), el LED rojo se apaga.
    if (ledState) {
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, RED_LED);  // Enciende el LED rojo
    } else {
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, 0);  // Apaga el LED rojo
    }
}
