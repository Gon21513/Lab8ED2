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

volatile uint32_t ui32Loop; //variable para el delay



//delay para los LEDS
void delay(void) {
    for(ui32Loop = 0; ui32Loop < 3000000; ui32Loop++) {} // Simple delay
}


#define TOGGLE_PIN   GPIO_PIN_6 // Definición del pin para toggle
#define RED_LED      GPIO_PIN_1 // Definición del pin para el LED rojo
#define GREEN_LED    GPIO_PIN_3 // Definición del pin para el LED verde
#define BLUE_LED     GPIO_PIN_2 // Definición del pin para el LED azul

// Variables globales para mantener el estado de los LEDs
volatile bool toggle_red = false;
volatile bool toggle_green = false;
volatile bool toggle_blue = false;

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
    // Configura el reloj del sistema a 40 MHz.
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  // Habilita el reloj para el puerto F
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));  // Espera a que el periférico esté listo
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | GREEN_LED | BLUE_LED);  // Configura los LEDs como salida

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  // Habilita el reloj para el puerto A
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));  // Espera a que el periférico esté listo
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, TOGGLE_PIN);  // Configura el pin de toggle como salida

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

    TimerLoadSet(TIMER0_BASE, TIMER_BOTH, SysCtlClockGet() / 2 - 1);  // Establece el valor de carga del Timer0 para una interrupción cada 2 segundos (0.5Hz)
    IntEnable(INT_TIMER0A);  // Habilita la interrupción del Timer0A
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  // Habilita la interrupción por timeout en el Timer0A
    TimerEnable(TIMER0_BASE, TIMER_A);  // Habilita el Timer0A
}

void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  // Limpia la interrupción del Timer0A

    if (toggle_red)
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, GPIOPinRead(GPIO_PORTF_BASE, RED_LED) ^ RED_LED);  // Toggle red LED
    if (toggle_green)
        GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED, GPIOPinRead(GPIO_PORTF_BASE, GREEN_LED) ^ GREEN_LED);  // Toggle green LED
    if (toggle_blue)
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, GPIOPinRead(GPIO_PORTF_BASE, BLUE_LED) ^ BLUE_LED);  // Toggle blue LED


    // Toggling TOGGLE_PIN en el puerto A
    GPIOPinWrite(GPIO_PORTA_BASE, TOGGLE_PIN, GPIOPinRead(GPIO_PORTA_BASE, TOGGLE_PIN) ^ TOGGLE_PIN);  // Toggle TOGGLE_PIN
}

void setupUART0(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);  // Habilita el reloj para el periférico UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  // Habilita el reloj para el puerto GPIOA (utilizado por UART0)
    GPIOPinConfigure(GPIO_PA0_U0RX);  // Configura los pines PA0 y PA1 para ser utilizados como pines de UART
    GPIOPinConfigure(GPIO_PA1_U0TX);  // Configura los pines PA0 y PA1 para ser utilizados como pines de UART
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);  // Configura los pines PA0 y PA1 para ser utilizados como pines de UART
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));  // Configura el UART0 con una tasa de baudios de 115200, 8 bits de datos, 1 bit de parada y sin paridad

    //uart handler


    IntEnable(INT_UART0); //Habilitar interrupciones para el UART0

    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);  // Habilita las interrupciones de recepción y transmisión del UART0

    UARTEnable(UART0_BASE); //Iniciar UART0
}



// Función handler para la interrupción del UART
void UARTIntHandler(void)
{
    char cReceived = 0;  // Variable para guardar el dato, inicializada en 0
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);  // Limpiar bandera de interrupción para recepción y transmisión

    // Solo proceder si hay caracteres disponibles
    if(UARTCharsAvail(UART0_BASE))
    {
        cReceived = UARTCharGetNonBlocking(UART0_BASE);  // Guardar el dato en una variable
        UARTCharPutNonBlocking(UART0_BASE, cReceived);  // Devuelve el dato recibido al transmisor
    }

    if (cReceived == 'r')  // Si se recibió una 'r'
    {

        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED, 0);//apaga

        toggle_red = !toggle_red;

        //GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, GPIOPinRead(GPIO_PORTF_BASE, RED_LED) ^ RED_LED);  // Toggle del led rojo
    }
    else if (cReceived == 'g')
    {
        //GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED, GPIOPinRead(GPIO_PORTF_BASE, GREEN_LED) ^ GREEN_LED);  // Toggle del led verde
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED, 0);//apaga

        toggle_green = !toggle_green;

    }
    else if (cReceived == 'b')
    {
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED, 0);//apaga

        //GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, GPIOPinRead(GPIO_PORTF_BASE, BLUE_LED) ^ BLUE_LED);  // Toggle del led azul
        toggle_blue = !toggle_blue;

    }
}




