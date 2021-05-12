/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    This software and any accompanying information is for suggestion only. It
    does not modify Microchip's standard warranty for its products. You agree
    that you are solely responsible for testing the software and determining its
    suitability. Microchip has no obligation to modify, test, certify, or
    support the software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY I
 * 025MPLIED
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

#include <p33CK256MP506.h>
#include <stdint.h>

#include "../mcc_generated_files/system.h"
#include "../mcc_generated_files/pin_manager.h"
#include "../mcc_generated_files/pwm.h"

#include "drv_pwrctrl_4SWBB.h"
#include "../_src/driver/drv_adc.h"
#include "drv_pwrctrl_4SWBB_fault.h"
#include "device/dev_button.h"

#include "Controller_pwrctrl_4SWBB.h"
#include "../../PoBo4SWBB_R0_CLtest_x.X/../PoBo4SWBB_R0_CLtest_x.X/dcdt/avgcm/dcdt_generated_code/avgcm_dcdt.h"

#include "mcc_generated_files/cmp1.h"

//------------------------------------------------------------------------------
#define SOFT_START_RAMP_SPEED 1
#define SOFT_START_RAMP_SPEED_I 1

#define Prozent90value (uint16_t) uQ15val(90) //90% clamp
#define Prozent7value uQ15val(7) //90% clamp     //200ns = 800*250ps

#define BuckPWM 1
#define BoostPWM 2
//------------------------------------------------------------------------------

PWR_CTRL_STATE_e pwr_ctrl_state = 1; //PCS_INIT;
PWR_CTRL_FLAGBITS_t pwr_ctrl_flagbits;

//------------------------------------------------------------------------------
static volatile uint16_t DCmaxClampBuck;
static volatile uint16_t DCmaxClampBoost;
static volatile uint16_t DCminClampBuck;
static volatile uint16_t DCminClampBoost;
static volatile uint16_t DCopenloopMaxVal;

static volatile uint16_t ref_ext = 0, Irefext = 0;
static volatile uint16_t ref_int = 0, Irefint = 0;
static volatile uint16_t IDCmax = 0;
static volatile uint16_t simple_integrator_outputval = 0;

uint32_t DCM01;
uint16_t DCM01i;

