/*
 * wm8994_ariank.c  --  WM8994 ALSA Soc Audio driver related Aries
 *
 *  Copyright (C) 2010 Samsung Electronics.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/map-base.h>
#include <mach/regs-clock.h>
#include <mach/gpio-aries.h>
#include "wm8994_samsung.h"

#define SUBJECT "wm8994_sone.c"

////////////////////////////////////////////////////////////////////////////////
//                           SHW-M130K: WM8994 Gain                           //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/***  DAC  ***/
#define TUNING_DAC1L_VOL                    0xC0 // 610h
#define TUNING_DAC1R_VOL                    0xC0 // 611h

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/***  Receiver  ***/
// Playback Call
#define TUNING_RCV_OUTMIX5_VOL                  0x00 // 31h
#define TUNING_RCV_OUTMIX6_VOL                  0x00 // 32h
#define TUNING_RCV_OPGAL_VOL                    0x3E // 20h
#define TUNING_RCV_OPGAR_VOL                    0x3E // 21h
#define TUNING_HPOUT2_VOL                       0x00 // 1Fh

// VoIP Call
#define TUNING_VOIPCALL_RCV_AIF1DAC_BOOST       0x02 // 301h
#define TUNING_VOIPCALL_RCV_OPGAL_VOL           0x36 // 20h
#define TUNING_VOIPCALL_RCV_OPGAR_VOL           0x36 // 21h

/////////////////////////////////////////////////////////////////////////////////////////
/***  Headset  ***/
// Playback
#define TUNING_MP3_OUTPUTL_VOL                  0x37 // 1Ch
#define TUNING_MP3_OUTPUTR_VOL                  0x37 // 1Dh

// Playback with SPK
#define TUNING_DUAL_OUTPUTL_VOL                 0x1C // 1Ch
#define TUNING_DUAL_OUTPUTR_VOL                 0x1C // 1Dh

// Playback Extra_Dock_speaker
#define TUNING_MP3_EXTRA_DOCK_SPK_OPGAL_VOL     0x39 // 20h
#define TUNING_MP3_EXTRA_DOCK_SPK_OPGAR_VOL     0x39 // 21h
#define TUNING_EXTRA_DOCK_SPK_OUTMIX5_VOL       0x00 // 31h
#define TUNING_EXTRA_DOCK_SPK_OUTMIX6_VOL       0x00 // 32h
#define TUNING_MP3_EXTRA_DOCK_SPK_VOL           0x00 // 1Eh

// Voice CALL
#define TUNING_CALL_OUTPUTL_VOL                 0x37 // 1Ch
#define TUNING_LOOPBACK_OUTPUTL_VOL             0x3C // 1Ch
#define TUNING_CALL_OUTPUTR_VOL                 0x37 // 1Dh
#define TUNING_LOOPBACK_OUTPUTR_VOL             0x3C // 1Dh
#define TUNING_CALL_OPGAL_VOL                   0x39 // 20h
#define TUNING_CALL_OPGAR_VOL                   0x39 // 21h

// VoIP Call (3pole/4pole)
#define TUNING_VOIPCALL_EAR_OUTPUTL_VOL         0x38 // 1Ch
#define TUNING_VOIPCALL_EAR_OUTPUTR_VOL         0x38 // 1Dh
#define TUNING_VOIPCALL_EAR_OPGAL_VOL           0x39 // 20h
#define TUNING_VOIPCALL_EAR_OPGAR_VOL           0x39 // 21h
#define TUNING_VOIPCALL_EAR_AIF1DAC_BOOST       0x00 // 301h

////////////////////////////////////////////////////////////////////////////////

/***  Speaker  ***/
// Playback, Playback with headset, Voice Call
#define TUNING_SPKMIXL_ATTEN                    0x00 // 22h
// Voice Call only
#define TUNING_SPKMIXR_ATTEN                    0x00 // 23h

// Playback, Playback with headset
#define TUNING_MP3_SPKL_VOL                     0x3D // 26h
#define TUNING_MP3_CLASSD_VOL                   0x06 // 25h

// Voice Call
#define TUNING_CALL_SPKL_VOL                    0x3E // 26h
#define TUNING_CALL_CLASSD_VOL                  0x06 // 25h

// VoIP Call
#define TUNING_VOIPCALL_SPKL_VOL                0x3e // 26h
#define TUNING_VOIPCALL_CLASSD_VOL              0x07 // 25h
#define TUNING_VOIPCALL_SPK_AIF1DAC_BOOST       0x02 // 301h

////////////////////////////////////////////////////////////////////////////////
// **** FILTERS **** //
#define TUNING_VOIPCALL_RCV_ADC1_FILTERS        0x3800 // 410h
#define TUNING_VOIPCALL_SPK_ADC1_FILTERS        0x3800 // 410h
#define TUNING_VOIPCALL_3P_ADC1_FILTERS         0x3800 // 410h
#define TUNING_VOIPCALL_EAR_ADC1_FILTERS        0x3800 // 410h

////////////////////////////////////////////////////////////////////////////////
/***  Onbaord MIC  ***/
// Normal rec. : main mic
#define TUNING_RECORD_MAIN_INPUTMIX_VOL         0x13 // 18h
//#define TUNING_VOICE_CALL_RCV_INPUTMIX_VOL    0x0B // 18h

// Voice recognition rec. : main mic
#define TUNING_RECOGNITION_MAIN_INPUTLINE_VOL   0x1A // 18h
#define TUNING_RECOGNITION_MAIN_AIF1ADCL_VOL    0xE0 // 400h
#define TUNING_RECOGNITION_MAIN_AIF1ADCR_VOL    0xE0 // 401h

// Voice Call 3P-EAR, SPK : sub mic
#define TUNING_VOICE_CALL_SPK_INPUTMIX_VOL      0x0B // 18h

// VoIP Call (RCV/SPK/3Pole)
#define TUNING_VOIPCALL_RCV_INPUTMIX_VOL        0x1f // 18h
#define TUNING_VOIPCALL_SPK_INPUTMIX_VOL        0x19 // 18h
#define TUNING_VOIPCALL_3P_INPUTMIX_VOL         0x15 // 18h
#define TUNING_VOIPCALL_MAIN_AIF1ADCL_VOL       0xEF // 400h
#define TUNING_VOIPCALL_MAIN_AIF1ADCR_VOL       0xEF // 401h

/***  Ear MIC  ***/
// Normal rec.
#define TUNING_RECORD_EAR_INPUTMIX_VOL          0x17 // 1Ah

// Voice recognition rec.
#define TUNING_RECOGNITION_EAR_INPUTMIX_VOL     0x17 // 1Ah
#define TUNING_RECOGNITION_EAR_AIF1ADCL_VOL     0xC0 // 400h
#define TUNING_RECOGNITION_EAR_AIF1ADCR_VOL     0xC0 // 401h

// Voice Call
#define TUNING_CALL_EAR_INPUTMIX_VOL            0x0B // 1Ah
#define TUNING_LOOPBACK_EAR_INPUTMIX_VOL        0x00 // 1Ah

// VoIP Call (4Pole)
#define TUNING_VOIPCALL_EAR_INPUTMIX_VOL        0x15 // 1Ah
#define TUNING_VOIPCALL_EAR_AIF1ADCL_VOL        0xC0 // 400h
#define TUNING_VOIPCALL_EAR_AIF1ADCR_VOL        0xC0 // 401h

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#define JACK_NO_DEVICE                  0x00
#define JACK_4_POLE_DEVICE              0x01
#define JACK_3_POLE_DEVICE              0x02
#define JACK_TVOUT_DEVICE               0x20
#define JACK_UNKNOWN_DEVICE             0x40


//------------------------------------------------
// Definitions of Mic path
//------------------------------------------------
#define SELECT_MAIN_MIC                 0
#define SELECT_SUB_MIC                  1

extern unsigned int get_headset_status(int *type, int *check);
//extern short int get_headset_status(void);		// For ear-jack control(MIC-Bias)
extern void set_recording_status(int value);	// For preventing MIC Bias off on using MIC.
extern unsigned int HWREV;
extern bool Earset_Loopback_Chk;
bool Record_on_Call = 0;

int audio_init(void)
{
#if 0
	/* GPIO_CODEC_LDO_EN (POWER) SETTTING */
	if (gpio_is_valid(GPIO_CODEC_LDO_EN))
	{
		if (gpio_request(GPIO_CODEC_LDO_EN, "CODEC_LDO_EN"))
			DEBUG_LOG_ERR("Failed to request CODEC_LDO_EN! \n");
		gpio_direction_output(GPIO_CODEC_LDO_EN, 0);
	}
	s3c_gpio_setpull(GPIO_CODEC_LDO_EN, S3C_GPIO_PULL_NONE);
	s3c_gpio_slp_setpull_updown(GPIO_CODEC_LDO_EN, S3C_GPIO_PULL_NONE);
#endif
	/* GPIO_CODEC_XTAL_EN (CLOCK) SETTTING */
	{
        if (gpio_is_valid(GPIO_CODEC_XTAL_EN)) {
        	if (gpio_request(GPIO_CODEC_XTAL_EN, "GPIO_CODEC_XTAL_EN"))
    			printk(KERN_ERR "Failed to request GPIO_CODEC_XTAL_EN! \n");

    		gpio_direction_output(GPIO_CODEC_XTAL_EN, 0);
    	}
    	s3c_gpio_setpull(GPIO_CODEC_XTAL_EN, S3C_GPIO_PULL_NONE);
	}


	/* GPIO_MICBIAS_EN SETTTING */
	if (gpio_is_valid(GPIO_MICBIAS_EN)) {
		if (gpio_request(GPIO_MICBIAS_EN, "GPJ4"))
			DEBUG_LOG_ERR("Failed to request GPIO_MICBIAS_EN! \n");
		gpio_direction_output(GPIO_MICBIAS_EN, 0);
	}
	s3c_gpio_setpull(GPIO_MICBIAS_EN, S3C_GPIO_PULL_NONE);
	s3c_gpio_slp_setpull_updown(GPIO_MICBIAS_EN, S3C_GPIO_PULL_NONE);

	audio_ctrl_sleep_gpio(0);

	return 0;

}

int audio_power(int en, int check)
{
	if(check)
	{
		if(en)
		{
			//gpio_set_value(GPIO_CODEC_LDO_EN, 1);
			//mdelay(10);

			{
				s3c_gpio_cfgpin(GPIO_CODEC_XTAL_EN, S3C_GPIO_OUTPUT);
				//s3c_gpio_setpin(GPIO_CODEC_XTAL_EN_R04, 1);
				gpio_set_value(GPIO_CODEC_XTAL_EN, 1);
			}
		} else {
			//gpio_set_value(GPIO_CODEC_LDO_EN, 0);
			//mdelay(125);

			{
				s3c_gpio_cfgpin(GPIO_CODEC_XTAL_EN, S3C_GPIO_OUTPUT);
				//s3c_gpio_setpin(GPIO_CODEC_XTAL_EN_R04, 0);
				gpio_set_value(GPIO_CODEC_XTAL_EN, 0);
			}
		}
	}

#if 1 //defined CONFIG_M110S
	{
		printk(SND_KERN_DEBUG "[WM8994] gpio en %d LDO %d XTAL %d\n", en, gpio_get_value(GPIO_CODEC_LDO_EN), gpio_get_value(GPIO_CODEC_XTAL_EN));
	}
#else // #elif defined CONFIG_M115S
    printk(SND_KERN_DEBUG "[WM8994] gpio en %d LDO %d XTAL %d\n", en, gpio_get_value(GPIO_CODEC_LDO_EN), gpio_get_value(GPIO_CODEC_XTAL_EN));
#endif

	return 0;
}

extern char *playback_path_name[];
void audio_ctrl_mic_bias_gpio(struct snd_soc_codec *codec, char callid)
{
	u32 enable = 0;
	struct wm8994_priv *wm8994 = codec->drvdata;
	int headset_state, headset_check;
	unsigned mic_sel = SELECT_MAIN_MIC;

	if (wm8994->codec_state & (CAPTURE_ACTIVE|CALL_ACTIVE))
		enable = 1;

	//if ( (wm8994->cur_path == BT) || (wm8994->rec_path == BT_REC))
	if ( (wm8994->cur_path == BT) )
		enable = 0;

	//headset_state = get_headset_status();
	get_headset_status(&headset_state, &headset_check);
	if(enable) {
		set_recording_status(1);
		if (headset_state == JACK_4_POLE_DEVICE) {
			if(gpio_get_value(GPIO_EAR_BIAS_EN) != 1) {
				gpio_set_value(GPIO_EAR_BIAS_EN, 1);
			}

			if (wm8994->cur_path != SPK) {
				gpio_set_value(GPIO_MICBIAS_EN, 0);
			} else {
				if(gpio_get_value(GPIO_MICBIAS_EN) != 1) {
					gpio_set_value(GPIO_MICBIAS_EN, 1);
				}
			}
		} else {
			if (headset_check == 1) {
				gpio_set_value(GPIO_EAR_BIAS_EN, 1);
			} else {
				gpio_set_value(GPIO_EAR_BIAS_EN, 0);
			}

			if(gpio_get_value(GPIO_MICBIAS_EN) != 1) {
				gpio_set_value(GPIO_MICBIAS_EN, 1);
			}
		}
	} else {
		set_recording_status(0);
		if (headset_state == JACK_4_POLE_DEVICE) {
			gpio_set_value(GPIO_EAR_BIAS_EN, 1);
		} else {
			if (headset_check == 1) {
				gpio_set_value(GPIO_EAR_BIAS_EN, 1);
			} else {
				gpio_set_value(GPIO_EAR_BIAS_EN, 0);
			}
		}
	}

    //////////////////////////////////////////////////////////////////////////////////
    // switching mic path
    if (wm8994->codec_state & CALL_ACTIVE)
    {
        switch(wm8994->cur_path)
        {
            case RCV:
                mic_sel = SELECT_MAIN_MIC;
                break;
                
            case HP:
            case HP_NO_MIC:
            case SPK:
            case SPK_HP:
                mic_sel = SELECT_SUB_MIC;
                break;
            
            case OFF:
            case BT:
            case EXTRA_DOCK_SPEAKER:
            default:
                break;
        }
    }
	gpio_set_value(GPIO_MIC_SEL, mic_sel);

    //////////////////////////////////////////////////////////////////////////////////
    // check mic state
	printk("mic state(%c,%s) : |%s|%s|%s|%s|\n", 
                callid, playback_path_name[wm8994->cur_path],
                gpio_get_value(GPIO_MICBIAS_EN)==1?"on":"off", 
                gpio_get_value(GPIO_MIC_SEL)==1?"sub":"main", 
                headset_state==JACK_4_POLE_DEVICE?"4P":headset_state==JACK_3_POLE_DEVICE?"3P":"No",
                gpio_get_value(GPIO_EAR_BIAS_EN)==1?"on":"off");
}

// If enable is false, set gpio to low on sleep
void audio_ctrl_sleep_gpio(int enable)
{
	int state;

	if(enable) {
		DEBUG_LOG("Set gpio to low on sleep.");
		state = S3C_GPIO_SLP_OUT0;
	} else {
		DEBUG_LOG("Set gpio to preserve on sleep.");
		state = S3C_GPIO_SLP_PREV;
	}

	// GPIO SLEEP CONFIG // preserve output state in sleep
	s3c_gpio_slp_cfgpin(GPIO_CODEC_LDO_EN, state);
	s3c_gpio_slp_cfgpin(GPIO_MICBIAS_EN, state);
	s3c_gpio_slp_cfgpin(GPIO_MIC_SEL, state);
	s3c_gpio_slp_cfgpin(GPIO_CODEC_XTAL_EN, state);
}

static void wait_for_dc_servo(struct snd_soc_codec *codec, unsigned int op)
{
	unsigned int reg;
	int count = 0;
	unsigned int val, start;

	val = (op|WM8994_DCS_ENA_CHAN_0|WM8994_DCS_ENA_CHAN_1);

	/* Trigger the command */
	snd_soc_write(codec, WM8994_DC_SERVO_1, val);

	start = jiffies;
	pr_debug("%s: Waiting for DC servo...\n", __func__);

	do {
		count++;
		msleep(1);
		reg = wm8994_read(codec, WM8994_DC_SERVO_1);
		pr_debug("%s: DC servo: %x\n", __func__, reg);
	} while (reg & op && count < 400);

	pr_debug("%s: DC servo took %dms\n", __func__, jiffies_to_msecs(jiffies - start));

	if (reg & op)
		pr_err("%s: Timed out waiting for DC Servo\n", __func__);
}

void wm8994_block_control_playback(int on, struct snd_soc_codec *codec)
{
	DEBUG_LOG("++ blk play %#x", on);
	DEBUG_LOG("-- blk play %#x", on);
}

void wm8994_block_control_record(int on, struct snd_soc_codec *codec)
{
	DEBUG_LOG("++ blk rec %#x", on);
	DEBUG_LOG("-- blk rec %#x", on);
}

#define FUNCTION_LIST_DISABLE

void wm8994_disable_path(struct snd_soc_codec *codec, enum audio_path path)
{
	return wm8994_disable_playback_path(codec, path);
}

