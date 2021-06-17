/* LaserDiodeDriver.cpp
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

#include "LaserDiodeDriver.h"

#include <iostream>
#include <sstream>

#ifdef BUILD_K8061
#include "K8061.h"
const char* g_BoardK8061 = "K8061";
#endif

#ifdef BUILD_DUMMY
#include "DummyBoard.h"
const char* g_BoardDummy = "Dummy";
#endif

#ifdef BUILD_ARDUINO
#include "Arduino.h"
const char* g_BoardArduino = "Arduino";
#endif

const char* const g_Msg_DEVICE_INVALID_BOARD_TYPE = "Please choose a valid device Type!";

const char* g_LaserDiodeDriverName = "LaserDiodeDriver";

const char* ON = "On";
const char* OFF = "Off";

#define DEVICE_INVALID_BOARD_TYPE 142

MODULE_API void InitializeModuleData()
{
   RegisterDevice(g_LaserDiodeDriverName, MM::GenericDevice, "Laser diode driver device adapter.");
}

MODULE_API MM::Device* CreateDevice(const char* deviceName)
{
   if (deviceName == 0)
      return 0;

   // decide which device class to create based on the deviceName parameter
   if (strcmp(deviceName, g_LaserDiodeDriverName) == 0)
   {
      // create camera
      return new LaserDiodeDriver();
   }

   // ...supplied name not recognized
   return 0;
}

MODULE_API void DeleteDevice(MM::Device* pDevice)
{
   delete pDevice;
}

LaserDiodeDriver::LaserDiodeDriver()
{
   // call the base class method to set-up default error codes/messages
   InitializeDefaultErrorMessages();
   SetErrorText(DEVICE_INVALID_BOARD_TYPE, g_Msg_DEVICE_INVALID_BOARD_TYPE);

   int ret;
   CPropertyAction* pAct = new CPropertyAction(this, &LaserDiodeDriver::OnBoardType);
   ret = CreateStringProperty("Device Type", "", false, pAct, true);
#ifdef BUILD_DUMMY
   AddAllowedValue("Device Type", g_BoardDummy);
#endif
#ifdef BUILD_K8061
   AddAllowedValue("Device Type", g_BoardK8061);
#endif
#ifdef BUILD_ARDUINO
   AddAllowedValue("Device Type", g_BoardArduino);
#endif
   pAct = new CPropertyAction(this, &LaserDiodeDriver::OnPort);
   ret = CreateStringProperty("Device Port", "Undefined", false, pAct, true);

   for (int i = 0; i < NUMBER_OF_LASERS; ++i) {
      CPropertyAction* pActLaserMinPower = new CPropertyAction (this, &LaserDiodeDriver::OnLaserMinPower);
      CPropertyAction* pActLaserMaxPower = new CPropertyAction (this, &LaserDiodeDriver::OnLaserMaxPower);

      char p_name[64];

      // Laser label
      sprintf(p_name, "Laser Label %d", i+1);
      ret = CreateStringProperty(p_name, "", false, nullptr, true);

      // Laser limits
      sprintf(p_name, "Min. Laser Power %d (%%)", i+1);
      ret = CreateFloatProperty(p_name, 0.0, false, pActLaserMinPower, true);
      ret = SetPropertyLimits(p_name, 0.0, 100.0);

      sprintf(p_name, "Max. Laser Power %d (%%)", i+1);
      ret = CreateFloatProperty(p_name, 100.0, false, pActLaserMaxPower, true);
      ret = SetPropertyLimits(p_name, 0.0, 100.0);
   }
}

LaserDiodeDriver::~LaserDiodeDriver()
{
   if (initialized_)
   {
      Shutdown();
   }
   delete interface_;
}

void LaserDiodeDriver::GetName(char* name) const
{
   CDeviceUtils::CopyLimitedString(name, g_LaserDiodeDriverName);
}

int LaserDiodeDriver::Initialize()
{
   int ret = DEVICE_OK;

   // TODO: This is rather ugly
#ifdef BUILD_K8061
   if (strcmp(boardType_.c_str(), g_BoardK8061) == 0) {
      char dir[MM::MaxStrLength];
      GetProperty("Device Port", dir);
   
      std::string deviceDir = std::string(dir);

      interface_ = new K8061(deviceDir);
   } else 
#endif
#ifdef BUILD_DUMMY
   if (strcmp(boardType_.c_str(), g_BoardDummy) == 0) {
      interface_ = new DummyBoard();
   } else 
#endif
#ifdef BUILD_ARDUINO
   if (strcmp(boardType_.c_str(), g_BoardArduino) == 0) {
      char dir[MM::MaxStrLength];
      GetProperty("Device Port", dir);
   
      std::string deviceDir = std::string(dir);
      
      interface_ = new Arduino(deviceDir);
   } else 
#endif
   return DEVICE_INVALID_BOARD_TYPE;
   
   interface_->Open();
   if (!interface_->DeviceIsOpen()) {
      LogMessage("The device could not be opened!");
      ret = DEVICE_NOT_CONNECTED;
   }
   
   if (ret != DEVICE_OK) {
      return ret;
   }

   std::vector<std::string> digitalValues;
   digitalValues.push_back(OFF);
   digitalValues.push_back(ON);

   for (int i = 0; i < NUMBER_OF_LASERS; ++i) {
      CPropertyAction* pActLaserPower = new CPropertyAction (this, &LaserDiodeDriver::OnLaserPower);
      CPropertyAction* pActLaserOnOff = new CPropertyAction (this, &LaserDiodeDriver::OnLaserOnOff);

      char p_name[64];

      // Laser power
      sprintf(p_name, "Laser Power %d (%%)", i+1);
      ret = CreateFloatProperty(p_name, 0.0, false, pActLaserPower);
      ret = SetPropertyLimits(p_name, 0.0, 100.0);

      // Laser on/off
      sprintf(p_name, "Enable Laser %d", i+1);
      ret = CreateStringProperty(p_name, OFF, false, pActLaserOnOff);
      ret = SetAllowedValues(p_name, digitalValues);
   }

   if (ret != DEVICE_OK) {
      return ret;
   }

   initialized_ = true;
   return DEVICE_OK;
}

int LaserDiodeDriver::Shutdown()
{
   if (initialized_ == false)
	{
		return DEVICE_OK;
	}

   initialized_ = false;
   return DEVICE_OK;
}

int LaserDiodeDriver::OnBoardType(MM::PropertyBase* pProp, MM::ActionType eAct) {
	if (eAct == MM::AfterSet)
	{
		pProp->Get(boardType_);
	}
   else if (eAct == MM::BeforeGet)
   {
      pProp->Set(boardType_.c_str());
   }
	return DEVICE_OK;
}

int LaserDiodeDriver::OnPort(MM::PropertyBase* pProp, MM::ActionType eAct) {
	return DEVICE_OK;
}

int LaserDiodeDriver::OnLaserMinPower(MM::PropertyBase* pProp, MM::ActionType eAct) {   
   if (eAct == MM::AfterSet) {
      double value;
      std::string pName = pProp->GetName();
      pProp->Get(value);

      int idx = -1;

      sscanf(pName.c_str(), "Min. Laser Power %d (%%)", &idx);

      double max_value = GetLaserMaxPower(idx);

      if (value > max_value) {
         value = max_value;
      }
   }

   return DEVICE_OK;
}

int LaserDiodeDriver::OnLaserMaxPower(MM::PropertyBase* pProp, MM::ActionType eAct) {   
   if (eAct == MM::AfterSet) {
      double value;
      std::string pName = pProp->GetName();
      pProp->Get(value);

      int idx = -1;

      sscanf(pName.c_str(), "Max. Laser Power %d (%%)", &idx);

      double min_value = GetLaserMinPower(idx);

      if (value < min_value) {
         value = min_value;
      }

      pProp->Set(value);
   }

   return DEVICE_OK;
}

int LaserDiodeDriver::OnLaserOnOff(MM::PropertyBase* pProp, MM::ActionType eAct) {   
   if (eAct == MM::AfterSet) {
      std::string value;
      std::string pName = pProp->GetName();
      pProp->Get(value);

      int ret;
      int idx = -1;

      sscanf(pName.c_str(), "Enable Laser %d", &idx);

      if (value == ON) {
         ret = SetLaserOnOff(idx-1, true);
      } else {
         ret = SetLaserOnOff(idx-1, false);
      }

      if (ret != DEVICE_OK) {
         // error occured; revert values and return
         if (value == ON) {
            pProp->Set(OFF);
         } else {
            pProp->Set(ON);
         }
         return ret;
      }
   }
   return DEVICE_OK;
}

int LaserDiodeDriver::OnLaserPower(MM::PropertyBase* pProp, MM::ActionType eAct) {
   if (eAct == MM::AfterSet) {
      double value;
      std::string pName = pProp->GetName();

      int ret;
      int idx = -1;
      sscanf(pName.c_str(), "Laser Power %d", &idx);

      ret = SetLaserPower(idx-1, value);

      if (ret != DEVICE_OK) {
         // error occure
         // TODO: Maybe this should be handled in some way?
         return ret;
      }
   }

   return DEVICE_OK;
}

double LaserDiodeDriver::GetLaserMaxPower(int idx) {
   double value;
   char p_name[64];
   sprintf(p_name, "Max. Laser Power %d (%%)", idx);
   GetProperty(p_name, value);
   return value;
}

double LaserDiodeDriver::GetLaserMinPower(int idx) {
   double value;
   char p_name[64];
   sprintf(p_name, "Min. Laser Power %d (%%)", idx);
   GetProperty(p_name, value);
   return value;
}

int LaserDiodeDriver::SetLaserOnOff(int idx, bool enabled) {
   if (!interface_->DeviceIsOpen()) {
      LogMessage("No open device!");
      return DEVICE_ERR;
   } else {
      LogMessage("Device is open; attempting to write.");
   }
   int ret = interface_->WriteDigital(idx, enabled);
   if (ret != DEVICE_OK) { // error
      return ret;
   }
   return DEVICE_OK;
}

int LaserDiodeDriver::SetLaserPower(int idx, double power) {
   double min_value = GetLaserMinPower(idx);
   double max_value = GetLaserMaxPower(idx);

   double relative_value = min_value + power / (max_value - min_value);
   if (relative_value > 1.0) {
      relative_value = 1.0;
   } else if (relative_value < 0.0) {
      relative_value = 0.0;
   }

   int ret = interface_->WriteAnalogRelative(idx, relative_value);
   if (ret == 1) { // error
      // Debug
      LogMessage("Could not set analog value!", false);
      return DEVICE_ERR;
   }
   return DEVICE_OK;
}
