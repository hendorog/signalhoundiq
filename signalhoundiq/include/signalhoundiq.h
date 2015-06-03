#ifndef __SH_IQ_H__
#define __SH_IQ_H__

int openDevice(void);
int closeDevice(int handle);
int startIQ(int id, double freq, double bandpass, int decimation);

#endif