void wm8994_disable_playback_path(struct snd_soc_codec *codec, enum audio_path path)
{
	struct wm8994_priv *wm8994 = codec->drvdata;
	u16 val;

	DEBUG_LOG("Path = [%d]", path);

	switch(path)
	{
		case RCV:
			// 01H // HPOUT2_ENA off
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
			val &= ~(WM8994_HPOUT2_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

			// 2DH // DAC1L_TO_MIXOUTL off
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

			// 2EH // DAC1L_TO_MIXOUTL off
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

			// 33H // MIXOUTL/RVOL_TO_HPOUT2 off
			val = wm8994_read(codec, WM8994_HPOUT2_MIXER);
			val &= ~(WM8994_MIXOUTLVOL_TO_HPOUT2_MASK|WM8994_MIXOUTRVOL_TO_HPOUT2_MASK);
			wm8994_write(codec, WM8994_HPOUT2_MIXER, val);

			// 03H // MIXOUTL/RVOL, MIXOUTL/R off
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_MIXOUTLVOL_ENA_MASK|WM8994_MIXOUTRVOL_ENA_MASK|WM8994_MIXOUTL_ENA_MASK|WM8994_MIXOUTR_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);
			break;

		case SPK:
			// 01H // SPKOUTL_ENA off
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
			val &= ~(WM8994_SPKOUTL_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

			// 03H // SPKLVOL_ENA off
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_SPKLVOL_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			// 24H // SPKMIXL/R off
			val = wm8994_read(codec, WM8994_SPKOUT_MIXERS);
			val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK|WM8994_SPKMIXR_TO_SPKOUTL_MASK|
				WM8994_SPKMIXR_TO_SPKOUTR_MASK|WM8994_SPKMIXL_TO_SPKOUTR_MASK);
			wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

			// 36H // DAC1L_TO_SPKMIXL off
			val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER, val);
			break;

		case HP:
		case HP_NO_MIC:
			if(wm8994->codec_state & VOICE_CALL_ACTIVE)
			{
				val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
				val &= ~(0x02C0);
				val |= 0x02C0;
				wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, 0x02C0);

				val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
				val &= ~(0x02C0);
				val |= 0x02C0;
				wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, 0x02C0);

				val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
				val &= ~(0x0022);
				val |= 0x0022;
				wm8994_write(codec, WM8994_ANALOGUE_HP_1, 0x0022);

				val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
				val &= ~(0x0);
				val |= 0x0;
				wm8994_write(codec, WM8994_OUTPUT_MIXER_1, 0x0);

				val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
				val &= ~(0x0);
				val |= 0x0;
				wm8994_write(codec, WM8994_OUTPUT_MIXER_2, 0x0);

				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
				val &= ~(0x0300);
				val |= 0x0300;
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, 0x0300);

				val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
				val &= ~(0x1F25);
				val |= 0x1F25;
				wm8994_write(codec, WM8994_CHARGE_PUMP_1, 0x1F25);
			}
			break;
#if 0
		case BT :
			// 06H // AIF3 interface off
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_6);
			val &= ~(WM8994_AIF3_ADCDAT_SRC_MASK|WM8994_AIF2_ADCDAT_SRC_MASK|WM8994_AIF2_DACDAT_SRC_MASK|WM8994_AIF1_DACDAT_SRC_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, val);

			// 420H // AIF1DAC1_MUTE on
			val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
			val &= ~(WM8994_AIF1DAC1_MUTE_MASK);
			val |= (WM8994_AIF1DAC1_MUTE);
			wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);
			break;
#endif
		case SPK_HP :
			if(wm8994->codec_state & VOICE_CALL_ACTIVE)
			{
				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
				val &= ~(WM8994_HPOUT1L_ENA_MASK|WM8994_HPOUT1R_ENA_MASK|WM8994_SPKOUTL_ENA_MASK);
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

				/* ------------------ Ear path setting ------------------ */
				// 2DH // DAC1L_TO_HPOUT1L, DAC1L_TO_MIXOUTL off
				val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
				val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK|WM8994_DAC1L_TO_MIXOUTL_MASK);
				wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

				// 2EH // DAC1R_TO_HPOUT1R, DAC1R_TO_MIXOUTR off
				val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
				val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK|WM8994_DAC1R_TO_MIXOUTR_MASK);
				wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

				// 4CH // CP_ENA off (charge pump off)
				val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
				val &= ~(WM8994_CP_ENA_MASK);
				val |= (WM8994_CP_ENA_DEFAULT); // from wolfson
				wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

				// 60H // HP Output off
				val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
				val &= ~(WM8994_HPOUT1R_DLY_MASK|WM8994_HPOUT1R_OUTP_MASK|WM8994_HPOUT1R_RMV_SHORT_MASK|
					WM8994_HPOUT1L_DLY_MASK|WM8994_HPOUT1L_OUTP_MASK|WM8994_HPOUT1L_RMV_SHORT_MASK);
				wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

				/* ------------------ Spk path setting ------------------ */
				// 03H // SPKLVOL_ENA off
				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
				val &= ~(WM8994_SPKLVOL_ENA_MASK);
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

				// 24H // SPKMIX_TO_SPKOUT off
				val = wm8994_read(codec, WM8994_SPKOUT_MIXERS);
				val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK|WM8994_SPKMIXR_TO_SPKOUTL_MASK|
					WM8994_SPKMIXR_TO_SPKOUTR_MASK|WM8994_SPKMIXL_TO_SPKOUTR_MASK);
				wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

				// 36H // DAC1L_TO_SPKMIXL off
				val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
				val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
				wm8994_write(codec, WM8994_SPEAKER_MIXER, val);
			}
			break;

		default:
			DEBUG_LOG("Path[%d] is not invaild!\n", path);
			break;
	}

	return;
}

void wm8994_disable_rec_path(struct snd_soc_codec *codec, enum mic_path rec_path)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

	DEBUG_LOG("");

	wm8994->rec_path = MIC_OFF;
    Record_on_Call = 0;

	//audio_ctrl_mic_bias_gpio(codec);

	switch(rec_path)
	{
		case MAIN:
			if (wm8994->codec_state & (CALL_ACTIVE))
			{
				DEBUG_LOG("Disable MAIN Mic incall rec\n");

				if(!wm8994->testmode_config_flag)
				{
					// 29H // Mute IN1L_TO_MIXINL, IN1L_TO_MIXINL_VOL
					//val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
					//val&= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
					//wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
				}

				// 28H // Disbale IN1LN/P_TO_IN1L
				//val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
				//val &= (WM8994_IN1LN_TO_IN1L_MASK|WM8994_IN1LP_TO_IN1L_MASK);
				//wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

				if(wm8994->codec_state & (VOIP_CALL_ACTIVE))
				{
					printk(SND_KERN_DEBUG "[WM8994] turn off hpf in voip main mic...\n");
#ifdef FEATURE_VOIP_DRC
	if this code enabled, build error
					wm8994_write(codec, WM8994_AIF1_DRC1_1, 0x0098); // 440h // DRC disable
//#endif
//#ifdef FEATURE_VOIP_HPFILTER
					// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter off
					val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
					val &= ~(WM8994_AIF1ADC1_HPF_CUT_MASK|WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
					wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, 0x0000);
#endif
					// 400H // AIF1 ADC1 Left Volume to default
					val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
					val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
					val |= (WM8994_AIF1ADC1_VU|0xC0); // ADC Digital Gain
					wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);
				}

				// 04H //
				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
				val &= ~(WM8994_ADCL_ENA_MASK|WM8994_AIF1ADC1L_ENA_MASK|WM8994_AIF1ADC1R_ENA_MASK);
                if(wm8994->cur_path == BT)
                    val |= (WM8994_ADCL_ENA_MASK);
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

				// 606H // Disaable ADC1L_TO_AIF1ADC1L (TIMESLOT 0)
				val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
				val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
				wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

				// 607H // Disaable ADC1R_TO_AIF1ADC1R (TIMESLOT 0)
				val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
				val &= ~(WM8994_ADC1R_TO_AIF1ADC1R_MASK);
				wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);

			}
			else // normal
			{
				DEBUG_LOG("Disable MAIN Mic normal rec\n");

				// 02H // Disable IN1L_ENA, MIXINL_ENA
				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
				val &= ~(WM8994_IN1L_ENA_MASK|WM8994_MIXINL_ENA_MASK);
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

				if(!wm8994->testmode_config_flag)
				{
					// 18H // Mute IN1L_VOL
					val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);
					val &= ~(WM8994_IN1L_MUTE_MASK|WM8994_IN1L_VOL_MASK);
					val |= (WM8994_IN1L_VU|WM8994_IN1L_MUTE);
					wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

					// 29H // Mute IN1L_TO_MIXINL, IN1L_TO_MIXINL_VOL
					val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
					val&= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
					wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
				}

#if 1 // Voice Search Tuning
				// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter on
				val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
				val &= ~(WM8994_AIF1ADC1_HPF_CUT_MASK|WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
				wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, 0x0000);

				// 400H // AIF1 ADC1 Left Volume
				val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
				val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
				val |= (WM8994_AIF1ADC1_VU|0xC0); // ADC Digital Gain
				wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);
#endif
				// 28H // Disbale IN1LN/P_TO_IN1L
				val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
				val &= (WM8994_IN1LN_TO_IN1L_MASK|WM8994_IN1LP_TO_IN1L_MASK);
				wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

				// 04H // Disable ADCL_ENA, AIF1ADC1L_ENA
				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
				val &= ~(WM8994_ADCL_ENA_MASK|WM8994_AIF1ADC1L_ENA_MASK);
                if(wm8994->cur_path == BT)
                    val |= (WM8994_ADCL_ENA_MASK);
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

				// 606H // Disable ADC1L_TO_AIF1ADC1L
				val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
				val &= ~(WM8994_ADC1L_TO_AIF1ADC1L);
				wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);
			}

			break;

		case EAR:
			DEBUG_LOG("disable EAR MIC path...\n");

			if (wm8994->codec_state & (CALL_ACTIVE))
			{
				printk(SND_KERN_DEBUG "[WM8994] disable ear mic - in call\n");

				// 02H // Disable MIXINR_ENA
				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
				//val &= ~(WM8994_IN1R_ENA_MASK|WM8994_MIXINR_ENA_MASK);
				val &= ~(WM8994_MIXINR_ENA_MASK);
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

				if(!wm8994->testmode_config_flag)
				{
					// 1AH // Mute IN1R_VOL
					//val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);
					//val &= ~(WM8994_IN1R_MUTE_MASK|WM8994_IN1R_VOL_MASK);
					//val |= (WM8994_IN1R_VU|WM8994_IN1R_MUTE);
					//wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME, val);

					// 2AH // Mute IN1R_TO_MIXINR, IN1R_TO_MIXINR_VOL
					val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
					val&= ~(WM8994_IN1R_TO_MIXINR_MASK|WM8994_IN1R_MIXINR_VOL_MASK|WM8994_MIXOUTR_MIXINR_VOL_MASK);
					wm8994_write(codec, WM8994_INPUT_MIXER_4, val);
				}

				// 28H // Disbale IN1RN/P_TO_IN1R
				//val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
				//val &= ~(WM8994_IN1RN_TO_IN1R_MASK|WM8994_IN1RP_TO_IN1R_MASK);
				//wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

				if(wm8994->codec_state & (VOIP_CALL_ACTIVE))
				{
					printk(SND_KERN_DEBUG "[WM8994] turn off hpf in voip ear mic...\n");
#ifdef FEATURE_VOIP_DRC
	if this code enabled, build error
					wm8994_write(codec, WM8994_AIF1_DRC1_1, 0x0098); // 440h // DRC disable
//#endif
//#ifdef FEATURE_VOIP_HPFILTER
					// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter off
					val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
					val &= ~(WM8994_AIF1ADC1_HPF_CUT_MASK|WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
					wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, 0x0000);
#endif
					// 401H // AIF1 ADC1 Right Volume to default
					val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME);
					val &= ~(WM8994_AIF1ADC1R_VOL_MASK);
					val |= (WM8994_AIF1ADC1_VU|0xC0); // ADC Digital Gain
					wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME, val);
				}


				// 04H //
				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
				val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_ADCR_ENA_MASK|WM8994_AIF1ADC1R_ENA_MASK);
				if(wm8994->cur_path == BT)
                    val |= (WM8994_ADCL_ENA_MASK);
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

				// 606H // Disaable ADC1L_TO_AIF1ADC1L (TIMESLOT 0)
				val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
				val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
				wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

				// 607H // Disaable ADC1R_TO_AIF1ADC1R (TIMESLOT 0)
				val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
				val &= ~(WM8994_ADC1R_TO_AIF1ADC1R_MASK);
				wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);
			}
			else
			{
				printk(SND_KERN_DEBUG "[WM8994] disable ear mic - normal\n");

				// 02H // Disable MIXINR_ENA
				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
				val &= ~(WM8994_IN1R_ENA_MASK|WM8994_MIXINR_ENA_MASK);
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

				if(!wm8994->testmode_config_flag)
				{
					// 1AH // Mute IN1R_VOL
					val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);
					val &= ~(WM8994_IN1R_MUTE_MASK|WM8994_IN1R_VOL_MASK);
					val |= (WM8994_IN1R_VU|WM8994_IN1R_MUTE);
					wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME, val);

					// 2AH // Mute IN1R_TO_MIXINR, IN1R_TO_MIXINR_VOL
					val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
					val&= ~(WM8994_IN1R_TO_MIXINR_MASK|WM8994_IN1R_MIXINR_VOL_MASK|WM8994_MIXOUTR_MIXINR_VOL_MASK);
					wm8994_write(codec, WM8994_INPUT_MIXER_4, val);
				}

				// 28H // Disbale IN1RN/P_TO_IN1R
				val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
				val &= ~(WM8994_IN1RN_TO_IN1R_MASK|WM8994_IN1RP_TO_IN1R_MASK);
				wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

				// 04H //
				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
				val &= ~(WM8994_ADCR_ENA_MASK|WM8994_AIF1ADC1R_ENA_MASK);
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

				// 607H // Disaable ADC1R_TO_AIF1ADC1R (TIMESLOT 0)
				val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
				val &= ~(WM8994_ADC1R_TO_AIF1ADC1R_MASK);
				wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);
			}
			break;
#if 0
		case BT_REC:
			if(wm8994->codec_state & CALL_ACTIVE)
			{
				// 02H //
				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
				val &= ~(WM8994_MIXINL_ENA_MASK|WM8994_MIXINR_ENA_MASK);
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

				// 2CH // [DE03-1723] // IN2LRP_MIXINR_VOL 0x01(-12dB)
				val = wm8994_read(codec, WM8994_INPUT_MIXER_6);
				val &= ~(WM8994_IN2LP_MIXINR_VOL_MASK);
				wm8994_write(codec, WM8994_INPUT_MIXER_6, val);

				// 04H // AIF1ADC1L_ENA, ADCL_ENA // BT CALL = WM8994_AIF2ADCL_ENA|WM8994_ADCL_ENA
				val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
				val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_AIF1ADC1R_ENA_MASK|WM8994_ADCL_ENA_MASK|WM8994_ADCR_ENA_MASK);
				val |= (WM8994_AIF2ADCL_ENA|WM8994_ADCL_ENA);
				wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

				//R1542(606h) - 0x0000
				val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
				val &= ~(WM8994_AIF2DACL_TO_AIF1ADC1L_MASK|WM8994_ADC1L_TO_AIF1ADC1L_MASK);
				wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

				//R1543(607h) - 0x0000
				val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
				val &= ~(WM8994_AIF2DACR_TO_AIF1ADC1R_MASK|WM8994_ADC1R_TO_AIF1ADC1R_MASK);
				wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);

				//R1312(520h) - 0x0200
				//val = wm8994_read(codec, WM8994_AIF2_DAC_FILTERS_1);
				//val &= ~(WM8994_AIF2DAC_MUTE_MASK);
				//val |= (WM8994_AIF2DAC_MUTE);
				//wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, val);
			}
			break;
#endif
		default:
			DEBUG_LOG("Path[%d] is not invaild!\n", rec_path);
			break;
	}
}

#define FUNCTION_LIST_RECORD

void wm8994_record_headset_mic(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

	DEBUG_LOG("Recording through Headset Mic\n");

	if(wm8994->codec_state & VOICE_CALL_ACTIVE)
	{
		printk(SND_KERN_DEBUG "[WM8994] Recording ear mic - in call\n");
        Record_on_Call = 1;

		// 300H // Mixing left channel output to right channel // val: 0x0010
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
		val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
		//val |= (WM8994_AIF1ADCL_SRC|WM8994_AIF1ADCR_SRC);
		val |= (WM8994_AIF1ADCL_SRC);
		wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

		// 39H // Ear mic volume issue fix
		wm8994_write(codec, WM8994_ANTIPOP_2, 0x68);

		// 01H // VMID_SEL, BIAS_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
		val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
		val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

		// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
		wm8994_write(codec, WM8994_GPIO_1, 0xA101);

		// 02H // MIXINR_ENA, IN1R_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
		val &= ~(WM8994_MIXINL_ENA_MASK|WM8994_MIXINR_ENA_MASK|WM8994_IN1R_ENA_MASK);
		val |= (WM8994_MIXINL_ENA|WM8994_MIXINR_ENA|WM8994_IN1R_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

		if(!wm8994->testmode_config_flag)
		{
			// 1AH // IN1R PGA // IN1R UNMUTE, SET VOL
			val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);
			val &= ~(WM8994_IN1R_MUTE_MASK|WM8994_IN1R_VOL_MASK);
			val |= (WM8994_IN1R_VU|TUNING_CALL_EAR_INPUTMIX_VOL);
			wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME, val);

			// 2AH // MIXINR PGA // IN2R_TO_MIXINR MUTE, IN1R_TO_MIXINR UNMUTE, 0dB
			val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
			val &= ~(WM8994_IN1R_TO_MIXINR_MASK|WM8994_IN1R_MIXINR_VOL_MASK|WM8994_MIXOUTR_MIXINR_VOL_MASK);
			//val |= (WM8994_IN1R_TO_MIXINR); //0db
			val |= (WM8994_IN1R_TO_MIXINR|WM8994_IN1R_MIXINR_VOL); // Boost On(+30dB)
			wm8994_write(codec, WM8994_INPUT_MIXER_4, val);
		}

        wm8994_write(codec, WM8994_INPUT_MIXER_6, 0x0002);

#if 1
        // printk("[WM8994] current path is %d \n", wm8994->cur_path);
		// 400H // AIF1 ADC1 Left Volume
		val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
		val |= (WM8994_AIF1ADC1_VU|0xef); // default volume
		wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);

        // 401H // AIF1 ADC1 Right Volume
		val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
		val |= (WM8994_AIF1ADC1_VU|0xef); // ADC Digital Gain
		wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME, val);
