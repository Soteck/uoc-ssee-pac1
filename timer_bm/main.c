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

// Includes standard
#include <stdio.h>
#include <stdint.h>

// Include DriverLib (MSP432 Peripheral Driver Library)
#include "driverlib.h"

int main(void)
{
  // Por defecto el procesador MSP432 funciona a MCLK=3 MHz utilizando el DCO

  // Configuracion del LED rojo (P1.0) como salida y nivel bajo (apagado)
  P1DIR |= BIT0;
  P1OUT &= ~BIT0;

  // Configuracion del LED rogo (P2.0) como salid y nivel bajo (apagado)
  P2DIR |= BIT0;
  P2OUT &= ~BIT0;

  // Run TimerA0 in up mode from ACLK 
  // El timer funciona a 32 kHz y tiene una resolucion de 16 bits, 
  // por lo que el overflow pasara cada 65535 / 32000 ~ 2 segundos
  TA0CTL |= TASSEL__ACLK | MC__CONTINUOUS;
 
  // Enciende el LED rojo (P2.0)
  P2OUT |= BIT0;

  // Bucle infinito
  while (1)
  {
    // Comprueba si la interrupcion CCIFG del TimerA0 se ha activado
    if ((TA0CCTL0 & CCIFG) == CCIFG)
    {
      // Limpia la interrupcion CCIFG del TimarA0
      TA0CCTL0 &= ~CCIFG;
      
      // Cambia el estado de los LEDs rojos (P1.0 y P2.0)
      P1OUT ^= BIT0;
      P2OUT ^= BIT0;
    }
  }
}
