#include <JuceHeader.h>
#include "PlugIn.h"

PlugIn::PlugIn()
{
	setSize(300, 200);
}

PlugIn::~PlugIn()
{
}

void PlugIn::processNextBlock(const AudioSourceChannelInfo& bufferToFill)
{
}
