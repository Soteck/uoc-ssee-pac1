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
//*****************************************************************************


// Include DriverLib (MSP432 Peripheral Driver Library)
#include "driverlib.h"

// Defines
#define TIMER_PERIOD    23437  // Numero de ciclos de clock

// Timer_A UpMode Configuration
const Timer_A_UpModeConfig upConfig =
{
  TIMER_A_CLOCKSOURCE_SMCLK,          // SMCLK source = 3 MHz
  TIMER_A_CLOCKSOURCE_DIVIDER_64,     // SMCLK/64
  TIMER_PERIOD,                       // 0.5 seconds
  TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable Timer interrupt
  TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, // Enable CCR0 interrupt
  TIMER_A_DO_CLEAR                    // Clear value
};

int main(void)
{
  // Configuracion del pin P1.0 como salida
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

  // Fijar a 0 la salida del pin P1.0
  MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

  // Configurar el Timer_A1 en Up Mode (incremental)
  MAP_Timer_A_configureUpMode(TIMER_A3_BASE, &upConfig);

  // Habilita el procesador para que duerma al acabar la ISR
  MAP_Interrupt_enableSleepOnIsrExit();

  // Habilita la interrupcion del Timer_A3
  MAP_Interrupt_enableInterrupt(INT_TA3_0);

  // Inicia el timer
  MAP_Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_UP_MODE);

  // Habilita al procesador para que responda a interrupciones
  MAP_Interrupt_enableMaster();

  while (1)
  {
    // Conmuta el microcontrolador a modo de bajo consumo LPM0
    MAP_PCM_gotoLPM0();
  }
}

// Rutina de Servicio a Interrupcion (ISR) del Timer_A3
void TA3_0_IRQHandler(void)
{
  // Conmuta el estado de la salida digital P1.0 (LED)
  MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

  // Resetea el flag de interrupcion del Timer_A3
  MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}
