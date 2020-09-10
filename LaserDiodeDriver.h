///////////////////////////////////////////////////////////////////////////////
// FILE:          LaserDiodeDriver.h
// PROJECT:       Micro-Manager
// SUBSYSTEM:     DeviceAdapters
//-----------------------------------------------------------------------------
// DESCRIPTION:   Device adapter for the laser driver used in a project by
//                Daniel Schröder at the FSU Jena. Uses a Velleman K8061
//                interface board via the comedilib driver.
//                
// AUTHOR:        John Wigg
//                
// COPYRIGHT:     
//
// LICENSE:       This file is distributed under the BSD license.
//                License text is included with the source distribution.
//
//                This file is distributed in the hope that it will be useful,
//                but WITHOUT ANY WARRANTY; without even the implied warranty
//                of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//                IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//                CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//                INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES.
//

#ifndef _LaserDiodeDriver_H_
#define _LaserDiodeDriver_H_

#include "InterfaceBoard.h"

#include "MMDevice.h"
#include "DeviceBase.h"
#include "ModuleInterface.h"

#include <string>

//////////////////////////////////////////////////////////////////////////////
// Error codes
//
#define ERR_UNKNOWN_MODE         102

class LaserDiodeDriver : public CGenericBase<LaserDiodeDriver>  
{
public:
   LaserDiodeDriver();
   ~LaserDiodeDriver();
  
   // MMDevice API
   // ------------
   int Initialize();
   int Shutdown();
  
   void GetName(char* name) const;      
   
   // LaserDiodeDriver API
   // ---------------
   int SetLaserPower(int idx, double power) const;
   int SetLaserOnOff(int idx, bool enabled) const;

   int OnLaserOnOff(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnLaserPower(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnLaserMinPower(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnLaserMaxPower(MM::PropertyBase* pProp, MM::ActionType eAct);

   double GetLaserMaxPower(int idx);
   double GetLaserMinPower(int idx);

   bool Busy() { return false; }

private:
   bool initialized_;
   InterfaceBoard *interface_;
   int numberOfLasers_;
};

#endif //_LaserDiodeDriver_H_
