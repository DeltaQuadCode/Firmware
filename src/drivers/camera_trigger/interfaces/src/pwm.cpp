#ifdef __PX4_NUTTX

#include <sys/ioctl.h>
#include <lib/mathlib/mathlib.h>
#include <parameters/param.h>


#include "drivers/drv_pwm_trigger.h"
#include "pwm.h"


CameraInterfacePWM::CameraInterfacePWM():
	CameraInterface()
{
	param_get(param_find("TRIG_PWM_SHOOT"), &_pwm_camera_shoot);
	param_get(param_find("TRIG_PWM_NEUTRAL"), &_pwm_camera_neutral);
        param_get(param_find("TRIG_PWM_SHOOT2"), &_pwm_camera_shoot2);
        param_get(param_find("TRIG_PWM_NEUT2"), &_pwm_camera_neutral2);
        get_pins();
	setup();
}

CameraInterfacePWM::~CameraInterfacePWM()
{
	// Deinitialise trigger channels
        up_pwm_trigger_deinit();
}

void CameraInterfacePWM::setup()
{
	// Precompute the bitmask for enabled channels
	uint8_t pin_bitmask = 0;

	for (unsigned i = 0; i < arraySize(_pins); i++) {
        //if (_pins[i] >= 0) {
			pin_bitmask |= (1 << _pins[i]);
        //}
	}

	// Initialize and arm channels
        up_pwm_trigger_init(pin_bitmask);

	// Set neutral pulsewidths
	for (unsigned i = 0; i < arraySize(_pins); i++) {
		if (_pins[i] >= 0) {
                    if(i == 1) {
                        up_pwm_trigger_set(_pins[i], math::constrain(_pwm_camera_neutral2, 0, 2100));
                    } else {
                        up_pwm_trigger_set(_pins[i], math::constrain(_pwm_camera_neutral, 0, 2100));
                    }
		}
	}

}

void CameraInterfacePWM::trigger(bool trigger_on_true)
{
        for (unsigned i = 0; i < arraySize(_pins); i++) {
		if (_pins[i] >= 0) {
			// Set all valid pins to shoot or neutral levels
                        if(i == 1) {
                            up_pwm_trigger_set(_pins[i], math::constrain(trigger_on_true ? _pwm_camera_shoot2 : _pwm_camera_neutral2, 0, 2100));
                        } else {
                            up_pwm_trigger_set(_pins[i], math::constrain(trigger_on_true ? _pwm_camera_shoot : _pwm_camera_neutral, 0, 2100));
                        }
		}
	}
}

void CameraInterfacePWM::setservo(int servo, int pwm)
{
    printf("Setting servo %i to %i\r\n", servo, pwm);
    up_pwm_trigger_set(servo, math::constrain(pwm, 0, 2100));
}

void CameraInterfacePWM::info()
{
    param_get(param_find("TRIG_PWM_SHOOT"), &_pwm_camera_shoot);
    param_get(param_find("TRIG_PWM_NEUTRAL"), &_pwm_camera_neutral);
    param_get(param_find("TRIG_PWM_SHOOT2"), &_pwm_camera_shoot2);
    param_get(param_find("TRIG_PWM_NEUT2"), &_pwm_camera_neutral2);

    PX4_INFO("PWM trigger mode (generic), pins enabled : [%d][%d][%d][%d][%d][%d]",
		 _pins[5], _pins[4], _pins[3], _pins[2], _pins[1], _pins[0]);
}

#endif /* ifdef __PX4_NUTTX */
