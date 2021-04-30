#pragma once

using namespace juce;

class EQBand final {

public:

	EQBand(int pBandType, double pFrequency, double pQFactor, float pGain, double pSampleRate = 44100);

	~EQBand();

	bool setBandType(int pBandType);
	void setFrequency(double pFrequency);
	void setQFactor(double pQFactor);
	void setGain(float pGain);
	void setSampleRate(double pSampleRate);
	void setEnabled(bool pIsActive);

	int getBandType();
	double getFrequency();
	double getQFactor();
	float getGain();
	bool isEnabled();

	void process(const AudioSourceChannelInfo& bufferToFill);

private:
	IIRFilter filterL;
	IIRFilter filterR;

	enum class BandType {
		LowPass,
		HighPass,
		LowShelf,
		HighShelf,
		Band
	};

	BandType bandType;
	float gain;
	double frequency;
	double qFactor;
	double sampleRate;
	
	bool isActive;

	void setCoefficients();
};