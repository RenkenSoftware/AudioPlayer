#pragma once

using namespace juce;

class PlugInWindow : public DocumentWindow
{
public:

	PlugInWindow(String name);

	~PlugInWindow();

	void closeButtonPressed() override;

	bool shouldBeDeleted();

	virtual void process(const AudioSourceChannelInfo& bufferToFill);

private:

	bool deletionFlag;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlugInWindow)
};