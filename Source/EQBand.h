#pragma once

using namespace juce;

class EQBand {

public:
	EQBand(int pBandType, double pSampleRate);

	~EQBand();

	bool setBandType(int pBandType);
	void setFrequency(double pFrequency);
	void setQFactor(double pQFactor);
	void setGain(float pGain);

	int getBandType();
	double getFrequency();
	double getQFactor();
	float getGain();

	void process(const AudioSourceChannelInfo& bufferToFill);

private:
	IIRFilter filter;

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

	void setCoefficients();
};