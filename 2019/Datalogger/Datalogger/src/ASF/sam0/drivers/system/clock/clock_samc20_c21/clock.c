/**
 * \file
 *
 * \brief SAM C2x Clock Driver
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
#include <clock.h>
#include <conf_clocks.h>
#include <system.h>

/**
 * \internal
 * \brief DPLL-specific data container.
 */
struct _system_clock_dpll_config {
	uint32_t frequency;
};


/**
 * \internal
 * \brief XOSC-specific data container.
 */
struct _system_clock_xosc_config {
	uint32_t frequency;
};

/**
 * \internal
 * \brief System clock module data container.
 */
struct _system_clock_module {
	volatile struct _system_clock_dpll_config dpll;

	volatile struct _system_clock_xosc_config xosc;
	volatile struct _system_clock_xosc_config xosc32k;
};

/**
 * \internal
 * \brief Internal module instance to cache configuration values.
 */
static struct _system_clock_module _system_clock_inst = {
		.dpll = {
			.frequency   = 0,
		},
		.xosc = {
			.frequency   = 0,
		},
		.xosc32k = {
			.frequency   = 0,
		},
	};

/**
 * \internal
 * \brief Wait for sync to the OSC32K control registers.
 */
static inline void _system_osc32k_wait_for_sync(void)
{
	while (!(OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_OSC32KRDY)) {
		/* Wait for OSC32K sync */
	}
}

/**
 * \brief Retrieve the frequency of a clock source.
 *
 * Determines the current operating frequency of a given clock source.
 *
 * \param[in] clock_source  Clock source to get the frequency of
 *
 * \returns Frequency of the given clock source, in Hz.
 */
uint32_t system_clock_source_get_hz(
		const enum system_clock_source clock_source)
{
	switch (clock_source) {
	case SYSTEM_CLOCK_SOURCE_XOSC:
		return _system_clock_inst.xosc.frequency;

	case SYSTEM_CLOCK_SOURCE_OSC48M:
		return 48000000UL / (OSCCTRL->OSC48MDIV.bit.DIV + 1);

	case SYSTEM_CLOCK_SOURCE_OSC32K:
		return 32768UL;

	case SYSTEM_CLOCK_SOURCE_ULP32K:
		return 32768UL;

	case SYSTEM_CLOCK_SOURCE_XOSC32K:
		return _system_clock_inst.xosc32k.frequency;

	case SYSTEM_CLOCK_SOURCE_DPLL:
		if (!(OSCCTRL->DPLLCTRLA.reg & OSCCTRL_DPLLCTRLA_ENABLE)) {
			return 0;
		}

		return _system_clock_inst.dpll.frequency;

	default:
		return 0;
	}
}

/**
 * \brief Configure the internal OSC48M oscillator clock source.
 *
 * Configures the 48MHz (nominal) internal RC oscillator with the given
 * configuration settings.
 *
 * \note Frequency selection can be done only when OSC48M is disabled.
 *
 * \param[in] config  OSC48M configuration structure containing the new config
 */
void system_clock_source_osc48m_set_config(
		struct system_clock_source_osc48m_config *const config)
{
	Assert(config);

	OSCCTRL_OSC48MCTRL_Type temp = OSCCTRL->OSC48MCTRL;

	/* Use temporary struct to reduce register access */
	temp.bit.ONDEMAND = config->on_demand;
	temp.bit.RUNSTDBY = config->run_in_standby;

	OSCCTRL->OSC48MCTRL = temp;

	if (config->div != OSCCTRL->OSC48MDIV.reg) {
		OSCCTRL->OSC48MDIV.reg = OSCCTRL_OSC48MDIV_DIV(config->div);
		while(OSCCTRL->OSC48MSYNCBUSY.reg) ;
	}

	OSCCTRL->OSC48MSTUP.reg = OSCCTRL_OSC48MSTUP_STARTUP(config->startup_time);
}

