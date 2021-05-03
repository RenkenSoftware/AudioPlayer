#pragma once

using namespace juce;

class PlugIn : public Component
{
public:
	PlugIn();

	~PlugIn();

	virtual void processNextBlock(const AudioSourceChannelInfo& bufferToFill);

private:

};