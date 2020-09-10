///////////////////////////////////////////////////////////////////////////////
// FILE:          LaserDiodeDriver.cpp
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

#include "LaserDiodeDriver.h"

#include <iostream>
#include <sstream>

#include "K8061.h"
#include "DummyBoard.h"

const char* g_BoardK8061 = "K8061";
const char* g_BoardDummy = "Dummy";

const char* const g_Msg_DEVICE_INVALID_BOARD_TYPE = "This board type is not supported. Supported values are: 'K8061' and 'Dummy'.";

const char* g_LaserDiodeDriverName = "LaserDiodeDriver";

const char* ON = "On";
const char* OFF = "Off";

#define DEVICE_INVALID_BOARD_TYPE 142

///////////////////////////////////////////////////////////////////////////////
// Exported MMDevice API
///////////////////////////////////////////////////////////////////////////////

/**
 * List all supported hardware devices here
 */
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

///////////////////////////////////////////////////////////////////////////////
// MMCamera implementation
// ~~~~~~~~~~~~~~~~~~~~~~~

/**
* MMCamera constructor.
* Setup default all variables and create device properties required to exist
* before intialization. In this case, no such properties were required. All
* properties will be created in the Initialize() method.
*
* As a general guideline Micro-Manager devices do not access hardware in the
* the constructor. We should do as little as possible in the constructor and
* perform most of the initialization in the Initialize() method.
*/
LaserDiodeDriver::LaserDiodeDriver() {
   // call the base class method to set-up default error codes/messages
   InitializeDefaultErrorMessages();
   SetErrorText(DEVICE_INVALID_BOARD_TYPE, g_Msg_DEVICE_INVALID_BOARD_TYPE);

   int ret;
   numberOfLasers_ = 1;
   ret = CreateStringProperty("Board Type", g_BoardK8061, false, NULL, true);
   ret = CreateIntegerProperty("Number of Lasers", 1, false, NULL, true);
   ret = CreateStringProperty("Device Directory (K8061 only)", "/dev/comedi0", false, NULL, true);
}

/**
* MMCamera destructor.
* If this device used as intended within the Micro-Manager system,
* Shutdown() will be always called before the destructor. But in any case
* we need to make sure that all resources are properly released even if
* Shutdown() was not called.
*/
LaserDiodeDriver::~LaserDiodeDriver()
{
   if (initialized_)
      Shutdown();
}

/**
* Obtains device name.
* Required by the MM::Device API.
*/
void LaserDiodeDriver::GetName(char* name) const
{
   // We just return the name we use for referring to this
   // device adapter.
   CDeviceUtils::CopyLimitedString(name, g_LaserDiodeDriverName);
}

/**
* Intializes the hardware.
* Typically we access and initialize hardware at this point.
* Device properties are typically created here as well.
* Required by the MM::Device API.
*/
int LaserDiodeDriver::Initialize()
{
   long value;
   GetProperty("Number of Lasers", value);
   numberOfLasers_ = (int)value; // :(

   //char *dir;
   //GetProperty("Device Directory", dir);
   
   int ret = DEVICE_OK;

   char boardType[MM::MaxStrLength];
   GetProperty("Board Type", boardType);

   if (strcmp(boardType, g_BoardK8061) == 0) {
      char dir[MM::MaxStrLength];
      GetProperty("Device Directory (K8061 only)", dir);
   
      std::string deviceDir = std::string(dir);

      interface_ = new K8061(deviceDir);
   } else if (strcmp(boardType, g_BoardDummy) == 0) {
      interface_ = new DummyBoard();
   } else {
      ret = DEVICE_INVALID_BOARD_TYPE;
      return ret;
   }
   
   interface_->Open();
   if (!interface_->DeviceIsOpen()) {
      LogMessage("The device could not be opened!");
      ret = DEVICE_NOT_CONNECTED;
   }
   
   if (ret != DEVICE_OK) {
      return ret;
   }

   CPropertyAction* pActLaserPower = new CPropertyAction (this, &LaserDiodeDriver::OnLaserPower);
   CPropertyAction* pActLaserOnOff = new CPropertyAction (this, &LaserDiodeDriver::OnLaserOnOff);
   CPropertyAction* pActLaserMinPower = new CPropertyAction (this, &LaserDiodeDriver::OnLaserMinPower);
   CPropertyAction* pActLaserMaxPower = new CPropertyAction (this, &LaserDiodeDriver::OnLaserMaxPower);

   std::vector<std::string> digitalValues;
   digitalValues.push_back(OFF);
   digitalValues.push_back(ON);

   for (int i = 0; i < numberOfLasers_; ++i) {
      char p_name[64];

      // Laser power
      sprintf(p_name, "Laser Power %d (%%)", i);
      ret = CreateFloatProperty(p_name, 0.0, false, pActLaserPower);
      ret = SetPropertyLimits(p_name, 0.0, 100.0);

      // Laser on/off
      sprintf(p_name, "Enable Laser %d", i);
      ret = CreateStringProperty(p_name, OFF, false, pActLaserOnOff);
      ret = SetAllowedValues(p_name, digitalValues);

      // Laser limits
      sprintf(p_name, "Min. Laser Power %d (%%)", i);
      ret = CreateFloatProperty(p_name, 0.0, false, pActLaserMinPower);
      ret = SetPropertyLimits(p_name, 0.0, 100.0);

      sprintf(p_name, "Max. Laser Power %d (%%)", i);
      ret = CreateFloatProperty(p_name, 100.0, false, pActLaserMaxPower);
      ret = SetPropertyLimits(p_name, 0.0, 100.0);
   }

   if (ret != DEVICE_OK) {
      return ret;
   }

   initialized_ = true;
   return DEVICE_OK;
}

