/**
  Summary:
  This header file was created using Microchip Digital Compensator Design Tool (DCDT)
  and includes system summary and digital compensator coefficients. This file
  should be included in the MPLAB X project directory.

  File Name:     avgcm_dcdt.h
  Project Name:  avgcm
  Date:          6/17/2020
  Time:          10:22.34

  Software License Agreement

  Copyright © 2020 Microchip Technology Inc.  All rights reserved.
  Microchip licenses to you the right to use, modify, copy and distribute
  Software only when embedded on a Microchip microcontroller or digital
  signal controller, which is integrated into your product or third party
  product (pursuant to the sublicense terms in the accompanying license
  agreement).

  You should refer to the license agreement accompanying this Software
  for additional information regarding your rights and obligations.

  SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
  OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
  PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
  OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
  BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
  DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
  INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
  COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
  CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
  OR OTHER SIMILAR COSTS.
 **/
#ifndef _AVGCM_DCDT_DEFINES_H
#define _AVGCM_DCDT_DEFINES_H

/**
  Compensator Type:  2P2Z
      Entry                Value  
    ---------            ---------
  Pole 0                 8.0000e+03 Hz
  Pole 2                 1.7500e+05 Hz
  Zero 1                 5.0000e+03 Hz
  Gain(Kdc)              1.000
  Warp                   false
  PWM Frequency          3.5000e+05
  PWM Sampling Ratio     1
  Sampling Frequency     3.5000e+05
  PWM Max Resolution     2.5000e-10
  Computational Delay    1.0000e-06
  Gate Drive Delay       5.0000e-08
  Control Output Min.    0
  Control Output Max.    22842


  PWM Calculations
      Name                Value  
    ---------           ---------
  Bits of Resolution    13.480
  Gain                  8.751e-05


  s-domain transfer function

               Wp0   Wp2(Wz1 + s)
  H(s) = Kdc X --- X ------------
                s    Wz1(Wp2 + s)

                  5.03e+04   1.10e+06(3.14e+04 + s)
  H(s) = 1.000 X -------- X ----------------------
                     s       3.14e+04(1.10e+06 + s)



  Digital Compensator Coefficients

  Name    Value     Normalized    Q15      Hex
  ----    -----     ----------    ---      ---
  a1      0.778     0.762         24953    0x6179
  a2      0.222     0.217         7121     0x1BD1
  b0      1.022     1.000         32764    0x7FFC
  b1      0.088     0.086         2814     0x0AFE
  b2      -0.934    -0.914        -29950   0x8B02


  z-domain transfer function

         u(z)  B0 + B1z^(-1) + B2z^(-2)
  H(z) = --- = ------------------------
         e(z)  A0 - A1z^(-1) - A2z^(-2)

          (1.022) + (0.088)z^(-1) + (-0.934)z^(-2)
  H(z) = ---------------------------------------------
          1 - (0.778)z^(-1) - (0.222)z^(-2)

**/


// Compensator Coefficient Defines
#define AVGCM_COMP_2P2Z_COEFF_A1      0x6179
#define AVGCM_COMP_2P2Z_COEFF_A2      0x1BD1
#define AVGCM_COMP_2P2Z_COEFF_B0      0x7FFC
#define AVGCM_COMP_2P2Z_COEFF_B1      0x0AFE
#define AVGCM_COMP_2P2Z_COEFF_B2      0x8B02
#define AVGCM_COMP_2P2Z_POSTSCALER    0x4161
#define AVGCM_COMP_2P2Z_POSTSHIFT     0xFFFF
#define AVGCM_COMP_2P2Z_PRESHIFT      0x0000


// Compensator Clamp Limits
#define AVGCM_COMP_2P2Z_MIN_CLAMP    0x0000
#define AVGCM_COMP_2P2Z_MAX_CLAMP    0x593A


#endif