/**
 * \brief Configure the internal OSC32K oscillator clock source.
 *
 * Configures the 32KHz (nominal) internal RC oscillator with the given
 * configuration settings.
 *
 * \param[in] config  OSC32K configuration structure containing the new config
 */
void system_clock_source_osc32k_set_config(
		struct system_clock_source_osc32k_config *const config)
{
	OSC32KCTRL_OSC32K_Type temp = OSC32KCTRL->OSC32K;


	/* Update settings via a temporary struct to reduce register access */
	temp.bit.EN1K     = config->enable_1khz_output;
	temp.bit.EN32K    = config->enable_32khz_output;
	temp.bit.STARTUP  = config->startup_time;
	temp.bit.ONDEMAND = config->on_demand;
	temp.bit.RUNSTDBY = config->run_in_standby;
	temp.bit.WRTLOCK  = config->write_once;

	OSC32KCTRL->OSC32K  = temp;
}

/**
 * \brief Configure the internal OSCULP32K oscillator clock source.
 *
 * Configures the Ultra Low Power 32KHz internal RC oscillator with the given
 * configuration settings.
 *
 * \param[in] config  OSCULP32K configuration structure containing the new config
 */
void system_clock_source_osculp32k_set_config(
		struct system_clock_source_osculp32k_config *const config)
{
	OSC32KCTRL_OSCULP32K_Type temp = OSC32KCTRL->OSCULP32K;
	/* Update settings via a temporary struct to reduce register access */
	temp.bit.WRTLOCK  = config->write_once;
	OSC32KCTRL->OSCULP32K  = temp;
}

/**
 * \brief Configure the external oscillator clock source.
 *
 * Configures the external oscillator clock source with the given configuration
 * settings.
 *
 * \param[in] config  External oscillator configuration structure containing
 *                    the new config
 */
void system_clock_source_xosc_set_config(
		struct system_clock_source_xosc_config *const config)
{
	OSCCTRL_XOSCCTRL_Type temp = OSCCTRL->XOSCCTRL;

	temp.bit.STARTUP = config->startup_time;

	if (config->external_clock == SYSTEM_CLOCK_EXTERNAL_CRYSTAL) {
		temp.bit.XTALEN = 1;
	} else {
		temp.bit.XTALEN = 0;
	}

	temp.bit.AMPGC = config->auto_gain_control;

	/* Set gain */
	if (config->frequency <= 2000000) {
		temp.bit.GAIN = 0;
	} else if (config->frequency <= 4000000) {
		temp.bit.GAIN = 1;
	} else if (config->frequency <= 8000000) {
		temp.bit.GAIN = 2;
	} else if (config->frequency <= 16000000) {
		temp.bit.GAIN = 3;
	} else if (config->frequency <= 32000000) {
		temp.bit.GAIN = 4;
	}

	temp.bit.ONDEMAND = config->on_demand;
	temp.bit.RUNSTDBY = config->run_in_standby;
	temp.bit.SWBEN    = config->enable_clock_switch_back;

	if (config->enable_clock_failure_detector) {
		Assert(OSCCTRL->OSC48MCTRL.reg & OSCCTRL_OSC48MCTRL_ENABLE);
		temp.bit.CFDEN    = config->enable_clock_failure_detector;
	}

	/* Store XOSC frequency for internal use */
	_system_clock_inst.xosc.frequency = config->frequency;

	OSCCTRL->EVCTRL.reg =
			config->enable_clock_failure_detector_event_outut << OSCCTRL_EVCTRL_CFDEO_Pos;

	OSCCTRL->CFDPRESC.reg = OSCCTRL_CFDPRESC_CFDPRESC(config->clock_failure_detector_prescaler) ;

	OSCCTRL->XOSCCTRL = temp;
}

/**
 * \brief Configure the XOSC32K external 32KHz oscillator clock source.
 *
 * Configures the external 32KHz oscillator clock source with the given
 * configuration settings.
 *
 * \param[in] config  XOSC32K configuration structure containing the new config
 */