#endif

		// 04H // AIF1ADC1L/R_ENA, ADCL/R_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
		val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_ADCL_ENA_MASK|WM8994_AIF1ADC1R_ENA_MASK|WM8994_ADCR_ENA_MASK);
		val |= (WM8994_AIF1ADC1L_ENA|WM8994_ADCL_ENA|WM8994_AIF1ADC1R_ENA|WM8994_ADCL_ENA|WM8994_ADCR_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

		// 606H // ADC1L_TO_AIF1ADC1L (TIMESLOT 0) ASSIGN
		val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
		val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
		val |= (WM8994_ADC1L_TO_AIF1ADC1L);
		wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

		// 607H // ADC1R_TO_AIF1ADC1R (TIMESLOT 0) ASSIGN
		val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
		val &= ~(WM8994_ADC1R_TO_AIF1ADC1R_MASK);
		val |= (WM8994_ADC1R_TO_AIF1ADC1R);
		wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);
	}
	else
	{
		printk(SND_KERN_DEBUG "[WM8994] Recording ear mic - normal / Video Call\n");

		// 39H // Ear mic volume issue fix
		wm8994_write(codec, WM8994_ANTIPOP_2, 0x68);

		// 01H // VMID_SEL, BIAS_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
		val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
		val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

		// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
		wm8994_write(codec, WM8994_GPIO_1, 0xA101);

		// 02H // MIXINR_ENA, IN1R_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
		val &= ~(WM8994_MIXINR_ENA_MASK|WM8994_IN1R_ENA_MASK);
		val |= (WM8994_MIXINR_ENA|WM8994_IN1R_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

		if(!wm8994->testmode_config_flag)
		{
			// 1AH // IN1R PGA // IN1R UNMUTE, SET VOL
			val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);
			val &= ~(WM8994_IN1R_MUTE_MASK|WM8994_IN1R_VOL_MASK);
			if(wm8994->input_source == RECOGNITION)
				val |= (WM8994_IN1R_VU|TUNING_RECOGNITION_EAR_INPUTMIX_VOL);
			else
				val |= (WM8994_IN1R_VU|TUNING_RECORD_EAR_INPUTMIX_VOL);
			wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME, val);

			// 2AH // MIXINR PGA // IN2R_TO_MIXINR MUTE, IN1R_TO_MIXINR UNMUTE, 0dB
			val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
			val &= ~(WM8994_IN1R_TO_MIXINR_MASK|WM8994_IN1R_MIXINR_VOL_MASK|WM8994_MIXOUTR_MIXINR_VOL_MASK);
			if(wm8994->input_source == RECOGNITION)
				val |= (WM8994_IN1R_TO_MIXINR|WM8994_IN1R_MIXINR_VOL); //30db
			else
				val |= (WM8994_IN1R_TO_MIXINR); //0db
			wm8994_write(codec, WM8994_INPUT_MIXER_4, val);
		}

		// 28H // INPUT MIXER // IN1RP/N_TO_IN1R PGA
		val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
		val &= ~(WM8994_IN1RP_TO_IN1R_MASK|WM8994_IN1RN_TO_IN1R_MASK);
		val |= (WM8994_IN1RP_TO_IN1R|WM8994_IN1RN_TO_IN1R);
		wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

		// 04H // AIF1ADC1R_ENA, ADCR_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
		val &= ~(WM8994_AIF1ADC1R_ENA_MASK|WM8994_ADCR_ENA_MASK);
		val |= (WM8994_AIF1ADC1R_ENA|WM8994_ADCR_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

		// 607H // ADC1R_TO_AIF1ADC1R (TIMESLOT 0) ASSIGN
		val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
		val &= ~(WM8994_ADC1R_TO_AIF1ADC1R_MASK);
		val |= (WM8994_ADC1R_TO_AIF1ADC1R);
		wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);

		// 300H // Mixing left channel output to right channel // val: 0x0010
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
		val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
		val |= (WM8994_AIF1ADCL_SRC|WM8994_AIF1ADCR_SRC);
		wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);
    }

	printk(SND_KERN_DEBUG "[WM8994] wm8994_record_headset_mic()\n");
}


void wm8994_record_onboard_mic(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

	DEBUG_LOG("+++");

	if (wm8994->codec_state & (VOICE_CALL_ACTIVE))
	{
		printk(SND_KERN_DEBUG "[WM8994] Recording Main Mic - in voice call\n");
        Record_on_Call = 1;
		// 300H // Mixing left channel output to right channel // val: 0x0010
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
		val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
        val |= (WM8994_AIF1ADCR_SRC_MASK);
		wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

		// 39H // Main mic volume issue fix
		wm8994_write(codec, WM8994_ANTIPOP_2, 0x68);

		// 01H // VMID_SEL_NORMAL, BIAS_ENA, MICB1_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
		val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
		val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

		// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
		wm8994_write(codec, WM8994_GPIO_1, 0xA101);

		// 02H // MIXINL_ENA, IN1L_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
		val &= ~(WM8994_MIXINL_ENA_MASK|WM8994_MIXINR_ENA_MASK|WM8994_IN1L_ENA_MASK);
		val |= (WM8994_MIXINL_ENA|WM8994_MIXINR_ENA|WM8994_IN1L_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

		if(!wm8994->testmode_config_flag)
		{
			// 18H // IN1L PGA // IN1L UNMUTE, SET VOL
			val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);
			val &= ~(WM8994_IN1L_MUTE_MASK|WM8994_IN1L_VOL_MASK);
			val |= (WM8994_IN1L_VU|TUNING_VOICE_CALL_SPK_INPUTMIX_VOL); //volume
			wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

			// 29H // MIXINL PGA // IN2L_TO_MIXINL MUTE, IN1L_TO_MIXINL UNMUTE, 0dB
			val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
			val &= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
			val |= (WM8994_IN1L_TO_MIXINL|WM8994_IN1L_MIXINL_VOL); // Boost On(+30dB)
			wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
		}

        wm8994_write(codec, WM8994_INPUT_MIXER_5, 0x0002);

#if 1
        // printk("[WM8994] current path is %d \n", wm8994->cur_path);
		// 400H // AIF1 ADC1 Left Volume
		val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
		val |= (WM8994_AIF1ADC1_VU|0xef); // default volume
		wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);

        // 401H // AIF1 ADC1 Right Volume
		val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
		val |= (WM8994_AIF1ADC1_VU|0xef); // ADC Digital Gain
		wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME, val);
#endif

		// 28H // INPUT MIXER // IN1LP/N_TO_IN1L PGA
		val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
		val &= (WM8994_IN1LP_TO_IN1L_MASK|WM8994_IN1LN_TO_IN1L_MASK);
		val |= (WM8994_IN1LP_TO_IN1L|WM8994_IN1LN_TO_IN1L);
		wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

		// 04H // AIF1ADC1L_ENA, ADCL_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
		val &= ~(WM8994_AIF1ADC1R_ENA_MASK|WM8994_ADCR_ENA_MASK|WM8994_AIF1ADC1L_ENA_MASK|WM8994_ADCL_ENA_MASK);
		val |= (WM8994_AIF1ADC1R_ENA|WM8994_ADCR_ENA|WM8994_AIF1ADC1L_ENA|WM8994_ADCL_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

		// 606H // ADC1L_TO_AIF1ADC1L (TIMESLOT 0) ASSIGN
		val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
		val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
		val |= (WM8994_ADC1L_TO_AIF1ADC1L);
		wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

		// 607H // ADC1R_TO_AIF1ADC1R (TIMESLOT 0) ASSIGN
		val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
		val &= ~(WM8994_ADC1R_TO_AIF1ADC1R_MASK);
		val |= (WM8994_ADC1R_TO_AIF1ADC1R);
		wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);
	}
	else
	{
		printk(SND_KERN_DEBUG "[WM8994] Recording Main Mic - normal recording or voice recognition\n");

		// 300H // copy AIF1ADCL_SRC data to AIF1ADCR_SRC // val: 0x0010
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
		val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
		wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

		// 39H // Main mic volume issue fix
		wm8994_write(codec, WM8994_ANTIPOP_2, 0x68);

		// 01H // VMID_SEL_NORMAL, BIAS_ENA, MICB1_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
		val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
		val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

		// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
		wm8994_write(codec, WM8994_GPIO_1, 0xA101);

		// 02H // MIXINL_ENA, IN1L_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
		val &= ~(WM8994_MIXINL_ENA_MASK|WM8994_IN1L_ENA_MASK);
		val |= (WM8994_MIXINL_ENA|WM8994_IN1L_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

		if(!wm8994->testmode_config_flag)
		{
			// 18H // IN1L PGA // IN1L UNMUTE, SET VOL
			val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);
			val &= ~(WM8994_IN1L_MUTE_MASK|WM8994_IN1L_VOL_MASK);
			if(wm8994->input_source == RECOGNITION)
				val |= (WM8994_IN1L_VU|TUNING_RECOGNITION_MAIN_INPUTLINE_VOL); //volume
			else
				val |= (WM8994_IN1L_VU|TUNING_RECORD_MAIN_INPUTMIX_VOL);
			wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

			// 29H // MIXINL PGA // IN2L_TO_MIXINL MUTE, IN1L_TO_MIXINL UNMUTE, 0dB
			val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
			val &= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
			if(wm8994->input_source == RECOGNITION)
				val |= (WM8994_IN1L_TO_MIXINL);	// 0db
			else
				val |= (WM8994_IN1L_TO_MIXINL|WM8994_IN1L_MIXINL_VOL); // Boost On(+30dB)
			wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
		}

		if(wm8994->input_source == RECOGNITION)
		{
			// Voice Search Tuning
			// 400H // AIF1 ADC1 Left Volume
			val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
			val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
			val |= (WM8994_AIF1ADC1_VU|TUNING_RECOGNITION_MAIN_AIF1ADCL_VOL); // ADC Digital Gain
			wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);
		}

		// 28H // INPUT MIXER // IN1LP/N_TO_IN1L PGA
		val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
		val &= (WM8994_IN1LP_TO_IN1L_MASK|WM8994_IN1LN_TO_IN1L_MASK);
		val |= (WM8994_IN1LP_TO_IN1L|WM8994_IN1LN_TO_IN1L);
		wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

		if(wm8994->input_source == RECOGNITION)
		{
			// for ASR Test
			// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter on
			val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
			val &= ~(WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
			val |= (WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF); // hi-path filter on (L/R)
			wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, 0x1000);
		}

		// 04H // AIF1ADC1L_ENA, ADCL_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
		val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_ADCL_ENA_MASK);
		val |= (WM8994_AIF1ADC1L_ENA|WM8994_ADCL_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

		// 606H // ADC1L_TO_AIF1ADC1L (TIMESLOT 0) ASSIGN
		val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
		val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
		val |= (WM8994_ADC1L_TO_AIF1ADC1L);
		wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);
	}
}

void wm8994_record_main_mic(struct snd_soc_codec *codec)
{
	DEBUG_LOG("+++");
	DEBUG_LOG("Recording through Main Mic\n");

	if (HWREV >= 2)
	{
		gpio_set_value(GPIO_MIC_SEL, SELECT_MAIN_MIC);
		DEBUG_LOG("gpio_get_value(GPIO_MIC_SEL) %d", gpio_get_value(GPIO_MIC_SEL));
	}

    wm8994_record_onboard_mic(codec);
    
	printk(SND_KERN_DEBUG "[WM8994] wm8994_record_main_mic()---\n");
}

void wm8994_record_sub_mic(struct snd_soc_codec *codec)
{
    DEBUG_LOG("+++");
	DEBUG_LOG("Recording through Sub Mic\n");
    
	gpio_set_value(GPIO_MIC_SEL, SELECT_SUB_MIC);
	DEBUG_LOG("gpio_get_value(GPIO_MIC_SEL) %d", gpio_get_value(GPIO_MIC_SEL));
    
    wm8994_record_onboard_mic(codec);

    printk(SND_KERN_DEBUG "[WM8994] wm8994_record_sub_mic()---\n");
}

void wm8994_record_bluetooth_mic(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

	DEBUG_LOG("Recording through Bluetooth mic\n");

	if (wm8994->codec_state & (CALL_ACTIVE))
	{
		printk(SND_KERN_DEBUG "[WM8994] Recording Bluetooth mic - in call\n");

		// 01H // VMID_SEL_NORMAL, BIAS_ENA, MICB1_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
		val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
		val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

		// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
		wm8994_write(codec, WM8994_GPIO_1, 0xA101);

		// 02H //
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
		val &= ~(WM8994_MIXINL_ENA_MASK|WM8994_MIXINR_ENA_MASK);
		val |= (WM8994_MIXINL_ENA|WM8994_MIXINR_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

		// 04H // AIF1ADC1L_ENA, ADCL_ENA // BT CALL = WM8994_AIF2ADCL_ENA|WM8994_ADCL_ENA
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
		val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_AIF1ADC1R_ENA_MASK|WM8994_ADCL_ENA_MASK|WM8994_ADCR_ENA_MASK);
		val |= (WM8994_AIF1ADC1L_ENA|WM8994_AIF1ADC1R_ENA|WM8994_ADCL_ENA|WM8994_ADCR_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

		//R1542(606h) - 0x0000
		val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
		val &= ~(WM8994_AIF2DACL_TO_AIF1ADC1L_MASK|WM8994_ADC1L_TO_AIF1ADC1L_MASK);
		val |= (WM8994_AIF2DACL_TO_AIF1ADC1L|WM8994_ADC1L_TO_AIF1ADC1L);
		wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

		//R1543(607h) - 0x0000
		val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
		val &= ~(WM8994_AIF2DACR_TO_AIF1ADC1R_MASK|WM8994_ADC1R_TO_AIF1ADC1R_MASK);
		val |= (WM8994_AIF2DACR_TO_AIF1ADC1R|WM8994_ADC1R_TO_AIF1ADC1R);
		wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);
	}
	else
	{
		printk(SND_KERN_DEBUG "[WM8994] Recording Bluetooth mic - normal\n");
	}

	printk(SND_KERN_DEBUG "[WM8994] wm8994_record_bluetooth_mic()\n");
}

#define FUNCTION_LIST_PLAYBACK

void wm8994_set_playback_receiver(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

	DEBUG_LOG("");

	if(!wm8994->testmode_config_flag)
	{
		// 31H // DAC1L_MIXOUTL_VOL set
	    val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
		val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
		val |= (TUNING_RCV_OUTMIX5_VOL<<WM8994_DACL_MIXOUTL_VOL_SHIFT);
		wm8994_write(codec, WM8994_OUTPUT_MIXER_5, val);

		// 32H // DAC1R_MIXOUTR_VOL set
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
		val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
		val |= (TUNING_RCV_OUTMIX6_VOL<<WM8994_DACR_MIXOUTR_VOL_SHIFT);
		wm8994_write(codec, WM8994_OUTPUT_MIXER_6, val);

		// 20H // MIXOUTL_VOL set
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK|WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTL_MUTE_N|TUNING_RCV_OPGAL_VOL);
		wm8994_write(codec, WM8994_LEFT_OPGA_VOLUME, val);

		// 21H // MIXOUTR_VOL set
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK|WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTR_MUTE_N|TUNING_RCV_OPGAR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OPGA_VOLUME, val);

		// 1FH // HPOUT2_VOL set (0dB, 0dB/-6dB)
        val = wm8994_read(codec, WM8994_HPOUT2_VOLUME);
		val &= ~(WM8994_HPOUT2_MUTE_MASK|WM8994_HPOUT2_VOL_MASK);
        val |= (TUNING_HPOUT2_VOL<<WM8994_HPOUT2_VOL_SHIFT);
		wm8994_write(codec, WM8994_HPOUT2_VOLUME, val);

		// 610H // DAC1L_VOL set
		val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
		val |= (WM8994_DAC1_VU|TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

		// 611H // DAC1R_VOL set (meaningless)
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
		val |= (WM8994_DAC1_VU|TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);
	}

	// 2DH // DAC1L_TO_MIXOUTL set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	// 2EH // DAC1R_TO_MIXOUTR set (meaningless)
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	// 33H // MIXOUTLVOL/R_TO_HPOUT2 set
	val = wm8994_read(codec, WM8994_HPOUT2_MIXER);
	val &= ~(WM8994_MIXOUTLVOL_TO_HPOUT2_MASK|WM8994_MIXOUTRVOL_TO_HPOUT2_MASK);
	val |= (WM8994_MIXOUTRVOL_TO_HPOUT2|WM8994_MIXOUTLVOL_TO_HPOUT2);
	wm8994_write(codec, WM8994_HPOUT2_MIXER, val);

	// 05H // AIF1DACL/R_ENA, DAC1L_ENA set (DAC1R is not used)
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	//val &= ~(WM8994_DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_AIF1DAC1L_ENA_MASK);
	//val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA|WM8994_DAC1R_ENA);
	val &= ~(WM8994_AIF1DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	// 420H // AIF1DAC1 enable (use mono mode for stereo source)
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE|WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	// 601H // AIF1DAC1L_TO_DAC1L set
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	// 602H // AIF1DAC1R_TO_DAC1R set
	//val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	//val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	//val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, 0);//val);

	// 208H // Clock set (meaningless)
	//val = wm8994_read(codec, WM8994_CLOCKING_1);
	//val &= ~(WM8994_DSP_FS1CLK_ENA_MASK|WM8994_DSP_FSINTCLK_ENA_MASK);
	//val |= (WM8994_DSP_FS1CLK_ENA|WM8994_DSP_FSINTCLK_ENA);
	//wm8994_write(codec, WM8994_CLOCKING_1, val);

	// 03H // MIXOUTL/R_ENA, MIXOUTL/RVOL_ENA set
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_LINEOUT2N_ENA_MASK|WM8994_LINEOUT2P_ENA_MASK|WM8994_MIXOUTLVOL_ENA_MASK|WM8994_MIXOUTRVOL_ENA_MASK|
		WM8994_MIXOUTL_ENA_MASK|WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_MIXOUTL_ENA|WM8994_MIXOUTR_ENA|WM8994_MIXOUTRVOL_ENA|WM8994_MIXOUTLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	// 01H // HPOUT2_ENA set
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_HPOUT2_ENA_MASK);
	val |= (WM8994_BIAS_ENA|WM8994_VMID_SEL_NORMAL|WM8994_HPOUT2_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

    // set digital gain 301h / 311h
    val = wm8994_read(codec, WM8994_AIF1_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);

    val = wm8994_read(codec, WM8994_AIF2_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF2DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);
    
	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_playback_receiver()\n");
}


