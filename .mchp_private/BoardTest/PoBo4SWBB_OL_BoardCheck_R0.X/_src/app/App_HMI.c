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
// @file App_HMI.c
//
// @brief contains the HMI application
//
// @note this application handles the inputs from the buttons and controls the 4SWBB and LEDs
//
//======================================================================================================================

#include <xc.h>
#include <stdbool.h>
#include "device/dev_button.h"
#include "../driver/power_controller/drv_pwrctrl_4SWBB.h"
#include "../driver/power_controller/drv_pwrctrl_4SWBB_fault.h"
#include "../mcc_generated_files/pin_manager.h"
#include"../driver/drv_adc.h"

#define HMT_STATE_TEST_INIT 0
#define HMT_STATE_TEST_WAIT_FOR_STARTBUTTON 1
#define HMT_STATE_TEST_WAIT_FOR_POWERUP 2
#define HMT_STATE_TEST_TESTING 3
#define HMT_STATE_TEST_TEST_SUCCESS 4
#define HMT_STATE_TEST_TEST_FAILED 5

#define LED_OFF 0
#define LED_ON 1
#define LED_BLINK 2
#define LED_BLINK_FAST 3

#define Num_Of_LEDS 3
#define LED_RED 0
#define LED_GREEN 1
#define LED_PIM_RED 2

uint8_t ledstates[Num_Of_LEDS];
uint8_t ledtimer[Num_Of_LEDS];

uint8_t hmi_state = 0;
uint8_t hmi_testfault = 0;

//local functions
void CheckButtons(void);
void SetLEDs_100ms(void);
void SetLED(uint8_t led_index, uint8_t led_state);
extern uint16_t os_resetCause;  //@sw??

//======================================================================================================================
// @brief initializes the HMI application
//======================================================================================================================

void App_HMI_Init(void)
{
  
}

//======================================================================================================================
// @brief this function contains 
//======================================================================================================================

void CheckButtons(void)
{
  uint8_t buttonresponse = Dev_Button_GetEvent();

  if (buttonresponse == DEV_BUTTON_EVENT_PRESSED_SHORT)
  {
    if (pwr_ctrl_flagbits.run)
    {
      Drv_PwrCtrl_4SWBB_Stop();
      LED_Green_SetLow();
    } else
    {
      Drv_PwrCtrl_4SWBB_Start();
      LED_Green_SetHigh();
    }
  } 
  
  //@ftx disbale long pressed not to enter closed loop mode
//  else if (buttonresponse == DEV_BUTTON_EVENT_PRESSED_LONG)
//  {
//    if (pwr_ctrl_flagbits.inopenloop)
//    {  
//      Drv_PwrCtrl_4SWBB_SetMode_ClosedLoop();
//    }  
//    else
//    {  
//      Drv_PwrCtrl_4SWBB_SetMode_OpenLoop();
//    }
//  }
}

//-----------------------------------------------------------------------------
// Limits:
//          Min     Nom     Max
// Vin [V]	11.5	12      12.5
// Vout [V]	14.5	15      16
// Vaux [V]	5.2     5.4     5.6
// Iout [A]	1.45	1.5     1.55
// Iin [A]	2       2.1     2.2
//
// ==> all Values +/- 5%  ==> test values
//          Min     Max
// Vin [V]	11.305	13.125  ==> val/0,00645         1753 ... 2035
// Vout [V]	13.775	16,8    ==> val/0,00645         2136 ... 2605
// Vaux [V]	4,94    5,88    ==> val/0,0016          3088 ... 3675
// Iout [A]	1.3775	1.643   ==> (val+4,025)/0,002   2701 ... 2834
// Iin [A]	1,9     2.31    ==> (val+0,874)/0,0017  1632 ... 1873
// 
    
