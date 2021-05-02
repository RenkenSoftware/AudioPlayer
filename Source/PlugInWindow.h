#pragma once

using namespace juce;

class PlugInWindow : public DocumentWindow
{
public:

	PlugInWindow(String name);

	void closeButtonPressed() override;

	virtual void process(const AudioSourceChannelInfo& bufferToFill);

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlugInWindow)
};