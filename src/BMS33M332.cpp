/*****************************************************************
File:        BMS33M332.cpp
Author:      Weng, BESTMODULES
Description: IIC communication with the sensor and obtain the corresponding value  
History:         
V1.0.1   -- initial version；2021-06-25；Arduino IDE :v1.8.15
******************************************************************/
#include "BMS33M332.h"
/**********************************************************
Description: Constructor
Parameters:  intPin :INT Output pin connection with Arduino 
             theWire : Wire object if your board has more than one I2C interface        
Return:      none    
Others:      none
**********************************************************/
BMS33M332::BMS33M332(uint8_t intPin,TwoWire *theWire)
{
     _intPin = intPin;
     _wire = theWire;
}
/**********************************************************
Description: Module Initial
Parameters:  addr :Module IIC address      
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::begin(uint8_t addr)
{
      pinMode(_intPin,INPUT);
      _i2caddr = addr;
      _wire->begin(); 
      /*-------STATE_REG 0x00---------*/
      writeRegBit(STATE_REG, EN_PS, ENABLE);//Enable EN_PS
      writeRegBit(STATE_REG, EN_ALS, ENABLE);//Enable EN_ALS
      setMeasureIntervalTime(0, true);//Enable EN_WAIT,wait period = 1.54 ms
      /*PS initial*/
      setPSIntelligentPersistence(PRST_PS_x1);//Enable EN_INTRLLI_PRST
      setPSGain(GAIN_PS_x8);
      setPSIntegrationTime(IT_PS_96US);
      /*ALS initial*/
      setALSIntelligentPersistence(PRST_ALS_x1);
      setALSGain(GAIN_ALS_x1);
      setALSIntegrationTime(IT_ALS_25MS);
      /*LED initial*/
      setLEDcurrent(CURRENT_100MA); 
}
/**********************************************************
Description: get PS ADC raw data
Parameters:  none
Return:      Proximity sensing AD data(2 byte)
Others:      none
**********************************************************/
uint16_t BMS33M332::readRawProximity()
{
      uint16_t psValue = 0;
      readReg(DATA1_PS_REG,dataBuff,2);
      psValue = ((uint16_t)dataBuff[0]<<8 | dataBuff[1]);
      return psValue;
}
/**********************************************************
Description: get ALS ADC raw data
Parameters:  none
Return:      Ambient light AD data(2 byte)    
Others:      none
**********************************************************/
uint16_t BMS33M332::readRawAmbient()
{   
      uint16_t alsValue = 0;
      readReg(DATA1_ALS_REG,dataBuff,2);
      alsValue = ((uint16_t)dataBuff[0]<<8 | dataBuff[1]);
      return alsValue;
}
/**********************************************************
Description: get ALS data
Parameters:  none
Return:      Ambient light data(unit:LUX)    
Others:      none
**********************************************************/
float BMS33M332::readAmbient()
{   
      uint16_t alsValue = 0;
      float ambient=0;
      _alsLsb = 0.8204/_alsIt/_alsGain;
      alsValue = readRawAmbient();
      ambient = alsValue * _alsLsb;
      return ambient;   
}
/**********************************************************
Description:Get product ID
Parameters: none
Return:     Product ID(1 byte)         
Others: PDT_ID = Product ID =0x52 to indicate the product information
**********************************************************/
uint8_t BMS33M332::getPDTID()
{
     uint8_t idVlaue = 0;
     readReg(PDT_ID_REG,dataBuff,1);
     idVlaue = dataBuff[0];
     return idVlaue;
}
/**********************************************************
Description: PS INT Function
Parameters:  thdh:PS high threshold
             thdl:PS low threshold
             isEnable = ture, enable ps_INT(default)
             isEnable = false,disable ps_INT     
Return:      none    
Others: PS_NF_MODE:When PS_ ADC exceeds thdh, int_ Pin output Low level, 
        When PS_ ADC lower than thdl, int_ Pin output High level 
**********************************************************/
void BMS33M332::setINT(uint16_t thdh,uint16_t thdl,bool isEnable)
{  
      if(isEnable == true)
      {
            writeReg(INTCTRL1_REG, 0x03);//set EN_PS_INT,PS_NF_MODE    
            writeReg(THDH1_PS_REG, thdh>>8);
            writeReg(THDH2_PS_REG, thdh);   //THDH_PS 
            writeReg(THDL1_PS_REG, thdl>>8);
            writeReg(THDL2_PS_REG, thdl);   //THDL_PS 
      }
      if(isEnable == false)
      {
          writeReg(INTCTRL1_REG, 0x00);//set EN_PS_INT,PS_NF_MODE  
      }
}
/**********************************************************
Description: getINT
Parameters:  
Return:      INT PIN status
Others:      1:INT status is HIGH
             0:INT status is LOW
**********************************************************/
uint8_t BMS33M332::getINT()
{
      uint8_t status = 0;
      status = digitalRead(_intPin);
      return status;
}
/**********************************************************
Description: getPositionStatus
Parameters:  
Return:      INT PIN status 1bit（0/1）
Others:      Object near/far status. Default status = 1, object in far state.
             0 : Object in near state
             1 : Object in far state
**********************************************************/
uint8_t BMS33M332::getPositionStatus()
{
      uint8_t status = 0;
      readReg(FLAG_REG,dataBuff,1);
      status = dataBuff[0] & 0x01; //bit 0
      return status;
}
/**********************************************************
Description: soft_reset
Parameters:  none    
Return:      none    
Others:      Write any value to execute reset
**********************************************************/
void BMS33M332::reset()
{
     writeReg(SOFT_RESET_REG, 0x55);  
}
/**********************************************************
Description: writeReg
Parameters:  addr :Register to be written
             data:Value to be written     
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::writeReg(uint8_t addr, uint8_t data)
{
    uint8_t sendBuf[2]={addr,data};
    writeBytes(sendBuf,2);
    delay(1);
}
/**********************************************************
Description: read Register data
Parameters:  addr :Register to be written    
Return:      one byte of the data     
Others:      user can use this function to read any register  
             including something are not mentioned.
**********************************************************/
uint8_t BMS33M332::readReg(uint8_t addr)
{
    clearBuf();
    uint8_t sendBuf[1] = {addr};
    writeBytes(sendBuf,1);
    delay(1);
    readBytes(dataBuff,1);
    delay(1);
    return dataBuff[0];
}
/**********************************************************
Description: read Register to get Data
Parameters:  addr:Register to be written
             rBuf:Variables for storing Data to be obtained
             rLen:the byte of the data       
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::readReg(uint8_t addr, uint8_t rBuf[], uint8_t rLen)
{
    clearBuf();
    uint8_t sendBuf[1] = {addr};
    writeBytes(sendBuf,1);
    delay(1);
    readBytes(rBuf,rLen);
    delay(1);
}
/**********************************************************
Description: get LED constant current
Parameters:  currentValue:Variables for storing current data
Return:      currentValue :0x00~0x06
                   0x00 : CURRENT_3_125MA
                   0x01 : CURRENT_6_25MA
                   0x02 : CURRENT_12_5MA
                   0x03 : CURRENT_25MA
                   0x04 : CURRENT_50MA
                   0x05 : CURRENT_100MA
                   0x06 : CURRENT_150MA    
Others:       none
**********************************************************/
uint8_t BMS33M332::getLEDcurrent()
{      
       uint8_t currentValue = 0;
       readReg(LEDCTRL_REG,dataBuff,1);
       currentValue = dataBuff[0] >> 5;
       return currentValue;
}
/**********************************************************
Description: get Measure Interval wait time
Parameters:  none        
Return:      time : WAIT register(0x05) data
Others:      none
**********************************************************/
uint8_t BMS33M332::getMeasureIntervalTime()
{
    uint8_t time = 0;
    readReg(WAIT_REG,dataBuff,1);
    time = dataBuff[0];
    return time;
}
/**********************************************************
Description: getPSHighThreshold
Parameters:  none  
Return:      thdh:PS high threshold(2 Byte)      
Others:      none
**********************************************************/
uint16_t BMS33M332::getPSHighThreshold()
{
      uint16_t thdh = 0;
      readReg(THDH1_PS_REG,dataBuff,2);
      thdh = ((uint16_t)dataBuff[0]<<8 | dataBuff[1]);
      return thdh;
      
}
/**********************************************************
Description: getPSLowThreshold
Parameters:  none  
Return:      thdl:PS Low threshold(2 Byte)      
Others:      none
**********************************************************/
uint16_t BMS33M332::getPSLowThreshold()
{
      uint16_t thdl = 0;
      readReg(THDL1_PS_REG,dataBuff,2);
      thdl = ((uint16_t)dataBuff[0]<<8 | dataBuff[1]);
      return thdl;   
}
/**********************************************************
Description: getALSHighThreshold
Parameters:  none  
Return:      thdh:ALS high threshold(2 Byte)      
Others:      none
**********************************************************/
uint16_t BMS33M332::getALSHighThreshold()
{
      uint16_t thdh = 0;
      readReg(THDH1_ALS_REG,dataBuff,2);
      thdh = ((uint16_t)dataBuff[0]<<8 | dataBuff[1]);
      return thdh;
}
/**********************************************************
Description: getALSLowThreshold
Parameters:  none  
Return:      thdl:ALS Low threshold(2 Byte)      
Others:      none
**********************************************************/
uint16_t BMS33M332::getALSLowThreshold()
{
      uint16_t thdl = 0;
      readReg(THDL1_ALS_REG,dataBuff,2);
      thdl = ((uint16_t)dataBuff[0]<<8 | dataBuff[1]);
      return thdl;   
}


