/*****************************************************************
File:             BMS33M332.h
Author:           Weng, BESTMODULES
Description:      Define classes and required variables
History：         
V1.0.1   -- initial version；2021-06-25；Arduino IDE :v1.8.15
******************************************************************/

#ifndef _BMS33M332_H_
#define _BMS33M332_H_

#include <Wire.h>
#include <Arduino.h>

#define OK                0x01
#define ERROR             0x02
const uint8_t BMS33M332_IICADDR = 0x47;

/*State Register*/
#define EN_PS             0x00
#define EN_ALS            0x01
#define EN_WAIT           0x02
#define EN_INTELLI_WAIT   0x03
#define EN_CTAUTOK        0x04
/*LED constant current setting*/
#define CURRENT_3_125MA   0x00
#define CURRENT_6_25MA    0x01
#define CURRENT_12_5MA    0x02
#define CURRENT_25MA      0x03
#define CURRENT_50MA      0x04
#define CURRENT_100MA     0x05
#define CURRENT_150MA     0x06
/*PS integration time*/
#define IT_PS_96US        0x00
#define IT_PS_192US       0x01
#define IT_PS_384US       0x02
#define IT_PS_768US       0x03
#define IT_PS_1_54MS      0x04
#define IT_PS_3_07MS      0x05
#define IT_PS_6_14MS      0x06
/*GAIN_PS*/
#define GAIN_PS_x1        0x00     
#define GAIN_PS_x2        0x01  
#define GAIN_PS_x4        0x02  
#define GAIN_PS_x8        0x03 
/*PS Persistence Setting*/
#define PRST_PS_x1        0x00
#define PRST_PS_x2        0x01
#define PRST_PS_x4        0x02
#define PRST_PS_x16       0x03

/*ALS integration time*/
#define IT_ALS_25MS       0x00
#define IT_ALS_50MS       0x01
#define IT_ALS_100MS      0x02
#define IT_ALS_200MS      0x03
#define IT_ALS_400MS      0x04
#define IT_ALS_800MS      0x05
#define IT_ALS_1600MS     0x06
/*GAIN_ALS*/
#define GAIN_ALS_x1       0x00 
#define GAIN_ALS_x4       0x01 
#define GAIN_ALS_x16      0x02 
#define GAIN_ALS_x64      0x03 
/*ALS Persistence Setting*/
#define PRST_ALS_x1       0x00
#define PRST_ALS_x2       0x01
#define PRST_ALS_x4       0x02
#define PRST_ALS_x8       0x03   
/*GAIN_C*/
#define GAIN_C_x1         0x00 
#define GAIN_C_x4         0x01 
#define GAIN_C_x16        0x02 
#define GAIN_C_x64        0x03 

#define ENABLE            1
#define DISABLE           0
/*STK3332 REGS*/
#define     STATE_REG                 0x00
#define     PSCTRL_REG                0x01
#define     ALSCTRL_REG               0x02
#define     LEDCTRL_REG               0x03
#define     INTCTRL1_REG              0x04
#define     WAIT_REG                  0x05
#define     THDH1_PS_REG              0x06
#define     THDH2_PS_REG              0x07
#define     THDL1_PS_REG              0x08
#define     THDL2_PS_REG              0x09
#define     THDH1_ALS_REG             0x0A
#define     THDH2_ALS_REG             0x0B
#define     THDL1_ALS_REG             0x0C
#define     THDL2_ALS_REG             0x0D
#define     FLAG_REG                  0x10
#define     DATA1_PS_REG              0x11
#define     DATA2_PS_REG              0x12
#define     DATA1_ALS_REG             0x13
#define     DATA2_ALS_REG             0x14
#define     DATA1_C_REG               0x1B
#define     DATA2_C_REG               0x1C
#define     DATA1_PS_OFFSET_REG       0x1D
#define     DATA2_PS_OFFSET_REG       0x1E
#define     DATA_CTIR1_REG            0x20
#define     DATA_CTIR2_REG            0x21
#define     DATA_CTIR3_REG            0x22
#define     DATA_CTIR4_REG            0x23
#define     PDT_ID_REG                0x3E
#define     RSRVD_REG                 0x3F
#define     ALSCTRL2_REG              0x4E
#define     INTELLI_WAIT_PS_REG       0x4F
#define     SOFT_RESET_REG            0x80
#define     INTCTRL2_REG              0xA5

class BMS33M332
{
   public:
   BMS33M332(uint8_t intPin,TwoWire *theWire = &Wire);
   void begin(uint8_t addr = BMS33M332_IICADDR);

   uint16_t readRawProximity();
   uint16_t readRawAmbient();
   float readAmbient();
   uint8_t getPDTID();
   void setINT(uint16_t thdh,uint16_t thdl,bool isEnable = true);
   uint8_t getINT();
   uint8_t getPositionStatus();
   void reset();

   void writeReg(uint8_t addr, uint8_t data);
   uint8_t readReg(uint8_t addr);
   void readReg(uint8_t addr, uint8_t rBuf[], uint8_t rLen);
   
   uint8_t getLEDcurrent();
   uint8_t getMeasureIntervalTime();
   uint16_t getPSHighThreshold();
   uint16_t getPSLowThreshold();
   uint16_t getALSHighThreshold();
   uint16_t getALSLowThreshold();
   
   void setLEDcurrent(uint8_t current);
   void setMeasureIntervalTime(uint8_t time,bool isEnable = true);
   void setPSHighThreshold(uint16_t thdh);
   void setPSLowThreshold(uint16_t thdl);
   void setALSHighThreshold(uint16_t thdh);
   void setALSLowThreshold(uint16_t thdl);
   
   private:
   uint16_t readClearChannelValue();
   void setALSIntegrationTime(uint8_t time);
   void setPSIntegrationTime(uint8_t time);
   void setALSClearChannelGain(uint8_t gain);
   void setPSGain(uint8_t gain);
   void setALSGain(uint8_t gain);
   void setPSIntelligentPersistence(uint8_t time,bool isEnable = true);
   void setALSIntelligentPersistence(uint8_t time,bool isEnable = true);
   void setPSOffset(uint16_t offset);
   uint16_t getPSOffset();

   void writeBytes(uint8_t wbuf[], uint8_t wlen);
   void writeRegBit(uint8_t addr,uint8_t bitNum, uint8_t bitValue);
   void readBytes(uint8_t rbuf[], uint8_t rlen);

   void clearBuf();
   uint8_t _i2caddr;
   uint8_t dataBuff[4];   //Store public data
   int dataCnt = 0;
   uint8_t _intPin;
   /*LUX/LSB Related parameters*/
   uint8_t _alsIt   = 4;
   uint8_t _alsGain = 1;
   float   _alsLsb  = 0.2051;//Gain*1 IT*4
   TwoWire *_wire;
};

/*Clear FLAG Register*/
#define Clr_FLG_NF()                writeRegBit(FLAG_REG, 0, 0); //bit0
#define Clr_FLG_INVALID_PS_INT()    writeRegBit(FLAG_REG, 1, 0); //bit1
#define Clr_FLG_ALS_SAT()           writeRegBit(FLAG_REG, 2, 0); //bit2
#define Clr_FLG_PS_INT()            writeRegBit(FLAG_REG, 4, 0); //bit4
#define Clr_FLG_ALS_INT()           writeRegBit(FLAG_REG, 5, 0); //bit5
#define Clr_FLG_PS_DR()             writeRegBit(FLAG_REG, 6, 0); //bit6
#define Clr_FLG_ALS_DR()            writeRegBit(FLAG_REG, 7, 0); //bit7


#endif
