//Luis Pedro Gonzalez 21513
//Lab8

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
#include "driverlib/pin_map.h"  // Necesario para la configuración de pines del UART
#include "driverlib/uart.h"     // Librería del driver UART

#define RED_LED   GPIO_PIN_1 // Definición del pin del LED rojo

// Prototipos de funciones
void setupTimer0(void);
void Timer0IntHandler(void);
void setupUART0(void);
void UARTIntHandler(void);


int main(void)
{

    // Configura el reloj del sistema a 40 MHz.
     // SYSCTL_XTAL_16MHZ  frecuencia del oscilador de 16 MHz.
     // SYSCTL_OSC_MAIN  oscilador principal.
     // SYSCTL_USE_PLL utilizar el PLL.
     // SYSCTL_SYSDIV_5 divide la frecuencia del oscilador por 5 para obtener 40 MHz (200 MHz / 5 = 40 MHz)
     // Configura el reloj del sistema a 40 MHz.
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);  // Configura el reloj del sistema a 40 MHz
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  // Habilita el reloj para el puerto F
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));  // Espera a que el periférico esté listo
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED);  // Configura el pin del LED rojo como salida
    setupTimer0();  // Configura el Timer0
    setupUART0();  // Configura el UART0
    IntMasterEnable();  // Habilita las interrupciones globales

    // Bucle infinito
    while(1)
    {
        //
    }
}

void setupTimer0(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);  // Habilita el reloj para el Timer0
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));  // Espera a que el periférico del Timer0 esté listo
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);  // Configura el Timer0 como periódico de 32 bits

    // TimerLoadSet(TIMER0_BASE, TIMER_BOTH, 40000000 * 2 - 1);

    TimerLoadSet(TIMER0_BASE, TIMER_BOTH, SysCtlClockGet() * 2 - 1);  // Establece el valor de carga del Timer0 para una interrupción cada 2 segundos (0.5Hz)
    IntEnable(INT_TIMER0A);  // Habilita la interrupción del Timer0A
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  // Habilita la interrupción por timeout en el Timer0A
    TimerEnable(TIMER0_BASE, TIMER_A);  // Habilita el Timer0A
}

void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  // Limpia la interrupción del Timer0A para evitar que se vuelva a activar de inmediato
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, GPIOPinRead(GPIO_PORTF_BASE, RED_LED) ^ RED_LED);  // Cambia el estado del LED rojo
}

void setupUART0(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);  // Habilita el reloj para el periférico UART0
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));  // Espera a que el periférico UART0 esté listo
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  // Habilita el reloj para el puerto GPIOA (utilizado por UART0)
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));  // Espera a que el periférico GPIOA esté listo
    GPIOPinConfigure(GPIO_PA0_U0RX);  // Configura los pines PA0 y PA1 para ser utilizados como pines de UART
    GPIOPinConfigure(GPIO_PA1_U0TX);  // Configura los pines PA0 y PA1 para ser utilizados como pines de UART
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);  // Configura los pines PA0 y PA1 para ser utilizados como pines de UART
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));  // Configura el UART0 con una tasa de baudios de 115200, 8 bits de datos, 1 bit de parada y sin paridad


    //uart handler
    IntEnable(INT_UART0);  // Habilita la interrupción del UART0
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_TX);  // Habilita las interrupciones de recepción y transmisión del UART0

}


// Función handler para la interrupción del UART
void UARTIntHandler(void)
{
    uint32_t status; //para almacenar el estao del uart

    status = UARTIntStatus(UART0_BASE, true);  // Actualizado a status
    UARTIntClear(UART0_BASE, status);  // Actualizado a status

}



