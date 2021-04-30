#include <JuceHeader.h>
#include "EQBand.h"

EQBand::EQBand(int pBandType, double pFrequency, double pQFactor, float pGain, double pSampleRate) : frequency(pFrequency), qFactor(pQFactor), gain(pGain), sampleRate(pSampleRate), isActive(false)
{	
	setBandType(pBandType);
}

EQBand::~EQBand()
{
	
}

bool EQBand::setBandType(int pBandType)
{

	if (pBandType < 0 || pBandType > 4)
	{
		return false;
	}

	switch (pBandType) {
	case 0:
		bandType = BandType::Band;
	case 1:
		bandType = BandType::HighPass;
	case 2:
		bandType = BandType::LowPass;
	case 3:
		bandType = BandType::HighShelf;
	case 4:
		bandType = BandType::LowShelf;
	}

	setCoefficients();
	
	return true;
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

void EQBand::setEnabled(bool pIsActive)
{
	isActive = pIsActive;
}

int EQBand::getBandType()
{
	switch (bandType) {
	case BandType::Band:
		return 0;
	case BandType::HighPass:
		return 1;
	case BandType::LowPass:
		return 2;
	case BandType::HighShelf:
		return 3;
	case BandType::LowShelf:
		return 4;
	}
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

bool EQBand::isEnabled()
{
	return isActive;
}

void EQBand::process(const AudioSourceChannelInfo& bufferToFill)
{
	if (isActive)
	{
		filterL.processSamples(bufferToFill.buffer->getWritePointer(0), bufferToFill.numSamples);
		filterR.processSamples(bufferToFill.buffer->getWritePointer(1), bufferToFill.numSamples);
	}
}

void EQBand::setCoefficients()
{
	switch (bandType) {
	case BandType::Band:
		filterL.setCoefficients(IIRCoefficients::makePeakFilter(sampleRate, frequency, qFactor, gain));
		filterR.setCoefficients(IIRCoefficients::makePeakFilter(sampleRate, frequency, qFactor, gain));
	case BandType::HighPass:
		filterL.setCoefficients(IIRCoefficients::makeHighPass(sampleRate, frequency));
		filterR.setCoefficients(IIRCoefficients::makeHighPass(sampleRate, frequency));
	case BandType::LowPass:
		filterL.setCoefficients(IIRCoefficients::makeLowPass(sampleRate, frequency));
		filterR.setCoefficients(IIRCoefficients::makeLowPass(sampleRate, frequency));
	case BandType::HighShelf:
		filterL.setCoefficients(IIRCoefficients::makeHighShelf(sampleRate, frequency, qFactor, gain));
		filterR.setCoefficients(IIRCoefficients::makeHighShelf(sampleRate, frequency, qFactor, gain));
	case BandType::LowShelf:
		filterL.setCoefficients(IIRCoefficients::makeLowShelf(sampleRate, frequency, qFactor, gain));
		filterR.setCoefficients(IIRCoefficients::makeLowShelf(sampleRate, frequency, qFactor, gain));
	}
}
