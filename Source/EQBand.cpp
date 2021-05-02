#include <JuceHeader.h>
#include "PlugIn.h"
#include "EQBand.h"

EQBand::EQBand(double pSampleRate) : frequency(1000), qFactor(1.0), gain(1.0f), sampleRate(pSampleRate)
{
	setCoefficients();
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

void EQBand::process(const AudioSourceChannelInfo& bufferToFill)
{
	filterL.processSamples(bufferToFill.buffer->getWritePointer(0), bufferToFill.numSamples);
	filterR.processSamples(bufferToFill.buffer->getWritePointer(1), bufferToFill.numSamples);

}

void EQBand::setCoefficients()
{
	filterL.setCoefficients(IIRCoefficients::makePeakFilter(sampleRate, frequency, qFactor, gain));
	filterR.setCoefficients(IIRCoefficients::makePeakFilter(sampleRate, frequency, qFactor, gain));
}
