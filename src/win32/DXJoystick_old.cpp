
#pragma warning( disable : 4786 ) //disable "identifier name too long" warning

#include "DXJoystick.h"

// constants used for scaling the input device
#define DEADZONE        300            // 3% of the axis range
#define RANGE_MAX       10000            // maximum positive axis value
#define RANGE_MIN       -10000           // minimum negative axis value

BOOL CALLBACK iEnumDeviceProc(const DIDEVICEINSTANCE *pdidi, LPVOID context);

struct S_PARAM
{
	string   JoysticProductName;
	GUID     guidDevice;
};


/**************** DXJoystick ******************/

bool DXJoystick::IsValid()
{
	return valid;
}

const char *DXJoystick::GetDescription()
{
	if (!IsValid())
		return "no joystick available!";
	return JoysticProductName.c_str();
}

int DXJoystick::GetNumAxis()
{
	return 8; //I don't poll this yet
}

int DXJoystick::GetNumButtons()
{
	return 32; //I don't poll this yet
}

REAL DXJoystick::GetAxisPos(int i)
{
	//assert(i>=0 && i<8);
	UpdateState();
	return axis[i];
}

bool DXJoystick::GetButtonPos(int i)
{
	//assert(i>=0 && i<32);
	UpdateState();
	return button[i];
}

void DXJoystick::SetForceFeedback(int i, REAL amplitude)
{
	if (!IsValid() || !ForceFeedback)
		return;


}

DXJoystick::DXJoystick(HINSTANCE hInst, HWND hWnd) :
	valid(false),
	ForceFeedback(false),
	gpdi(NULL),
	gpdiJoystick(NULL),
	gpdiEffect(NULL)
{
    HRESULT             hRes;
    LPDIRECTINPUTDEVICE pDevice;
    LPDIRECTINPUTDEVICE2 pDevice2 = NULL;
    DIDEVCAPS           didc;
	
    // create the base DirectInput object
    hRes = DirectInputCreate(hInst, DIRECTINPUT_VERSION, &gpdi, NULL);
    if(FAILED(hRes))
    {
        cout << "Joystick support requires DirectX 5 or later.\n";
        return;
    }
	
    // enumerate for joystick devices
    S_PARAM s_param;
    hRes = gpdi->EnumDevices(
		DIDEVTYPE_JOYSTICK,
		iEnumDeviceProc,
		&s_param,
		DIEDFL_ATTACHEDONLY
	);
	
    if(FAILED(hRes))
    {
        cout << "EnumDevices() failed, joystick unavailable\n";
        return;
    }
	
    JoysticProductName = s_param.JoysticProductName;

    // create a temporary "Device 1" object
    hRes = gpdi->CreateDevice(s_param.guidDevice, &pDevice, NULL);
    if(FAILED(hRes))
    {
        cout << "This option of ""CarWorld"" requires a joystick device.\n";
        return;
    }
	
    // get a "Device 2" object
    // this is needed for access to the ForceFeedback functionality
    hRes = pDevice->QueryInterface(IID_IDirectInputDevice2, (void **)&pDevice2);
	pDevice->Release();
    if(FAILED(hRes))
    {
        cout << "QueryInterface() failed, joystick unavailable\n";
        return;
    }

	gpdiJoystick = pDevice2;
	
    // set the device's data format
    //
    // This tells the device object to act like a specific device --
    // in our case, like a joystick
    hRes = gpdiJoystick->SetDataFormat(&c_dfDIJoystick);
    if(FAILED(hRes))
    {
		cout << "SetDataFormat() failed, joystick unavailable\n";
        return;
    }
	
    // set the device's cooperative level
    //
    // ForceFeedback requires Exclusive access to the device.
    hRes = gpdiJoystick->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	//hRes = gpdiJoystick->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE);
    if(FAILED(hRes))
    {
		cout << "SetCooperativeLevel() failed, joystick unavailable\n";
        return;
    }
	
    // get the device capabilities
    //
    // We're going to check to see if the device we created supports
    // ForceFeedback.  If so, we will create effects, if not, we'll
    // support standard joystick functionality
    didc.dwSize = sizeof(DIDEVCAPS);
    hRes = gpdiJoystick->GetCapabilities(&didc);
    if (FAILED(hRes))
    {
        cout << "GetCapabilities() failed, joystick unavailable\n";
        return;
    }

	if (didc.dwFlags & DIDC_FORCEFEEDBACK)
	{
		if (!inputInitForceFeedback())
		{
			cout << "Force feedback initalisation failed, joystick unavailable\n";
			return;
		}
		ForceFeedback = true;
	}

    // set joystick parameters (deadzone, etc)
    if(!inputPrepareDevice())
    {
        cout << "Device preparation failed, joystick unavailable\n";
        return;
    }

    // if we get here, we succeeded
    valid = true;
}

