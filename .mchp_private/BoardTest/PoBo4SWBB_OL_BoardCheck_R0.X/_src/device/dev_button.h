//======================================================================================================================
// Copyright(c) 2018 Microchip Technology Inc. and its subsidiaries.
// Subject to your compliance with these terms, you may use Microchip software and any derivatives exclusively with
// Microchip products. It is your responsibility to comply with third party license terms applicable to your use of
// third-party software (including open source software) that may accompany Microchip software.
// THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO
// THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
// PURPOSE.
// IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE,
// COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED
// OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY
// ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE
// PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
//======================================================================================================================

//======================================================================================================================
// @file dev_button.h
//
// @brief contains the driver for the button
//
// @note this driver is not suited for multiple buttons but only for one button
//
//======================================================================================================================

#ifndef _DEV_BUTTON_H_
#define	_DEV_BUTTON_H_

#include <stdint.h>
#include <xc.h>

#define DEV_BUTTON_EVENT_NONE           0
#define DEV_BUTTON_EVENT_PRESSED_SHORT  1
#define DEV_BUTTON_EVENT_PRESSED_LONG   2

//======================================================================================================================
// @brief initializes the button device, needs to be called once at boot up before that device can be used
//======================================================================================================================
extern void Dev_Button_Init(void);

//======================================================================================================================
// @brief this function returns the event of the button like specified in the Dev_BUTTON_EVENT_* events above
// @returns event of the button
//======================================================================================================================
extern uint8_t Dev_Button_GetEvent(void);

//======================================================================================================================
// @brief this function evaluates the button status/events and needs to be called approximately every 10 ms
//======================================================================================================================
extern void Dev_Button_Task_10ms(void);


#endif	// _DEV_BUTTON_H_
