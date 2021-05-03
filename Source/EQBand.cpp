#include <JuceHeader.h>
#include "PlugIn.h"
#include "EQBand.h"

EQBand::EQBand(double pSampleRate) : frequency(1000), qFactor(1.0), gain(1.0f), sampleRate(pSampleRate)
{
	setSize(560, 200);

	setCoefficients();

	addAndMakeVisible(frequencySlider);
	frequencySlider.setRange(1, 20000);
	frequencySlider.setSliderStyle(Slider::SliderStyle::Rotary);
	frequencySlider.setValue(frequency);
	frequencySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 30);
	frequencySlider.addListener(this);

	addAndMakeVisible(qFactorSlider);
	qFactorSlider.setRange(1, 10);
	qFactorSlider.setSliderStyle(Slider::SliderStyle::Rotary);
	qFactorSlider.setValue(qFactor);
	qFactorSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 30);
	qFactorSlider.addListener(this);

	addAndMakeVisible(gainSlider);
	gainSlider.setRange(1, 10);
	gainSlider.setSliderStyle(Slider::SliderStyle::Rotary);
	gainSlider.setValue(gain);
	gainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 30);
	gainSlider.addListener(this);

	addAndMakeVisible(frequencyLabel);
	frequencyLabel.setText("Hz", dontSendNotification);
	frequencyLabel.attachToComponent(&frequencySlider, true);

	addAndMakeVisible(qFactorLabel);
	qFactorLabel.setText("Q", dontSendNotification);
	qFactorLabel.attachToComponent(&qFactorSlider, true);

	addAndMakeVisible(gainLabel);
	gainLabel.setText("dB", dontSendNotification);
	gainLabel.attachToComponent(&gainSlider, true);
}

EQBand::~EQBand()
{
}

void EQBand::setFrequency(double pFrequency)
{
	frequency = pFrequency;
	setCoefficients();
}

void EQBand::setQFactor(double pQFactor)
{
	qFactor = pQFactor;
	setCoefficients();
}

void EQBand::setGain(float pGain)
{
	gain = pGain;
	setCoefficients();
}

void EQBand::setSampleRate(double pSampleRate)
{
	sampleRate = pSampleRate;
	setCoefficients();
}

double EQBand::getFrequency()
{
	return frequency;
}

double EQBand::getQFactor()
{
	return qFactor;
}

float EQBand::getGain()
{
	return gain;
}

void EQBand::processNextBlock(const AudioSourceChannelInfo& bufferToFill)
{
	filterL.processSamples(bufferToFill.buffer->getWritePointer(0), bufferToFill.numSamples);
	filterR.processSamples(bufferToFill.buffer->getWritePointer(1), bufferToFill.numSamples);

}

void EQBand::resized()
{
	frequencySlider.setBounds(10, 10, 150, 150);
	qFactorSlider.setBounds(200, 10, 150, 150);
	gainSlider.setBounds(390, 10, 150, 150);
}

void EQBand::sliderValueChanged(Slider* slider)
{
	if (slider == &frequencySlider)
	{
		frequency = slider->getValue();
	} else if (slider == &qFactorSlider)
	{
		qFactor = slider->getValue();
	} else if (slider == &gainSlider)
	{
		gain = slider->getValue();
	}

	setCoefficients();
}

void EQBand::setCoefficients()
{
	filterL.setCoefficients(IIRCoefficients::makePeakFilter(sampleRate, frequency, qFactor, gain));
	filterR.setCoefficients(IIRCoefficients::makePeakFilter(sampleRate, frequency, qFactor, gain));
}
