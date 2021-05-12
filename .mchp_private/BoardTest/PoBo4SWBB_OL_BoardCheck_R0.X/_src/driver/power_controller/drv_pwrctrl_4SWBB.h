/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    This software and any accompanying information is for suggestion only. It
    does not modify Microchip's standard warranty for its products. You agree
    that you are solely responsible for testing the software and determining its
    suitability. Microchip has no obligation to modify, test, certify, or
    support the software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef _DRV_PWRCTRL_4SWBB_H_
#define	_DRV_PWRCTRL_4SWBB_H_

#include <xc.h> // include processor files - each processor file is guarded.  

//------------------------------------------------------------------------------

typedef enum
{
    PCS_INIT					= 1,    // Init peripherals that are involved
    PCS_WAIT_IF_FAULT_ACTIVE	= 2,    // goes on if no fault is active
    PCS_STANDBY					= 3,    // wait for reference to be greater X
    PCS_POWER_ON_DELAY  		= 4,    // wait a short time until power can ramp
    
    PCS_SOFT_START_OL			= 5,    // Soft-Start Ramp Up/Down Output Voltage
           
            
    PCS_SOFT_START				= 6,    // Soft-Start Ramp Up/Down Output Voltage
    PCS_UP_AND_RUNNING          = 7,    // Soft-Start is complete, power is up and running
    PCS_SHUTDOWN                = 8     // shutting down the controller/reference voltage
}PWR_CTRL_STATE_e;
extern PWR_CTRL_STATE_e pwr_ctrl_state;

//------------------------------------------------------------------------------

typedef union
{
    struct
    {
        volatile unsigned                   : 3;    // Bit <0:2> (reserved)
        volatile unsigned                   : 3;    // Bit <3:5> (reserved)
        volatile unsigned BoardTestOK       : 1;    // Bit  6:
        volatile unsigned StartupPhaseCL    : 1;    // Bit  7:
        volatile unsigned enabled           : 1;    // Bit  8: 
        volatile unsigned run               : 1;    // Bit  9: 
        volatile unsigned auto_start        : 1;    // Bit 10: 
        volatile unsigned pwm_running       : 1;    // Bit 11: 
        volatile unsigned inopenloop        : 1;    // Bit 12:
        volatile unsigned inclosedloop      : 1;    // Bit 13:
        volatile unsigned ref_from_poti     : 1;    // Bit 14:
        volatile unsigned ref_from_GUI      : 1;    // Bit 15:
    };
    uint16_t CollectivePwrCtrlFlagbits;
}   __attribute__((packed))PWR_CTRL_FLAGBITS_t;

extern PWR_CTRL_FLAGBITS_t pwr_ctrl_flagbits;

//------------------------------------------------------------------------------

void Drv_PwrCtrl_4SWBB_Init(void);
void Drv_PwrCtrl_4SWBB_Task_100us(void);
void Drv_PwrCtrl_4SWBB_SetReferenceRaw(uint16_t ref);
void Drv_PwrCtrl_4SWBB_SetReference_mV(uint16_t ref_mV);
void Drv_PwrCtrl_4SWBB_Start(void);
void Drv_PwrCtrl_4SWBB_Stop(void);
void Drv_PwrCtrl_4SWBB_SetMode_OpenLoop(void);
void Drv_PwrCtrl_4SWBB_SetMode_ClosedLoop(void);

#define sQ15val(val_) ((val_ < 0.0) ? (32768 * (val_) - 0.5) : (32767 * (val_) + 0.5))
#define uQ15val(val_) (uint16_t)((float)val_ * 0xFFFF)

#endif	// _DRV_PWRCTRL_4SWBB_H_