void system_clock_source_xosc32k_set_config(
		struct system_clock_source_xosc32k_config *const config)
{
	OSC32KCTRL_XOSC32K_Type temp = OSC32KCTRL->XOSC32K;

	temp.bit.STARTUP = config->startup_time;

	if (config->external_clock == SYSTEM_CLOCK_EXTERNAL_CRYSTAL) {
		temp.bit.XTALEN = 1;
	} else {
		temp.bit.XTALEN = 0;
	}

	temp.bit.EN1K = config->enable_1khz_output;
	temp.bit.EN32K = config->enable_32khz_output;

	temp.bit.ONDEMAND = config->on_demand;
	temp.bit.RUNSTDBY = config->run_in_standby;
	temp.bit.WRTLOCK  = config->write_once;

	/* Cache the new frequency in case the user needs to check the current
	 * operating frequency later */
	_system_clock_inst.xosc32k.frequency = config->frequency;

	OSC32KCTRL->CFDCTRL.reg =
		(config->clock_failure_detector_prescaler << OSC32KCTRL_CFDCTRL_CFDPRESC_Pos) |
		(config->enable_clock_failure_detector << OSC32KCTRL_CFDCTRL_CFDEN_Pos)|
		(config->enable_clock_switch_back << OSC32KCTRL_CFDCTRL_SWBACK_Pos);

	OSC32KCTRL->EVCTRL.reg =
			(config->enable_clock_failure_detector_event_outut << OSC32KCTRL_EVCTRL_CFDEO_Pos);

	OSC32KCTRL->XOSC32K = temp;
}

/**
 * \brief Configure the DPLL clock source.
 *
 * Configures the Digital Phase-Locked Loop clock source with the given
 * configuration settings.
 *
 * \note The DPLL will be running when this function returns, as the DPLL module
 *       needs to be enabled in order to perform the module configuration.
 *
 * \param[in] config  DPLL configuration structure containing the new config
 */
void system_clock_source_dpll_set_config(
		struct system_clock_source_dpll_config *const config)
{

	uint32_t tmpldr;
	uint8_t  tmpldrfrac;
	uint32_t refclk;

	refclk = config->reference_frequency;

	/* Only reference clock REF1 can be divided */
	if (config->reference_clock == SYSTEM_CLOCK_SOURCE_DPLL_REFERENCE_CLOCK_XOSC) {
		refclk = refclk / (2 * (config->reference_divider + 1));
	}

	/* Calculate LDRFRAC and LDR */
	tmpldr = (config->output_frequency << 4) / refclk;
	tmpldrfrac = tmpldr & 0x0f;
	tmpldr = (tmpldr >> 4) - 1;

	OSCCTRL->DPLLCTRLA.reg =
			((uint32_t)config->on_demand << OSCCTRL_DPLLCTRLA_ONDEMAND_Pos) |
			((uint32_t)config->run_in_standby << OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos);

	OSCCTRL->DPLLRATIO.reg =
			OSCCTRL_DPLLRATIO_LDRFRAC(tmpldrfrac) |
			OSCCTRL_DPLLRATIO_LDR(tmpldr);

	while(OSCCTRL->DPLLSYNCBUSY.reg & OSCCTRL_DPLLSYNCBUSY_DPLLRATIO){
		}

	OSCCTRL->DPLLCTRLB.reg =
			OSCCTRL_DPLLCTRLB_DIV(config->reference_divider) |
			((uint32_t)config->lock_bypass << OSCCTRL_DPLLCTRLB_LBYPASS_Pos) |
			OSCCTRL_DPLLCTRLB_LTIME(config->lock_time) |
			OSCCTRL_DPLLCTRLB_REFCLK(config->reference_clock) |
			((uint32_t)config->wake_up_fast << OSCCTRL_DPLLCTRLB_WUF_Pos) |
			((uint32_t)config->low_power_enable << OSCCTRL_DPLLCTRLB_LPEN_Pos) |
			OSCCTRL_DPLLCTRLB_FILTER(config->filter);

