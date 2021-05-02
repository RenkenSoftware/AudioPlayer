#include <JuceHeader.h>
#include "PlugInWindow.h"

PlugInWindow::PlugInWindow(String name) : DocumentWindow(name, Colours::lightgrey, DocumentWindow::allButtons)
{
    centreWithSize(560, 200);
    setVisible(true);
}

void PlugInWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

void PlugInWindow::process(const AudioSourceChannelInfo& bufferToFill)
{
}