//local functions
static inline void Drv_PwrCtrl_4SWBB_DriveBuckBoost(uint16_t drive_val);
void Drv_PwrCtrl_4SWBB_FaultCheck(void);
void DoStartUp(void);
void DoStartUpTest(void);
void StartUpTestMaxCurrentMethodCurrent(void);
void StartUpTestMaxCurrentMethodVoltage(void);

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_Init(void)
{
  uint32_t l_zw_;

  l_zw_ = __builtin_muluu(uQ15val(0.9), PG1PER);
  DCmaxClampBuck = (uint16_t) (l_zw_ >> 16);
  l_zw_ = __builtin_muluu(uQ15val(0.07), PG1PER);
  DCminClampBuck = (uint16_t) (l_zw_ >> 16) + PG1DTH;

  l_zw_ = __builtin_muluu(uQ15val(0.9), PG1PER);
  DCmaxClampBoost = (uint16_t) (l_zw_ >> 16);
  l_zw_ = __builtin_muluu(uQ15val(0.07), PG1PER);
  DCminClampBoost = (uint16_t) (l_zw_ >> 16) + PG2DTH;

  DCopenloopMaxVal = DCmaxClampBuck + DCmaxClampBoost;
  pwr_ctrl_flagbits.CollectivePwrCtrlFlagbits = 0;
  pwr_ctrl_flagbits.inopenloop = 1;
  pwr_ctrl_flagbits.enabled = 1;

  pwr_ctrl_flagbits.StartupPhaseOL = 1;
  pwr_ctrl_flagbits.StartupPhaseCL = 0;


  //============================================================================
  // @brief initialize Controllers
  CurrentController_PwrCtrl_4SWBB_Init();
  VoltageController_PwrCtrl_4SWBB_Init();
  //============================================================================
  //ONLY THAT PROCEDURE WORKS---------------------------------------------------
  //PENH/L must be enabled in MCC that PWM pins show up in Pin Module
  //The following procedure prevents glitch at both PWM startup
  // After that init the PWMs are in override mode
  //----------------------------------------------------------------------------
  PG2IOCONHbits.PENH = 0; // PWMxH Output Port Disable: PWM generator NOT controls the PWMxH output pin
  PG2IOCONHbits.PENL = 0; // PWMxL Output Port Disable: PWM generator NOT controls the PWMxL output pin
  PG2DC = DCminClampBoost;
  PWM_GeneratorEnable(BoostPWM);
  PWM_SoftwareUpdateRequest(BoostPWM); // Update all PWM registers
  while (PWM_SoftwareUpdatePending(BoostPWM));
  PG2IOCONHbits.PENH = 1; // PWMxH Output Port Enable: PWM generator controls the PWMxH output pin
  PG2IOCONHbits.PENL = 1; // PWMxL Output Port Enable: PWM generator controls the PWMxL output pin

  PG1IOCONHbits.PENH = 0; // PWMxH Output Port Disable: PWM generator NOT controls the PWMxH output pin
  PG1IOCONHbits.PENL = 0; // PWMxL Output Port Disable: PWM generator NOT controls the PWMxL output pin
  PG1DC = DCminClampBuck;
  PWM_GeneratorEnable(BuckPWM);
  PWM_SoftwareUpdateRequest(BuckPWM); // Update all PWM registers
  while (PWM_SoftwareUpdatePending(BuckPWM));
  PG1IOCONHbits.PENH = 1; // PWMxH Output Port Enable: PWM generator controls the PWMxH output pin
  PG1IOCONHbits.PENL = 1; //@ftx // PWMxL Output Port Enable: PWM generator controls the PWMxL output pin
  //ONLY THAT PROVEDURE WORKS---------------------------------------------------

  pwr_ctrl_flagbits.ref_from_poti = 1; //startup with poti as input for Vout reference
  Drv_PwrCtrl_4SWBB_Init_Fault();
  
}

//==============================================================================
// @brief this function contains 
// @ftx document PWM startup signals; difference OL-CL
//==============================================================================