void wm8994_set_playback_headset(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val, val1;

	u16 valBefore = 0;
	u16 valAfter = 0;
	u16 valLow1 = 0;
	u16 valHigh1 = 0;
	u8 valLow = 0;
	u8 valHigh = 0;

	DEBUG_LOG("");

	// 601H // AIF1DAC1L_TO_DAC1L set
	// Configuring the Digital Paths
	// Enable the Timeslot0 to DAC1L
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= WM8994_AIF1DAC1L_TO_DAC1L;
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	// 602H // AIF1DAC1R_TO_DAC1R set
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= WM8994_AIF1DAC1R_TO_DAC1R;
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	/* DC Servo sequence */
	val = wm8994_read(codec, 0x102);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, 0x56);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec, 0x56, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0000);
	val = (0x0000);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, WM8994_CLASS_W_1);
	val &= ~(0x0005);
	val |= (0x0005);
	wm8994_write(codec, WM8994_CLASS_W_1, val);

	// Headset Control
	if(!wm8994->testmode_config_flag)
	{
		// 1CH // HPOUT1L_VOL set
		val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK|WM8994_HPOUT1L_VOL_MASK);

        val1= wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
		val1&= ~(WM8994_HPOUT1R_MUTE_N_MASK|WM8994_HPOUT1R_VOL_MASK);

		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1L_MUTE_N|TUNING_MP3_OUTPUTL_VOL);
		val1|= (WM8994_HPOUT1_VU|WM8994_HPOUT1R_MUTE_N|TUNING_MP3_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME,val1);

#if 0
		// 20H // MIXOUTL_VOL set
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK|WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTL_MUTE_N|TUNING_MP3_OPGAL_VOL);
		wm8994_write(codec, WM8994_LEFT_OPGA_VOLUME, val);

		// 21H // MIXOUTR_VOL set
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK|WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTR_MUTE_N|TUNING_MP3_OPGAR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OPGA_VOLUME, val);
#endif
	}

	/* DC Servo sequence */
	val = wm8994_read(codec, WM8994_DC_SERVO_2);
	val &= ~(0x03E0);
	val = (0x03E0);
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	// 01H // HPOUT1L/R_ENA set
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_HPOUT1L_ENA_MASK|WM8994_HPOUT1R_ENA_MASK);
	val |= (WM8994_BIAS_ENA|WM8994_VMID_SEL_NORMAL|WM8994_HPOUT1R_ENA|WM8994_HPOUT1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	// 60H // HP Output set
	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
	val &= ~(0x0022);
	val = 0x0022;
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	// 4CH // CP_ENA set (charge pump on)
	val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
	val &= ~(WM8994_CP_ENA_MASK);
	val |= (WM8994_CP_ENA|WM8994_CP_ENA_DEFAULT); // from wolfson
	wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

	msleep(5);// 20ms delay

	// 05H // AIF1DAC1L/R_ENA, DAC1L/R_ENA set
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_AIF1DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK|WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA|WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	// 2DH // DAC1L_TO_HPOUT1L set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &=  ~(WM8994_DAC1L_TO_HPOUT1L_MASK|WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	// 2EH // DAC1R_TO_HPOUT1R set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK|WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	// 03H // MIXOUTL/R_ENA, MIXOUTL/RVOL_ENA set
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_LINEOUT2N_ENA_MASK|WM8994_LINEOUT2P_ENA_MASK|WM8994_MIXOUTLVOL_ENA_MASK|WM8994_MIXOUTRVOL_ENA_MASK|
		WM8994_MIXOUTL_ENA_MASK|WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_MIXOUTL_ENA|WM8994_MIXOUTR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	/* DC Servo sequence */
#if 1 // test DC Servo Sequence
	if (!wm8994->dc_servo[DCS_MEDIA]) {
		wait_for_dc_servo(codec, (WM8994_DCS_TRIG_SERIES_0|WM8994_DCS_TRIG_SERIES_1));

		valBefore = wm8994_read(codec, WM8994_DC_SERVO_4);
		valLow = (signed char)(valBefore & 0xff);
		valHigh = (signed char)((valBefore>>8) & 0xff);
		valLow1 = ((signed short)(valLow-5)) & 0x00ff;
		valHigh1 = (((signed short)(valHigh-5)<<8) & 0xff00);
		valAfter = (valLow1|valHigh1);
	} else {
		valAfter = wm8994->dc_servo[DCS_MEDIA];
	}

	wm8994_write(codec, WM8994_DC_SERVO_4, valAfter);
	wm8994->dc_servo[DCS_MEDIA] = valAfter;
	wait_for_dc_servo(codec, (WM8994_DCS_TRIG_DAC_WR_0|WM8994_DCS_TRIG_DAC_WR_1));
#else
	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x0303);
	val = (0x0303);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(160); // 160ms delay

	valBefore = wm8994_read(codec, WM8994_DC_SERVO_4);

	valLow=(signed char)(valBefore & 0xff);
	valHigh=(signed char)((valBefore>>8)&0xff);
	valLow1 = ((signed short)(valLow-5))&0x00ff;
	valHigh1 = (((signed short)(valHigh-5))<<8)&0xff00;
	valAfter = (valLow1|valHigh1);
	wm8994_write(codec, WM8994_DC_SERVO_4, valAfter);

	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x000F);
	val = (0x000F);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(20);
#endif
	// 60H // HP Output set
	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
	val &= ~(WM8994_HPOUT1R_DLY_MASK|WM8994_HPOUT1R_OUTP_MASK|WM8994_HPOUT1R_RMV_SHORT_MASK|
		WM8994_HPOUT1L_DLY_MASK|WM8994_HPOUT1L_OUTP_MASK|WM8994_HPOUT1L_RMV_SHORT_MASK);
	val = (WM8994_HPOUT1L_RMV_SHORT|WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY|WM8994_HPOUT1R_RMV_SHORT|
		WM8994_HPOUT1R_OUTP|WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	// 610H // DAC1L_VOL set
	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

	// 611H // DAC1R_VOL set
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (TUNING_DAC1R_VOL);
	wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

	// 420H // AIF1DAC1 enable
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

    // set digital gain 301h / 311h
    val = wm8994_read(codec, WM8994_AIF1_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);

    val = wm8994_read(codec, WM8994_AIF2_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF2DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);
    
	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_playback_headset()\n");
}

void wm8994_set_playback_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

	DEBUG_LOG("");

	// 01H // All output off for preventing pop up noise.
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0003);
	//wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	// 03H // SPKLVOL_ENA set
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_SPKLVOL_ENA_MASK);
	val |= (WM8994_SPKLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	if(!wm8994->testmode_config_flag)
	{
		// 22H // SPKMIXL_VOL set (max 0dB)
		val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
		val &= ~(WM8994_SPKMIXL_VOL_MASK);
		val |= (TUNING_SPKMIXL_ATTEN);
		wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);

		// 23H // SPKMIXR_VOL off (max 0dB) (Unused)
		val = wm8994_read(codec, WM8994_SPKMIXR_ATTENUATION);
		val &= ~(WM8994_SPKMIXR_VOL_MASK);
		//val |= (TUNING_SPKMIXR_ATTEN);
		wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);

		// 26H // SPKOUTL_VOL set
		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK|WM8994_SPKOUTL_VOL_MASK);
		val |= (WM8994_SPKOUT_VU|WM8994_SPKOUTL_MUTE_N|TUNING_MP3_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);

		// 27H // SPKOUTR_VOL set
		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK|WM8994_SPKOUTR_VOL_MASK);
		//val |= (WM8994_SPKOUT_VU|WM8994_SPKOUTR_MUTE_N|TUNING_MP3_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);

		// 25H // SPKOUTL_BOOST set (max +12dB) (Boost only)
		val = wm8994_read(codec, WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		val |= (TUNING_MP3_CLASSD_VOL<<WM8994_SPKOUTL_BOOST_SHIFT);
		wm8994_write(codec, WM8994_CLASSD, val);

		// 610H // DAC1L_VOL set
		val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
		val |= (TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

		// 611H // DAC1R_VOL set
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
		val |= (WM8994_DAC1_VU|TUNING_DAC1R_VOL); //0 db volume
		wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);
	}

	// 24H // SPKMIXL_TO_SPKOUTL set (R Channel unused)
	val = wm8994_read(codec, WM8994_SPKOUT_MIXERS);
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK|WM8994_SPKMIXR_TO_SPKOUTL_MASK|
		WM8994_SPKMIXR_TO_SPKOUTR_MASK|WM8994_SPKMIXL_TO_SPKOUTR_MASK);
	val |= (WM8994_SPKMIXL_TO_SPKOUTL);
	wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

#if 0 // 20100915, soukjin.bae, open DAC1L only
	//Unmute the DAC path
	val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
	val &= ~(WM8994_MIXOUTL_TO_SPKMIXL_MASK|WM8994_MIXOUTR_TO_SPKMIXR_MASK|
		WM8994_DAC1L_TO_SPKMIXL_MASK);
	val |= (WM8994_DAC1L_TO_SPKMIXL);
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);
#else
    wm8994_write(codec, WM8994_SPEAKER_MIXER, WM8994_DAC1L_TO_SPKMIXL);
#endif

	// 05H // AIF1DAC1L/R_ENA, DAC1L_ENA set
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_AIF1DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	// 420H // AIF1DAC1 enable (use mono mode for stereo source)
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE|WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	// 601H // AIF1DAC1L_TO_DAC1L set
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	// 602H // AIF1DAC1R_TO_DAC1R unset
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	//val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	// 01H // SPKOUTL_ENA set
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
	val |= (WM8994_SPKOUTL_ENA|WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

    // set digital gain 301h / 311h
    val = wm8994_read(codec, WM8994_AIF1_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);

    val = wm8994_read(codec, WM8994_AIF2_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF2DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);
    
	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_playback_speaker()\n");
}