	OSCCTRL->DPLLPRESC.reg  = OSCCTRL_DPLLPRESC_PRESC(config->prescaler);
	while(OSCCTRL->DPLLSYNCBUSY.reg & OSCCTRL_DPLLSYNCBUSY_DPLLPRESC){
		}
	/*
	 * Fck = Fckrx * (LDR + 1 + LDRFRAC / 16) / (2^PRESC)
	 */
	_system_clock_inst.dpll.frequency =
			(refclk *
			 (((tmpldr + 1) << 4) + tmpldrfrac)
			) >> (4 + config->prescaler);
}

/**
 * \brief Writes the calibration values for a given oscillator clock source.
 *
 * Writes an oscillator calibration value to the given oscillator control
 * registers. The acceptable ranges are:
 *
 * For OSC32K:
 *  - 7 bits (max value 128)
 * For OSC48MHZ:
 *  - 8 bits (Max value 255)
 * For OSCULP:
 *  - 5 bits (Max value 32)
 *
 * \note The frequency range parameter applies only when configuring the 8MHz
 *       oscillator and will be ignored for the other oscillators.
 *
 * \param[in] clock_source       Clock source to calibrate
 * \param[in] calibration_value  Calibration value to write
 * \param[in] freq_range         Frequency range (8MHz oscillator only)
 *
 * \retval STATUS_OK               The calibration value was written
 *                                 successfully.
 * \retval STATUS_ERR_INVALID_ARG  The setting is not valid for selected clock
 *                                 source.
 */
enum status_code system_clock_source_write_calibration(
		const enum system_clock_source clock_source,
		const uint16_t calibration_value,
		const uint8_t freq_select)
{
	switch (clock_source) {
	case SYSTEM_CLOCK_SOURCE_OSC48M:
		//to enable DSU test mode and add calibration value
		return STATUS_OK;

	case SYSTEM_CLOCK_SOURCE_OSC32K:

		if (calibration_value > 128) {
			return STATUS_ERR_INVALID_ARG;
		}

		_system_osc32k_wait_for_sync();
		OSC32KCTRL->OSC32K.bit.CALIB = calibration_value;
		break;

	case SYSTEM_CLOCK_SOURCE_ULP32K:

		if (calibration_value > 32) {
			return STATUS_ERR_INVALID_ARG;
		}

		OSC32KCTRL->OSCULP32K.bit.CALIB = calibration_value;
		break;

	default:
		Assert(false);
		return STATUS_ERR_INVALID_ARG;
		break;
	}

	return STATUS_OK;
}

/**
 * \brief Enables a clock source.
 *
 * Enables a clock source which has been previously configured.
 *
 * \param[in] clock_source       Clock source to enable
 *
 * \retval STATUS_OK               Clock source was enabled successfully and
 *                                 is ready
 * \retval STATUS_ERR_INVALID_ARG  The clock source is not available on this
 *                                 device
 */