void Drv_PwrCtrl_4SWBB_SwitchOnPWM(void)
{
  PWM_GeneratorDisable(BuckPWM);
  PWM_GeneratorDisable(BoostPWM);

  SMPS_Controller2P2ZInitialize(&VMC2p2z); // Clear histories Voltage Controller
  SMPS_Controller2P2ZInitialize(&AVG_CM2p2z); // Clear histories Voltage Controller

  PG1TRIGA = 0;
  PWM_DutyCycleSet(BuckPWM, 0);
  PWM_SoftwareUpdateRequest(BuckPWM); // Update all PWM registers
  while (PWM_SoftwareUpdatePending(BuckPWM));

  PWM_DutyCycleSet(BoostPWM, 0);
  PWM_SoftwareUpdateRequest(BoostPWM); // Update all PWM registers
  while (PWM_SoftwareUpdatePending(BoostPWM));

  PWM_OverrideHighDisable(BoostPWM);
  PWM_GeneratorEnable(BoostPWM);
  PWM_OverrideLowDisable(BoostPWM);


//  PWM_OverrideLowDisable(BuckPWM);  //@ftx
  PWM_GeneratorEnable(BuckPWM);
  PWM_OverrideHighDisable(BuckPWM);


  pwr_ctrl_flagbits.pwm_running = 1;
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_SwitchOffPWM(void)
{
  ref_int = 0; //start from zero volt	
  Irefint = 0; //start from zero current
  pwr_ctrl_flagbits.StartupPhaseOL = 1;
  pwr_ctrl_flagbits.StartupPhaseCL = 0;

  PWM_OverrideHighEnable(BuckPWM);
  PWM_OverrideHighEnable(BoostPWM);
  PWM_OverrideLowEnable(BuckPWM);
  PWM_OverrideLowEnable(BoostPWM);
  pwr_ctrl_flagbits.pwm_running = 0;

}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_Start(void)
{
  while (pwr_ctrl_flagbits.run == 0) //it needs to be a while function to avoid interrupt access conflicts
  {
    pwr_ctrl_flagbits.run = 1;
  }
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_Stop(void)
{
  while (pwr_ctrl_flagbits.run == 1) //it needs to be a while function to avoid interrupt access conflicts
  {
    pwr_ctrl_flagbits.run = 0;
  }
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_SetMode_OpenLoop(void)
{
  //it needs to be a while function to avoid interrupt access conflicts
  while (pwr_ctrl_flagbits.inopenloop == 0 || pwr_ctrl_flagbits.inclosedloop == 1)
  {
    pwr_ctrl_flagbits.inopenloop = 1;
    pwr_ctrl_flagbits.inclosedloop = 0;
    pwr_ctrl_flagbits.StartupPhaseCL = 0; //@ftx
  }
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_SetMode_ClosedLoop(void)
{
  //it needs to be a while function to avoid interrupt access conflicts
  while (pwr_ctrl_flagbits.inopenloop == 1 || pwr_ctrl_flagbits.inclosedloop == 0)
  {
    pwr_ctrl_flagbits.inopenloop = 0;
    pwr_ctrl_flagbits.inclosedloop = 1;
    pwr_ctrl_flagbits.StartupPhaseCL = 1; //@ftx
  }
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_Task_100us(void)
{
  static uint16_t taskCounter = 0;

  //Check for Fault
  Drv_PwrCtrl_4SWBB_Fault_Check(); //@ftx

  switch (pwr_ctrl_state)
  {
      //------------------------------------------------------------------------
    case PCS_INIT: //1
      //peripherals should be already initialized by MCC, so goto next state
      //            if (FourSWBBFaults.CollectiveFaults == 0)
    {
      Drv_PwrCtrl_4SWBB_SetFaultCounters();
      pwr_ctrl_state = PCS_WAIT_IF_FAULT_ACTIVE;
    }
      break;

      //------------------------------------------------------------------------
    case PCS_WAIT_IF_FAULT_ACTIVE: //2
      if ((FourSWBBFaults.CollectiveFaults == 0))
      {
        ref_int = 0; //start from zero volt
        pwr_ctrl_state = PCS_STANDBY;
      }
      break;

      //------------------------------------------------------------------------
    case PCS_STANDBY: //3
      if (FourSWBBFaults.CollectiveFaults)
      {
        pwr_ctrl_state = PCS_WAIT_IF_FAULT_ACTIVE;
      }
      else if (ref_ext > 0 && pwr_ctrl_flagbits.run == 1)
      {
        Drv_PwrCtrl_4SWBB_SwitchOnPWM();
        pwr_ctrl_state = PCS_POWER_ON_DELAY;
        //        pwr_ctrl_flagbits.inopenloop = 1; //@ftx
        //        pwr_ctrl_flagbits.inclosedloop = 0; //@ftx
      }
      break;

      //------------------------------------------------------------------------
    case PCS_POWER_ON_DELAY: //4

      pwr_ctrl_state = PCS_SOFT_START_OL;

      Irefext = 2600;
      if(pwr_ctrl_flagbits.inclosedloop)
        ref_int = 500;//750;
      else
        ref_int = 0;
      
      Irefint = 600; //1429;
//      AVG_CM2p2z.maxOutput = 0;
      
//      pwr_ctrl_state = PCS_SOFT_START_OL;
//      Irefext = 2600;
//
//      pwr_ctrl_flagbits.inopenloop = 1;
//      pwr_ctrl_flagbits.inclosedloop = 0;

      break;

      //------------------------------------------------------------------------
    case PCS_SOFT_START_OL: //5
      Drv_PwrCtrl_4SWBB_FaultCheck();
      StartUpTestMaxCurrentMethodCurrent();

      break;

      //------------------------------------------------------------------------
    case PCS_SOFT_START: //6
      Drv_PwrCtrl_4SWBB_FaultCheck();
      StartUpTestMaxCurrentMethodCurrent();
      StartUpTestMaxCurrentMethodVoltage();

      break;

      //------------------------------------------------------------------------
    case PCS_UP_AND_RUNNING: //7

      Drv_PwrCtrl_4SWBB_FaultCheck();

      if (ref_ext == 0 || pwr_ctrl_flagbits.run == 0)
      {
        Drv_PwrCtrl_4SWBB_SwitchOffPWM();
        pwr_ctrl_state = PCS_STANDBY;
        break;
      }

      if (ref_int != ref_ext)
      {
        pwr_ctrl_state = PCS_SOFT_START;
        break;
      }
      //      ref_int = ref_ext;
      break;

      //------------------------------------------------------------------------
    case PCS_SHUTDOWN: //8
      break;

      //------------------------------------------------------------------------

  }

}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_SetReferenceRaw(uint16_t ref)
{
  if (pwr_ctrl_flagbits.ref_from_poti)
    ref_ext = ref;
  if (pwr_ctrl_flagbits.ref_from_GUI)
    ref_ext = pwr_ctrl_adc_data.drv_comm_val_VoutRef;
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_SetReference_mV(uint16_t ref_mV)
{
  //Todo
}

//==============================================================================
// @brief|| this function contains the AVG Current Controller (every PWM cycle) 
// @brief|| and the Voltage Controller every x-PWM cycles
//==============================================================================

void Drv_PwrCtrl_4SWBB_CtrlLoop()
{
  static uint16_t loopCounterV = 0;
//  uint32_t DCM01;
  
  
//  TP34_RX_SetHigh();

//  if (pwr_ctrl_flagbits.inclosedloop)
  {


    //Debug DACout------------------------------------
//        CMP1_SetDACDataHighValue(Controller_4SWBB._VDCref);
    CMP1_SetDACDataHighValue(Controller_4SWBB._IACref);
//        CMP1_SetDACDataHighValue(DCM01i);
    //Debug DACout------------------------------------

    if (loopCounterV++ > 1)
    {
      Controller_4SWBB._VDCref = ref_int >> 0; //xxV  

      SMPS_Controller2P2ZUpdate(&VMC2p2z, &pwr_ctrl_adc_data.drv_adc_val_FB_Vout, Controller_4SWBB._VDCref, &Controller_4SWBB._IACref);
      
//      if((pwr_ctrl_state == PCS_SOFT_START_OL) && (Controller_4SWBB._IACref < Irefint))
      if((pwr_ctrl_state == PCS_SOFT_START_OL) && (pwr_ctrl_adc_data.drv_adc_val_FB_Vout > 500))
      {
//        Irefint = Irefext;
//        ref_int = pwr_ctrl_adc_data.drv_adc_val_FB_Vout;
        pwr_ctrl_state = PCS_SOFT_START;
        TP35_TX_SetHigh();
      }  
//      else  if (Controller_4SWBB._IACref > Irefint) 
      if ((Controller_4SWBB._IACref > Irefint) && (pwr_ctrl_state < PCS_SOFT_START)) 
      {  
        Controller_4SWBB._IACref = Irefint; //2600=3.5A
        TP35_TX_SetLow();
      }
      else
      {
//        TP35_TX_SetLow();
      }  
      
      loopCounterV = 0;
    }

    //    if(pwr_ctrl_state < PCS_UP_AND_RUNNING) Controller_4SWBB._IACref = Irefint;
    SMPS_Controller2P2ZUpdate(&AVG_CM2p2z, &pwr_ctrl_adc_data.drv_adc_val_FB_Iin, Controller_4SWBB._IACref, &Controller_4SWBB._CurrentControllerResult);
    
    //DCM DCM ------------------------------------------------------------------
//    DCM01 = pwr_ctrl_adc_data.drv_adc_val_FB_Vout * PG1PER;
//    DCM01i = (uint16_t)(DCM01/Controller_4SWBB._CurrentControllerResult);
    
//    if(pwr_ctrl_state < PCS_UP_AND_RUNNING)
//    {
//      PWM_OverrideLowEnable(BuckPWM);
//      TP35_TX_SetLow(); 
//    }  
//    else if(DCM01i < pwr_ctrl_adc_data.drv_adc_val_FB_Vdd)
//    {
//      PWM_OverrideLowDisable(BuckPWM);
//      TP35_TX_SetHigh();
//    }  
//    else
//    {
//      PWM_OverrideLowEnable(BuckPWM);
//      TP35_TX_SetLow();
//    }  
    
    //----------------------------------------------------------------------
    if (Controller_4SWBB._CurrentControllerResult > DCmaxClampBuck)
    {
      Controller_4SWBB._DCBuckset = DCmaxClampBuck;
    }
    else if (Controller_4SWBB._CurrentControllerResult < DCminClampBuck)
    {
      Controller_4SWBB._DCBuckset = DCminClampBuck;
    }
    else
    {
      Controller_4SWBB._DCBuckset = Controller_4SWBB._CurrentControllerResult;
    }
    if (pwr_ctrl_flagbits.inclosedloop)
    {
      PWM_TriggerCompareValueSet(BUCK_LEG_PWM, (Controller_4SWBB._DCBuckset >> 1));
      PWM_DutyCycleSet(BUCK_LEG_PWM, Controller_4SWBB._DCBuckset);
      PWM_SoftwareUpdateRequest(BUCK_LEG_PWM);
      while (PWM_SoftwareUpdatePending(BUCK_LEG_PWM));
    }
    //----------------------------------------------------------------------
    if (Controller_4SWBB._CurrentControllerResult > DCmaxClampBuck /*PG1PER*/)
    {
      Controller_4SWBB._DCBoostset = Controller_4SWBB._CurrentControllerResult - DCmaxClampBuck /*PG1PER*/;
      if (Controller_4SWBB._DCBoostset > DCmaxClampBoost)
      {
        Controller_4SWBB._DCBoostset = DCmaxClampBoost;
      }
      else if (Controller_4SWBB._DCBoostset < DCminClampBoost)
      {
        Controller_4SWBB._DCBoostset = DCminClampBoost;
      }
      else
      {
        Controller_4SWBB._DCBoostset = Controller_4SWBB._DCBoostset;
      }
      if (pwr_ctrl_flagbits.inclosedloop)
      {
        PWM_DutyCycleSet(BOOST_LEG_PWM, Controller_4SWBB._DCBoostset);
        PWM_SoftwareUpdateRequest(BOOST_LEG_PWM);
        while (PWM_SoftwareUpdatePending(BOOST_LEG_PWM));
      }
    }
    else
    {
      if (pwr_ctrl_flagbits.inclosedloop)
      {
        Controller_4SWBB._DCBoostset = DCminClampBoost;
        PWM_DutyCycleSet(BOOST_LEG_PWM, Controller_4SWBB._DCBoostset);
        PWM_SoftwareUpdateRequest(BOOST_LEG_PWM);
        while (PWM_SoftwareUpdatePending(BOOST_LEG_PWM));
      }
    }
    //----------------------------------------------------------------------
  }

  if (pwr_ctrl_flagbits.inopenloop)
  {
    //Debug DACout------------------------------------
    //    CMP1_SetDACDataHighValue(ref_int);
    //Debug DACout------------------------------------

    Drv_PwrCtrl_4SWBB_DriveBuckBoost(ref_int);
  }
  else
  {

  }

//  TP34_RX_SetLow();
}
//==============================================================================
// @brief this function contains 
//-----------------------------------------------------------------------------
// AN15:      0...........2048.........4096
// fract:     0...........32767........65535
// Value      0...........0.5..........1
// 
// Mode       Buck........Boost........
// ValueDC    0...........1/0..........1         
// DC         0       100%/0.........100% 
// over the poti range the DC must go twice from 0% to 100%
// AN15 value is shifted by 5 instead of 4 for 12 Bit to 16 bit
//==============================================================================

inline void Drv_PwrCtrl_4SWBB_DriveBuckBoost(uint16_t drive_val)
{
  uint32_t l_BoostDC, l_BuckDC;
  uint16_t BoostDC, BuckDC;

  //----------------------------------------------------------------------------
  //Boost
  if (drive_val > (0x0800))
  {
    l_BoostDC = __builtin_muluu(drive_val << 5, PG2PER); //
    BoostDC = (uint16_t) (l_BoostDC >> 16);
  }
  else
  {
    BoostDC = DCminClampBoost;
  }

  if (BoostDC > DCmaxClampBoost)
  {
    BoostDC = DCmaxClampBoost;
  }
  else if (BoostDC < DCminClampBoost)
  {
    BoostDC = DCminClampBoost;
  }

  //----------------------------------------------------------------------------
  //Buck
  if (drive_val < 0x0800)
  {
    l_BuckDC = __builtin_muluu(drive_val << 5, PG1PER); //
    BuckDC = (uint16_t) (l_BuckDC >> 16);
  }
  else
  {
    BuckDC = DCmaxClampBuck;
  }

  if (BuckDC > DCmaxClampBuck)
  {
    BuckDC = DCmaxClampBuck;
  }
  else if (BuckDC < DCminClampBuck)
  {
    BuckDC = DCminClampBuck;
  }

  PG1TRIGA = BuckDC >> 1;
  PWM_DutyCycleSet(1, BuckDC);
  PWM_DutyCycleSet(2, BoostDC);

  PG1STATbits.UPDREQ = 1;
  PG2STATbits.UPDREQ = 1;
  while (PG1STATbits.UPDREQ == 1);
  while (PG2STATbits.UPDREQ == 1);

  //  PWM_SoftwareUpdateRequest(BUCK_LEG_PWM);
  //  while (PWM_SoftwareUpdatePending(BUCK_LEG_PWM));
  //  PWM_SoftwareUpdateRequest(BOOST_LEG_PWM);
  //  while (PWM_SoftwareUpdatePending(BOOST_LEG_PWM));

}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_FaultCheck(void)
{
  if (FourSWBBFaults.CollectiveFaults != 0)
  {
    Drv_PwrCtrl_4SWBB_SwitchOffPWM();
    pwr_ctrl_state = PCS_WAIT_IF_FAULT_ACTIVE;
  }
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void DoStartUp(void)
{
  //  if ((ref_int == ref_ext))
  //  if ((ref_int == ref_ext) && (Irefint == Irefext))

  if ((ref_int == ref_ext) && (AVG_CM2p2z.maxOutput > 22841))
  {
    //    if(   (pwr_ctrl_adc_data.drv_adc_val_FB_Vout > (Controller_4SWBB._VDCref-(Controller_4SWBB._VDCref>>4)))
    //       && (pwr_ctrl_adc_data.drv_adc_val_FB_Vout < (Controller_4SWBB._VDCref+(Controller_4SWBB._VDCref>>4))) )    
    {
      pwr_ctrl_state = PCS_UP_AND_RUNNING;
    }
  }
  else
  {
    //    if (Irefint < Irefext)
    //    {
    //      if (Irefint + SOFT_START_RAMP_SPEED_I < Irefext)
    //      {  
    //        Irefint += SOFT_START_RAMP_SPEED_I;
    //      }
    //      else
    //      {  
    //        Irefint = Irefext;
    //      }
    //    }

    //    if(AVG_CM2p2z.maxOutput < 22842) AVG_CM2p2z.maxOutput+=4;

    if (ref_int < ref_ext)
    {
      if (ref_int + SOFT_START_RAMP_SPEED < ref_ext)
      {

        if (AVG_CM2p2z.maxOutput < 22842) AVG_CM2p2z.maxOutput += 4;


        if ((pwr_ctrl_adc_data.drv_adc_val_FB_Vout > (Controller_4SWBB._VDCref - (Controller_4SWBB._VDCref >> 3)))
                && (pwr_ctrl_adc_data.drv_adc_val_FB_Vout < (Controller_4SWBB._VDCref + (Controller_4SWBB._VDCref >> 3))))
        {
          ref_int += SOFT_START_RAMP_SPEED;
          //            if(AVG_CM2p2z.maxOutput < 22842) AVG_CM2p2z.maxOutput+=4;
        }






      }
      else
      {
        ref_int = ref_ext;
      }
    }
    else
    {
      if (ref_int + SOFT_START_RAMP_SPEED > ref_ext)
        ref_int -= SOFT_START_RAMP_SPEED;
      else
        ref_int = ref_ext;
    }
  }
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void DoStartUpTest(void)
{
  if (pwr_ctrl_adc_data.drv_adc_val_FB_Vout < 875) //775=5V
  {
    if (ref_int < ref_ext)
    {
      if (ref_int + SOFT_START_RAMP_SPEED < ref_ext)
        ref_int += SOFT_START_RAMP_SPEED;
    }
  }
  else
  {
    if ((pwr_ctrl_adc_data.drv_adc_val_FB_Vout > (Controller_4SWBB._VDCref - (Controller_4SWBB._VDCref >> 3)))
            && (pwr_ctrl_adc_data.drv_adc_val_FB_Vout < (Controller_4SWBB._VDCref + (Controller_4SWBB._VDCref >> 3))))
    {
      //      Irefint = 0;//pwr_ctrl_adc_data.drv_adc_val_FB_Iin;
      //      AVG_CM2p2z.maxOutput = PG1DC;

      pwr_ctrl_state = PCS_SOFT_START;
      pwr_ctrl_flagbits.inopenloop = 0;
      pwr_ctrl_flagbits.inclosedloop = 1;
    }
  }
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void StartUpTestMaxCurrentMethodCurrent(void)
{
   static uint16_t _CounterStartupCurrent = 0;
   
   //IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
//  if (_CounterStartupCurrent++ > 0)
  {
    if (Irefint < Irefext)
    {
      if (Irefint + SOFT_START_RAMP_SPEED_I < Irefext)
      {
        Irefint += SOFT_START_RAMP_SPEED_I;
      }
      else
      {
        Irefint = Irefext;
//        pwr_ctrl_state = PCS_SOFT_START;
//        ref_int = pwr_ctrl_adc_data.drv_adc_val_FB_Vout;
        TP34_RX_SetHigh();
      }
    }
    _CounterStartupCurrent = 0;

  }
//   //IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
////  if (_CounterStartupCurrent++ > 2)
//  {
//     Irefint = Irefext;
//     
//    if (AVG_CM2p2z.maxOutput++ < AVGCM_COMP_2P2Z_MAX_CLAMP)
//    {
//    }
//    else
//    {
//      pwr_ctrl_state = PCS_SOFT_START;
//      ref_int = pwr_ctrl_adc_data.drv_adc_val_FB_Vout;
//      TP34_RX_SetHigh();
//    }
//     
//    _CounterStartupCurrent = 0;
//  }
   
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void StartUpTestMaxCurrentMethodVoltage(void)
{
  //UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU 
  {
    if (ref_int < ref_ext)
    {
      if (ref_int + SOFT_START_RAMP_SPEED < ref_ext)
              ref_int += SOFT_START_RAMP_SPEED;
      else
      {
        ref_int = ref_ext;
        pwr_ctrl_state = PCS_UP_AND_RUNNING;
        TP34_RX_SetLow();

      }
    }
    else
    {
      if (ref_int + SOFT_START_RAMP_SPEED > ref_ext)
              ref_int -= SOFT_START_RAMP_SPEED;
      else
      {
        ref_int = ref_ext;
        pwr_ctrl_state = PCS_UP_AND_RUNNING;
      }

    }
  }
  //--------------------------------------------------------------------------

}

//==============================================================================
//==============================================================================
