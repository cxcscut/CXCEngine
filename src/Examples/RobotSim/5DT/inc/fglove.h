/*--------------------------------------------------------------------------*/
// fglove.h
//
// 5DT Data Glove driver SDK
// Version 2.00
//
// Copyright (C) 2000-2004, 5DT <Fifth Dimension Technologies>
// http://www.5dt.com/
/*--------------------------------------------------------------------------*/
#ifndef _FGLOVE_H_
#define _FGLOVE_H_
/*--------------------------------------------------------------------------*/

// Product ID Defines for USB Gloves
#define DG14U_R			0x00
#define DG14U_L			0x01
#define DG5U_R			0x10
#define DG5U_L			0x11

enum EfdGloveHand
{
	FD_HAND_LEFT,   // left-handed glove
	FD_HAND_RIGHT   // right-handed glove
};

enum EfdGloveTypes
{
	FD_GLOVENONE,   // no glove
	FD_GLOVE5U,		// DG5 Ultra serial
	FD_GLOVE5UW,	// DG5 Ultra serial, wireless
	FD_GLOVE5U_USB,	// DG5 Ultra USB
	FD_GLOVE7,      // 7-sensor
	FD_GLOVE7W,     // 7-sensor, wireless
	FD_GLOVE16,     // 16-sensor
	FD_GLOVE16W,    // 16-sensor, wireless
	FD_GLOVE14U,	// DG14 Ultra serial
	FD_GLOVE14UW,	// DG14 Ultra serial, wireless
	FD_GLOVE14U_USB	// DG14 Ultra USB
};

enum EfdSensors
{
	FD_THUMBNEAR=0,
	FD_THUMBFAR,
	FD_THUMBINDEX,
	FD_INDEXNEAR,
	FD_INDEXFAR,
	FD_INDEXMIDDLE,
	FD_MIDDLENEAR,
	FD_MIDDLEFAR,
	FD_MIDDLERING,
	FD_RINGNEAR,
	FD_RINGFAR,
	FD_RINGLITTLE,
	FD_LITTLENEAR,
	FD_LITTLEFAR,
	FD_THUMBPALM,
	FD_WRISTBEND,
	FD_PITCH,
	FD_ROLL
};
/*--------------------------------------------------------------------------*/
typedef struct
{
	// The contents of this struct are platform-dependent and subject to
	// change. You should not manipulate the contents of this struct directly.
	void           *m_pStuff;
} fdGlove;

/*--------------------------------------------------------------------------*/
fdGlove *fdOpen(char *pPort);
int   fdClose(fdGlove *pFG);
int   fdGetGloveHand(fdGlove *pFG);
int   fdGetGloveType(fdGlove *pFG);
int   fdGetNumSensors(fdGlove *pFG);
void  fdGetSensorRawAll(fdGlove *pFG, unsigned short *pData);
unsigned short fdGetSensorRaw(fdGlove *pFG, int nSensor);
void  fdSetSensorRawAll(fdGlove *pFG, unsigned short *pData);
void  fdSetSensorRaw(fdGlove *pFG, int nSensor, unsigned short nRaw);
void  fdGetSensorScaledAll(fdGlove *pFG, float *pData);
float fdGetSensorScaled(fdGlove *pFG, int nSensor);
int   fdGetNumGestures(fdGlove *pFG);
int   fdGetGesture(fdGlove *pFG);
void  fdGetCalibrationAll(fdGlove *pFG, unsigned short *pUpper, unsigned short *pLower);
void  fdGetCalibration(fdGlove *pFG, int nSensor, unsigned short *pUpper, unsigned short *pLower);
void  fdSetCalibrationAll(fdGlove *pFG, unsigned short *pUpper, unsigned short *pLower);
void  fdSetCalibration(fdGlove *pFG, int nSensor, unsigned short nUpper, unsigned short nLower);
void  fdResetCalibration(fdGlove *pFG, int nSensor);
void  fdResetCalibration(fdGlove *pFG);
void  fdGetSensorMaxAll(fdGlove *pFG, float *pMax);
float fdGetSensorMax(fdGlove *pFG, int nSensor);
void  fdSetSensorMaxAll(fdGlove *pFG, float *pMax);
void  fdSetSensorMax(fdGlove *pFG, int nSensor, float fMax);
void  fdGetThresholdAll(fdGlove *pFG, float *pUpper, float *pLower);
void  fdGetThreshold(fdGlove *pFG, int nSensor, float *pUpper, float *pLower);
void  fdSetThresholdAll(fdGlove *pFG, float *pUpper, float *pLower);
void  fdSetThreshold(fdGlove *pFG, int nSensor, float fUpper, float fLower);
void  fdGetGloveInfo(fdGlove *pFG, unsigned char *pData);
void  fdGetDriverInfo(fdGlove *pFG, unsigned char *pData);
void  fdSetCallback(fdGlove *pFG,void *pFunc,LPVOID param);
int	  fdGetPacketRate(fdGlove *pFG);
bool  fdNewData(fdGlove *pFG);
int	  fdGetFWVersionMajor(fdGlove *pFG);
int	  fdGetFWVersionMinor(fdGlove *pFG);
bool  fdGetAutoCalibrate(fdGlove *pFG);
bool  fdSetAutoCalibrate(fdGlove *pFG, bool bAutoCalibrate);
bool  fdSaveCalibration(fdGlove *pFG,const char *pFileName);
bool  fdLoadCalibration(fdGlove *pFG,const char *pFileName);
int	  fdScanUSB(unsigned short *aPID, int &nNumMax);

/*--------------------------------------------------------------------------*/
#endif // #ifndef _FGLOVE_H_
/*--------------------------------------------------------------------------*/