enum status_code system_clock_source_enable(
		const enum system_clock_source clock_source)
{
	switch (clock_source) {
	case SYSTEM_CLOCK_SOURCE_OSC48M:
		OSCCTRL->OSC48MCTRL.reg |= OSCCTRL_OSC48MCTRL_ENABLE;
		return STATUS_OK;

	case SYSTEM_CLOCK_SOURCE_OSC32K:
		OSC32KCTRL->OSC32K.reg |= OSC32KCTRL_OSC32K_ENABLE;
		break;

	case SYSTEM_CLOCK_SOURCE_XOSC:
		OSCCTRL->XOSCCTRL.reg |= OSCCTRL_XOSCCTRL_ENABLE;
		break;

	case SYSTEM_CLOCK_SOURCE_XOSC32K:
		OSC32KCTRL->XOSC32K.reg |= OSC32KCTRL_XOSC32K_ENABLE;
		break;
	case SYSTEM_CLOCK_SOURCE_DPLL:
		OSCCTRL->DPLLCTRLA.reg |= OSCCTRL_DPLLCTRLA_ENABLE;
		while(OSCCTRL->DPLLSYNCBUSY.reg & OSCCTRL_DPLLSYNCBUSY_ENABLE){
		}
		break;

	case SYSTEM_CLOCK_SOURCE_ULP32K:
		/* Always enabled */
		return STATUS_OK;

	default:
		Assert(false);
		return STATUS_ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

/**
 * \brief Disables a clock source.
 *
 * Disables a clock source that was previously enabled.
 *
 * \param[in] clock_source  Clock source to disable
 *
 * \retval STATUS_OK               Clock source was disabled successfully
 * \retval STATUS_ERR_INVALID_ARG  An invalid or unavailable clock source was
 *                                 given
 */
enum status_code system_clock_source_disable(
		const enum system_clock_source clock_source)
{
	switch (clock_source) {
	case SYSTEM_CLOCK_SOURCE_OSC48M:
		OSCCTRL->OSC48MCTRL.reg &= ~OSCCTRL_OSC48MCTRL_ENABLE;
		break;

	case SYSTEM_CLOCK_SOURCE_OSC32K:
		OSC32KCTRL->OSC32K.reg &= ~OSC32KCTRL_OSC32K_ENABLE;
		break;

	case SYSTEM_CLOCK_SOURCE_XOSC:
		OSCCTRL->XOSCCTRL.reg &= ~OSCCTRL_XOSCCTRL_ENABLE;
		break;

	case SYSTEM_CLOCK_SOURCE_XOSC32K:
		OSC32KCTRL->XOSC32K.reg &= ~OSC32KCTRL_XOSC32K_ENABLE;
		break;

	case SYSTEM_CLOCK_SOURCE_DPLL:
		OSCCTRL->DPLLCTRLA.reg &= ~OSCCTRL_DPLLCTRLA_ENABLE;
		break;
	case SYSTEM_CLOCK_SOURCE_ULP32K:
		/* Not possible to disable */

	default:
		Assert(false);
		return STATUS_ERR_INVALID_ARG;

	}

	return STATUS_OK;
}

/**
 * \brief Checks if a clock source is ready.
 *
 * Checks if a given clock source is ready to be used.
 *
 * \param[in] clock_source  Clock source to check if ready
 *
 * \returns Ready state of the given clock source.
 *
 * \retval true   Clock source is enabled and ready
 * \retval false  Clock source is disabled or not yet ready
 */
bool system_clock_source_is_ready(
		const enum system_clock_source clock_source)
{
	uint32_t mask = 0;

	switch (clock_source) {
	case SYSTEM_CLOCK_SOURCE_OSC48M:
		mask = OSCCTRL_STATUS_OSC48MRDY;
		return ((OSCCTRL->STATUS.reg & mask) == mask);

	case SYSTEM_CLOCK_SOURCE_OSC32K:
		mask = OSC32KCTRL_STATUS_OSC32KRDY;
		return ((OSC32KCTRL->STATUS.reg & mask) == mask);

	case SYSTEM_CLOCK_SOURCE_XOSC:
		mask = OSCCTRL_STATUS_XOSCRDY;
		return ((OSCCTRL->STATUS.reg & mask) == mask);

	case SYSTEM_CLOCK_SOURCE_XOSC32K:
		mask = OSC32KCTRL_STATUS_XOSC32KRDY;
		return ((OSC32KCTRL->STATUS.reg & mask) == mask);

	case SYSTEM_CLOCK_SOURCE_DPLL:
		return ((OSCCTRL->DPLLSTATUS.reg &
				(OSCCTRL_DPLLSTATUS_CLKRDY | OSCCTRL_DPLLSTATUS_LOCK)) ==
				(OSCCTRL_DPLLSTATUS_CLKRDY | OSCCTRL_DPLLSTATUS_LOCK));
	case SYSTEM_CLOCK_SOURCE_ULP32K:
		/* Not possible to disable */
		return true;

	default:
		return false;
	}
}

/* Include some checks for conf_clocks.h validation */
#include "clock_config_check.h"

#if !defined(__DOXYGEN__)
/** \internal
 *
 * Configures a Generic Clock Generator with the configuration from \c conf_clocks.h.
 */
#  define _CONF_CLOCK_GCLK_CONFIG(n, unused) \
	if (CONF_CLOCK_GCLK_##n##_ENABLE == true) { \
		struct system_gclk_gen_config gclk_conf;                          \
		system_gclk_gen_get_config_defaults(&gclk_conf);                  \
		gclk_conf.source_clock    = CONF_CLOCK_GCLK_##n##_CLOCK_SOURCE;   \
		gclk_conf.division_factor = CONF_CLOCK_GCLK_##n##_PRESCALER;      \
		gclk_conf.run_in_standby  = CONF_CLOCK_GCLK_##n##_RUN_IN_STANDBY; \
		gclk_conf.output_enable   = CONF_CLOCK_GCLK_##n##_OUTPUT_ENABLE;  \
		system_gclk_gen_set_config(GCLK_GENERATOR_##n, &gclk_conf);       \
		system_gclk_gen_enable(GCLK_GENERATOR_##n);                       \
	}

/** \internal
 *
 * Configures a Generic Clock Generator with the configuration from \c conf_clocks.h,
 * provided that it is not the main Generic Clock Generator channel.
 */
#  define _CONF_CLOCK_GCLK_CONFIG_NONMAIN(n, unused) \
		if (n > 0) { _CONF_CLOCK_GCLK_CONFIG(n, unused); }
#endif

/**
 * \brief Initialize clock system based on the configuration in conf_clocks.h.
 *
 * This function will apply the settings in conf_clocks.h when run from the user
 * application. All clock sources and GCLK generators are running when this function
 * returns.
 *
 * \note OSC48M is always enabled and if the user selects other clocks for GCLK generators,
 * the OSC48M default enable can be disabled after system_clock_init. Make sure the
 * clock switches successfully before disabling OSC48M.
 */
void system_clock_init(void)
{
	/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	SUPC->INTFLAG.reg = SUPC_INTFLAG_BODVDDRDY | SUPC_INTFLAG_BODVDDDET;

	system_flash_set_waitstates(CONF_CLOCK_FLASH_WAIT_STATES);

	/* XOSC */
#if CONF_CLOCK_XOSC_ENABLE == true
	struct system_clock_source_xosc_config xosc_conf;
	system_clock_source_xosc_get_config_defaults(&xosc_conf);

	xosc_conf.external_clock    = CONF_CLOCK_XOSC_EXTERNAL_CRYSTAL;
	xosc_conf.startup_time      = CONF_CLOCK_XOSC_STARTUP_TIME;
	xosc_conf.frequency         = CONF_CLOCK_XOSC_EXTERNAL_FREQUENCY;
	xosc_conf.run_in_standby    = CONF_CLOCK_XOSC_RUN_IN_STANDBY;
	xosc_conf.clock_failure_detector_prescaler = CONF_CLOCK_XOSC_FAILURE_DETECTOR_PRE;
	xosc_conf.enable_clock_failure_detector    = CONF_CLOCK_XOSC_FAILURE_DETECTOR_ENABLE;
	xosc_conf.enable_clock_failure_detector_event_outut =
		CONF_CLOCK_XOSC_FAILURE_DETECTOR_EVENT_OUTPUT_ENABLE;
	xosc_conf.enable_clock_switch_back = CONF_CLOCK_XOSC_FAILURE_SWITCH_BACK_ENABLE;

	system_clock_source_xosc_set_config(&xosc_conf);
	system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC);
	while(!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_XOSC));
	if (CONF_CLOCK_XOSC_ON_DEMAND || CONF_CLOCK_XOSC_AUTO_GAIN_CONTROL) {
		OSCCTRL->XOSCCTRL.reg |=
			(CONF_CLOCK_XOSC_ON_DEMAND << OSCCTRL_XOSCCTRL_ONDEMAND_Pos) |
			(CONF_CLOCK_XOSC_AUTO_GAIN_CONTROL << OSCCTRL_XOSCCTRL_AMPGC_Pos);
	}
#endif

	/* XOSC32K */
#if CONF_CLOCK_XOSC32K_ENABLE == true
	struct system_clock_source_xosc32k_config xosc32k_conf;
	system_clock_source_xosc32k_get_config_defaults(&xosc32k_conf);

	xosc32k_conf.frequency           = 32768UL;
	xosc32k_conf.external_clock      = CONF_CLOCK_XOSC32K_EXTERNAL_CRYSTAL;
	xosc32k_conf.startup_time        = CONF_CLOCK_XOSC32K_STARTUP_TIME;
	xosc32k_conf.enable_1khz_output  = CONF_CLOCK_XOSC32K_ENABLE_1KHZ_OUPUT;
	xosc32k_conf.enable_32khz_output = CONF_CLOCK_XOSC32K_ENABLE_32KHZ_OUTPUT;
	xosc32k_conf.on_demand           = false;
	xosc32k_conf.run_in_standby      = CONF_CLOCK_XOSC32K_RUN_IN_STANDBY;
	xosc32k_conf.clock_failure_detector_prescaler = CONF_CLOCK_XOSC32K_FAILURE_DETECTOR_PRE;
	xosc32k_conf.enable_clock_failure_detector    = CONF_CLOCK_XOSC32K_FAILURE_DETECTOR_ENABLE;
	xosc32k_conf.enable_clock_failure_detector_event_outut =
											CONF_CLOCK_XOSC32K_FAILURE_DETECTOR_EVENT_OUTPUT_ENABLE;
	xosc32k_conf.enable_clock_switch_back = CONF_CLOCK_XOSC32K_FAILURE_SWITCH_BACK_ENABLE;

	system_clock_source_xosc32k_set_config(&xosc32k_conf);
	system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC32K);
	while(!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_XOSC32K));
	if (CONF_CLOCK_XOSC32K_ON_DEMAND) {
		OSC32KCTRL->XOSC32K.bit.ONDEMAND = 1;
	}
#endif

	/* OSCK32K */
#if CONF_CLOCK_OSC32K_ENABLE == true

	//OSC32KCTRL->OSC32K.bit.CALIB = OSC32KCTRL_OSC32K_CALIB(16);
	struct system_clock_source_osc32k_config osc32k_conf;
	system_clock_source_osc32k_get_config_defaults(&osc32k_conf);

	osc32k_conf.startup_time        = CONF_CLOCK_OSC32K_STARTUP_TIME;
	osc32k_conf.enable_1khz_output  = CONF_CLOCK_OSC32K_ENABLE_1KHZ_OUTPUT;
	osc32k_conf.enable_32khz_output = CONF_CLOCK_OSC32K_ENABLE_32KHZ_OUTPUT;
	osc32k_conf.on_demand           = CONF_CLOCK_OSC32K_ON_DEMAND;
	osc32k_conf.run_in_standby      = CONF_CLOCK_OSC32K_RUN_IN_STANDBY;

	system_clock_source_osc32k_set_config(&osc32k_conf);
	system_clock_source_enable(SYSTEM_CLOCK_SOURCE_OSC32K);
#endif

	/* OSC48M */
	OSCCTRL->OSC48MCTRL.reg |= (CONF_CLOCK_OSC48M_ON_DEMAND << OSCCTRL_OSC48MCTRL_ONDEMAND_Pos)
								|(CONF_CLOCK_OSC48M_RUN_IN_STANDBY << OSCCTRL_OSC48MCTRL_RUNSTDBY_Pos);

	if (CONF_CLOCK_OSC48M_FREQ_DIV != SYSTEM_OSC48M_DIV_12){
		OSCCTRL->OSC48MDIV.reg = OSCCTRL_OSC48MDIV_DIV(CONF_CLOCK_OSC48M_FREQ_DIV);
		while(OSCCTRL->OSC48MSYNCBUSY.reg) ;
	}

	/* GCLK */
#if CONF_CLOCK_CONFIGURE_GCLK == true
	system_gclk_init();

	/* Configure all GCLK generators except for the main generator, which
	 * is configured later after all other clock systems are set up */
	MREPEAT(GCLK_GEN_NUM, _CONF_CLOCK_GCLK_CONFIG_NONMAIN, ~);
#endif

	/* DPLL */
#  if (CONF_CLOCK_DPLL_ENABLE == true)

	/* Enable DPLL reference clock */
	if (CONF_CLOCK_DPLL_REFERENCE_CLOCK == SYSTEM_CLOCK_SOURCE_DPLL_REFERENCE_CLOCK_XOSC32K) {
		/* XOSC32K should have been enabled for GCLK_XOSC32 */
		Assert(CONF_CLOCK_XOSC32K_ENABLE);
	}else if (CONF_CLOCK_DPLL_REFERENCE_CLOCK == SYSTEM_CLOCK_SOURCE_DPLL_REFERENCE_CLOCK_XOSC) {
		/* XOSC should have been enabled for GCLK_XOSC */
		Assert(CONF_CLOCK_XOSC_ENABLE);
	} else if (CONF_CLOCK_DPLL_REFERENCE_CLOCK == SYSTEM_CLOCK_SOURCE_DPLL_REFERENCE_CLOCK_GCLK) {
		struct system_gclk_chan_config dpll_gclk_chan_conf;
		system_gclk_chan_get_config_defaults(&dpll_gclk_chan_conf);
		dpll_gclk_chan_conf.source_generator = CONF_CLOCK_DPLL_REFERENCE_GCLK_GENERATOR;
		system_gclk_chan_set_config(OSCCTRL_GCLK_ID_FDPLL, &dpll_gclk_chan_conf);
		system_gclk_chan_enable(OSCCTRL_GCLK_ID_FDPLL);
	} else {
		Assert(false);
	}
	struct system_clock_source_dpll_config dpll_config;
	system_clock_source_dpll_get_config_defaults(&dpll_config);

	dpll_config.on_demand        = false;
	dpll_config.run_in_standby   = CONF_CLOCK_DPLL_RUN_IN_STANDBY;
	dpll_config.lock_bypass      = CONF_CLOCK_DPLL_LOCK_BYPASS;
	dpll_config.wake_up_fast     = CONF_CLOCK_DPLL_WAKE_UP_FAST;
	dpll_config.low_power_enable = CONF_CLOCK_DPLL_LOW_POWER_ENABLE;

	dpll_config.filter           = CONF_CLOCK_DPLL_FILTER;

	dpll_config.reference_clock     = CONF_CLOCK_DPLL_REFERENCE_CLOCK;
	dpll_config.reference_frequency = CONF_CLOCK_DPLL_REFERENCE_FREQUENCY;
	dpll_config.reference_divider   = CONF_CLOCK_DPLL_REFERENCE_DIVIDER;
	dpll_config.output_frequency    = CONF_CLOCK_DPLL_OUTPUT_FREQUENCY;
	dpll_config.prescaler           = CONF_CLOCK_DPLL_PRESCALER;

	system_clock_source_dpll_set_config(&dpll_config);
	system_clock_source_enable(SYSTEM_CLOCK_SOURCE_DPLL);
	while(!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_DPLL));
	if (CONF_CLOCK_DPLL_ON_DEMAND) {
		OSCCTRL->DPLLCTRLA.bit.ONDEMAND = 1;
	}

#  endif

	/* CPU and BUS clocks */
	system_cpu_clock_set_divider(CONF_CLOCK_CPU_DIVIDER);

	/* GCLK 0 */
#if CONF_CLOCK_CONFIGURE_GCLK == true
	/* Configure the main GCLK last as it might depend on other generators */
	_CONF_CLOCK_GCLK_CONFIG(0, ~);
#endif

}
