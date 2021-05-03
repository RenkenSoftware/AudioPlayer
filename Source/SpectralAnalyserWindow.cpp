#include <JuceHeader.h>
#include "PlugInWindow.h"
#include "PlugIn.h"
#include "SpectralAnalyser.h"
#include "SpectralAnalyserWindow.h"

SpectralAnalyserWindow::SpectralAnalyserWindow(String name) : PlugInWindow(name)
{
	analyser = new SpectralAnalyser();
	setContentOwned(analyser, true);
	centreWithSize(880, 350);
	setResizable(false, true);
}

SpectralAnalyserWindow::~SpectralAnalyserWindow()
{
	delete analyser;
}

void SpectralAnalyserWindow::process(const AudioSourceChannelInfo& bufferToFill)
{
	analyser->processNextBlock(bufferToFill);
}
