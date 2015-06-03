// signalhoundiq.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "signalhoundiq.h"
#include "sa_api.h"

int main(int argc, char **argv)
{
	
	if (argc >= 3) {

		float freq = strtof(argv[1], NULL);
		float bandwidth = strtof(argv[2], NULL);
		int decimation = atoi(argv[3]);

		std::cout << "Frequency: " << freq << "\n";
		std::cout << "Bandwidth " << bandwidth << "\n";
		std::cout << "Decimation " << decimation << "\n";

		std::cout << "Opening Signalhound SA device\n";
		int id = openDevice();

		startIQ(id, freq, bandwidth, decimation);

//		startIQ(id, 915.0e6, 50.0e3, 8);

		closeDevice(id);
	}
	else {
		std::cout << "Stream IQ pairs at a particualar frequency\n";
		std::cout << "Usage:\n";

		std::cout << "signalhoundiq frequency bandwidth [decimation]\n";
		std::cout << "Frequency in Hz\n";
		std::cout << "Bandwidth in Hz\n";
		std::cout << "Decimation 1-16 (default 8)\n";
	}
	return 0;
}

int openDevice(void) {
	// Open the first device found 
	int handle = -1; 
	saStatus openStatus = saOpenDevice(&handle);
	if (openStatus == saNoError) { 
		std::cout << "Device opened, handle is: " << handle <<"\n";
	}
	else {
		std::cout << "Device open failed\n";
	}
	return handle;
}

int closeDevice(int handle) {
	// Finished with device 
	saCloseDevice(handle);
	std::cout << "Device closed\n";
	return handle;
}

int startIQ(int id, double freq, double bandpass, int decimation) {
	// Configure a center frequency of freq MHz 
	// Span is ignored for I/Q data streaming 
	saConfigCenterSpan(id, freq, 200.0e3);
	// Specify that we expect our input level to be 
	// at or below -20dBm. If our input signal 
	// is higher than our specified reference level, 
	// we risk overloading the device, and if we specify 
	// a reference level much higher than our input, 
	// we risk reducing our overall dynamic range. 
	saConfigLevel(id, 0.0);
	// Specify a sample rate of 486111.111 / 8 = 60763 
	// and a bandpass filter of 50kHz 
	saConfigIQ(id, decimation, bandpass);
	// Configure the device to start streaming I/Q data 
	saInitiate(id, SA_IQ, 0);
	int len; 
	double bandwidth, sampleRate;
	// Verify bandwidth and samplerate. Also determine 
	// the number if I/Q data pairs the API returns 
	// for each I/Q data query. 
	saQueryStreamInfo(id, &len, &bandwidth, &sampleRate);
	// Allocate enough memory for len I/Q data pairs 
	float *iq_array = new float[len * 2];
	// The API returns interleaved I/Q data pairs 
	// I and Q are alternating in the return array 
	// Call this function continuously for the full 
	// continuous data stream 
	saGetIQ_32f(id, iq_array);

	for (int i = 0; i < len; i+=2) {
		std::cout << "i: " << iq_array[i] << ", q: " << iq_array[i + 1] << "\n";
	}

	// free the allocated heap
	delete iq_array;

	return 0;
}