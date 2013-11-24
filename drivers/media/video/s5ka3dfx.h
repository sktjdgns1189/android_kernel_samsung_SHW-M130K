/*
 * Driver for S5KA3DFX (VGA camera) from Samsung Electronics
 *
 * 1/4" 2.0Mp CMOS Image Sensor SoC with an Embedded Image Processor
 *
 * Copyright (C) 2009, Jinsung Yang <jsgood.yang@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef __S5KA3DFX_H__
#define __S5KA3DFX_H__

struct s5ka3dfx_reg {
	unsigned char addr;
	unsigned char val;
};

struct s5ka3dfx_regset_type {
	unsigned char *regset;
	int len;
};

/*
 * Macro
 */
#define REGSET_LENGTH(x)	(sizeof(x) / sizeof(s5ka3dfx_reg))

/*
 * User defined commands
 */
/* S/W defined features for tune */
#define REG_DELAY	0xFF00	/* in ms */
#define REG_CMD		0xFFFF	/* Followed by command */

/* Following order should not be changed */
enum image_size_s5ka3dfx {
	SVGA,			/* 800*600 */
};

/*
 * Following values describe controls of camera
 * in user aspect and must be match with index of s5ka3dfx_regset[]
 * These values indicates each controls and should be used
 * to control each control
 */
enum s5ka3dfx_control {
	S5KA3DFX_INIT,
	S5KA3DFX_EV,
	S5KA3DFX_AWB,
	S5KA3DFX_MWB,
	S5KA3DFX_EFFECT,
	S5KA3DFX_CONTRAST,
	S5KA3DFX_SATURATION,
	S5KA3DFX_SHARPNESS,
};

#define S5KA3DFX_REGSET(x)	{	\
	.regset = (x),			\
	.len = sizeof((x))/sizeof(s5ka3dfx_reg),}

/*
 * VGA Self shot init setting
 */
struct s5ka3dfx_reg s5ka3dfx_init_vga_reg[] = {
  {0xef, 0x02},  //100516
  {0x13, 0xa0},  //OB sel(R reference)
  {0x23, 0x53},  //tx_width           
  {0x26, 0x24},  //clp level          
  {0x2c, 0x05},  //S1S                
  {0x05, 0x00},  //S1S end value      
  {0x03, 0x58},  //S1R end value      

  {0x24, 0x0a},  //cds //1a                                

  {0x0b, 0x84},  //Analog Offset                           
  {0x1e, 0xb7},  //Global gain                             
  {0x56, 0x05},  //ADLC                                    
  {0x28, 0x96},  //CFPN                                    
  {0x67, 0x3c},  //or 3Ch, or 38h //to reduce HN at low lux

  {0xef, 0x03},
  {0x50, 0xd2},  //MCLK: 24MHz       

  {0x0f, 0x31},  //ADD vblank value

  {0xef, 0x03},  //FPS setting     
  {0x5F, 0x03},  //NT Cintr Max
  {0x60, 0x02},  //PAL Cintr Max
  {0x61, 0x0F},  //NT shutter Max (FrameAE mode)
  {0x62, 0x0C},  //PAL shutter Max (FrameAE mode)
  {0x63, 0x01},  //NT Vblank
  {0x64, 0xE7},  
  {0x65, 0x01},  //PAL Vblank
  {0x66, 0xE7},

  {0x6d, 0x59},  //55 AGC Min for Suppress B 
  {0x6e, 0x90},  //Dgain Min(Temporary. Don't move)
  {0x6f, 0x90},  //Dgain Max(Temporary. Don't move)
  {0x70, 0x00},  //mirror default

  {0x4c, 0x00},  //Hblank of 50Hz. 50Hz Default value is not 9Eh.
  {0x4d, 0x9e},

  {0xef, 0x03},
  {0x00, 0x07},  //AE ON [0], AWB ON [1] //87 : 50Hz Fixed mode, 07 : 60Hz Fixed mode
  {0x01, 0x80},  //AE target (Temporary. Don't move)
  {0x02, 0x7f},  //AE th. (Temporary. Don't move)

  {0x2b, 0x41},  //Auto E-shutter enable     

  {0x31, 0x00},  //N.L. BT(Brightness)            
  {0x32, 0x0c},  //L.L. BT(Brightness)                      

  {0x33, 0x80},  //N.L. CT(Contrast)              
  {0x34, 0x79},  //L.L. CT(Contrast)                        

  {0x36, 0x3a},  //3a 39 3C 38 N.L. ST(Saturation)            
  {0x37, 0x30},  //38 L.L. ST(Saturation)                        

  {0x6a, 0x00},  //BPR start point                    
  {0x7b, 0x05},  //H.L. BPR                            
  {0x38, 0x05},  //N.L. BPR                              
  {0x39, 0x03},  //L.L. BPR                              

  {0x2d, 0x04},  //08 08 N.L. NR gain                   
  {0x2e, 0x20},  //20 2c N.L. NR edge
  {0x2f, 0x2C},  //30 4a L.L. NR gain
  {0x30, 0xff},  //ff ff L.L. NR edge
  {0x7c, 0x02},  //07 06 06 H.L. NR gain
  {0x7d, 0x20},  //1e 20 2a H.L. NR edge
  {0x7e, 0x08},  //17 0c 0c M.L. NR gain
  {0x7f, 0x20},  //1c 20 2a M.L. NR edge

  {0x28, 0x02},  //Sharpness ON
  {0x29, 0xa6},  //9f 8f N.L Sharpness gain
  {0x2a, 0x00},  //L.L Sharpness gain

  {0x13, 0x00},  //AWB Outdoor Cintr limit  
  {0x14, 0xa0},                            

  {0x1a, 0x60},  //Outdoor Rgain Max       
  {0x1b, 0x5a},  //Outdoor Rgain Min       
  {0x1c, 0x63},  //Outdoor Bgain Max       
  {0x1d, 0x4f},  //Outdoor Bgain Min       

  {0x1e, 0x68},  //64  //68 Indoor Rgain Max        
  {0x1f, 0x45},  //47  //44 3e  //Indoor Rgain Min        
  {0x20, 0x82},  //81  //Indoor Bgain Max        
  {0x21, 0x4b},  //4b  //4c Indoor Bgain Min        

  {0x3a, 0x13},  //[7:4]AWB speed, [3:0] AWB threshold        
  {0x3b, 0x3c},  //AWB skip BRT <= same value with AE target<3. 01>
  {0x3c, 0x00},  //AWB skip Gain                                   
  {0x3d, 0x18},  //AWB skip AVG                                    

