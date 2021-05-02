#pragma once

using namespace juce;

class EQBand : public PlugIn {

public:

	EQBand(double pSampleRate);

	~EQBand();

	void setFrequency(double pFrequency);
	void setQFactor(double pQFactor);
	void setGain(float pGain);
	void setSampleRate(double pSampleRate);

	double getFrequency();
	double getQFactor();
	float getGain();

	void process(const AudioSourceChannelInfo& bufferToFill);

private:
	void setCoefficients();

	IIRFilter filterL;
	IIRFilter filterR;

	float gain;
	double frequency;
	double qFactor;
	double sampleRate;
};