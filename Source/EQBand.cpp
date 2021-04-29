#include <JuceHeader.h>
#include "EQBand.h"

EQBand::EQBand(int pBandType, double pSampleRate) : sampleRate(pSampleRate), frequency(1000), qFactor(1.0), gain(1.0f)
{	
	setBandType(pBandType);
	setCoefficients();
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
	
	return true;
}

void EQBand::setFrequency(double pFrequency)
{
	frequency = pFrequency;
}

void EQBand::setQFactor(double pQFactor)
{
	qFactor = pQFactor;
}

void EQBand::setGain(float pGain)
{
	gain = pGain;
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

void EQBand::process(const AudioSourceChannelInfo& bufferToFill)
{
	filter.processSamples(bufferToFill.buffer->getWritePointer(0), bufferToFill.numSamples);
	filter.processSamples(bufferToFill.buffer->getWritePointer(1), bufferToFill.numSamples);
}

void EQBand::setCoefficients()
{
	switch (bandType) {
	case BandType::Band:
		filter.setCoefficients(IIRCoefficients::makePeakFilter(sampleRate, frequency, qFactor, gain));
	case BandType::HighPass:
		filter.setCoefficients(IIRCoefficients::makeHighPass(sampleRate, frequency));
	case BandType::LowPass:
		filter.setCoefficients(IIRCoefficients::makeLowPass(sampleRate, frequency));
	case BandType::HighShelf:
		filter.setCoefficients(IIRCoefficients::makeHighShelf(sampleRate, frequency, qFactor, gain));
	case BandType::LowShelf:
		filter.setCoefficients(IIRCoefficients::makeLowShelf(sampleRate, frequency, qFactor, gain));
	}
}
