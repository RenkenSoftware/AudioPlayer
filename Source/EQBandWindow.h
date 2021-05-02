#pragma once

using namespace juce;

class EQBandWindow : public PlugInWindow {

public:

	EQBandWindow(String name, double pSampleRate);

	~EQBandWindow();

	void setFrequency(double pFrequency);
	void setQFactor(double pQFactor);
	void setGain(float pGain);
	void setSampleRate(double pSampleRate);

	double getFrequency();
	double getQFactor();
	float getGain();

	void process(const AudioSourceChannelInfo& bufferToFill) override;

private:

	EQBand band;
};