  {0x23, 0x80},  //AWB window select                               

  {0x15, 0x0b},  //AWB CNT                                         
  {0x16, 0xd2},
  {0x17, 0x64},
  {0x18, 0x78},

  {0xef, 0x00},  //X shade
  {0xde, 0x00},
  {0xdf, 0x1F},
  {0xe0, 0x00},
  {0xe1, 0x37},
  {0xe2, 0x08},
  {0xe3, 0x42},
  {0xe4, 0x00},
  {0xe5, 0x12},
  {0xe6, 0x9E},
  {0xe9, 0x00},
  {0xe7, 0x00},
  {0xe8, 0xA8},
  {0xe9, 0x01},
  {0xe7, 0x00},
  {0xe8, 0xB1},
  {0xe9, 0x02},
  {0xe7, 0x00},
  {0xe8, 0xC4},
  {0xe9, 0x03},
  {0xe7, 0x00},
  {0xe8, 0xBA},
  {0xe9, 0x04},
  {0xe7, 0x00},
  {0xe8, 0xC4},
  {0xe9, 0x05},
  {0xe7, 0x00},
  {0xe8, 0xE5},
  {0xe9, 0x06},
  {0xe7, 0x00}, //01,
  {0xe8, 0x13},
  {0xe9, 0x07},
  {0xe7, 0x00},
  {0xe8, 0x4B},
  {0xe9, 0x08},
  {0xe7, 0x00},
  {0xe8, 0x4B},
  {0xe9, 0x09},
  {0xe7, 0x00},
  {0xe8, 0x4E},
  {0xe9, 0x0A},
  {0xe7, 0x00},
  {0xe8, 0x4F},
  {0xe9, 0x0B},
  {0xe7, 0x00},
  {0xe8, 0x42},
  {0xe9, 0x0C},
  {0xe7, 0x00},
  {0xe8, 0x6A},
  {0xe9, 0x0D},
  {0xe7, 0x00},
  {0xe8, 0x89},
  {0xe9, 0x0E},
  {0xe7, 0x07},
  {0xe8, 0xD9},
  {0xe9, 0x0F},
  {0xe7, 0x07},
  {0xe8, 0xD8},
  {0xe9, 0x10},
  {0xe7, 0x07},
  {0xe8, 0xDD},
  {0xe9, 0x11},
  {0xe7, 0x07},
  {0xe8, 0xD1},
  {0xe9, 0x12},
  {0xe7, 0x07},
  {0xe8, 0xCA},
  {0xe9, 0x13},
  {0xe7, 0x07},
  {0xe8, 0xCE},
  {0xe9, 0x14},
  {0xe7, 0x07},
  {0xe8, 0xFA},
  {0xe9, 0x15},
  {0xe7, 0x07},
  {0xe8, 0x6A},
  {0xe9, 0x16},
  {0xe7, 0x07},
  {0xe8, 0x71},
  {0xe9, 0x17},
  {0xe7, 0x07},
  {0xe8, 0x70},
  {0xe9, 0x18},
  {0xe7, 0x07},
  {0xe8, 0x5E},
  {0xe9, 0x19},
  {0xe7, 0x07},
  {0xe8, 0x50},
  {0xe9, 0x1A},
  {0xe7, 0x07},
  {0xe8, 0x55},
  {0xe9, 0x1B},
  {0xe7, 0x07},
  {0xe8, 0x6D},
  {0xe9, 0x1C},
  {0xe7, 0x07},
  {0xe8, 0x1C},
  {0xe9, 0x1D},
  {0xe7, 0x07},
  {0xe8, 0x12},
  {0xe9, 0x1E},
  {0xe7, 0x07},
  {0xe8, 0x07},
  {0xe9, 0x1F},
  {0xe7, 0x06},
  {0xe8, 0xFA},
  {0xe9, 0x20},
  {0xe7, 0x06},
  {0xe8, 0xE5},
  {0xe9, 0x21},
  {0xe7, 0x06},
  {0xe8, 0xF0},
  {0xe9, 0x22},
  {0xe7, 0x07},
  {0xe8, 0x0A},
  {0xe9, 0x40},
                                                                                      
  {0xd1, 0x08},  //YC order [3]YC [2]RB 48                
  {0xdd, 0x03},  //X shade ON                       

  {0x23, 0x17},  //GrGb                        
  {0x24, 0x17},                                
  {0x25, 0x17},                                
  {0x27, 0x18},                                
  {0x29, 0x60},                                
  {0x2a, 0x22},                                

  {0x2f, 0x01},  //INTP_COEF_Sharpness              

  {0x36, 0x01},  //Shading ON(R, G, B)                
  {0x37, 0xdc},  //e0,  //bb,  //ba                                   
  {0x38, 0xba},  //c7,  //a8,  //98,a0                                  
  {0x39, 0xae},  //b1,  //8e,  //8e,96                                   
  {0x3a, 0x00},                                     
  {0x3b, 0xf0},                                     
  {0x3c, 0x01},  // X-320                                     
  {0x3d, 0x40},                                     

  {0xb9, 0x02},  //Sharpness ON(HW initial)                   
  {0xbb, 0xb0},  //HPF for Sharpness                            
  {0xbc, 0x18},                                     
  {0xbd, 0x30},                                     
  {0xbf, 0x38},  //Sharpness Core                             
  {0xc1, 0x88},                                     

  {0xc8, 0x11},  //YC delay                         
  {0xeb, 0x81},  //ECS ON                             
  {0xed, 0x05},  //ECS th.                                   

  {0xb1, 0x00},  //AWB window                       
  {0xb2, 0x62},                                     
  {0xb3, 0x00},                                     
  {0xb4, 0x00},                                     
  {0xb5, 0x01},                                     
  {0xb6, 0xa3},                                     
  {0xb7, 0x02},                                     
  {0xb8, 0x80},                                     

  {0x79, 0x3a},  //39
  {0x77, 0x00},  //HUE                   
  {0x78, 0x00},                         

  {0xef, 0x00},                                     
  {0x93, 0x40},  //AWB Map                          
  {0x94, 0x80},                                     
  {0x95, 0xc0},                                     
  {0x96, 0xc0},                                     
  {0x97, 0x20},                                     
  {0x98, 0x20},                                     
  {0x99, 0x30},                                     
  {0xA0, 0x00},                                     
  {0xA1, 0x00},                                     
  {0xA2, 0x1c},                                     
  {0xA3, 0x16},                                     
  {0xA4, 0x03},                                     
  {0xA5, 0x07},                                     
  {0xA6, 0x00},                                     

