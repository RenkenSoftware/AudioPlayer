#pragma once

using namespace juce;

class EQBand : public PlugIn, public Slider::Listener {

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

	void processNextBlock(const AudioSourceChannelInfo& bufferToFill) override;

	void resized() override;

	void sliderValueChanged(Slider* slider) override;

private:
	void setCoefficients();

	Slider frequencySlider;
	Slider qFactorSlider;
	Slider gainSlider;

	Label frequencyLabel;
	Label qFactorLabel;
	Label gainLabel;

	IIRFilter filterL;
	IIRFilter filterR;

	float gain;
	double frequency;
	double qFactor;
	double sampleRate;
};