bool Are_Test_values_ok(void)
{
    //Check Vin
    //if ((pwr_ctrl_adc_data.drv_adc_val_FB_Vdd < 1790) >> (pwr_ctrl_adc_data.drv_adc_val_FB_Vdd > 1940))
    if ((pwr_ctrl_adc_data.drv_adc_val_FB_Vdd < 1753) >> (pwr_ctrl_adc_data.drv_adc_val_FB_Vdd > 2035))
    {
        hmi_testfault = 1;
        return false;
    }
    //Check Vout
    //if ((pwr_ctrl_adc_data.drv_adc_val_FB_Vout < 2240) || (pwr_ctrl_adc_data.drv_adc_val_FB_Vout > 2400))
    if ((pwr_ctrl_adc_data.drv_adc_val_FB_Vout < 2136) || (pwr_ctrl_adc_data.drv_adc_val_FB_Vout > 2605))
    {
        hmi_testfault = 2;
        return false;
    }
    //Check Vaux
    //if ((pwr_ctrl_adc_data.drv_adc_val_FB_Vaux < 3100) || (pwr_ctrl_adc_data.drv_adc_val_FB_Vaux > 3720))
    if ((pwr_ctrl_adc_data.drv_adc_val_FB_Vaux < 3088) || (pwr_ctrl_adc_data.drv_adc_val_FB_Vaux > 3675))
    {
        hmi_testfault = 3;
        return false;
    }
    //Check Iout
    //if ((pwr_ctrl_adc_data.drv_adc_val_FB_Iout < 2700) || (pwr_ctrl_adc_data.drv_adc_val_FB_Iout > 2800))
    if ((pwr_ctrl_adc_data.drv_adc_val_FB_Iout < 2701) || (pwr_ctrl_adc_data.drv_adc_val_FB_Iout > 2834))
    {
        hmi_testfault = 4;
        return false;
    }
    //Check Iin
//    if ((pwr_ctrl_adc_data.drv_adc_val_FB_Iin < 1650) || (pwr_ctrl_adc_data.drv_adc_val_FB_Iin > 1750))
    if ((pwr_ctrl_adc_data.drv_adc_val_FB_Iin < 1632) || (pwr_ctrl_adc_data.drv_adc_val_FB_Iin > 1873))
    {
        hmi_testfault = 5;
        return false;
    }
    hmi_testfault = 0;
    return true;
}


void HMI_Run_Test_Task_10ms(void)
{
    static uint16_t timer10ms = 0;
    static uint16_t faulttimer10ms = 0;
    uint8_t buttonresponse = Dev_Button_GetEvent();
    
    switch (hmi_state)
    {
        case HMT_STATE_TEST_INIT:
            SetLED(LED_PIM_RED, LED_BLINK);
            SetLED(LED_RED, LED_BLINK);
            SetLED(LED_GREEN, LED_OFF);
            hmi_state = HMT_STATE_TEST_WAIT_FOR_STARTBUTTON;
            //hmi_state = HMT_STATE_TEST_TESTING;
            break;
            
        case HMT_STATE_TEST_WAIT_FOR_STARTBUTTON:
            if (buttonresponse == DEV_BUTTON_EVENT_PRESSED_SHORT)
            {
                Drv_PwrCtrl_4SWBB_Start();
                SetLED(LED_RED, LED_OFF);   //to synchronize blinking
                SetLED(LED_RED, LED_BLINK);
                SetLED(LED_GREEN, LED_BLINK);
                timer10ms = 0;
                hmi_state = HMT_STATE_TEST_TESTING;
            }
            break;

        case HMT_STATE_TEST_WAIT_FOR_POWERUP:
            if (pwr_ctrl_state == HMT_STATE_TEST_TESTING)
            {
                timer10ms = 0;
                hmi_state = HMT_STATE_TEST_TESTING;
            }
            else
            {
                if (++timer10ms >= 100)
                    hmi_state = HMT_STATE_TEST_TEST_FAILED;
            }
            break;
                    
        case HMT_STATE_TEST_TESTING:
            if (Are_Test_values_ok() == false)
            {
                if (++faulttimer10ms >= 500)
                    hmi_state = HMT_STATE_TEST_TEST_FAILED;
            }
            else if (++timer10ms >= 500)
                hmi_state = HMT_STATE_TEST_TEST_SUCCESS;
            break;

        case HMT_STATE_TEST_TEST_SUCCESS:
            SetLED(LED_RED, LED_OFF);
            SetLED(LED_GREEN, LED_ON);
            Drv_PwrCtrl_4SWBB_Stop();
            break;
            
        case HMT_STATE_TEST_TEST_FAILED:
            SetLED(LED_RED, LED_ON); 
            SetLED(LED_GREEN, LED_OFF);
            Drv_PwrCtrl_4SWBB_Stop();
            break;

        default:
            break;
    };
}