void wm8994_set_playback_speaker_headset(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

	u16 nReadServo4Val = NULL;
	u16 ncompensationResult = NULL;
	u16 nCompensationResultLow = NULL;
	u16 nCompensationResultHigh = NULL;
	u8 nServo4Low = 0;
	u8 nServo4High = 0;

	DEBUG_LOG("");

	//------------------  Common Settings ------------------
	// Enable the Timeslot0 to DAC1L
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	//Enable the Timeslot0 to DAC1R
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	//------------------  Speaker Path Settings ------------------

	// Speaker Volume Control
	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK|WM8994_SPKOUTL_VOL_MASK);
		val |= (WM8994_SPKOUTL_MUTE_N|TUNING_MP3_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);

		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK|WM8994_SPKOUTR_VOL_MASK);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);

		val = wm8994_read(codec, WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		val |= (TUNING_MP3_CLASSD_VOL<<WM8994_SPKOUTL_BOOST_SHIFT);
		wm8994_write(codec, WM8994_CLASSD, val);
	}

	val = wm8994_read(codec, WM8994_SPKOUT_MIXERS);
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK|WM8994_SPKMIXR_TO_SPKOUTL_MASK|
		WM8994_SPKMIXR_TO_SPKOUTR_MASK|WM8994_SPKMIXL_TO_SPKOUTR_MASK);
	val |= (WM8994_SPKMIXL_TO_SPKOUTL);
	wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

	//Unmute the DAC path
	val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
	val &= ~(WM8994_MIXOUTL_TO_SPKMIXL_MASK|WM8994_MIXOUTR_TO_SPKMIXR_MASK|
		WM8994_DAC1L_TO_SPKMIXL_MASK);
	val |= (WM8994_DAC1L_TO_SPKMIXL);
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

	//------------------  Ear Path Settings ------------------
	//Configuring the Digital Paths

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, 0x56);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec,0x56, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0000);
	val = (0x0000);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, WM8994_CLASS_W_1);
	val &= ~(0x0005);
	val = (0x0005);
	wm8994_write(codec, WM8994_CLASS_W_1, val);

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK|WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1L_MUTE_N|TUNING_DUAL_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK|WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1R_MUTE_N|TUNING_DUAL_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	}

	// DC Servo Series Count
	val = 0x03E0;
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_HPOUT1L_ENA_MASK|WM8994_HPOUT1R_ENA_MASK|WM8994_SPKOUTL_ENA_MASK);
	val |= (WM8994_BIAS_ENA|WM8994_VMID_SEL_NORMAL|WM8994_HPOUT1R_ENA|WM8994_HPOUT1L_ENA|WM8994_SPKOUTL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = (WM8994_HPOUT1L_DLY|WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	// Enable Charge Pump
	val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
	val &= ~(WM8994_CP_ENA_MASK);
	val |= (WM8994_CP_ENA|WM8994_CP_ENA_DEFAULT); // this is from wolfson
	wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

	msleep(5);

	// Enable DAC1 and DAC2 and the Timeslot0 for AIF1
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_AIF1DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA|WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	// 2DH // DAC1L_TO_HPOUT1L set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK|WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	// 2EH // DAC1R_TO_HPOUT1R set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK|WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	// 03H // MIXOUTL/R_ENA, SPKLVOL_ENA set
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_LINEOUT2N_ENA_MASK|WM8994_LINEOUT2P_ENA_MASK|WM8994_MIXOUTLVOL_ENA_MASK|WM8994_MIXOUTRVOL_ENA_MASK|
		WM8994_MIXOUTL_ENA_MASK|WM8994_MIXOUTR_ENA_MASK|WM8994_SPKLVOL_ENA_MASK);
	val |= (WM8994_MIXOUTL_ENA|WM8994_MIXOUTR_ENA|WM8994_SPKLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	// DC Servo
	val = (WM8994_DCS_TRIG_SERIES_1|WM8994_DCS_TRIG_SERIES_0|WM8994_DCS_ENA_CHAN_1|WM8994_DCS_ENA_CHAN_0);
	wm8994_write(codec, WM8994_DC_SERVO_1, 0x0303);

	msleep(160);

	nReadServo4Val = wm8994_read(codec, WM8994_DC_SERVO_4);
	nServo4Low = (signed char)(nReadServo4Val & 0xff);
	nServo4High = (signed char)((nReadServo4Val>>8)&0xff);

	nCompensationResultLow = ((signed short)nServo4Low-5)&0x00ff;
	nCompensationResultHigh = (((signed short)(nServo4High-5))<<8)&0xff00;
	ncompensationResult = (nCompensationResultLow|nCompensationResultHigh);
	wm8994_write(codec, WM8994_DC_SERVO_4, ncompensationResult);

	val = (WM8994_DCS_TRIG_DAC_WR_1|WM8994_DCS_TRIG_DAC_WR_0|WM8994_DCS_ENA_CHAN_1|WM8994_DCS_ENA_CHAN_0);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(15);

	// Intermediate HP settings
	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
	val &= ~(WM8994_HPOUT1R_DLY_MASK|WM8994_HPOUT1R_OUTP_MASK|WM8994_HPOUT1R_RMV_SHORT_MASK|
		WM8994_HPOUT1L_DLY_MASK|WM8994_HPOUT1L_OUTP_MASK|WM8994_HPOUT1L_RMV_SHORT_MASK);
	val |= (WM8994_HPOUT1L_RMV_SHORT|WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY|WM8994_HPOUT1R_RMV_SHORT|
		WM8994_HPOUT1R_OUTP|WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	if(!wm8994->testmode_config_flag)
	{
		//Unmute DAC1 left
		val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
		val |= (TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
		val |= (TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);
	}

	//------------------  Common Settings ------------------
	// Unmute the AIF1DAC1
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE|WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	if(!wm8994->testmode_config_flag)
	{
		// Unmute the SPKMIXVOLUME
		val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
		val &= ~(WM8994_SPKMIXL_VOL_MASK);
		val |= (TUNING_SPKMIXL_ATTEN);
		wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);

		val = wm8994_read(codec, WM8994_SPKMIXR_ATTENUATION);
		val &= ~(WM8994_SPKMIXR_VOL_MASK);
		wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);
	}

    // set digital gain 301h / 311h
    val = wm8994_read(codec, WM8994_AIF1_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);

    val = wm8994_read(codec, WM8994_AIF2_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF2DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);
    
	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_playback_speaker_headset()\n");
}


void wm8994_set_playback_extra_dock_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

	DEBUG_LOG("");

	//OUTPUT mute
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_LINEOUT2N_ENA_MASK|WM8994_LINEOUT2P_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	if(!wm8994->testmode_config_flag)
	{
        val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
		val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
        val |= (TUNING_EXTRA_DOCK_SPK_OUTMIX5_VOL<<WM8994_DACL_MIXOUTL_VOL_SHIFT);
        wm8994_write(codec, WM8994_OUTPUT_MIXER_5, val);

		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
		val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
		val |= (TUNING_EXTRA_DOCK_SPK_OUTMIX6_VOL<<WM8994_DACR_MIXOUTR_VOL_SHIFT);
		wm8994_write(codec, WM8994_OUTPUT_MIXER_6, val);

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK|WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTL_MUTE_N|TUNING_MP3_EXTRA_DOCK_SPK_OPGAL_VOL);
		wm8994_write(codec, WM8994_LEFT_OPGA_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK|WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTR_MUTE_N|TUNING_MP3_EXTRA_DOCK_SPK_OPGAR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OPGA_VOLUME, val);

		val = wm8994_read(codec, WM8994_LINE_OUTPUTS_VOLUME);
		val &= ~(WM8994_LINEOUT2_VOL_MASK);
		val |= (TUNING_MP3_EXTRA_DOCK_SPK_VOL<<WM8994_LINEOUT2_VOL_SHIFT);
		wm8994_write(codec, WM8994_LINE_OUTPUTS_VOLUME, val);

		//Unmute DAC1 left
		val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
		val |= (TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
		val |= (TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

		//val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		//val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		//val |= (WM8994_AIF1DAC1_VU|TUNING_DAC1L_VOL);
		//wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		//val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		//val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		//val |= (WM8994_AIF1DAC1_VU|TUNING_DAC1R_VOL);
		//wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}

	// 2DH // DAC1L_TO_HPOUT1L set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	val = wm8994_read(codec, WM8994_LINE_MIXER_2);
	val &= ~(WM8994_MIXOUTR_TO_LINEOUT2N_MASK|WM8994_MIXOUTL_TO_LINEOUT2N_MASK|WM8994_LINEOUT2_MODE_MASK|WM8994_MIXOUTR_TO_LINEOUT2P_MASK);
	//val |= (WM8994_MIXOUTL_TO_LINEOUT2N|WM8994_LINEOUT2_MODE|WM8994_MIXOUTR_TO_LINEOUT2P); // singled-ended output
	val |= (WM8994_MIXOUTR_TO_LINEOUT2P); // differential output
	wm8994_write(codec, WM8994_LINE_MIXER_2, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_AIF1DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA|WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE|WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	val = wm8994_read(codec, WM8994_LINE_OUTPUTS_VOLUME);
	val &= ~(WM8994_LINEOUT2N_MUTE_MASK|WM8994_LINEOUT2P_MUTE_MASK);
	wm8994_write(codec, WM8994_LINE_OUTPUTS_VOLUME, val);

	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	//val = wm8994_read(codec, WM8994_CLOCKING_1);
	//val &= ~(WM8994_DSP_FS1CLK_ENA_MASK|WM8994_DSP_FSINTCLK_ENA_MASK);
	//val |= (WM8994_DSP_FS1CLK_ENA|WM8994_DSP_FSINTCLK_ENA);
	//wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_LINEOUT2N_ENA_MASK|WM8994_LINEOUT2P_ENA_MASK|WM8994_MIXOUTLVOL_ENA_MASK|WM8994_MIXOUTRVOL_ENA_MASK|WM8994_MIXOUTL_ENA_MASK|WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_LINEOUT2N_ENA|WM8994_LINEOUT2P_ENA|WM8994_MIXOUTL_ENA|WM8994_MIXOUTR_ENA|WM8994_MIXOUTRVOL_ENA|WM8994_MIXOUTLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_HPOUT2_ENA_MASK);
	val |= (WM8994_BIAS_ENA|WM8994_VMID_SEL_NORMAL);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

    // set digital gain 301h / 311h
    val = wm8994_read(codec, WM8994_AIF1_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);

    val = wm8994_read(codec, WM8994_AIF2_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF2DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);

	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_playback_extra_dock_speaker()\n");
}

#define FUNCTION_LIST_VOICECALL

void wm8994_set_voicecall_receiver(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	int val;

	DEBUG_LOG("");

	audio_ctrl_mic_bias_gpio(codec, '1');

	/* CHARGE PUMP */
	wm8994_write(codec, 0x4C, 0x1F25); // Charge Pump

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 0x6240); // PM_2 // TSHUT_ENA, TSHUT_OPDIS, MIXINL_ENA, IN1L_ENA

	/* DIGITAL - AIF1DAC1 */
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, 0x0303); // AIF1DAC1L/R_ENA, DAC1L/R_ENA
	wm8994_write(codec,0x420, 0x0000); // AIF1DAC1 On
	wm8994_write(codec,0x601, 0x0001); // AIF1DAC1L_TO_DAC1L
	wm8994_write(codec,0x602, 0x0001); // AIF1DAC1R_TO_DAC1R

	val = wm8994_read(codec, WM8994_AIF1_CONTROL_2); 
	val &= ~(WM8994_AIF1DAC_BOOST_MASK);
	val |= (0x00<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
	wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);

	val = wm8994_read(codec, WM8994_AIF2_CONTROL_2); 
	val &= ~(WM8994_AIF1DAC_BOOST_MASK);
	val |= (0x00<<WM8994_AIF2DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
	wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);
    
	if(!wm8994->testmode_config_flag)
	{
		/* ANALOG INPUT PATH */
		wm8994_write(codec, 0x1A, 0x018B); // IN1RN/P(EAR_MIC_P/N) DISABLE // 0x0B(0.0dB)
		wm8994_write(codec, 0x18, 0x010B); // IN1LN/P(MAIN_MIC_P/N) // 0x0B(0.0dB)
		wm8994_write(codec, 0x28, 0x0030); // IN1LN/P(MAIN_MIC_P/N)_TO_IN1L
		//wm8994_write(codec, 0x29, 0x0020); // IN1L_TO_MIXINL, 0dB

		wm8994_write(codec, 0x2C, 0x0002); // 2Ch // [DG18-1445] // IN2LRP_MIXINR_VOL 0x03(-6dB)

		// HPOUT2
		wm8994_write(codec, 0x1F, 0x0000); // HPOUT2(RCV) Unmute, 0x0(0dB, not -6dB)

		// DAC1L/RVOL
		wm8994_write(codec,0x610, 0x00C0); // DAC1L Digital Vol 0xC0(0.0dB)
		wm8994_write(codec,0x611, 0x01C0); // DAC1R Digital Vol 0xC0(0.0dB), update!
	}

	/* ANALOG OUTPUT - LINEOUT1N/P(To BaseBand) */
	wm8994_write(codec, 0x1E, 0x0006); // LINEOUT1N/P(CP_MIC_N/P) 0x0(0dB)
	wm8994_write(codec, 0x34, 0x0002); // IN1L_TO_LINEOUT1P(diff.)

	/* OUTPUT MIXER */
	wm8994_write(codec, 0x2D, 0x0001); // DAC1L_TO_MIXOUTL
	wm8994_write(codec, 0x2E, 0x0001); // DAC1R_TO_MIXOUTR

	/* ANALOG OUTPUT CONFIGRATION */
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x30F0); // LINEOUT1N/P_ENA, MIXOUTL/RVOL_ENA, MIXOUTL/R_ENA
	wm8994_write(codec, 0x33, 0x0038); // IN2LRP_TO_HPOUT2(direct), MIXOUTL/RVOL_TO_HPOUT2
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0803); // HPOUT2_ENA, VMID_SEL_NORMAL, BIAS_ENA

    if (Record_on_Call)
        wm8994_record_main_mic(codec);

    wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, 0);//val);

	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voicecall_receiver()\n");
}


void wm8994_set_voicecall_headset(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	int val;

	DEBUG_LOG("");

#if 0
    if (wm8994->prev_path == SPK)
    {
        close_rx_path(codec, SPK);
        //audio_ctrl_mic_bias_off();
    }
#endif

	audio_ctrl_mic_bias_gpio(codec, '2');

	u16 valBefore = 0;
	u16 valAfter = 0;
	u16 valLow1 = 0;
	u16 valHigh1 = 0;
	u8 valLow = 0;
	u8 valHigh = 0;

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0003);

	wm8994_write(codec, 0x1F, 0x0020); // HPOUT2_MUTE

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x0000); // MIXOUTL/R_VOL // OFF

	// Digital Path Enables and Unmutes
#if 0 // poptup
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, 0x0021);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, 0x0021);
#else
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, 0x0001);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, 0x0001);
#endif

	// Analogue Input Configuration
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 0x6310); // MIXINL/R_ENA, IN1R_ENA

	//wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 0x0000);

	val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME); // IN1LN/P(MAIN_MIC_P/N)
	val &= ~(WM8994_IN1L_VOL_MASK); // Mute IN1L
	//val |= (WM8994_IN1L_VU|WM8994_IN1L_MUTE|TUNING_CALL_EAR_INPUTMIX_VOL);
	wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME); // IN1RN/P(EAR_MIC_P/N) // [DB16-2232] 0x0B(0.0dB) HW req.
		val &= ~(WM8994_IN1R_MUTE_MASK|WM8994_IN1R_VOL_MASK); // Unmute IN1R
		if (!Earset_Loopback_Chk)
		    val |= (WM8994_IN1R_VU|TUNING_CALL_EAR_INPUTMIX_VOL);
        else
            val |= (WM8994_IN1R_VU|TUNING_LOOPBACK_EAR_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME, val);

		// unmute right pga, set volume
		val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
		val&= ~(WM8994_IN1R_TO_MIXINR_MASK|WM8994_IN1R_MIXINR_VOL_MASK|WM8994_MIXOUTR_MIXINR_VOL_MASK);
		val |= (WM8994_IN1R_TO_MIXINR);//0db
		wm8994_write(codec, WM8994_INPUT_MIXER_4, val);

		//wm8994_write(codec, WM8994_INPUT_MIXER_6, 0x0001); // 2Ch // [DE03-1723] // IN2LRP_MIXINR_VOL 0x01(-12dB)
		wm8994_write(codec, WM8994_INPUT_MIXER_5, 0x0001); // 2Bh // [DF29-1315] // IN2LRP_MIXINL_VOL 0x01 (-12dB)
	}

	wm8994_write(codec, WM8994_INPUT_MIXER_2, 0x0003); // IN1RN(EAR_MIC_P)_TO_IN1R

	// code add
	wm8994_write(codec, WM8994_LINE_MIXER_1, 0x0004); // IN1R_TO_LINEOUT1P(diff.)

	wm8994_write(codec, WM8994_LINE_OUTPUTS_VOLUME, 0x0006); // LINEOUT1N/P On, LINEOUT1_VOL 0dB(MAX)
	//~code add

	//wm8994_write(codec, 0x600, 0x000C); // Left // test - DD24-0226
	//wm8994_write(codec, 0x600, 0x0180); // Right

	// Unmute
	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK|WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTL_MUTE_N|TUNING_CALL_OPGAL_VOL);
		wm8994_write(codec, WM8994_LEFT_OPGA_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK|WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTR_MUTE_N|TUNING_CALL_OPGAR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OPGA_VOLUME, val);
	}

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0003);
	val = 0x0003;
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, 0x56);
	val &= ~(0x0003);
	val = 0x0003;
	wm8994_write(codec,0x56, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0000);
	val = 0x0000;
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, WM8994_CLASS_W_1);
	val &= ~(0x0005);
	val |= 0x0005;
	wm8994_write(codec, WM8994_CLASS_W_1, val);

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK|WM8994_HPOUT1L_VOL_MASK);
		//val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1L_MUTE_N|TUNING_CALL_OUTPUTL_VOL);
		if (!Earset_Loopback_Chk)
    		val |= (WM8994_HPOUT1L_MUTE_N|TUNING_CALL_OUTPUTL_VOL);
        else
            val |= (WM8994_HPOUT1L_MUTE_N|TUNING_LOOPBACK_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK|WM8994_HPOUT1R_VOL_MASK);
		if (!Earset_Loopback_Chk)
    		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1R_MUTE_N|TUNING_CALL_OUTPUTR_VOL);
        else
            val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1R_MUTE_N|TUNING_LOOPBACK_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	}

	val = wm8994_read(codec, WM8994_DC_SERVO_2);
	val &= ~(0x03E0);
	val = 0x03E0;
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0303);
	wm8994_write(codec, 0x60, 0x0022);
	wm8994_write(codec, 0x4C, 0x9F25);

	msleep(250);

    wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, 0x0303);


	/*Analogue Output Configuration*/	
#if 0 // poptup
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, 0x0001); // 2Dh //
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, 0x0001); // 2Eh //
#else
    wm8994_write(codec, WM8994_OUTPUT_MIXER_1, 0x0041); // 2Dh //
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, 0x0081); // 2Eh //
#endif

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x3030); // LINEOUTL/R_ENA, MIXOUTL/R_ENA

	wm8994_write(codec, 0x54, 0x0303);

	msleep(160); // 160ms delay

	valBefore = wm8994_read(codec, WM8994_DC_SERVO_4);
	valLow = (signed char)(valBefore & 0xff);
	valHigh = (signed char)((valBefore>>8) & 0xff);

	valLow1 = ((signed short)valLow-5)&0x00ff;
	valHigh1 = (((signed short)(valHigh-5))<<8)&0xff00;
	valAfter = valLow1|valHigh1;
	wm8994_write(codec, WM8994_DC_SERVO_4, valAfter);

	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x000F);
	val = (0x000F);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(15);

	wm8994_write(codec, 0x60, 0x00EE);

	// Unmute DAC1L/R
	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1R_VOL); //0 db volume
	wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

    val = wm8994_read(codec, WM8994_AIF1_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x02<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);

    val = wm8994_read(codec, WM8994_AIF2_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF2DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);

	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, 0x0000); // 420h //

    Earset_Loopback_Chk = 0;

    if (Record_on_Call)
        wm8994_record_headset_mic(codec);

	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voicecall_headset()\n");
}

void wm8994_set_voicecall_headphone(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	int val;

	DEBUG_LOG("");

	audio_ctrl_mic_bias_gpio(codec, '3');

	u16 valBefore = 0;
	u16 valAfter = 0;
	u16 valLow1 = 0;
	u16 valHigh1 = 0;
	u8 valLow = 0;
	u8 valHigh = 0;

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0003);

	wm8994_write(codec, 0x1F, 0x0020); // HPOUT2_MUTE

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x0000); // MIXOUTL/R_VOL // OFF

	// Digital Path Enables and Unmutes
#if 0 // poptup
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, 0x0021);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, 0x0021);
#else
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, 0x0001);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, 0x0001);
#endif

	// Analogue Input Configuration
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 0x6340); // MIXINL/R_ENA, IN1L_ENA // MAIN MIC

	// test - DD24-0226
	//wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 0x0001);

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME); // IN1LN/P(MAIN_MIC_P/N) // [DB16-2232] 0x0B(0.0dB) HW req.
		val &= ~(WM8994_IN1L_MUTE_MASK|WM8994_IN1L_VOL_MASK); // Unmute IN1L
		val |= (WM8994_IN1L_VU|TUNING_VOICE_CALL_SPK_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

		// unmute left pga, set volume
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val&= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
		val |= (WM8994_IN1L_TO_MIXINL); //0db
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val);

		wm8994_write(codec, WM8994_INPUT_MIXER_6, 0x0001); // 2Ch // [DF29-1315] // IN2LRP_MIXINR_VOL 0x01 (-12dB)
	}

	wm8994_write(codec, WM8994_INPUT_MIXER_2, 0x0030); // IN1LN/P(MAIN_MIC_P/N)_TO_IN1L

	// test //
	//wm8994_write(codec, 0x34, 0x0004); // IN1R_TO_LINEOUT1P(diff.) // 4-POLE Path
	wm8994_write(codec, 0x34, 0x0002); // IN1L_TO_LINEOUT1P(diff.) 	// 3-POLE Path

	wm8994_write(codec, 0x1E, 0x0006); // LINEOUT1N/P On, LINEOUT1_VOL 0dB(MAX)
	//~test //

	// test - DD24-0226
	//wm8994_write(codec, 0x600, 0x0180); // Right

	/* Unmute*/
	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK|WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTL_MUTE_N|TUNING_CALL_OPGAL_VOL);
		wm8994_write(codec, WM8994_LEFT_OPGA_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK|WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTR_MUTE_N|TUNING_CALL_OPGAR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OPGA_VOLUME, val);
	}

	//wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 0x2001); // Power management 4 was here

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0003);
	val = 0x0003;
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, 0x56);
	val &= ~(0x0003);
	val = 0x0003;
	wm8994_write(codec,0x56, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0000);
	val = 0x0000;
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, WM8994_CLASS_W_1);
	val &= ~(0x0005);
	val |= 0x0005;
	wm8994_write(codec, WM8994_CLASS_W_1, val);

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK|WM8994_HPOUT1L_VOL_MASK);
		//val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1L_MUTE_N|TUNING_CALL_OUTPUTL_VOL);
		val |= (WM8994_HPOUT1L_MUTE_N|TUNING_CALL_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK|WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1R_MUTE_N|TUNING_CALL_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	}

	val = wm8994_read(codec, WM8994_DC_SERVO_2);
	val &= ~(0x03E0);
	val = 0x03E0;
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0303);
	wm8994_write(codec, 0x60, 0x0022);
	wm8994_write(codec, 0x4C, 0x9F25);

	msleep(5);

    wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, 0x0303);


	/*Analogue Output Configuration*/	
