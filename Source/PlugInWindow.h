#pragma once

using namespace juce;

class PlugInWindow : public DocumentWindow
{
public:

	PlugInWindow(String name);

	void closeButtonPressed() override;
private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlugInWindow)
};