#ifndef FAULT_H
#define FAULT_H

//#include <xc.h>

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
   int16_t  fltThreshold;        // Fault threshold limit: Range of -32768 to 32767
   int16_t  fltHysLimit;         // Fault hysteresis limit. If hysteresis is not needed, fltThreshold = fltHysLimit
   uint16_t  fltThresholdCnt;     // Number of consecutive fault events before fault becomes active.  
   uint32_t  fltHysCnt;           // Number of consecutive events when flt input outside hysteresis limit in order to remove fault, set to 0 to disable Hys 
   uint16_t  fltCounter;          // Internal counter for activating/removing fault 
   uint16_t  fltCode;             // Code that can be used to display fault (1st fault occurred) to global variable
   uint8_t   faultActive;         // Set/Cleared inside flt check loop        
}FAULT_OBJ_T;   

typedef void(*FAULT_CALLBACK)(FAULT_OBJ_T* faultInput);

void FAULT_Init(FAULT_OBJ_T *faultInput,
                int16_t  fltThreshold, 
                int16_t  fltHysLimit,
                uint16_t fltThresholdCnt,
                uint32_t fltHysCnt);

void FAULT_SetCounters(FAULT_OBJ_T *faultInput, uint16_t fltThresholdCnt, uint32_t fltHysCnt);
void FAULT_SetThresholds(FAULT_OBJ_T *faultInput, int16_t fltThreshold, int16_t fltHysLimit);
bool FAULT_CheckMax(FAULT_OBJ_T *faultInput, int16_t faultSource, FAULT_CALLBACK callback);
bool FAULT_CheckMin(FAULT_OBJ_T *faultInput, int16_t faultSource, FAULT_CALLBACK callback);
bool FAULT_CheckBit(FAULT_OBJ_T *faultInput, bool faultBit, FAULT_CALLBACK callback);

#endif 

