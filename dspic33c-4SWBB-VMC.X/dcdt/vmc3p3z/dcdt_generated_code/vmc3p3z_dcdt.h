/**
  Summary:
  This header file was created using Microchip Digital Compensator Design Tool (DCDT)
  and includes system summary and digital compensator coefficients. This file
  should be included in the MPLAB X project directory.

  File Name:     vmc3p3z_dcdt.h
  Project Name:  vmc3p3z
  Date:          5/5/2021
  Time:          15:53.18

  Software License Agreement

  Copyright © 2021 Microchip Technology Inc.  All rights reserved.
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
#ifndef _VMC3P3Z_DCDT_DEFINES_H
#define _VMC3P3Z_DCDT_DEFINES_H

/**
  Compensator Type:  3P3Z
      Entry                Value  
    ---------            ---------
  Pole 0                 4.0000e+03 Hz
  Pole 2                 3.5000e+04 Hz
  Pole 3                 1.7500e+05 Hz
  Zero 1                 8.0000e+02 Hz
  Zero 2                 4.5000e+03 Hz
  Gain(Kdc)              1.000
  Warp                   false
  PWM Frequency          3.5000e+05
  PWM Sampling Ratio     2
  Sampling Frequency     1.7500e+05
  PWM Max Resolution     2.5000e-10
  Computational Delay    1.5000e-06
  Gate Drive Delay       1.5000e-07
  Control Output Min.    0
  Control Output Max.    21000


  PWM Calculations
      Name                Value  
    ---------           ---------
  Bits of Resolution    14.480
  Gain                  4.375e-05


  s-domain transfer function

               Wp0   Wp2(Wp3)(Wz1 + s)(Wz2 + s)
  H(s) = Kdc X --- X --------------------------
                s    Wz1(Wz2)(Wp2 + s)(Wp3 + s)

                 2.51e+04   2.20e+05(1.10e+06)(5.03e+03 + s)(2.83e+04 + s)
  H(s) = 1.000 X -------- X ----------------------------------------------
                    s       5.03e+03(2.83e+04)(2.20e+05 + s)(1.10e+06 + s)



  Digital Compensator Coefficients

  Name    Value      Normalized    Q15      Hex
  ----    -----      ----------    ---      ---
  a1      0.711      0.036         1173     0x0495
  a2      0.407      0.020         671      0x029F
  a3      -0.118     -0.006        -194     0xFF3E
  b0      19.861     1.000         32764    0x7FFC
  b1      -16.330    -0.822        -26938   0x96C6
  b2      -19.777    -0.996        -32626   0x808E
  b3      16.414     0.826         27077    0x69C5


  z-domain transfer function

         u(z)  B0 + B1z^(-1) + B2z^(-2) + B3z^(-3)
  H(z) = --- = -----------------------------------
         e(z)  A0 - A1z^(-1) - A2z^(-2) - A3z^(-3)

          (19.861) + (-16.330)z^(-1) + (-19.777)z^(-2) + (16.414)z^(-3)
  H(z) = -----------------------------------------------------------
          1 - (0.711)z^(-1) - (0.407)z^(-2) - (-0.118)z^(-3)

**/


// Compensator Coefficient Defines
#define VMC3P3Z_COMP_3P3Z_COEFF_A1      0x0495
#define VMC3P3Z_COMP_3P3Z_COEFF_A2      0x029F
#define VMC3P3Z_COMP_3P3Z_COEFF_A3      0xFF3E
#define VMC3P3Z_COMP_3P3Z_COEFF_B0      0x7FFC
#define VMC3P3Z_COMP_3P3Z_COEFF_B1      0x96C6
#define VMC3P3Z_COMP_3P3Z_COEFF_B2      0x808E
#define VMC3P3Z_COMP_3P3Z_COEFF_B3      0x69C5
#define VMC3P3Z_COMP_3P3Z_POSTSCALER    0x4F73
#define VMC3P3Z_COMP_3P3Z_POSTSHIFT     0xFFFB
#define VMC3P3Z_COMP_3P3Z_PRESHIFT      0x0000


// Compensator Clamp Limits
#define VMC3P3Z_COMP_3P3Z_MIN_CLAMP    0x0000
#define VMC3P3Z_COMP_3P3Z_MAX_CLAMP    0x5208


#endif
