/* LaserDiodeDriver.h
 *
 * Copyright (C) 2020 John Wigg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef LASERDIODEDRIVER_H_
#define LASERDIODEDRIVER_H_

#include "InterfaceBoard.h"

#include "MMDevice.h"
#include "DeviceBase.h"
#include "ModuleInterface.h"

#include <string>

#define ERR_UNKNOWN_MODE         102
#define NUMBER_OF_LASERS         6

class LaserDiodeDriver : public CGenericBase<LaserDiodeDriver>  
{
public:
   LaserDiodeDriver();
   ~LaserDiodeDriver();
  
   // MMDevice API
   int Initialize();
   int Shutdown();
  
   void GetName(char* name) const;      
   
   // LaserDiodeDriver API
   int SetLaserPower(int idx, double power);
   int SetLaserOnOff(int idx, bool enabled);

   int OnNumberOfLasers(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnBoardType(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnPort(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnLaserOnOff(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnLaserPower(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnLaserLabel(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnLaserMinPower(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnLaserMaxPower(MM::PropertyBase* pProp, MM::ActionType eAct);

   double GetLaserMaxPower(int idx);
   double GetLaserMinPower(int idx);

   bool Busy() { return false; }

private:
   bool initialized_ = false;
   InterfaceBoard *interface_ = nullptr;
   std::string boardType_;
};

#endif //LASERDIODEDRIVER_H_