void DXJoystick::UpdateState()
{
	if (!valid) return;
	
	
    HRESULT     hRes;
    DIJOYSTATE  dijs;
    DWORD       dwInput = 0;
	gpdiJoystick->Acquire();
    // poll the joystick to read the current state
    hRes = gpdiJoystick->Poll();
	
    // read the device state
    hRes = gpdiJoystick->GetDeviceState(sizeof(DIJOYSTATE), &dijs);
	
    if(FAILED(hRes))
    {
        if((hRes == DIERR_INPUTLOST))
            inputAcquireDevices();
        // we did not read anything, return no motion
        return;
    }
	
	axis[0] = ((REAL)-dijs.lX)/RANGE_MAX;
	axis[1] = ((REAL)-dijs.lY)/RANGE_MAX;
	axis[2] = ((REAL)-dijs.lZ)/RANGE_MAX;
	axis[3] = ((REAL)-dijs.lRx)/RANGE_MAX;
	axis[4] = ((REAL)-dijs.lRy)/RANGE_MAX;
	axis[5] = ((REAL)-dijs.lRz)/RANGE_MAX;
	axis[6] = ((REAL)-dijs.rglSlider[0])/RANGE_MAX;
	axis[7] = ((REAL)-dijs.rglSlider[1])/RANGE_MAX;
	
	for (int i=0 ; i<32 ; i++)
		button[i] = (dijs.rgbButtons[i] & 0x80) != 0;
}


DXJoystick::~DXJoystick()
{
	if (!valid) return;
	
	
    OutputDebugString("Cleaning up after DirectInput\n");
	
    // Unacquire() and Release() device objects
    //
    // It is perfectly safe to call Unacquire() on a device that is not
    // currently acquired.  In fact, it is good practice to call
    // Unacquire() just prior to Release().
    if(gpdiJoystick)
    {
        gpdiJoystick->Unacquire();
        gpdiJoystick->Release();
        gpdiJoystick = NULL;
    }
	
    // Release() base object
    if(gpdi)
    {
        gpdi->Release();
        gpdi = NULL;
    }
}


/********************  direct input methods *********************/
/****this code was taken and modified from the DirectX 6.0 SDK***/
/********************by Marcus Hewat 1999************************/


//===========================================================================
// inputInitDirectInput
// Creates and initializes DirectInput objects
// Returns: true if succeeded
//===========================================================================



// Enumerates DirectInput devices of type specified in call to
//  IDirectInput::EnumDevices()

BOOL CALLBACK iEnumDeviceProc(const DIDEVICEINSTANCE *pdidi, LPVOID context)
{
	S_PARAM *s_param = (S_PARAM *)context;
	s_param->JoysticProductName = pdidi->tszProductName;
	
    // report back the instance guid of the device we enumerated
    s_param->guidDevice = pdidi->guidInstance;
	
    // BUGBUG for now, stop after the first device has been found
    return DIENUM_STOP;
	
} //*** end inputEnumDeviceProc()



// Acquires the input device(s).
bool DXJoystick::inputAcquireDevices()
{
    if(!gpdiJoystick)
    {
        return false;
    }
	
    // reacquire the device
    if(SUCCEEDED(gpdiJoystick->Acquire()))
    {
        // DirectInput automatically resets the device whenever
        // ownership changes, so we can assume we've got a device
        // unsullied by its previous owner.
        //inputCreateEffect(EF_BOUNCE | EF_FIRE | EF_EXPLODE);
		
        return true;
    }
    // if we get here, we did >not< acquire the device
    return false;
	
} //*** end inputAcquireDevices()