/**
* Shuts down (unloads) the device.
* Ideally this method will completely unload the device and release all resources.
* Shutdown() may be called multiple times in a row.
* Required by the MM::Device API.
*/
int LaserDiodeDriver::Shutdown()
{
   initialized_ = false;
   delete interface_;
   return DEVICE_OK;
}

int LaserDiodeDriver::OnLaserMinPower(MM::PropertyBase* pProp, MM::ActionType eAct) {   
   if (eAct == MM::AfterSet) {
      double value;
      std::string pName = pProp->GetName();
      pProp->Get(value);

      int ret;
      int idx = -1;

      sscanf(pName.c_str(), "Min. Laser Power %d (%%)", &idx);

      double max_value = GetLaserMaxPower(idx);

      if (value > max_value) {
         value = max_value;
      }

      pProp->Set(value);

      char p_name_power[64];
      sprintf(p_name_power, "Laser Power %d (%%)", idx);
      double power;
      GetProperty(p_name_power, power);
      SetProperty(p_name_power, std::to_string(power).c_str());
   }

   return DEVICE_OK;
}

int LaserDiodeDriver::OnLaserMaxPower(MM::PropertyBase* pProp, MM::ActionType eAct) {   
   if (eAct == MM::AfterSet) {
      double value;
      std::string pName = pProp->GetName();
      pProp->Get(value);

      int ret;
      int idx = -1;

      sscanf(pName.c_str(), "Max. Laser Power %d (%%)", &idx);

      double min_value = GetLaserMinPower(idx);

      if (value < min_value) {
         value = min_value;
      }

      pProp->Set(value);

      char p_name_power[64];
      sprintf(p_name_power, "Laser Power %d (%%)", idx);
      double power;
      GetProperty(p_name_power, power);
      SetProperty(p_name_power, std::to_string(power).c_str());
   }

   return DEVICE_OK;
}

int LaserDiodeDriver::OnLaserOnOff(MM::PropertyBase* pProp, MM::ActionType eAct) {   
   // TODO: Check MM::BeforeSet to check whether values are as expected.
   if (eAct == MM::AfterSet) {
      std::string value;
      std::string pName = pProp->GetName();
      pProp->Get(value);

      int ret;
      int idx = -1;

      sscanf(pName.c_str(), "Enable Laser %d", &idx);

      if (value == ON) {
         ret = SetLaserOnOff(idx, true);
      } else {
         ret = SetLaserOnOff(idx, false);
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
      pProp->Get(value);

      int ret;
      int idx = -1;

      sscanf(pName.c_str(), "Laser Power %d (%%)", &idx);

      double min_value = GetLaserMinPower(idx);
      double max_value = GetLaserMaxPower(idx);

      if (value < min_value) {
         value = min_value;
      }

      if (value > max_value) {
         value = max_value;
      }

      pProp->Set(value);

      ret = SetLaserPower(idx, value);

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

int LaserDiodeDriver::SetLaserOnOff(int idx, bool enabled) const {
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

int LaserDiodeDriver::SetLaserPower(int idx, double power) const {
   double relative_value = power / 100.0;
   if (relative_value > 1.0) {
      relative_value = 1.0;
   }
   int ret = interface_->WriteAnalogRelative(idx, relative_value);
   if (ret == 1) { // error
      // Debug
      LogMessage("Could not set analog value!", false);
      return DEVICE_ERR;
   }
   return DEVICE_OK;
}