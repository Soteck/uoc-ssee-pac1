//*****************************************************************************
//
// Copyright (C) 2017 Universitat Oberta de Cataluya
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the
//  distribution.
//
//  Neither the name of Texas Instruments Incorporated nor the names of
//  its contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

/*****************************************************************************
 * Description: This program will use the SysTick module to blink an LED with
 * a one second period. Once setup, the application will go into LPM3
 * mode and only wake up to toggle the GPIO pin.
 *******************************************************************************/

// Includes standard
#include <stdio.h>
#include <stdint.h>

// Include DriverLib (MSP432 Peripheral Driver Library)
#include "driverlib.h"

void ToggleBlueTimer(void);
void EnableBlueTimer(void);
void DisableBlueTimer(void);
uint8_t blueCounter;
uint8_t timerStatus;

// Defines
#define TIMER_PERIOD    46874  // Numero de ciclos de clock
#define BLUE_TICKS    10

// Timer_A UpMode Configuration
const Timer_A_UpModeConfig upConfig = {
TIMER_A_CLOCKSOURCE_SMCLK,          // SMCLK source = 3 MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_64,     // SMCLK/64
        TIMER_PERIOD / BLUE_TICKS,                       // 1 seconds
        TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                    // Clear value
        };

int main(void)
{

    blueCounter = BLUE_TICKS + 1;
    timerStatus = 0;

    // Configuracion del pin P1.0 como salida
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    // Configuracion del pin P2.0 como salida
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    // Configuracion del pin P1.1 como entrada
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    // Configuracion del pin P1.4 como entrada
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);

    // Fijar a 0 la salida del pin P1.0
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    // Fijar a 0 la salida del pin P2.2
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    // Configurar el Timer_A1 en Up Mode (incremental)
    MAP_Timer_A_configureUpMode(TIMER_A3_BASE, &upConfig);

    // Habilita el procesador para que duerma al acabar la ISR
    MAP_Interrupt_enableSleepOnIsrExit();

    // Habilita la interrupcion del Timer_A3
    MAP_Interrupt_enableInterrupt(INT_TA3_0);

    // Inicia el timer
    //MAP_Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_UP_MODE);

    // Habilita al procesador para que responda a interrupciones
    MAP_Interrupt_enableMaster();

    // Habilita el temporizador SysTick
    MAP_SysTick_enableModule();
    MAP_SysTick_setPeriod((3000000) / 2);
    MAP_SysTick_enableInterrupt();

    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    MAP_Interrupt_enableInterrupt(INT_PORT1);

    while (1)
    {
        // Conmuta el microcontrolador a modo de bajo consumo LPM0
        MAP_PCM_gotoLPM0();
    }
}

// Rutina de Servicio a Interrupcion (ISR) del SysTick
void SysTick_Handler(void)
{
    // Conmuta el estado de la salida digital P1.0 (LED)
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

// Rutina de Servicio a Interrupcion (ISR) del Timer_A3
void TA3_0_IRQHandler(void)
{
    blueCounter--;
    if (blueCounter == 0)
    {
        // Conmuta el estado de la salida digital P2.2 (LED)
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
        blueCounter = BLUE_TICKS + 1;
    }
    else if (blueCounter == BLUE_TICKS)
    {
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }

    // Resetea el flag de interrupcion del Timer_A3
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE,
    TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

// Rutina de Servicio a Interrupcion (ISR) del PORT1
void PORT1_IRQHandler(void)
{
    uint32_t status;

    // Lee el estado de la interrupcion generada por GPIO_PORT_P1
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    // Reset del flag de interrupcion del pin que la genera
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    // Chequea si la interrupcion la genero el pin P1.1
    if (status & GPIO_PIN1)
    {

        EnableBlueTimer();
    }
    else if (status & GPIO_PIN4)
    {
        DisableBlueTimer();
    }
}

void ToggleBlueTimer(void)
{
    if (timerStatus == 1)
    {
        EnableBlueTimer();
    }
    else
    {
        DisableBlueTimer();
    }
}
void DisableBlueTimer(void)
{
    MAP_Timer_A_stopTimer(TIMER_A3_BASE);
    //Es probable que se haya pulsado cuando estaba el led encendido, asi que aseguramos su apagado
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    blueCounter = BLUE_TICKS + 1;
    timerStatus = 0;
}
void EnableBlueTimer(void)
{
    MAP_Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_UP_MODE);
    timerStatus = 1;
}