  {0xef, 0x00},                    
  {0xad, 0xd0},  //AWB up data                      
  {0xaf, 0x10},  //AWB dn data                                       

  {0xef, 0x00},   
  {0x42, 0x65},  //5d Rgain (start point of AWB)                                   
  {0x44, 0x60},  //Bgain (start point of AWB)                    
  {0x57, 0x00},  //AE Min skip 

  {0xef, 0x03},
  {0x01, 0x3c},  //AE target        
  {0x02, 0x05},  //AE threshold     
  {0x03, 0x20},  //AE step          
  {0x04, 0x65},  //AGC Max of LowLux
  {0x06, 0x1c},  //AGC Max of HL    
  {0x07, 0x01},  //AE win_A weight  
  {0x08, 0x03},  //AE win_B weight  
  {0x0b, 0x01},  //cintc max high   

  {0x51, 0x10},   //CHIP_DBG                    
  {0x52, 0x00},                                 
  {0x53, 0x00},                                 
  {0x54, 0x00},                                 
  {0x55, 0x22},                                 
  {0x56, 0x01},                                 
  {0x57, 0x61},                                 
  {0x58, 0x25},                                 
  {0x67, 0xcf},   //AE speed                    
  {0x69, 0x17},   //AE ROW FLAG                 

  {0xef, 0x00},                 
  {0x58, 0x00},  //AE window(무선 center weight)
  {0x59, 0x00},
  {0x5a, 0x02},
  {0x5b, 0x73},
  {0x5c, 0x00},
  {0x5d, 0x00},
  {0x5e, 0x01},
  {0x5f, 0xe0},
  {0x60, 0x00},
  {0x61, 0xae},
  {0x62, 0x01},
  {0x63, 0xbb},
  {0x64, 0x00},
  {0x65, 0x7e},
  {0x66, 0x01},
  {0x67, 0x8e},

  {0xef, 0x00},  //Flicker setting                 
  {0x6a, 0x01},  //080331 Flicker H SIZE High      
  {0x6b, 0xe0},  //080331 Flicker H SIZE low       
  {0x6c, 0x05},  //04 //080331 Flicker WINDOW VSIZE
  {0x6d, 0x00},  //080331 Flicker V SIZE START H   
  {0x6e, 0x0e},  //080331 Flicker V SIZE START L   
  {0x6f, 0x00},  //080331 Flicker H SIZE START H   
  {0x70, 0x10},  //080331 Flicker H SIZE START L   

  {0xef, 0x03},
  {0x22, 0x24},  //flicker sensitivity H/L            
  {0x3e, 0x23},                                            
  {0x3f, 0x23},                                            
  {0x40, 0x00},                                            
  {0x41, 0x09},  //60hz light - 50hz setting threshold     
  {0x4a, 0x09},  //50hz light - 60hz setting threshold     
  {0x4b, 0x04},                                            
  {0x5b, 0x20},  //detection haunting protection count
  {0x5d, 0x35},                                        
  {0x5e, 0x13},                                             
  {0x78, 0x0f},                                             

  {0xef, 0x00},
  {0x4c, 0x80},  //79 //CCM
  {0x4d, 0xbb},  //be
  {0x4e, 0x84},  //08
  {0x4f, 0x91},  //9c
  {0x50, 0x64},  //7a
  {0x51, 0x93},  //9e
  {0x52, 0x03},  //8f
  {0x53, 0xc7},  //bb
  {0x54, 0x83},  //8a

  {0xef, 0x03},
  {0x6e, 0x40},  //Dgain Min 
  {0x6f, 0x6a},  //Dgain Max

  {0xef, 0x00},  //RGB Gamma (Feburary updated version for 계조구분)
  {0x48, 0x00},
  {0x49, 0x00},
  {0x4a, 0x03},
  {0x48, 0x01},
  {0x49, 0x00},
  {0x4a, 0x06},
  {0x48, 0x02},
  {0x49, 0x00},
  {0x4a, 0x28},
  {0x48, 0x03},
  {0x49, 0x00},
  {0x4a, 0x8a},
  {0x48, 0x04},
  {0x49, 0x01},
  {0x4a, 0x20},
  {0x48, 0x05},
  {0x49, 0x01},
  {0x4a, 0xb4},
  {0x48, 0x06},
  {0x49, 0x02},
  {0x4a, 0x23},
  {0x48, 0x07},
  {0x49, 0x02},
  {0x4a, 0x72},
  {0x48, 0x08},
  {0x49, 0x02},
  {0x4a, 0xbe},
  {0x48, 0x09},
  {0x49, 0x02},
  {0x4a, 0xfa},
  {0x48, 0x0a},
  {0x49, 0x03},
  {0x4a, 0x27},
  {0x48, 0x0b},
  {0x49, 0x03},
  {0x4a, 0x55},
  {0x48, 0x0c},
  {0x49, 0x03},
  {0x4a, 0x81},
  {0x48, 0x0d},
  {0x49, 0x03},
  {0x4a, 0xa2},
  {0x48, 0x0e},
  {0x49, 0x03},
  {0x4a, 0xbc},
  {0x48, 0x0f},
  {0x49, 0x03},
  {0x4a, 0xd4},
  {0x48, 0x10},
  {0x49, 0x03},
  {0x4a, 0xe8},
  {0x48, 0x11},
  {0x49, 0x03},
  {0x4a, 0xf4},
  {0x48, 0x12},
  {0x49, 0x03},
  {0x4a, 0xff},