//======================================================================================================================
// @brief this function contains 
//======================================================================================================================

void SetLEDs_100ms()
{
  static uint8_t ledtimerPIM = 0, ledtimerRED = 0, ledtimerGREEN = 0;
  uint8_t ledtimerspeedPIM = 5, ledtimerspeedRED = 4, ledtimerspeedGREEN = 4;

  //DP-PIM red LED heartbeat
  if (++ledtimerPIM >= ledtimerspeedPIM) //toggle the LED every 500ms
  {
    ledtimerPIM = 0;
    PIM_LEDred_Toggle();
  }

  //state < running --> toggle grenn LED on power board
  if (!pwr_ctrl_flagbits.run)
  {
    if (++ledtimerGREEN >= ledtimerspeedGREEN) //toggle the LED every 500ms
    {
      ledtimerGREEN = 0;
      LED_Green_Toggle();
    }
  }

  //fault fheck
  PWR_CTRL_FAULTS faults = Drv_PwrCtrl_4SWBB_GetFaults(); //@ftx

  //any fault --> fast toggle red LED on power board
  if (os_resetCause & 0x01 || faults.CollectiveFaults != 0)
  {
    LED_Red_SetHigh();
  } 
  else if (pwr_ctrl_flagbits.inopenloop)
  {
    if (++ledtimerRED >= ledtimerspeedRED) //toggle the LED every 500ms
    {
      ledtimerRED = 0;
      LED_Red_Toggle();
    }
  } else
  {
    LED_Red_SetLow();
  }
}

void SetLED(uint8_t led_index, uint8_t led_state)
{
    if (led_index >= Num_Of_LEDS)
        return;
    if ( ledstates[led_index] != led_state)
    {
        ledstates[led_index] = led_state;
        switch(led_state)
        {
            case LED_OFF:
                switch(led_index)
                {
                    case LED_RED:       LED_Red_SetLow(); break;
                    case LED_GREEN:     LED_Green_SetLow(); break;
                    case LED_PIM_RED:   PIM_LEDred_SetLow(); break;
                }
                break;
                
            case LED_ON:
                switch(led_index)
                {
                    case LED_RED:       LED_Red_SetHigh(); break;
                    case LED_GREEN:     LED_Green_SetHigh(); break;
                    case LED_PIM_RED:   PIM_LEDred_SetHigh(); break;
                }
                break;

            default:
                switch(led_index)
                {
                    case LED_RED:       LED_Red_Toggle(); break;
                    case LED_GREEN:     LED_Green_Toggle(); break;
                    case LED_PIM_RED:   PIM_LEDred_Toggle(); break;
                }
                ledtimer[led_index] = 0;
                break;
        }
    }
}

void LED_Task_100ms()
{
    uint8_t led_index=0;
    uint8_t timermax=0; 

    for (led_index = 0; led_index < Num_Of_LEDS; led_index++)
    {
        if (ledstates[led_index] > LED_ON)
        {
            if (ledstates[led_index] == LED_BLINK)
                timermax = 5;
            else
                timermax = 2;
            if (++ledtimer[led_index] >= timermax)
            {
                ledtimer[led_index] = 0;
                switch(led_index)
                {
                    case LED_RED:       LED_Red_Toggle(); break;
                    case LED_GREEN:     LED_Green_Toggle(); break;
                    case LED_PIM_RED:   PIM_LEDred_Toggle(); break;
                }
            }
        }
    }
}


//======================================================================================================================
// @brief this function contains the HMI application that handles the button inputs
//======================================================================================================================

void App_HMI_Task_10ms(void)
{ 
//    CheckButtons(); 
//    HMI_Run_Test_Satemachine();
    HMI_Run_Test_Task_10ms();
}

//======================================================================================================================
// @brief this function contains the HMI application that handles the LED outputs
//======================================================================================================================

void App_HMI_Task_100ms(void)
{
    LED_Task_100ms();
//    if(!pwr_ctrl_flagbits.BoardTestOK)
//    SetLEDs_100ms();
}

//======================================================================================================================
//======================================================================================================================