bool DXJoystick::inputInitForceFeedback()
{
	HRESULT hRes;
    // Since we will be playing force feedback effects,
    // we should disable the auto-centering spring.
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = FALSE;

	hRes = gpdiJoystick->SetProperty( DIPROP_AUTOCENTER, &dipdw.diph );
    if ( FAILED(hRes) ) 
        return false;

    // This application needs only one effect:  Applying raw forces.
    DIEFFECT eff;
    DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
    LONG rglDirection[2] = { 0, 0 };
    DICONSTANTFORCE cf = { 0 };

    eff.dwSize = sizeof(DIEFFECT);
    eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    eff.dwDuration = INFINITE;
    eff.dwSamplePeriod = 0;
    eff.dwGain = DI_FFNOMINALMAX;
    eff.dwTriggerButton = DIEB_NOTRIGGER;
    eff.dwTriggerRepeatInterval = 0;
    eff.cAxes = 2;
    eff.rgdwAxes = rgdwAxes;
    eff.rglDirection = rglDirection;
    eff.lpEnvelope = 0;
    eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    eff.lpvTypeSpecificParams = &cf;

    // create the prepared effect
    hRes = gpdiJoystick->CreateEffect( GUID_ConstantForce,
                                    &eff,
                                    &gpdiEffect,
                                    NULL );
    if ( FAILED(hRes) ) 
        return false;
	return true;

}


// Performs device preparation by setting the device's parameters (ie
// deadzone).
bool DXJoystick::inputPrepareDevice()
{
    HRESULT       hRes;
    DIPROPRANGE   dipr;
    DIPROPDWORD   dipdw;
	
	
    // quick check to make sure that the object pointer is non-NULL
    if(!gpdiJoystick)
        return false;
	
    // call Unacquire() on the device
    //
    // SetParameter() will fail if a device is currently acquired, we are
    // doing this here in case we get careless and forget to call this
    // function either before we call Acquire() or after we call Unacquire().
    gpdiJoystick->Unacquire();
	
    // set the axis ranges for the device
    //
    // We will use the same range for the X and Y axes.  We are setting them
    // fairly low since we are not concerned with anything other than
    // "left", "right", "forward", "backward" and "centered"
	//* prepare DIPROPRANGE structure
    dipr.diph.dwSize        = sizeof(DIPROPRANGE);
	dipr.diph.dwHeaderSize  = sizeof(dipr.diph);
	dipr.diph.dwHow         = DIPH_BYOFFSET;
	dipr.lMin               = RANGE_MIN;  // negative to the left/top
	dipr.lMax               = RANGE_MAX;  // positive to the right/bottom
    //* x-axis
    dipr.diph.dwObj         = DIJOFS_X;
    //* set the x-axis range property
    hRes = gpdiJoystick->SetProperty(DIPROP_RANGE, &dipr.diph);
    if(FAILED(hRes))
    {
        OutputDebugString("SetProperty(RANGE, X-Axis) failed.\n");
        return false;
    }
    //* y-axis
    dipr.diph.dwObj         = DIJOFS_Y;
    hRes = gpdiJoystick->SetProperty(DIPROP_RANGE, &dipr.diph);
    if(FAILED(hRes))
    {
        OutputDebugString("SetProperty(RANGE, Y-Axis) failed.\n");
        return false;
    }
	
    // set the deadzone for the device
    //
    // We will use the same deadzone percentage for the X and Y axes.
    // This call uses a symbolic constant for the deadzone percentage so that
    // it is easy to change if we decide we don't like it.
	//* prepare DIPROPDWORD structure
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwHow        = DIPH_BYOFFSET;
	dipdw.dwData            = DEADZONE;
    //* set the x-axis range property
    dipdw.diph.dwObj         = DIJOFS_X;
    hRes = gpdiJoystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
    if(FAILED(hRes))
    {
        OutputDebugString("SetProperty(DEADZONE, X-Axis) failed.\n");
        return false;
    }
    //* y-axis
    dipdw.diph.dwObj         = DIJOFS_Y;
    hRes = gpdiJoystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
    if(FAILED(hRes))
    {
        OutputDebugString("SetProperty(DEADZONE, Y-Axis) failed.\n");
        return false;
    }
	
    // Acquire the device(s)
    //
    // This is being done as a convenience since we unacquired earlier in
    // this function.  This does not guarantee that the device will be
    // acquired at the time we return from the function (in other words, we
    // are not going to spin here until we get a succeessful acquisition).
    inputAcquireDevices();
	
    // we've actually done somthing here
    return true;
	
} //** end inputPrepareDevice()