  {0x48, 0x20},
  {0x49, 0x00},
  {0x4a, 0x03},
  {0x48, 0x21},
  {0x49, 0x00},
  {0x4a, 0x06},
  {0x48, 0x22},
  {0x49, 0x00},
  {0x4a, 0x28},
  {0x48, 0x23},
  {0x49, 0x00},
  {0x4a, 0x8a},
  {0x48, 0x24},
  {0x49, 0x01},
  {0x4a, 0x20},
  {0x48, 0x25},
  {0x49, 0x01},
  {0x4a, 0xb4},
  {0x48, 0x26},
  {0x49, 0x02},
  {0x4a, 0x23},
  {0x48, 0x27},
  {0x49, 0x02},
  {0x4a, 0x72},
  {0x48, 0x28},
  {0x49, 0x02},
  {0x4a, 0xbe},
  {0x48, 0x29},
  {0x49, 0x02},
  {0x4a, 0xfa},
  {0x48, 0x2a},
  {0x49, 0x03},
  {0x4a, 0x27},
  {0x48, 0x2b},
  {0x49, 0x03},
  {0x4a, 0x55},
  {0x48, 0x2c},
  {0x49, 0x03},
  {0x4a, 0x81},
  {0x48, 0x2d},
  {0x49, 0x03},
  {0x4a, 0xa2},
  {0x48, 0x2e},
  {0x49, 0x03},
  {0x4a, 0xbc},
  {0x48, 0x2f},
  {0x49, 0x03},
  {0x4a, 0xd4},
  {0x48, 0x30},
  {0x49, 0x03},
  {0x4a, 0xe8},
  {0x48, 0x31},
  {0x49, 0x03},
  {0x4a, 0xf4},
  {0x48, 0x32},
  {0x49, 0x03},
  {0x4a, 0xff},

  {0x48, 0x40},
  {0x49, 0x00},
  {0x4a, 0x03},
  {0x48, 0x41},
  {0x49, 0x00},
  {0x4a, 0x06},
  {0x48, 0x42},
  {0x49, 0x00},
  {0x4a, 0x28},
  {0x48, 0x43},
  {0x49, 0x00},
  {0x4a, 0x8a},
  {0x48, 0x44},
  {0x49, 0x01},
  {0x4a, 0x20},
  {0x48, 0x45},
  {0x49, 0x01},
  {0x4a, 0xb4},
  {0x48, 0x46},
  {0x49, 0x02},
  {0x4a, 0x23},
  {0x48, 0x47},
  {0x49, 0x02},
  {0x4a, 0x72},
  {0x48, 0x48},
  {0x49, 0x02},
  {0x4a, 0xbe},
  {0x48, 0x49},
  {0x49, 0x02},
  {0x4a, 0xfa},
  {0x48, 0x4a},
  {0x49, 0x03},
  {0x4a, 0x27},
  {0x48, 0x4b},
  {0x49, 0x03},
  {0x4a, 0x55},
  {0x48, 0x4c},
  {0x49, 0x03},
  {0x4a, 0x81},
  {0x48, 0x4d},
  {0x49, 0x03},
  {0x4a, 0xa2},
  {0x48, 0x4e},
  {0x49, 0x03},
  {0x4a, 0xbc},
  {0x48, 0x4f},
  {0x49, 0x03},
  {0x4a, 0xd4},
  {0x48, 0x50},
  {0x49, 0x03},
  {0x4a, 0xe8},
  {0x48, 0x51},
  {0x49, 0x03},
  {0x4a, 0xf4},
  {0x48, 0x52},
  {0x49, 0x03},
  {0x4a, 0xff},
};

/*
 * VT init setting
 */
struct s5ka3dfx_reg s5ka3dfx_init_vga_vt_reg[] = {
  {0xef, 0x02},
  {0x13, 0xa0},
  {0x23, 0x53},
  {0x26, 0x24},
  {0x2c, 0x05},
  {0x05, 0x00},
  {0x03, 0x58},

  {0x24, 0x0a},

  {0x0b, 0x81},
  {0x1e, 0xb7},
  {0x56, 0x05},
  {0x28, 0x96},
  {0x67, 0x3c},

  {0xef, 0x03},
  {0x50, 0xd2},

  {0x0f, 0x31},

  {0xef, 0x03},
  {0x5F, 0x03},
  {0x60, 0x02},
  {0x61, 0x0f},
  {0x62, 0x0c},
  {0x63, 0x05},
  {0x64, 0x43},
  {0x65, 0x05},
  {0x66, 0x43},

  {0x6d, 0x62}, //67, //65
  {0x6f, 0x41},

  {0x4c, 0x00},
  {0x4d, 0x9e},

  {0xef, 0x03},
  {0x00, 0x07},
  {0x01, 0x80},
  {0x02, 0x7f},

  {0x2b, 0x41},

  {0x31, 0x07},
  {0x32, 0x18},

  {0x33, 0x80},
  {0x34, 0x79},

  {0x36, 0x3a},  //28,
  {0x37, 0x36},

  {0x6a, 0x00},
  {0x7b, 0x05},
  {0x38, 0x05},
  {0x39, 0x03},

  {0x2d, 0x08},
  {0x2e, 0x2c},
  {0x2f, 0x4a},
  {0x30, 0xff},
  {0x7c, 0x06},
  {0x7d, 0x2a},
  {0x7e, 0x0c},
  {0x7f, 0x2a},

  {0x28, 0x02},
  {0x29, 0xa8},  //8f,
  {0x2a, 0x00},

  {0x13, 0x00},
  {0x14, 0xa0},
  {0x19, 0x43},
  {0x1a, 0x6a},
  {0x1b, 0x50},	//5f,
  {0x1c, 0x63},	//61,
  {0x1d, 0x4b},

  {0x1e, 0x6a},
  {0x1f, 0x48},  //3c,	//46,
  {0x20, 0x9a},	//7a,
  {0x21, 0x4b},

  {0x3a, 0x13},
  {0x3b, 0x3e},
  {0x3c, 0x00},
  {0x3d, 0x18},

  {0x23, 0x80},

  {0x15, 0x0b},
  {0x16, 0xd2},
  {0x17, 0x64},
  {0x18, 0x78},