#if 0 // poptup
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, 0x0001); // 2Dh //
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, 0x0001); // 2Eh //
#else
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, 0x0081); // 2Dh //
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, 0x0041); // 2Eh //
#endif

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x3030); // LINEOUTL/R_ENA, MIXOUTL/R_ENA

	wm8994_write(codec, 0x54, 0x0303);

	msleep(160); // 160ms delay

	valBefore = wm8994_read(codec, WM8994_DC_SERVO_4);
	valLow = (signed char)(valBefore & 0xff);
	valHigh = (signed char)((valBefore>>8) & 0xff);

	valLow1 = ((signed short)valLow-5)&0x00ff;
	valHigh1 = (((signed short)(valHigh-5)<<8)&0xff00);
	valAfter = valLow1|valHigh1;
	wm8994_write(codec, WM8994_DC_SERVO_4, valAfter);

	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x000F);
	val = (0x000F);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(15);

	wm8994_write(codec, 0x60, 0x00EE);

	// Unmute DAC1L/R
	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1R_VOL); //0 db volume
	wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

    val = wm8994_read(codec, WM8994_AIF1_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x02<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);

    val = wm8994_read(codec, WM8994_AIF2_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF2DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);

	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, 0x0000); // 420h //

    if (Record_on_Call)
        wm8994_record_headset_mic(codec);

	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voicecall_headphone()\n");
}

void wm8994_set_voicecall_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	int val;

	DEBUG_LOG("");

#if 0
    if (wm8994->prev_path == HP)
    {
        close_rx_path(codec, HP);
        //audio_ctrl_mic_bias_off();
    }
#endif

	// [DE15-2239] hi99.an // reduction path change noise from (CALL_ACTIVE + JACK_4_POLE_DEVICE) to (CALL_ACTIVE + JACK_4_POLE_DEVICE + SPK)
	wm8994_write(codec, 0x01, 0x0003);

	wm8994_write(codec, 0x1F, 0x0020); // HPOUT2_MUTE

	wm8994_write(codec, 0x03, 0x0000); // MIXOUTL/R_VOL // OFF
	//~[DE15-2239] hi99.an //

#if 0    
    if (wm8994->prev_path == OFF)
    {
        printk(SND_KERN_DEBUG "[WM8994] Patch for soft open when SPK path open.\n");
        
        val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK|WM8994_SPKOUTL_VOL_MASK);
		val |= (WM8994_SPKOUT_VU|WM8994_SPKOUTL_MUTE_N|(TUNING_CALL_SPKL_VOL/3));
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);

		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK|WM8994_SPKOUTR_VOL_MASK);
		val |= (WM8994_SPKOUT_VU|WM8994_SPKOUTL_MUTE_N|(TUNING_CALL_SPKL_VOL/3));
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);

        //msleep(50);
        //close_rx_path(codec, SPK);
    }
    
#endif

    audio_ctrl_mic_bias_gpio(codec, '4');
    
	wm8994_write(codec, 0x601, 0x0001);
	wm8994_write(codec, 0x602, 0x0001);
	//wm8994_write(codec,0x603, 0x000C);

	// Tx -> AIF2 Path
	//wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, WM8994_ADC1_TO_DAC2L);

	/*Analogue Input Configuration*/
	// code add
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 0x6340);
	//~code add
	wm8994_write(codec, 0x28, 0x0030); // IN1LN/P(MAIN_MIC_P/N)_TO_IN1L // IN1LN/P(MAIN_MIC_P/N)_TO_IN1L, IN2RN/P_TO_IN2R (0x3C)

	if(!wm8994->testmode_config_flag)
	{
		// Volume Control - Input
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3); // IN1L_TO_MIXINL 0dB
		val &= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
		val |= (WM8994_IN1L_TO_MIXINL); //0db
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val);

		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME); // IN1LN/P(MAIN_MIC_P/N) // [DB16-2232] 0x0B(0.0dB) HW req.
		val &= ~(WM8994_IN1L_MUTE_MASK|WM8994_IN1L_VOL_MASK); // Unmute IN1L
		val |= (WM8994_IN1L_VU|TUNING_VOICE_CALL_SPK_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

		//wm8994_write(codec, 0x2A, 0x0100); // IN2R_TO_MIXINL 0dB // if 28h reg == 0x3C, uncomment
		//wm8994_write(codec, 0x1B, 0x0111); // IN2R PGA Control // if 28h reg == 0x3C, uncomment
	}

	// Analogue Output Configuration
	// code add
	wm8994_write(codec, 0x34, 0x0002); // IN1L_TO_LINEOUT1P(diff)

	wm8994_write(codec, 0x1E, 0x0006); // LINEOUT1N/P(CP_MIC_N/P) 0x0(0dB, not -6dB)

	wm8994_write(codec, 0x2C, 0x0005); // IN2LRP_MIXINR_VOL[2:0] = b101 // RxVoice to MIXINR

	if(!wm8994->testmode_config_flag)
	{
		wm8994_write(codec, 0x31, 0x0000); // MIXINR_MIXOUTL_VOL 0dB(0x0)
		wm8994_write(codec, 0x32, 0x0000); // MIXINR_MIXOUTR_VOL 0dB(0x0)
	}

	wm8994_write(codec, 0x2D, 0x0081); // MIXINR_TO_MIXOUTL, DAC1L_TO_MIXOUTL
	wm8994_write(codec, 0x2E, 0x0041); // MIXINR_TO_MIXOUTR, DAC1R_TO_MIXOUTR
	//~code add

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x33F0); // LINEOUT1N/P_ENA, SPKl/RVOL_ENA, MIXOUTL/RVOL_ENA, MIXOUTL/R_ENA
	//wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x3330); // LINEOUT1N/P_ENA, SPKl/RVOL_ENA, MIXOUTL/R_ENA
	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, 0X0303); // AIF1DAC1L/R_ENA, DAC1L/R_ENA

	if(!wm8994->testmode_config_flag)
	{
		// Volume Control - Output
		// Unmute the SPKMIXVOLUME
		val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
		val &= ~(WM8994_SPKMIXL_VOL_MASK);
		val |= (TUNING_SPKMIXL_ATTEN);
		wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);

		val = wm8994_read(codec, WM8994_SPKMIXR_ATTENUATION);
		val &= ~(WM8994_SPKMIXR_VOL_MASK);
		val |= (WM8994_SPKOUT_CLASSAB|TUNING_SPKMIXR_ATTEN);
		wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);

		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK|WM8994_SPKOUTL_VOL_MASK);
		val |= (WM8994_SPKOUT_VU|WM8994_SPKOUTL_MUTE_N|TUNING_CALL_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);

		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK|WM8994_SPKOUTR_VOL_MASK);
		val |= (WM8994_SPKOUT_VU|WM8994_SPKOUTL_MUTE_N|TUNING_CALL_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);

		val = wm8994_read(codec, WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		val |= (TUNING_CALL_CLASSD_VOL<<WM8994_SPKOUTL_BOOST_SHIFT);
		wm8994_write(codec, WM8994_CLASSD, val);
	}

	// code add
	wm8994_write(codec, WM8994_SPEAKER_MIXER, 0x000C); // MIXOUTL/R_TO_SPKMIXL/R

	val = wm8994_read(codec, WM8994_SPKOUT_MIXERS);
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK|WM8994_SPKMIXR_TO_SPKOUTL_MASK|
		WM8994_SPKMIXR_TO_SPKOUTR_MASK|WM8994_SPKMIXL_TO_SPKOUTR_MASK);
	val |= (WM8994_SPKMIXL_TO_SPKOUTL|WM8994_SPKMIXR_TO_SPKOUTL); // SPKMIXL/R_TO_SPKOUTL On
	wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);
	//~code add

	// code add
	//wm8994_write(codec, WM8994_SPEAKER_MIXER, 0x000C); // MIXOUTL/R_TO_SPKMIXL/R
	//~code add

	/* Digital Path Enables and Unmutes*/
	//wm8994_write(codec, WM8994_SIDETONE, 0x01C0);

	wm8994_write(codec, 0x60, 0x0000);
	wm8994_write(codec, 0x54, 0x0000); // DC Servo 1

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, WM8994_SPKOUTL_ENA|WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);

	if(!wm8994->testmode_config_flag)
	{
		//Unmute DAC1 left
		val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
		val |= (WM8994_DAC1_VU|TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
		val |= (WM8994_DAC1_VU|TUNING_DAC1R_VOL); //0 db volume
		wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);
	}

    val = wm8994_read(codec, WM8994_AIF1_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);

    val = wm8994_read(codec, WM8994_AIF2_CONTROL_2); 
    val &= ~(WM8994_AIF1DAC_BOOST_MASK);
    val |= (0x00<<WM8994_AIF2DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
    wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);

	//wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, WM8994_AIF1DAC1_UNMUTE);

    if (Record_on_Call)
        wm8994_record_sub_mic(codec);

	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voicecall_speaker()\n");
}

void wm8994_set_voicecall_bluetooth(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	int val;

	DEBUG_LOG("");

#if 1
	/***** from S1-EUR: voice_common_settings() *****/

	/* GPIO Configuration */
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);
	wm8994_write(codec, WM8994_GPIO_2, 0x8100);
	wm8994_write(codec, WM8994_GPIO_3, 0x0100);
	wm8994_write(codec, WM8994_GPIO_4, 0x0100);
	wm8994_write(codec, WM8994_GPIO_5, 0x8100);
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);
	wm8994_write(codec, WM8994_GPIO_7, 0x0100);
	wm8994_write(codec, WM8994_GPIO_8, 0xA101);
	wm8994_write(codec, WM8994_GPIO_9, 0xA101);
	wm8994_write(codec, WM8994_GPIO_10, 0xA101);
	wm8994_write(codec, WM8994_GPIO_11, 0xA101);

	/* FLL2	Setting */
	wm8994_write(codec, WM8994_FLL2_CONTROL_2, 0x2F00); // FLL1 Ctrl2, FLL1 Setting
	wm8994_write(codec, WM8994_FLL2_CONTROL_3, 0x3126); // FLL1 Ctrl3, K Value
	wm8994_write(codec, WM8994_FLL2_CONTROL_4, 0x0100); // FLL1 Ctrl4, N Value
	wm8994_write(codec, WM8994_FLL2_CONTROL_5, 0x0C88); // FLL1 Ctrl5
	wm8994_write(codec, WM8994_FLL2_CONTROL_1, (WM8994_FLL2_FRACN_ENA|WM8994_FLL2_ENA));

	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0018); // AIF2 Clock Source = FLL2
	//wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0009); // Enable AIF2 Clock, AIF2 Clock Source = MCLK2 (from CP)

	/* Clocking - 8KHz */
	wm8994_write(codec, WM8994_AIF2_RATE, (0x3<<WM8994_AIF2CLK_RATE_SHIFT));

	/* AIF2 Interface - PCM Stereo mode */
	wm8994_write(codec, WM8994_AIF2_CONTROL_1, 0x4118); // DSP Mode, BCLK invert, LRCLK normal
    wm8994_write(codec, WM8994_AIF2_BCLK, 0x40);
    wm8994_write(codec, WM8994_AIF2_CONTROL_2, 0x4000); // Left & Right DAC receives left interface data
    wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, 0x7000);
    wm8994_write(codec, WM8994_AIF1_CONTROL_2, 0x4800);

    /* Analog Output Power Management */
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0003);
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 0x6200); // MIXINL_ENA

	/* Digital Path Enables and Unmutes */
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 0x2002); // AIF2ADCL_ENA, ADCL_ENA
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, 0x220A); // AIF2DACL_ENA, AIF1DAC1L_ENA, DAC2L_ENA, DAC1L_ENA

	/* Clocking */
	val = wm8994_read(codec, WM8994_CLOCKING_1);
	val |= (WM8994_DSP_FS2CLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

    wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x0000);
#endif
#if 1
    wm8994_write(codec, 0x2B, 0x0002); // IN2LRP_MIXINL_VOL[2:0], -12dB(0x1) -> -9dB(0x2) // HW req.
    wm8994_write(codec, 0x1E, 0x0010); // WM8994_LINE_OUTPUTS_VOLUME // 0x00(0dB), 0x10(-6dB)

    wm8994_write(codec, 0x20, 0x0179); // MIXOUTL_VOL, 0dB, On
    wm8994_write(codec, 0x21, 0x0139); // MIXOUTR_VOL, 0dB, mute

    wm8994_write(codec, 0x34, 0x0001); // WM8994_LINE_MIXER_1 // LINEOUT1_MODE = diff, MIXOUTL_TO_LINEOUT1P = diff output from MIXOUTL
    wm8994_write(codec, WM8994_GPIO_3, 0x0100); // 702h // GPIO3|BCLK2
    //wm8994_write(codec, WM8994_GPIO_4, 0x0100); // 703h // GPIO4|LRCLK2
    wm8994_write(codec, WM8994_GPIO_5, 0x0100); // 704h // GPIO5|DACDAT2
    //wm8994_write(codec, WM8994_GPIO_7, 0x0100); // 706h // GPIO7|ADCDAT2

    wm8994_write(codec, WM8994_GPIO_8, 0x8100); // GPIO8|DACDAT3
    wm8994_write(codec, WM8994_GPIO_9, 0x0100); // GPIO9|ADCDAT3
    wm8994_write(codec, WM8994_GPIO_10, 0x0100); // GPIO10|LRCLK3
    wm8994_write(codec, WM8994_GPIO_11, 0x0100); // GPIO11|BCLK3

	// If Input MIC is enabled, bluetooth Rx is muted.
    wm8994_write(codec, 0x18, 0x008B);
    wm8994_write(codec, 0x1A, 0x008B);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, 0x0000);
	wm8994_write(codec, WM8994_INPUT_MIXER_3, 0x0000);
	wm8994_write(codec, WM8994_INPUT_MIXER_4, 0x0000);

	//for BT DTMF Play
	//Rx Path: AIF2ADCDAT2 select
	//CP(CALL) Path:GPIO5/DACDAT2 select
	//AP(DTMF) Path: DACDAT1 select
	//Tx Path: GPIO8/DACDAT3 select
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x000A);

	// AIF1 & AIF2 Output is connected to DAC1	
    wm8994_write(codec, 0x600, 0x0000); // WM8994_DAC1_MIXER_VOLUMES

    wm8994_write(codec, 0x601, WM8994_AIF2DACL_TO_DAC1L); // WM8994_DAC1_LEFT_MIXER_ROUTING
    wm8994_write(codec, 0x602, 0x0000); // WM8994_DAC1_RIGHT_MIXER_ROUTING

    wm8994_write(codec, 0x603, 0x000C); // WM8994_DAC2_MIXER_VOLUMES // ADCL_DAC2_VOL[3:0], 0dB

    wm8994_write(codec, 0x604, WM8994_ADC1_TO_DAC2L|WM8994_AIF1DAC1L_TO_DAC2L); // WM8994_DAC2_LEFT_MIXER_ROUTING
    wm8994_write(codec, 0x605, WM8994_ADC1_TO_DAC2R|WM8994_AIF1DAC1R_TO_DAC2R); // WM8994_DAC2_RIGHT_MIXER_ROUTING

    wm8994_write(codec, 0x610, 0x01C0); // DAC1L/R_VOL, 0dB, update
    wm8994_write(codec, 0x611, 0x01C0); 

    wm8994_write(codec, 0x612, 0x01C0); // DAC2L/R_VOL, 0dB, update
    wm8994_write(codec, 0x613, 0x01C0); 

	wm8994_write(codec, WM8994_OVERSAMPLING, 0x0000);
    wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019); // AIF2 Clock Source = FLL2

    /* Analog Output Power Management */
    wm8994_write(codec, WM8994_LINE_MIXER_1, 0x01); // MIXOUTL to LINEOUT1N/P by Single-Ended
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x3020); // LINEOUT1N/P_ENA, MIXOUTLVOL_ENA, MIXOUTL_ENA

	//Unmute DAC1 left
	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (TUNING_DAC1L_VOL);
    wm8994_write(codec, 0x610, val);
	
	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME); 
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (TUNING_DAC1R_VOL);
    wm8994_write(codec, 0x611, val);

    // Enable DAC1L to HPOUT1L path
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, WM8994_DAC1L_TO_MIXOUTL);

	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, 0x0000);
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, 0x0080);
#endif
	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voicecall_bluetooth()\n");
}

#define FUNCTION_LIST_VOIPCALL

