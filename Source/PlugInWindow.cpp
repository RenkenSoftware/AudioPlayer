#include <JuceHeader.h>
#include "PlugInWindow.h"

PlugInWindow::PlugInWindow(String name) : DocumentWindow(name, Colours::lightgrey, DocumentWindow::allButtons)
{
    centreWithSize(300, 200);
    setVisible(true);
}

void PlugInWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