  {0xef, 0x00},
  {0xde, 0x00},
  {0xdf, 0x1F},
  {0xe0, 0x00},
  {0xe1, 0x37},
  {0xe2, 0x08},
  {0xe3, 0x42},
  {0xe4, 0x00},
  {0xe5, 0x12},
  {0xe6, 0x9E},
  {0xe9, 0x00},
  {0xe7, 0x00},
  {0xe8, 0xA8},
  {0xe9, 0x01},
  {0xe7, 0x00},
  {0xe8, 0xB1},
  {0xe9, 0x02},
  {0xe7, 0x00},
  {0xe8, 0xC4},
  {0xe9, 0x03},
  {0xe7, 0x00},
  {0xe8, 0xBA},
  {0xe9, 0x04},
  {0xe7, 0x00},
  {0xe8, 0xC4},
  {0xe9, 0x05},
  {0xe7, 0x00},
  {0xe8, 0xE5},
  {0xe9, 0x06},
  {0xe7, 0x01},
  {0xe8, 0x13},
  {0xe9, 0x07},
  {0xe7, 0x00},
  {0xe8, 0x4B},
  {0xe9, 0x08},
  {0xe7, 0x00},
  {0xe8, 0x4B},
  {0xe9, 0x09},
  {0xe7, 0x00},
  {0xe8, 0x4E},
  {0xe9, 0x0A},
  {0xe7, 0x00},
  {0xe8, 0x4F},
  {0xe9, 0x0B},
  {0xe7, 0x00},
  {0xe8, 0x42},
  {0xe9, 0x0C},
  {0xe7, 0x00},
  {0xe8, 0x6A},
  {0xe9, 0x0D},
  {0xe7, 0x00},
  {0xe8, 0x89},
  {0xe9, 0x0E},
  {0xe7, 0x07},
  {0xe8, 0xD9},
  {0xe9, 0x0F},
  {0xe7, 0x07},
  {0xe8, 0xD8},
  {0xe9, 0x10},
  {0xe7, 0x07},
  {0xe8, 0xDD},
  {0xe9, 0x11},
  {0xe7, 0x07},
  {0xe8, 0xD1},
  {0xe9, 0x12},
  {0xe7, 0x07},
  {0xe8, 0xCA},
  {0xe9, 0x13},
  {0xe7, 0x07},
  {0xe8, 0xCE},
  {0xe9, 0x14},
  {0xe7, 0x07},
  {0xe8, 0xFA},
  {0xe9, 0x15},
  {0xe7, 0x07},
  {0xe8, 0x6A},
  {0xe9, 0x16},
  {0xe7, 0x07},
  {0xe8, 0x71},
  {0xe9, 0x17},
  {0xe7, 0x07},
  {0xe8, 0x70},
  {0xe9, 0x18},
  {0xe7, 0x07},
  {0xe8, 0x5E},
  {0xe9, 0x19},
  {0xe7, 0x07},
  {0xe8, 0x50},
  {0xe9, 0x1A},
  {0xe7, 0x07},
  {0xe8, 0x55},
  {0xe9, 0x1B},
  {0xe7, 0x07},
  {0xe8, 0x6D},
  {0xe9, 0x1C},
  {0xe7, 0x07},
  {0xe8, 0x1C},
  {0xe9, 0x1D},
  {0xe7, 0x07},
  {0xe8, 0x12},
  {0xe9, 0x1E},
  {0xe7, 0x07},
  {0xe8, 0x07},
  {0xe9, 0x1F},
  {0xe7, 0x06},
  {0xe8, 0xFA},
  {0xe9, 0x20},
  {0xe7, 0x06},
  {0xe8, 0xE5},
  {0xe9, 0x21},
  {0xe7, 0x06},
  {0xe8, 0xF0},
  {0xe9, 0x22},
  {0xe7, 0x07},
  {0xe8, 0x0A},
  {0xe9, 0x40},

  {0xd1, 0x08},
  {0xdd, 0x03},

  {0x23, 0x10},
  {0x24, 0x10},
  {0x25, 0x10},
  {0x27, 0x18},
  {0x29, 0x60},
  {0x2a, 0x22},

  {0x2f, 0x01},

  {0x36, 0x01},  //Shading on
  {0x37, 0xe1},
  {0x38, 0xc7},
  {0x39, 0xb1},
  {0x3a, 0x01},
  {0x3b, 0x04},
  {0x3c, 0x01},
  {0x3d, 0x54},

  {0xb9, 0x02},
  {0xbb, 0xb0},
  {0xbc, 0x18},
  {0xbd, 0x30},
  {0xbf, 0x38},
  {0xc1, 0x88},

  {0xc8, 0x11},
  {0xeb, 0x81},
  {0xed, 0x05},

  {0xb1, 0x00},
  {0xb2, 0x62},
  {0xb3, 0x00},
  {0xb4, 0x00},
  {0xb5, 0x01},
  {0xb6, 0xa3},
  {0xb7, 0x02},
  {0xb8, 0x80},

  {0x77, 0x00},
  {0x78, 0x10},

  {0xef, 0x00},
  {0x93, 0x40},
  {0x94, 0x80},
  {0x95, 0xc0},
  {0x96, 0xc0},
  {0x97, 0x20},
  {0x98, 0x20},
  {0x99, 0x30},
  {0xA0, 0x00},
  {0xA1, 0x00},
  {0xA2, 0x1c},
  {0xA3, 0x16},
  {0xA4, 0x03},
  {0xA5, 0x07},
  {0xA6, 0x00},

  {0xef, 0x00},
  {0xad, 0xd0},
  {0xaf, 0x10},

  {0xef, 0x00},
  {0x42, 0x5d},
  {0x44, 0x60},

  {0x57, 0x00},

  {0xef, 0x03},
  {0x00, 0x07},
  {0x01, 0x3c},
  {0x02, 0x05},
  {0x03, 0x21},
  {0x04, 0x73},
  {0x06, 0x1c},
  {0x07, 0x01},
  {0x08, 0x10},
  {0x0b, 0x01},