void close_output_path_all(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;
	int val;

	DEBUG_LOG("");

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTR_ENA|WM8994_SPKOUTL_ENA|WM8994_HPOUT2_ENA|WM8994_HPOUT1L_ENA|WM8994_HPOUT1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x0000);

	if (wm8994->cur_path == HP)
	{
		wm8994_disable_rec_path(codec, 0);
	}
	else
	{
		// for ear tick noise reduction
		// 01H // HPOUT1L/R_ENA off
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
		val &= ~(WM8994_HPOUT1L_ENA_MASK|WM8994_HPOUT1R_ENA_MASK);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

		// 1CH // HPOUT1L_VOL mute
		val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK|WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|TUNING_VOIPCALL_EAR_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		// 1DH // HPOUT1R_VOL mute
		val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK|WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|TUNING_VOIPCALL_EAR_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);

		// 2DH // DAC1L_TO_HPOUT1L, DAC1L_TO_MIXOUTL off
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
		val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK|WM8994_DAC1L_TO_MIXOUTL_MASK);
		wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

		// 2EH // DAC1R_TO_HPOUT1R, DAC1R_TO_MIXOUTR off
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
		val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK|WM8994_DAC1R_TO_MIXOUTR_MASK);
		wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

		// 4CH // CP_ENA off (charge pump off)
		val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
		val &= ~(WM8994_CP_ENA_MASK);
		val |= (WM8994_CP_ENA_DEFAULT); // from wolfson
		wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

		// 60H // HP Output off
		val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
		val &= ~(WM8994_HPOUT1R_DLY_MASK|WM8994_HPOUT1R_OUTP_MASK|WM8994_HPOUT1R_RMV_SHORT_MASK|
			WM8994_HPOUT1L_DLY_MASK|WM8994_HPOUT1L_OUTP_MASK|WM8994_HPOUT1L_RMV_SHORT_MASK);
		wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

		wm8994_disable_rec_path(codec, 1);
	}

	val = wm8994_read(codec, WM8994_AIF1_CONTROL_2);
	val &= ~(WM8994_AIF1DAC_BOOST_MASK);
	wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);

#ifdef FEATURE_VOIP_DRC
	if this code enabled, build error
	// 481H // 480H // AIF1_DAC1_EQ_GAIN clear
	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_1, 0x6318);
	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_2, 0x6300);
#endif
}

void wm8994_set_voipcall_receiver(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

    close_output_path_all(codec);
	DEBUG_LOG("");
    //gpio_set_value(GPIO_MICBIAS_EN, '6');
    audio_ctrl_mic_bias_gpio(codec, '6');

    val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
	val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
	val |= (TUNING_RCV_OUTMIX5_VOL<<WM8994_DACL_MIXOUTL_VOL_SHIFT);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_5, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
	val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
	val |= (TUNING_RCV_OUTMIX6_VOL<<WM8994_DACR_MIXOUTR_VOL_SHIFT);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_6, val);

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK|WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTL_MUTE_N|TUNING_VOIPCALL_RCV_OPGAL_VOL);
		wm8994_write(codec, WM8994_LEFT_OPGA_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK|WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTR_MUTE_N|TUNING_VOIPCALL_RCV_OPGAR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OPGA_VOLUME, val);
	}

    val = wm8994_read(codec, WM8994_HPOUT2_VOLUME);
	val &= ~(WM8994_HPOUT2_MUTE_MASK|WM8994_HPOUT2_VOL_MASK);
    val |= (TUNING_HPOUT2_VOL<<WM8994_HPOUT2_VOL_SHIFT);
	wm8994_write(codec, WM8994_HPOUT2_VOLUME, val);

	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1L_VOL); // 0xC0 = 0dB (MAX)
	wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1R_VOL);
	wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_2);
		val &= ~(WM8994_AIF1DAC_BOOST_MASK);
		val |= (TUNING_VOIPCALL_RCV_AIF1DAC_BOOST<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
		wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);
	}

#ifdef FEATURE_VOIP_DRC
	if this code enabled, build error
	// 481H // 480H // AIF1_DAC1_EQ_GAIN set
	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_2, 0x9600); // 481H //
	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_1, 0x0199); // 480H //
#endif

	// 2DH // DAC1L_TO_HPOUT1L set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	val = wm8994_read(codec, WM8994_HPOUT2_MIXER);
	val &= ~(WM8994_MIXOUTLVOL_TO_HPOUT2_MASK|WM8994_MIXOUTRVOL_TO_HPOUT2_MASK);
	val |= (WM8994_MIXOUTRVOL_TO_HPOUT2|WM8994_MIXOUTLVOL_TO_HPOUT2);
	wm8994_write(codec, WM8994_HPOUT2_MIXER, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	//val &= ~(WM8994_AIF1DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK|WM8994_DAC1R_ENA_MASK);
	//val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA|WM8994_DAC1R_ENA);
	val &= ~(WM8994_AIF1DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE|WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	// 601H // AIF1DAC1L_TO_DAC1L // Enable timeslot0 to left dac
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	// 602H // AIF1DAC1R_TO_DAC1R // Enable timeslot0 to right dac
	//val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	//val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	//val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, 0);//val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);
	val &= ~(WM8994_DSP_FS1CLK_ENA_MASK|WM8994_DSP_FSINTCLK_ENA_MASK);
	val |= (WM8994_DSP_FS1CLK_ENA|WM8994_DSP_FSINTCLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK|WM8994_MIXOUTRVOL_ENA_MASK|WM8994_MIXOUTL_ENA_MASK|WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_MIXOUTL_ENA|WM8994_MIXOUTR_ENA|WM8994_MIXOUTRVOL_ENA|WM8994_MIXOUTLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_HPOUT2_ENA_MASK);
	val |= (WM8994_BIAS_ENA|WM8994_VMID_SEL_NORMAL|WM8994_HPOUT2_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voipcall_receiver - rec main mic\n");

	// 300H // Mixing left channel output to right channel // val: 0x0010
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
	val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

	wm8994_write(codec, WM8994_ANTIPOP_2, 0x68); // Main mic volume issue fix

	// 01H // VMID_SEL_NORMAL, BIAS_ENA, MICB1_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
	val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	// for popup when RCV->EAR
    //wm8994_write(codec, WM8994_ANALOGUE_HP_1, 0x0);

	// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);

	// 02H // MIXINL_ENA, IN1L_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
	val &= ~(WM8994_MIXINL_ENA_MASK|WM8994_IN1L_ENA_MASK);
	val |= (WM8994_MIXINL_ENA|WM8994_IN1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

	if(!wm8994->testmode_config_flag)
	{
		// 18H // IN1L PGA // IN1L UNMUTE, SET VOL
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);
		val &= ~(WM8994_IN1L_MUTE_MASK|WM8994_IN1L_VOL_MASK);
		val |= (WM8994_IN1L_VU|TUNING_VOIPCALL_RCV_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

		// 29H // MIXINL PGA // IN2L_TO_MIXINL MUTE, IN1L_TO_MIXINL UNMUTE, 0dB
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val &= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
		val |= (WM8994_IN1L_TO_MIXINL);	// BoostOff (0dB)
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
	}

#ifdef FEATURE_VOIP_DRC
	// 400H // AIF1 ADC1 Left Volume // Gain Tuning // H/W req.
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
	val |= (WM8994_AIF1ADC1_VU|TUNING_VOIPCALL_MAIN_AIF1ADCL_VOL); // ADC Digital Gain

	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);
#endif

	// 28H // INPUT MIXER // IN1LP/N_TO_IN1L PGA
	val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
	val &= (WM8994_IN1LP_TO_IN1L_MASK|WM8994_IN1LN_TO_IN1L_MASK);
	val |= (WM8994_IN1LP_TO_IN1L|WM8994_IN1LN_TO_IN1L);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

#ifdef FEATURE_VOIP_DRC
	if this code enabled, build error
	/* DRC Sequence - RCV table // [DJ05-2239] // disable DRC and EQ */
	wm8994_write(codec, WM8994_AIF1_DRC1_1, 0x0098); // 440h // DRC disable
	wm8994_write(codec, WM8994_AIF1_DRC1_5, 0x0355); // 444h //
	wm8994_write(codec, WM8994_AIF1_DRC1_4, 0x03ED); // 443h //
	wm8994_write(codec, WM8994_AIF1_DRC1_3, 0x0C02); // 442h //
	wm8994_write(codec, WM8994_AIF1_DRC1_2, 0x0811); // 441h //
	wm8994_write(codec, WM8994_AIF1_DRC1_1, 0x01BA); // 440h // DRC1 NG enable
//#endif
//#ifdef FEATURE_VOIP_HPFILTER
	// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter on
	val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
	val &= ~(WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
	val |= (WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF); // hi-path filter on (L/R)
	wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, TUNING_VOIPCALL_RCV_ADC1_FILTERS);
#endif

	// 04H // AIF1ADC1L_ENA, ADCL_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
	val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_ADCL_ENA_MASK);
	val |= (WM8994_AIF1ADC1L_ENA|WM8994_ADCL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	// 606H // ADC1L_TO_AIF1ADC1L (TIMESLOT 0) ASSIGN
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
	val |= (WM8994_ADC1L_TO_AIF1ADC1L);
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

    printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voipcall_receiver()\n");
}

void wm8994_set_voipcall_headset(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

    close_output_path_all(codec);
	DEBUG_LOG("");
    audio_ctrl_mic_bias_gpio(codec, '7');

	u16 valBefore = 0;
	u16 valAfter = 0;
	u16 valLow1 = 0;
	u16 valHigh1 = 0;
	u8 valLow = 0;
	u8 valHigh = 0;

	//Configuring the Digital Paths
	// 601H // AIF1DAC1L_TO_DAC1L // Enable timeslot0 to left dac
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	// 602H // AIF1DAC1R_TO_DAC1R // Enable timeslot0 to right dac
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, 0x56);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec,0x56, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0000);
	val = (0x0000);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, WM8994_CLASS_W_1);
	val &= ~(0x0005);
	val |= (0x0005);
	wm8994_write(codec, WM8994_CLASS_W_1, val);

	// Headset Control
	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK|WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1L_MUTE_N|TUNING_VOIPCALL_EAR_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK|WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1R_MUTE_N|TUNING_VOIPCALL_EAR_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK|WM8994_MIXOUTL_VOL_MASK);
		//val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTL_MUTE_N|TUNING_VOIP_EAR_OPGAL_VOL);
		wm8994_write(codec, WM8994_LEFT_OPGA_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK|WM8994_MIXOUTR_VOL_MASK);
		//val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTR_MUTE_N|TUNING_VOIP_EAR_OPGAR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OPGA_VOLUME, val);
	}

	val = wm8994_read(codec, WM8994_DC_SERVO_2);
	val &= ~(0x03E0);
	val = (0x03E0);
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	//Enable vmid,bias, hp left and right
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_HPOUT1L_ENA_MASK|WM8994_HPOUT1R_ENA_MASK);
	val |= (WM8994_BIAS_ENA|WM8994_VMID_SEL_NORMAL|WM8994_HPOUT1R_ENA|WM8994_HPOUT1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
	val &= ~(0x0022);
	val = 0x0022;
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Enable Charge Pump
	val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
	val &= ~(WM8994_CP_ENA_MASK);
	val |= (WM8994_CP_ENA|WM8994_CP_ENA_DEFAULT); // this is from wolfson
	wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

	msleep(5);// 20ms delay

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_2);
		val &= ~(WM8994_AIF1DAC_BOOST_MASK);
		val |= (TUNING_VOIPCALL_EAR_AIF1DAC_BOOST<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
		wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);
	}

#ifdef FEATURE_VOIP_DRC
	if this code enabled, build error
	// 481H // 480H // AIF1_DAC1_EQ_GAIN set
	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_2, 0x9600); // 481H //
	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_1, 0x0199); // 480H //
#endif

	//Enable Dac1 and DAC2 and the Timeslot0 for AIF1
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_AIF1DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA|WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	// 2DH // DAC1L_TO_HPOUT1L set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &=  ~(WM8994_DAC1L_TO_HPOUT1L_MASK|WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	// Enable DAC1R to HPOUT1R path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK|WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK|WM8994_MIXOUTRVOL_ENA_MASK|WM8994_MIXOUTL_ENA_MASK|WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_MIXOUTL_ENA|WM8994_MIXOUTR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x0303);
	val = (0x0303);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(160); // 160ms delay

	valBefore = wm8994_read(codec, WM8994_DC_SERVO_4);

	valLow=(signed char)(valBefore & 0xff);
	valHigh=(signed char)((valBefore>>8)&0xff);
	valLow1 = ((signed short)(valLow-5))&0x00ff;
	valHigh1 = (((signed short)(valHigh-5))<<8)&0xff00;
	valAfter = (valLow1|valHigh1);
	wm8994_write(codec, WM8994_DC_SERVO_4, valAfter);

	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x000F);
	val = (0x000F);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(20);

	// Intermediate HP settings
	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
	val &= ~(WM8994_HPOUT1R_DLY_MASK|WM8994_HPOUT1R_OUTP_MASK|WM8994_HPOUT1R_RMV_SHORT_MASK|
		WM8994_HPOUT1L_DLY_MASK|WM8994_HPOUT1L_OUTP_MASK|WM8994_HPOUT1L_RMV_SHORT_MASK);
	val = (WM8994_HPOUT1L_RMV_SHORT|WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY|WM8994_HPOUT1R_RMV_SHORT|
		WM8994_HPOUT1R_OUTP|WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Unmute DAC1 left
	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (TUNING_DAC1R_VOL); //0 db volume
	wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

	// Unmute the AF1DAC1
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voipcall_headset - rec ear mic\n");

	wm8994_write(codec, WM8994_ANTIPOP_2, 0x68); // Ear mic volume issue fix

	// 01H // VMID_SEL, BIAS_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
	val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);

	// 02H // MIXINR_ENA, IN1R_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
	val &= ~(WM8994_MIXINR_ENA_MASK|WM8994_IN1R_ENA_MASK);
	val |= (WM8994_MIXINR_ENA|WM8994_IN1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

	if(!wm8994->testmode_config_flag)
	{
		// 1AH // IN1R PGA // IN1R UNMUTE, SET VOL
		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);
		val &= ~(WM8994_IN1R_MUTE_MASK|WM8994_IN1R_VOL_MASK);
        val |= (WM8994_IN1R_VU|TUNING_VOIPCALL_EAR_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME, val);

		// 2AH // MIXINR PGA // IN2R_TO_MIXINR MUTE, IN1R_TO_MIXINR UNMUTE, 0dB
		val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
		val &= ~(WM8994_IN1R_TO_MIXINR_MASK|WM8994_IN1R_MIXINR_VOL_MASK|WM8994_MIXOUTR_MIXINR_VOL_MASK);
		val |= (WM8994_IN1R_TO_MIXINR);	// BoostOff (0dB)
		wm8994_write(codec, WM8994_INPUT_MIXER_4, val);
	}

	// 401H // AIF1 ADC1 Right Volume
	val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME);
	val &= ~(WM8994_AIF1ADC1R_VOL_MASK);
	val |= (WM8994_AIF1ADC1_VU|TUNING_VOIPCALL_EAR_AIF1ADCR_VOL); // ADC Digital Gain
	wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME, val);

	// 28H // INPUT MIXER // IN1RP/N_TO_IN1R PGA
	val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
	val &= ~(WM8994_IN1RP_TO_IN1R_MASK|WM8994_IN1RN_TO_IN1R_MASK);
	val |= (WM8994_IN1RP_TO_IN1R|WM8994_IN1RN_TO_IN1R);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

#ifdef FEATURE_VOIP_DRC
	if this code enabled, build error
	/* DRC Sequence - 4Pole table // [DJ05-2239] // disable DRC and EQ */
	wm8994_write(codec, WM8994_AIF1_DRC1_1, 0x0098); // 440h // DRC disable
	wm8994_write(codec, WM8994_AIF1_DRC1_5, 0x0355); // 444h //
	wm8994_write(codec, WM8994_AIF1_DRC1_4, 0x03ED); // 443h //
	wm8994_write(codec, WM8994_AIF1_DRC1_3, 0x0C02); // 442h //
	wm8994_write(codec, WM8994_AIF1_DRC1_2, 0x0811); // 441h //
	wm8994_write(codec, WM8994_AIF1_DRC1_1, 0x01BA); // 440h // DRC1 NG enable
//#endif
//#ifdef FEATURE_VOIP_HPFILTER
	// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter on
	val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
	val &= ~(WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
	val |= (WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF); // hi-path filter on (L/R)
	wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, TUNING_VOIPCALL_EAR_ADC1_FILTERS);
#endif

	// 04H // AIF1ADC1R_ENA, ADCR_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
	val &= ~(WM8994_AIF1ADC1R_ENA_MASK|WM8994_ADCR_ENA_MASK);
	val |= (WM8994_AIF1ADC1R_ENA|WM8994_ADCR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	// 607H // ADC1R_TO_AIF1ADC1R (TIMESLOT 0) ASSIGN
	val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_ADC1R_TO_AIF1ADC1R_MASK);
	val |= (WM8994_ADC1R_TO_AIF1ADC1R);
	wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);

	// 300H // Mixing left channel output to right channel // val: 0x0010
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
	val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
	val |= (WM8994_AIF1ADCL_SRC|WM8994_AIF1ADCR_SRC);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

    printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voipcall_headset()\n");
}

