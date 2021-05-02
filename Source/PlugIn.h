#pragma once

using namespace juce;

class PlugIn : public Component
{
public:
	PlugIn();

	~PlugIn();

	void processNextBlock(const AudioSourceChannelInfo& bufferToFill);

private:

};