  {0x51, 0x10},
  {0x52, 0x00},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x22},
  {0x56, 0x01},
  {0x57, 0x61},
  {0x58, 0x25},
  {0x67, 0xcf},
  {0x69, 0x17},

  {0xef, 0x00},
  {0x58, 0x00},
  {0x59, 0x00},
  {0x5a, 0x02},
  {0x5b, 0x73},
  {0x5c, 0x00},
  {0x5d, 0x00},
  {0x5e, 0x01},
  {0x5f, 0xe0},
  {0x60, 0x00},
  {0x61, 0xb1},
  {0x62, 0x01},
  {0x63, 0xbe},
  {0x64, 0x00},
  {0x65, 0x81},
  {0x66, 0x01},
  {0x67, 0x61},

  {0xef, 0x00},
  {0x6a, 0x01},
  {0x6b, 0xe0},
  {0x6c, 0x05},
  {0x6d, 0x00},
  {0x6e, 0x0e},
  {0x6f, 0x00},
  {0x70, 0x10},

  {0xef, 0x03},
  {0x22, 0x24},
  {0x3e, 0x23},
  {0x3f, 0x23},
  {0x40, 0x00},
  {0x41, 0x09},
  {0x4a, 0x09},
  {0x4b, 0x04},
  {0x5b, 0x20},
  {0x5d, 0x35},
  {0x5e, 0x13},
  {0x78, 0x0f},

  {0xef, 0x00},
  {0x4c, 0x80},
  {0x4d, 0xbb},
  {0x4e, 0x84},
  {0x4f, 0x91},
  {0x50, 0x64},
  {0x51, 0x93},
  {0x52, 0x03},
  {0x53, 0xc1},
  {0x54, 0x83},

  {0xef, 0x03},
  {0x70, 0x00},		//mirror default in vt call

  {0xef, 0x00},
  {0x48, 0x00},
  {0x49, 0x00},
  {0x4A, 0x03},
  {0x48, 0x01},
  {0x49, 0x00},
  {0x4A, 0x08},
  {0x48, 0x02},
  {0x49, 0x00},
  {0x4A, 0x4A}, //
  {0x48, 0x03},
  {0x49, 0x00},
  {0x4A, 0x96},
  {0x48, 0x04},
  {0x49, 0x01},
  {0x4A, 0x24},
  {0x48, 0x05},
  {0x49, 0x01},
  {0x4A, 0xB4},
  {0x48, 0x06},
  {0x49, 0x02},
  {0x4A, 0x23},
  {0x48, 0x07},
  {0x49, 0x02},
  {0x4A, 0x72},
  {0x48, 0x08},
  {0x49, 0x02},
  {0x4A, 0xBE},
  {0x48, 0x09},
  {0x49, 0x02},
  {0x4A, 0xFA},
  {0x48, 0x0A},
  {0x49, 0x03},
  {0x4A, 0x27},
  {0x48, 0x0B},
  {0x49, 0x03},
  {0x4A, 0x55},
  {0x48, 0x0C},
  {0x49, 0x03},
  {0x4A, 0x81},
  {0x48, 0x0D},
  {0x49, 0x03},
  {0x4A, 0xA2},
  {0x48, 0x0E},
  {0x49, 0x03},
  {0x4A, 0xBC},
  {0x48, 0x0F},
  {0x49, 0x03},
  {0x4A, 0xD4},
  {0x48, 0x10},
  {0x49, 0x03},
  {0x4A, 0xE8},
  {0x48, 0x11},
  {0x49, 0x03},
  {0x4A, 0xF4},
  {0x48, 0x12},
  {0x49, 0x03},
  {0x4A, 0xFF},
  {0x48, 0x20},
  {0x49, 0x00},
  {0x4A, 0x03},
  {0x48, 0x21},
  {0x49, 0x00},
  {0x4A, 0x08},
  {0x48, 0x22},
  {0x49, 0x00},
  {0x4A, 0x4A}, //
  {0x48, 0x23},
  {0x49, 0x00},
  {0x4A, 0x96},
  {0x48, 0x24},
  {0x49, 0x01},
  {0x4A, 0x24},
  {0x48, 0x25},
  {0x49, 0x01},
  {0x4A, 0xB4},
  {0x48, 0x26},
  {0x49, 0x02},
  {0x4A, 0x23},
  {0x48, 0x27},
  {0x49, 0x02},
  {0x4A, 0x72},
  {0x48, 0x28},
  {0x49, 0x02},
  {0x4A, 0xBE},
  {0x48, 0x29},
  {0x49, 0x02},
  {0x4A, 0xFA},
  {0x48, 0x2A},
  {0x49, 0x03},
  {0x4A, 0x27},
  {0x48, 0x2B},
  {0x49, 0x03},
  {0x4A, 0x55},
  {0x48, 0x2C},
  {0x49, 0x03},
  {0x4A, 0x81},
  {0x48, 0x2D},
  {0x49, 0x03},
  {0x4A, 0xA2},
  {0x48, 0x2E},
  {0x49, 0x03},
  {0x4A, 0xBC},
  {0x48, 0x2F},
  {0x49, 0x03},
  {0x4A, 0xD4},
  {0x48, 0x30},
  {0x49, 0x03},
  {0x4A, 0xE8},
  {0x48, 0x31},
  {0x49, 0x03},
  {0x4A, 0xF4},
  {0x48, 0x32},
  {0x49, 0x03},
  {0x4A, 0xFF},
  {0x48, 0x40},
  {0x49, 0x00},
  {0x4A, 0x03},
  {0x48, 0x41},
  {0x49, 0x00},
  {0x4A, 0x08},
  {0x48, 0x42},
  {0x49, 0x00},
  {0x4A, 0x4A}, //
  {0x48, 0x43},
  {0x49, 0x00},
  {0x4A, 0x96},
  {0x48, 0x44},
  {0x49, 0x01},
  {0x4A, 0x24},
  {0x48, 0x45},
  {0x49, 0x01},
  {0x4A, 0xB4},
  {0x48, 0x46},
  {0x49, 0x02},
  {0x4A, 0x23},
  {0x48, 0x47},
  {0x49, 0x02},
  {0x4A, 0x72},
  {0x48, 0x48},
  {0x49, 0x02},
  {0x4A, 0xBE},
  {0x48, 0x49},
  {0x49, 0x02},
  {0x4A, 0xFA},
  {0x48, 0x4A},
  {0x49, 0x03},
  {0x4A, 0x27},
  {0x48, 0x4B},
  {0x49, 0x03},
  {0x4A, 0x55},
  {0x48, 0x4C},
  {0x49, 0x03},
  {0x4A, 0x81},
  {0x48, 0x4D},
  {0x49, 0x03},
  {0x4A, 0xA2},
  {0x48, 0x4E},
  {0x49, 0x03},
  {0x4A, 0xBC},
  {0x48, 0x4F},
  {0x49, 0x03},
  {0x4A, 0xD4},
  {0x48, 0x50},
  {0x49, 0x03},
  {0x4A, 0xE8},
  {0x48, 0x51},
  {0x49, 0x03},
  {0x4A, 0xF4},
  {0x48, 0x52},
  {0x49, 0x03},
  {0x4A, 0xFF},
};


/* Exposure Value Setting */
/* EV bias */
/*
struct s5ka3dfx_reg s5ka3dfx_ev_m5[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0xb0 },
	{ 0x32, 0x98 },
}; */