void wm8994_set_voipcall_headphone(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

    close_output_path_all(codec);
	DEBUG_LOG("");
    audio_ctrl_mic_bias_gpio(codec, '8');

	u16 valBefore = 0;
	u16 valAfter = 0;
	u16 valLow1 = 0;
	u16 valHigh1 = 0;
	u8 valLow = 0;
	u8 valHigh = 0;

	//Configuring the Digital Paths
	// 601H // AIF1DAC1L_TO_DAC1L // Enable timeslot0 to left dac
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	// 602H // AIF1DAC1R_TO_DAC1R // Enable timeslot0 to right dac
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, 0x56);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec,0x56, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0000);
	val = (0x0000);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, WM8994_CLASS_W_1);
	val &= ~(0x0005);
	val |= (0x0005);
	wm8994_write(codec, WM8994_CLASS_W_1, val);

	// Headset Control
	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK|WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1L_MUTE_N|TUNING_VOIPCALL_EAR_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK|WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1R_MUTE_N|TUNING_VOIPCALL_EAR_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK|WM8994_MIXOUTL_VOL_MASK);
		//val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTL_MUTE_N|TUNING_VOIP_EAR_OPGAL_VOL);
		wm8994_write(codec, WM8994_LEFT_OPGA_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK|WM8994_MIXOUTR_VOL_MASK);
		//val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTR_MUTE_N|TUNING_VOIP_EAR_OPGAR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OPGA_VOLUME, val);
	}

	val = wm8994_read(codec, WM8994_DC_SERVO_2);
	val &= ~(0x03E0);
	val = (0x03E0);
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	//Enable vmid,bias, hp left and right
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_HPOUT1L_ENA_MASK|WM8994_HPOUT1R_ENA_MASK);
	val |= (WM8994_BIAS_ENA|WM8994_VMID_SEL_NORMAL|WM8994_HPOUT1R_ENA|WM8994_HPOUT1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
	val &= ~(0x0022);
	val = 0x0022;
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Enable Charge Pump
	val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
	val &= ~(WM8994_CP_ENA_MASK);
	val |= (WM8994_CP_ENA|WM8994_CP_ENA_DEFAULT); // this is from wolfson
	wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

	msleep(5);// 20ms delay

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_2);
		val &= ~(WM8994_AIF1DAC_BOOST_MASK);
		val |= (TUNING_VOIPCALL_EAR_AIF1DAC_BOOST<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
		wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);
	}

#ifdef FEATURE_VOIP_DRC
	if this code enabled, build error
	// 481H // 480H // AIF1_DAC1_EQ_GAIN set
	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_2, 0x9600); // 481H //
	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_1, 0x0199); // 480H //
#endif

	//Enable Dac1 and DAC2 and the Timeslot0 for AIF1
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_AIF1DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA|WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	// 2DH // DAC1L_TO_HPOUT1L set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &=  ~(WM8994_DAC1L_TO_HPOUT1L_MASK|WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	// Enable DAC1R to HPOUT1R path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK|WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK|WM8994_MIXOUTRVOL_ENA_MASK|WM8994_MIXOUTL_ENA_MASK|WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_MIXOUTL_ENA|WM8994_MIXOUTR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x0303);
	val = (0x0303);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(160); // 160ms delay

	valBefore = wm8994_read(codec, WM8994_DC_SERVO_4);

	valLow=(signed char)(valBefore & 0xff);
	valHigh=(signed char)((valBefore>>8)&0xff);
	valLow1 = ((signed short)(valLow-5))&0x00ff;
	valHigh1 = (((signed short)(valHigh-5))<<8)&0xff00;
	valAfter = (valLow1|valHigh1);
	wm8994_write(codec, WM8994_DC_SERVO_4, valAfter);

	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x000F);
	val = (0x000F);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(20);

	// Intermediate HP settings
	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
	val &= ~(WM8994_HPOUT1R_DLY_MASK|WM8994_HPOUT1R_OUTP_MASK|WM8994_HPOUT1R_RMV_SHORT_MASK|
		WM8994_HPOUT1L_DLY_MASK|WM8994_HPOUT1L_OUTP_MASK|WM8994_HPOUT1L_RMV_SHORT_MASK);
	val = (WM8994_HPOUT1L_RMV_SHORT|WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY|WM8994_HPOUT1R_RMV_SHORT|
		WM8994_HPOUT1R_OUTP|WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Unmute DAC1 left
	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (TUNING_DAC1R_VOL); //0 db volume
	wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

	// Unmute the AF1DAC1
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voipcall_headphone - rec sub mic\n");

	wm8994_write(codec, WM8994_ANTIPOP_2, 0x68); // Main mic volume issue fix

	// 01H // VMID_SEL_NORMAL, BIAS_ENA, MICB1_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
	val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);

	// 02H // MIXINL_ENA, IN1L_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
	val &= ~(WM8994_MIXINL_ENA_MASK|WM8994_IN1L_ENA_MASK);
	val |= (WM8994_MIXINL_ENA|WM8994_IN1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

	if(!wm8994->testmode_config_flag)
	{
        // 18H // IN1L PGA // IN1L UNMUTE, SET VOL
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);
		val &= ~(WM8994_IN1L_MUTE_MASK|WM8994_IN1L_VOL_MASK);
		val |= (WM8994_IN1L_VU|TUNING_VOIPCALL_3P_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

		// 29H // MIXINL PGA // IN2L_TO_MIXINL MUTE, IN1L_TO_MIXINL UNMUTE, 0dB
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val &= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
		val |= (WM8994_IN1L_TO_MIXINL);	// BoostOff (0dB)
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
	}

#ifdef FEATURE_VOIP_DRC
	// 400H // AIF1 ADC1 Left Volume // Gain Tuning // H/W req.
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
	val |= (WM8994_AIF1ADC1_VU|TUNING_VOIPCALL_MAIN_AIF1ADCL_VOL); // ADC Digital Gain
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);
#endif

	// 28H // INPUT MIXER // IN1LP/N_TO_IN1L PGA
	val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
	val &= (WM8994_IN1LP_TO_IN1L_MASK|WM8994_IN1LN_TO_IN1L_MASK);
	val |= (WM8994_IN1LP_TO_IN1L|WM8994_IN1LN_TO_IN1L);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

#ifdef FEATURE_VOIP_DRC
	if this code enabled, build error
	/* DRC Sequence - 3Pole table // [DJ05-2239] // disable DRC and EQ */
	wm8994_write(codec, WM8994_AIF1_DRC1_1, 0x0098); // 440h // DRC disable
	wm8994_write(codec, WM8994_AIF1_DRC1_5, 0x0355); // 444h //
	wm8994_write(codec, WM8994_AIF1_DRC1_4, 0x03ED); // 443h //
	wm8994_write(codec, WM8994_AIF1_DRC1_3, 0x0C02); // 442h //
	wm8994_write(codec, WM8994_AIF1_DRC1_2, 0x0811); // 441h //
	wm8994_write(codec, WM8994_AIF1_DRC1_1, 0x01BA); // 440h // DRC1 NG enable
//#endif
//#ifdef FEATURE_VOIP_HPFILTER
	// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter on
	val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
	val &= ~(WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
	val |= (WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF); // hi-path filter on (L/R)
	wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, TUNING_VOIPCALL_3P_ADC1_FILTERS);
#endif

	// 04H // AIF1ADC1L_ENA, ADCL_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
	val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_ADCL_ENA_MASK);
	val |= (WM8994_AIF1ADC1L_ENA|WM8994_ADCL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	// 606H // ADC1L_TO_AIF1ADC1L (TIMESLOT 0) ASSIGN
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
	val |= (WM8994_ADC1L_TO_AIF1ADC1L);
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

	// 300H // Mixing left channel output to right channel // val: 0x0010
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
	val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);
    
    printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voipcall_headphone()\n");
}

void wm8994_set_voipcall_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

    close_output_path_all(codec);
	DEBUG_LOG("");
    audio_ctrl_mic_bias_gpio(codec, '9');

	// 01H // Disable end point for preventing pop up noise.
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0003);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_SPKLVOL_ENA_MASK);
	val |= (WM8994_SPKLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	// Unmute the SPKMIXVOLUME
	val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
	val &= ~(WM8994_SPKMIXL_VOL_MASK);
	val |= (TUNING_SPKMIXL_ATTEN);
	wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);

	val = wm8994_read(codec, WM8994_SPKMIXR_ATTENUATION);
	val &= ~(WM8994_SPKMIXR_VOL_MASK);
	//val |= (TUNING_SPKMIXR_ATTEN);
	wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK|WM8994_SPKOUTL_VOL_MASK);
		val |= (WM8994_SPKOUT_VU|WM8994_SPKOUTL_MUTE_N|TUNING_VOIPCALL_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);

		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK|WM8994_SPKOUTR_VOL_MASK);
		//val |= (WM8994_SPKOUT_VU|WM8994_SPKOUTR_MUTE_N|TUNING_MP3_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);

		val = wm8994_read(codec, WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		val |= (TUNING_VOIPCALL_CLASSD_VOL<<WM8994_SPKOUTL_BOOST_SHIFT);
		wm8994_write(codec, WM8994_CLASSD, val);
	}

#ifdef FEATURE_VOIP_DRC
	if this code enabled, build error
	// 481H // 480H // AIF1_DAC1_EQ_GAIN set
	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_2, 0x9600); // 481H //
	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_1, 0x0199); // 480H //
#endif

	// 610H // DAC1 Left VOL, Unmute
	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

	// 611H // DAC1 Right VOL, Unmute
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1R_VOL);
	wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_2);
		val &= ~(WM8994_AIF1DAC_BOOST_MASK);
		val |= (TUNING_VOIPCALL_SPK_AIF1DAC_BOOST<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 0dB 01 +6dB 02 +12dB 03 +18dB
		wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);
	}

	val = wm8994_read(codec, WM8994_SPKOUT_MIXERS);
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK|WM8994_SPKMIXR_TO_SPKOUTL_MASK|
		WM8994_SPKMIXR_TO_SPKOUTR_MASK|WM8994_SPKMIXL_TO_SPKOUTR_MASK);
	val |= (WM8994_SPKMIXL_TO_SPKOUTL);
	wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

	// 36H // Unmute the DAC path
	val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
	val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
	val |= (WM8994_DAC1L_TO_SPKMIXL);
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

	// 05H // AIF1DAC1L/R_ENA, DAC1L_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_AIF1DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	// 420H // AIF1DAC1 Unmute, Mono
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE|WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	// 601H // AIF1DAC1L_TO_DAC1L // Enable timeslot0 to left dac
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	// 602H // AIF1DAC1R_TO_DAC1R // Enable timeslot0 to right dac
	//val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	//val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	//val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, 0);//val);

	// 01H // SPKOUTL_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
	val |= (WM8994_SPKOUTL_ENA|WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voipcall_speaker - rec sub mic\n");

	// 300H // Mixing left channel output to right channel // val: 0x0010
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
	val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

	wm8994_write(codec, WM8994_ANTIPOP_2, 0x68); // Main mic volume issue fix

	// 01H // VMID_SEL_NORMAL, BIAS_ENA, MICB1_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
	val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);

	// 02H // MIXINL_ENA, IN1L_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
	val &= ~(WM8994_MIXINL_ENA_MASK|WM8994_IN1L_ENA_MASK);
	val |= (WM8994_MIXINL_ENA|WM8994_IN1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

	if(!wm8994->testmode_config_flag)
	{
        // 18H // IN1L PGA // IN1L UNMUTE, SET VOL
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);
		val &= ~(WM8994_IN1L_MUTE_MASK|WM8994_IN1L_VOL_MASK);
		val |= (WM8994_IN1L_VU|TUNING_VOIPCALL_SPK_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

        // 29H // MIXINL PGA // IN2L_TO_MIXINL MUTE, IN1L_TO_MIXINL UNMUTE, 0dB
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val &= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
		val |= (WM8994_IN1L_TO_MIXINL);	// BoostOff (0dB)
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
	}

#ifdef FEATURE_VOIP_DRC
	// 400H // AIF1 ADC1 Left Volume // Gain Tuning // H/W req.
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
	val |= (WM8994_AIF1ADC1_VU|TUNING_VOIPCALL_MAIN_AIF1ADCL_VOL); // ADC Digital Gain
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);
#endif

	// 28H // INPUT MIXER // IN1LP/N_TO_IN1L PGA
	val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
	val &= (WM8994_IN1LP_TO_IN1L_MASK|WM8994_IN1LN_TO_IN1L_MASK);
	val |= (WM8994_IN1LP_TO_IN1L|WM8994_IN1LN_TO_IN1L);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

#ifdef FEATURE_VOIP_DRC
	if this code enabled, build error
	/* DRC Sequence - SPK table // [DJ05-2239] // disable DRC and EQ */
	wm8994_write(codec, WM8994_AIF1_DRC1_1, 0x0098); // 440h // DRC disable
	wm8994_write(codec, WM8994_AIF1_DRC1_5, 0x0355); // 444h //
	wm8994_write(codec, WM8994_AIF1_DRC1_4, 0x03ED); // 443h //
	wm8994_write(codec, WM8994_AIF1_DRC1_3, 0x0C02); // 442h //
	wm8994_write(codec, WM8994_AIF1_DRC1_2, 0x0811); // 441h //
	wm8994_write(codec, WM8994_AIF1_DRC1_1, 0x01BA); // 440h // DRC1 NG enable
//#endif
//#ifdef FEATURE_VOIP_HPFILTER
	// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter on
	val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
	val &= ~(WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
	val |= (WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF); // hi-path filter on (L/R)
	wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, TUNING_VOIPCALL_SPK_ADC1_FILTERS);
#endif

	// 04H // AIF1ADC1L_ENA, ADCL_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
	val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_ADCL_ENA_MASK);
	val |= (WM8994_AIF1ADC1L_ENA|WM8994_ADCL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	// 606H // ADC1L_TO_AIF1ADC1L (TIMESLOT 0) ASSIGN
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
	val |= (WM8994_ADC1L_TO_AIF1ADC1L);
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

    printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voipcall_speaker()\n");
}

void wm8994_set_voipcall_bluetooth(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;

	u16 val;

    close_output_path_all(codec);
	DEBUG_LOG("");

    /* GPIO Configuration */
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);
    wm8994_write(codec, WM8994_GPIO_2, 0xA101);
    wm8994_write(codec, WM8994_GPIO_3, 0xA101);
    wm8994_write(codec, WM8994_GPIO_4, 0xA101);
    wm8994_write(codec, WM8994_GPIO_5, 0xA101);
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);
    wm8994_write(codec, WM8994_GPIO_7, 0xA101);
	wm8994_write(codec, WM8994_GPIO_8, 0xA101);
	wm8994_write(codec, WM8994_GPIO_9, 0xA101);
	wm8994_write(codec, WM8994_GPIO_10, 0xA101);
	wm8994_write(codec, WM8994_GPIO_11, 0xA101);

	/* FLL2	Setting */
	wm8994_write(codec, WM8994_FLL2_CONTROL_2, 0x2F00); // FLL1 Ctrl2, FLL1 Setting
	wm8994_write(codec, WM8994_FLL2_CONTROL_3, 0x3126); // FLL1 Ctrl3, K Value
	wm8994_write(codec, WM8994_FLL2_CONTROL_4, 0x0100); // FLL1 Ctrl4, N Value
	wm8994_write(codec, WM8994_FLL2_CONTROL_5, 0x0C88); // FLL1 Ctrl5
	wm8994_write(codec, WM8994_FLL2_CONTROL_1, (WM8994_FLL2_FRACN_ENA|WM8994_FLL2_ENA));

	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0018); // AIF2 Clock Source = FLL2

	/* Clocking - 8KHz */
    wm8994_write(codec, WM8994_AIF2_RATE, 0x0003);

	/* AIF2 Interface - PCM Stereo mode */
	wm8994_write(codec, WM8994_AIF2_CONTROL_1, 0x4118); // DSP Mode, BCLK invert, LRCLK normal
    wm8994_write(codec, WM8994_AIF2_BCLK, 0x40);
    wm8994_write(codec, WM8994_AIF2_CONTROL_2, 0x4000); // Left & Right DAC receives left interface data
    wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, 0x7000);

    /* Analog Output Power Management */
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0003);
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 0x6000);

	/* Digital Path Enables and Unmutes */
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 0x2202); // AIF2ADCL_ENA, AIF1ADC1L_ENA, ADCL_ENA
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, 0x220A); // AIF2DACL_ENA, AIF1DAC1L_ENA, DAC2L_ENA, DAC1L_ENA

	/* Clocking */
	val = wm8994_read(codec, WM8994_CLOCKING_1);
	val |= (WM8994_DSP_FS2CLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

    wm8994_write(codec, WM8994_GPIO_3, 0x0100); // 702h // GPIO3|BCLK2
    wm8994_write(codec, WM8994_GPIO_5, 0x0100); // 704h // GPIO5|DACDAT2
    wm8994_write(codec, WM8994_GPIO_8, 0x8100); // GPIO8|DACDAT3
    wm8994_write(codec, WM8994_GPIO_9, 0x0100); // GPIO9|ADCDAT3
    wm8994_write(codec, WM8994_GPIO_10, 0x0100); // GPIO10|LRCLK3
    wm8994_write(codec, WM8994_GPIO_11, 0x0100); // GPIO11|BCLK3

	//for BT DTMF Play
	//Rx Path: AIF2ADCDAT2 select
    //CP(CALL) Path: Analog
	//AP(DTMF) Path: DACDAT1 select
	//Tx Path: GPIO8/DACDAT3 select
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x000A);

    wm8994_write(codec, 0x604, WM8994_AIF1DAC1L_TO_DAC2L); // WM8994_DAC2_LEFT_MIXER_ROUTING
    wm8994_write(codec, 0x612, 0x01C0); // DAC2L/R_VOL, 0dB, update

	wm8994_write(codec, WM8994_OVERSAMPLING, 0x0000);
    wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019); // AIF2 Clock Source = FLL2

	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, 0x0000);
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, 0x0080);

    // 300H // Mixing left channel output to right channel // val: 0x0010
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
	val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

	// 606H // ADC1L_TO_AIF1ADC1L (TIMESLOT 0) ASSIGN
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
	val |= (WM8994_AIF2DACL_TO_AIF1ADC1L);
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

    printk(SND_KERN_DEBUG "[WM8994] wm8994_set_voipcall_bluetooth()\n");
}