/**********************************************************
Description: set LED constant current
Parameters:  current :CURRENT_3_125MA/CURRENT_6_25MA/CURRENT_12_5MA
                      CURRENT_25MA/CURRENT_50MA
                      CURRENT_100MA/CURRENT_150MA     
Return:      none   
Others:      none
**********************************************************/
void BMS33M332::setLEDcurrent(uint8_t current)
{
       writeRegBit(LEDCTRL_REG, 5, current & 0x01); //write in IRDR_LED[0] 
       writeRegBit(LEDCTRL_REG, 6, current & 0x02); //write in IRDR_LED[1] 
       writeRegBit(LEDCTRL_REG, 7, current & 0x04); //write in IRDR_LED[2]  
}
/**********************************************************
Description: set Measure Interval wait time
Parameters:  time : wait period = (time + 1) * 1.54 ms
             isEnable =  ture,  enable wait time(default)
             isEnable =  false, disable wait time
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::setMeasureIntervalTime(uint8_t time,bool isEnable)
{
       if(isEnable == true)
       {  
            writeRegBit(STATE_REG, EN_WAIT, ENABLE);     //write in EN_WAIT = 1,enable
            writeReg(WAIT_REG, time);  //wait period = (time + 1) * 1.54 ms
       }
       if(isEnable == false)
       {
            writeRegBit(STATE_REG, EN_WAIT, DISABLE);     //write in EN_WAIT = 0,disable
       }
}
/**********************************************************
Description: setPSHighThreshold
Parameters:  thdh:PS high threshold      
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::setPSHighThreshold(uint16_t thdh)
{
      writeReg(THDH1_PS_REG, thdh>>8);
      writeReg(THDH2_PS_REG, thdh);
}

/**********************************************************
Description: setPSLowThreshold
Parameters:  thdl:PS low threshold   
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::setPSLowThreshold(uint16_t thdl)
{
      writeReg(THDL1_PS_REG, thdl>>8);
      writeReg(THDL2_PS_REG, thdl);
}
/**********************************************************
Description: setALSHighThreshold
Parameters:  thdh:ALS high threshold      
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::setALSHighThreshold(uint16_t thdh)
{
      writeReg(THDH1_ALS_REG, thdh>>8);
      writeReg(THDH2_ALS_REG, thdh);
}
/**********************************************************
Description: setALSLowThreshold
Parameters:  thdl:ALS low threshold   
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::setALSLowThreshold(uint16_t thdl)
{
      writeReg(THDL1_ALS_REG, thdl>>8);
      writeReg(THDL2_ALS_REG, thdl);
}


/**********************************************************
Description: read Clear Channe lValue
Parameters:  none
Return:      clearChannelValue(2 byte)  
Others:      none
**********************************************************/
uint16_t BMS33M332::readClearChannelValue()
{
      uint16_t clearChannelValue = 0;
      readReg(DATA1_C_REG,dataBuff,2);
      clearChannelValue = ((uint16_t)dataBuff[0]<<8 | dataBuff[1]);
      return clearChannelValue;
}
/**********************************************************
Description: set ALS Integration Time
Parameters:  time:Optional:
               IT_ALS_25MS 
               IT_ALS_50MS     
               IT_ALS_100MS    
               IT_ALS_200MS   
               IT_ALS_400MS     
               IT_ALS_800MS    
               IT_ALS_1600MS      
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::setALSIntegrationTime(uint8_t time)
{
       if(time<=6) 
       {
          //_alsIt =  pow(2, time);
          _alsIt = 1;
          for(int i=0;i<time;i++)
          {
              _alsIt *=2;
          }
       }
       else _alsIt = 4;
       writeRegBit(ALSCTRL_REG, 0, time & 0x01);
       writeRegBit(ALSCTRL_REG, 1, time & 0x02); 
       writeRegBit(ALSCTRL_REG, 2, time & 0x04); 
       writeRegBit(ALSCTRL_REG, 3, time & 0x08); 
}
/**********************************************************
Description: setPSIntegrationTime
Parameters:  time:Optional:
              IT_PS_96US       
              IT_PS_192US      
              IT_PS_384US       
              IT_PS_768US      
              IT_PS_1_54MS      
              IT_PS_3_07MS      
              IT_PS_6_14MS        
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::setPSIntegrationTime(uint8_t time)
{
       writeRegBit(PSCTRL_REG, 0, time & 0x01); 
       writeRegBit(PSCTRL_REG, 1, time & 0x02); 
       writeRegBit(PSCTRL_REG, 2, time & 0x04); 
       writeRegBit(PSCTRL_REG, 3, time & 0x08); 
}
/**********************************************************
Description: set ALS Clear Channel Gain
Parameters:  gain:Optional:
              GAIN_C_x1       
              GAIN_C_x4       
              GAIN_C_x16   
              GAIN_C_x64    
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::setALSClearChannelGain(uint8_t gain)
{
       writeRegBit(ALSCTRL2_REG, 4, gain & 0x01); 
       writeRegBit(ALSCTRL2_REG, 5, gain & 0x02);      
}
/**********************************************************
Description: setPSGain
Parameters:  gain:Optional:
               GAIN_PS_x1
               GAIN_PS_x2
               GAIN_PS_x4
               GAIN_PS_x8            
Return:      none     
Others:      none
**********************************************************/
void BMS33M332::setPSGain(uint8_t gain)
{
       writeRegBit(PSCTRL_REG, 4, gain & 0x01); 
       writeRegBit(PSCTRL_REG, 5, gain & 0x02); 
}
/**********************************************************
Description: seALSGain
Parameters:  gain:Optional:
              GAIN_ALS_x1
              GAIN_ALS_x4
              GAIN_ALS_x16
              GAIN_ALS_x64
Return:      none     
Others:      none
**********************************************************/
void BMS33M332::setALSGain(uint8_t gain)
{
      
       switch(gain)
       {
            case GAIN_ALS_x1: {_alsGain = 1;break;}
            case GAIN_ALS_x4: {_alsGain = 4;break;}
            case GAIN_ALS_x16:{_alsGain = 16;break;}
            case GAIN_ALS_x64:{_alsGain = 64;break;}  
            default:          {_alsGain = 1;break;} 
       }
       writeRegBit(ALSCTRL_REG, 4, gain & 0x01); 
       writeRegBit(ALSCTRL_REG, 5, gain & 0x02); 
}
/**********************************************************
Description: set PS Intelligent Persistence
Parameters:  time: Optional:
               PRST_PS_x1      
               PRST_PS_x2        
               PRST_PS_x4      
               PRST_PS_x16  
             isEnable =  ture,  enable wait time(default)
             isEnable =  false,  disable wait time  
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::setPSIntelligentPersistence(uint8_t time,bool isEnable)
{
  if(isEnable == true)
  {
       writeRegBit(STATE_REG , EN_INTELLI_WAIT, ENABLE); 
       writeRegBit(PSCTRL_REG, 6, time & 0x01); 
       writeRegBit(PSCTRL_REG, 7, time & 0x02); 
  }
  if(isEnable == false) 
  {
        writeRegBit(STATE_REG, EN_INTELLI_WAIT, DISABLE); 
  } 
}
/**********************************************************
Description: set ALS Intelligent Persistence
Parameters:  time: Optional:
              PRST_ALS_x1
              PRST_ALS_x2 
              PRST_ALS_x4
              PRST_ALS_x8 
             isEnable =  ture,  enable wait time(default)
             isEnable =  false,  disable wait time    
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::setALSIntelligentPersistence(uint8_t time,bool isEnable)
{
  if(isEnable == true)
  {
       writeRegBit(STATE_REG , EN_INTELLI_WAIT, ENABLE); 
       writeRegBit(ALSCTRL_REG, 6, time & 0x01); 
       writeRegBit(ALSCTRL_REG, 7, time & 0x02); 
  }
  if(isEnable == false) 
  {
        writeRegBit(STATE_REG, EN_INTELLI_WAIT, DISABLE); 
  }       
}

/**********************************************************
Description: set PS Offset
Parameters:  offset:PS offset
Return:      none    
Others: The DATA_PS will be the ADC output subtract offset data.
**********************************************************/
void BMS33M332::setPSOffset(uint16_t offset)
{
     writeReg(DATA1_PS_OFFSET_REG, offset>>8);
     writeReg(DATA2_PS_OFFSET_REG, offset);
}
/**********************************************************
Description: get PS Offset
Parameters:  none
Return:      PS offset Value(2 byte)    
Others:      none
**********************************************************/
uint16_t BMS33M332::getPSOffset()
{
        uint16_t offsetValue = 0;
        readReg(DATA1_PS_OFFSET_REG,dataBuff,2);
        offsetValue = ((uint16_t)dataBuff[0]<<8 | dataBuff[1]);
        return offsetValue; 
}
/**********************************************************
Description: writeBytes
Parameters:  wbuf[]:Variables for storing Data to be sent
             wlen:Length of data sent  
Return:   
Others:
**********************************************************/
void BMS33M332::writeBytes(uint8_t wbuf[], uint8_t wlen)
{
    while(_wire->available() > 0)
    {
      _wire->read();
    }
    _wire->beginTransmission(_i2caddr); //IIC start with 7bit addr
    _wire->write(wbuf, wlen);
    _wire->endTransmission();
}
/**********************************************************
Description: write a bit data
Parameters:  bitNum :Number of bits(bit7-bit0)
             bitValue   :Value written        
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::writeRegBit(uint8_t addr,uint8_t bitNum, uint8_t bitValue)
{
      uint8_t data;
      readReg(addr,dataBuff,1);
      data = dataBuff[0];
      data = (bitValue != 0)? (data|(1<<bitNum)) : (data & ~(1 << bitNum));
      writeReg(addr, data);
}
/**********************************************************
Description: readBytes
Parameters:  rbuf[]:Variables for storing Data to be obtained
             rlen:Length of data to be obtained
Return:   
Others:
**********************************************************/
void BMS33M332::readBytes(uint8_t rbuf[], uint8_t rlen)
{
    _wire->requestFrom(_i2caddr, rlen);
    if(_wire->available()==rlen)
    {
      for(uint8_t i = 0; i < rlen; i++)
      {
        rbuf[i] = _wire->read();
      }
    }
}
/**********************************************************
Description: clear Buff
Parameters:  none            
Return:      none    
Others:      none
**********************************************************/
void BMS33M332::clearBuf()
{
      for(int a = 0; a < 4; a++)
      {
        dataBuff[a] = 0;
      } 
}