struct s5ka3dfx_reg s5ka3dfx_ev_m4[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0xa7 },
	{ 0x32, 0x90 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_m3[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x98 },
	{ 0x32, 0x88 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_m2[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x90 },
	{ 0x32, 0x00 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_m1[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x88 },
	{ 0x32, 0x08 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_default[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x00 },
	{ 0x32, 0x0c },
};

struct s5ka3dfx_reg s5ka3dfx_ev_p1[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x08 },
	{ 0x32, 0x20 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_p2[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x10 },
	{ 0x32, 0x30 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_p3[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x20 },
	{ 0x32, 0x38 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_p4[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x28 },
	{ 0x32, 0x40 },
};

/*
struct s5ka3dfx_reg s5ka3dfx_ev_p5[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x30 },
	{ 0x32, 0x48 },
}; */

/* EV bias for VT  */
/*
struct s5ka3dfx_reg s5ka3dfx_ev_vt_m5[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0xa9 },
	{ 0x32, 0xa4 },
}; */

struct s5ka3dfx_reg s5ka3dfx_ev_vt_m4[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x99 },
	{ 0x32, 0x9c },
};

struct s5ka3dfx_reg s5ka3dfx_ev_vt_m3[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x91 },
	{ 0x32, 0x94 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_vt_m2[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x89 },
	{ 0x32, 0x08 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_vt_m1[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x81 },
	{ 0x32, 0x10 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_vt_default[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x07 },
	{ 0x32, 0x18 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_vt_p1[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x0e },
	{ 0x32, 0x28 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_vt_p2[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x17 },
	{ 0x32, 0x38 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_vt_p3[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x27 },
	{ 0x32, 0x40 },
};

struct s5ka3dfx_reg s5ka3dfx_ev_vt_p4[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x2a },
	{ 0x32, 0x3e },
};

/*
struct s5ka3dfx_reg s5ka3dfx_ev_vt_p5[] = {
	{ 0xef, 0x03 },
	{ 0x31, 0x32 },
	{ 0x32, 0x46 },
}; */

/* White Balance Setting */
struct s5ka3dfx_reg s5ka3dfx_wb_auto[] = {
  {0xef, 0x03},
  {0x00, 0x07},  
  {0xef, 0x00},  
  {0x42, 0x65},  
  {0x43, 0x40},  
  {0x44, 0x60},  
};

struct s5ka3dfx_reg s5ka3dfx_wb_tungsten[] = {
  {0xef, 0x03},
  {0x00, 0x05},  
  {0xef, 0x00},  
  {0x42, 0x44},  
  {0x43, 0x3e},  
  {0x44, 0x79},  
};

struct s5ka3dfx_reg s5ka3dfx_wb_fluorescent[] = {
  {0xef, 0x03},
  {0x00, 0x05},  
  {0xef, 0x00},  
  {0x42, 0x5d},  
  {0x43, 0x40},  
  {0x44, 0x70}, 
};

struct s5ka3dfx_reg s5ka3dfx_wb_sunny[] = {
  {0xef, 0x03},
  {0x00, 0x05},  
  {0xef, 0x00},  
  {0x42, 0x68},  
  {0x43, 0x3e},  
  {0x44, 0x4e},    
};

struct s5ka3dfx_reg s5ka3dfx_wb_cloudy[] = {
  {0xef, 0x03},
  {0x00, 0x05},  
  {0xef, 0x00},  
  {0x42, 0x83},  
  {0x43, 0x40},  
  {0x44, 0x48},    
};

/* Effect Setting */
struct s5ka3dfx_reg s5ka3dfx_effect_none[] = {
	{ 0xef, 0x00 },
	{ 0xd3, 0x00 },
	{ 0xd4, 0x00 },
	{ 0xd5, 0x01 },
	{ 0xd6, 0xa3 },
};

struct s5ka3dfx_reg s5ka3dfx_effect_gray[] = {
	{ 0xef, 0x00 },
	{ 0xd3, 0x00 },
	{ 0xd4, 0x03 },
	{ 0xd5, 0x80 },
	{ 0xd6, 0x80 },
};

struct s5ka3dfx_reg s5ka3dfx_effect_sepia[] = {
	{ 0xef, 0x00 },
	{ 0xd3, 0x00 },
	{ 0xd4, 0x03 },
	{ 0xd5, 0x60 },
	{ 0xd6, 0x8c },
};

struct s5ka3dfx_reg s5ka3dfx_effect_negative[] = {
	{ 0xef, 0x00 },
	{ 0xd3, 0x01 },
	{ 0xd4, 0x00 },
	{ 0xd5, 0x2c },
	{ 0xd6, 0x81 },
};

struct s5ka3dfx_reg s5ka3dfx_effect_aqua[] = {
	{ 0xef, 0x00 },
	{ 0xd3, 0x00 },
	{ 0xd4, 0x03 },
	{ 0xd5, 0xdc },
	{ 0xd6, 0x00 },
};

struct s5ka3dfx_reg s5ka3dfx_effect_red[] = 
{
  { 0xef, 0x00 },
  { 0xd3, 0x00 },
  { 0xd4, 0x13 },
  { 0xd5, 0xaa },
  { 0xd6, 0x81 },
};

struct s5ka3dfx_reg s5ka3dfx_effect_green[] = 
{
  { 0xef, 0x00 },
  { 0xd3, 0x00 },
  { 0xd4, 0x03 },
  { 0xd5, 0x2c },
  { 0xd6, 0x41 },
};

/* Blur Setting */
/*Self shot*/
struct s5ka3dfx_reg s5ka3dfx_blur_none[] = {
  {0xef, 0x03},
  {0x33, 0x80},
  {0x34, 0x79},
};

struct s5ka3dfx_reg s5ka3dfx_blur_p1[] = {
  {0xef, 0x03},
  {0x33, 0x90},
  {0x34, 0x89},
};

struct s5ka3dfx_reg s5ka3dfx_blur_p2[] = {
  {0xef, 0x03},
  {0x33, 0xA0},
  {0x34, 0x99},
};

struct s5ka3dfx_reg s5ka3dfx_blur_p3[] = {
  {0xef, 0x03},
  {0x33, 0xB0},
  {0x34, 0xA9},
};

/*vt call*/
struct s5ka3dfx_reg s5ka3dfx_blur_vt_none[] = {
  {0xef, 0x03},
  {0x33, 0x80},
  {0x34, 0x79},
};

struct s5ka3dfx_reg s5ka3dfx_blur_vt_p1[] = {
  {0xef, 0x03},
  {0x33, 0x90},
  {0x34, 0x89},
};

struct s5ka3dfx_reg s5ka3dfx_blur_vt_p2[] = {
  {0xef, 0x03},
  {0x33, 0xA0},
  {0x34, 0x99},
};

struct s5ka3dfx_reg s5ka3dfx_blur_vt_p3[] = {
  {0xef, 0x03},
  {0x33, 0xB0},
  {0x34, 0xA9},
};

struct s5ka3dfx_reg s5ka3dfx_dataline[] = {
	{ 0xef, 0x00 },
	{ 0x0a, 0x01 },		/* s/w reset */
	{ 0x04, 0x01 },
	{ 0xcb, 0x01 },
	{ 0xd1, 0x08 },
};

struct s5ka3dfx_reg s5ka3dfx_dataline_stop[] = {
	{ 0xef, 0x00 },
	{ 0xcb, 0x00 },
};

/* FPS */
struct s5ka3dfx_reg s5ka3dfx_fps_7[] = {
	{ 0xEF, 0x03 },
	{ 0x5F, 0x03 },
	{ 0x60, 0x02 },
	{ 0x61, 0x11 },
	{ 0x62, 0x0E },
	{ 0x63, 0x06 },
	{ 0x64, 0x4B },
	{ 0x65, 0x06 },
	{ 0x66, 0x4B },
	{ 0x48, 0x00 },
	{ 0x49, 0x9E },
	{ 0x4C, 0x00 },
	{ 0x4D, 0x9E },
	{ 0xEF, 0x03 },
	{ 0x51, 0x10 },
	{ 0x52, 0x00 },
	{ 0x53, 0x00 },
	{ 0x54, 0x00 },
	{ 0x56, 0x01 },
	{ 0x57, 0x61 },
	{ 0x58, 0x25 },
	{ 0x67, 0xCF },
};

struct s5ka3dfx_reg s5ka3dfx_fps_10[] = {
	{ 0xEF, 0x03 },
	{ 0x5F, 0x03 },
	{ 0x60, 0x02 },
	{ 0x61, 0x0C },
	{ 0x62, 0x0A },
	{ 0x63, 0x03 },
	{ 0x64, 0xD3 },
	{ 0x65, 0x03 },
	{ 0x66, 0xD3 },
	{ 0x48, 0x00 },
	{ 0x49, 0x9E },
	{ 0x4C, 0x00 },
	{ 0x4D, 0x9E },
	{ 0xEF, 0x03 },
	{ 0x51, 0x10 },
	{ 0x52, 0x00 },
	{ 0x53, 0x00 },
	{ 0x54, 0x00 },
	{ 0x56, 0x01 },
	{ 0x57, 0x61 },
	{ 0x58, 0x25 },
	{ 0x67, 0xCF },
};

struct s5ka3dfx_reg s5ka3dfx_fps_15[] = {
	{ 0xEF, 0x03 },
	{ 0x5F, 0x03 },
	{ 0x60, 0x02 },
	{ 0x61, 0x08 },
	{ 0x62, 0x06 },
	{ 0x63, 0x01 },
	{ 0x64, 0xE7 },
	{ 0x65, 0x01 },
	{ 0x66, 0xE7 },
	{ 0x48, 0x00 },
	{ 0x49, 0x9E },
	{ 0x4C, 0x00 },
	{ 0x4D, 0x9E },
	{ 0xEF, 0x03 },
	{ 0x51, 0x10 },
	{ 0x52, 0x00 },
	{ 0x53, 0x00 },
	{ 0x54, 0x00 },
	{ 0x56, 0x01 },
	{ 0x57, 0x61 },
	{ 0x58, 0x25 },
	{ 0x67, 0xCF },
};

struct s5ka3dfx_reg s5ka3dfx_vt_fps_7[] = {
	{ 0xef, 0x03 },
	{ 0x50, 0xd2 },
	{ 0x0f, 0x31 },
	{ 0xef, 0x03 },
	{ 0x5f, 0x11 },
	{ 0x60, 0x0e },
	{ 0x61, 0x11 },
	{ 0x62, 0x0e },
	{ 0x63, 0x06 },
	{ 0x64, 0x4b },
	{ 0x65, 0x06 },
	{ 0x66, 0x4b },
	{ 0x6d, 0x6c },
	{ 0x6E, 0x40 },
	{ 0x6f, 0x40 },
	{ 0x4c, 0x00 },
	{ 0x4d, 0x9E },
	{ 0x51, 0x10 },
	{ 0x52, 0x00 },
	{ 0x53, 0x00 },
	{ 0x54, 0x00 },
	{ 0x55, 0x22 },
	{ 0x56, 0x01 },
	{ 0x57, 0x61 },
	{ 0x58, 0x25 },
	{ 0x67, 0xcf },
};

struct s5ka3dfx_reg s5ka3dfx_vt_fps_10[] = {
	{ 0xef, 0x03 },
	{ 0x50, 0xd2 },
	{ 0x0f, 0x31 },
	{ 0xef, 0x03 },
	{ 0x5f, 0x0c },
	{ 0x60, 0x0a },
	{ 0x61, 0x0c },
	{ 0x62, 0x0a },
	{ 0x63, 0x03 },
	{ 0x64, 0xd3 },
	{ 0x65, 0x03 },
	{ 0x66, 0xd3 },
	{ 0x6d, 0x6C },
	{ 0x6E, 0x40 },
	{ 0x6f, 0x40 },
	{ 0x4c, 0x00 },
	{ 0x4d, 0x9E },
	{ 0x51, 0x10 },
	{ 0x52, 0x00 },
	{ 0x53, 0x00 },
	{ 0x54, 0x00 },
	{ 0x55, 0x22 },
	{ 0x56, 0x01 },
	{ 0x57, 0x61 },
	{ 0x58, 0x25 },
	{ 0x67, 0xcf },
};

struct s5ka3dfx_reg s5ka3dfx_vt_fps_15[] = {
	{ 0xef, 0x03 },
	{ 0x50, 0xd2 },
	{ 0x0f, 0x31 },
	{ 0xef, 0x03 },
	{ 0x5f, 0x08 },
	{ 0x60, 0x06 },
	{ 0x61, 0x08 },
	{ 0x62, 0x06 },
	{ 0x63, 0x01 },
	{ 0x64, 0xE7 },
	{ 0x65, 0x01 },
	{ 0x66, 0xE7 },
	{ 0x6d, 0x6C },
	{ 0x6E, 0x40 },
	{ 0x6f, 0x40 },
	{ 0x4c, 0x00 },
	{ 0x4d, 0x9E },
	{ 0x51, 0x10 },
	{ 0x52, 0x00 },
	{ 0x53, 0x00 },
	{ 0x54, 0x00 },
	{ 0x55, 0x22 },
	{ 0x56, 0x01 },
	{ 0x57, 0x61 },
	{ 0x58, 0x25 },
	{ 0x67, 0xcf },
};
#endif
