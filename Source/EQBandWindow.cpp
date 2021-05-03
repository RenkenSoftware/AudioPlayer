#include <JuceHeader.h>
#include "PlugInWindow.h"
#include "PlugIn.h"
#include "EQBand.h"
#include "EQBandWindow.h"

EQBandWindow::EQBandWindow(String name, double pSampleRate) : PlugInWindow(name)
{
	band = new EQBand(pSampleRate);
	setContentOwned(band, true);
}

EQBandWindow::~EQBandWindow()
{
	delete band;
}

void EQBandWindow::setFrequency(double pFrequency)
{
	band->setFrequency(pFrequency);
}

void EQBandWindow::setQFactor(double pQFactor)
{
	band->setQFactor(pQFactor);
}

void EQBandWindow::setGain(float pGain)
{
	band->setGain(pGain);
}

void EQBandWindow::setSampleRate(double pSampleRate)
{
	band->setSampleRate(pSampleRate);
}

double EQBandWindow::getFrequency()
{
	return band->getFrequency();
}

double EQBandWindow::getQFactor()
{
	return band->getQFactor();
}

float EQBandWindow::getGain()
{
	return band->getGain();
}

void EQBandWindow::process(const AudioSourceChannelInfo& bufferToFill)
{
	band->process(bufferToFill);
}
