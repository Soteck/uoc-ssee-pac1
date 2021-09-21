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
  // Por defecto el procesador MSP432 funciona
  // a MCLK=3 MHz utilizando el DCO


  P1DIR |= BIT0;
  P1OUT &= ~BIT0;

  P1DIR &= ~BIT1;
  P1REN |= BIT1;
  P1OUT |= BIT1;
  
  P1IES |= BIT1;
  
  P1IE |= BIT1;
  
  NVIC_EnableIRQ(PORT1_IRQn);
    
  // Bucle infinito
  while (1)
  {
    // El procesador no hace nada en este bucle, esta
    // esperando a que el usuario presione el boton S1 (P1.1)
  }
}

// Rutina de Servicio a Interrupcion (ISR) del PORT1
void PORT1_IRQHandler(void)
{
  // Cuando el usuario presiona el boton S1 (P1.1) se genera una interrupcion y
  // el procesador deja de ejectuar el bucle principal (main) y pasa a ejecutar
  // la rutina correspondiente a la interrupcion asociada al evento (PORT1_IRQHandler)
  
  // Chequea si la interrupcion la genero el boton S1 (P1.1)
  if ((P1IFG & BIT1) == BIT1)
  {
    // Limpia el bit de la interrupcion
    P1IFG &= ~BIT1;
    
    // Cambia el estado del LED rojo (P1.0)
    P1OUT ^= BIT0;
  }
}
