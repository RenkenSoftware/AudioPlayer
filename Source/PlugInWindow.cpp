#include <JuceHeader.h>
#include "PlugInWindow.h"

PlugInWindow::PlugInWindow(String name) : DocumentWindow(name, Colours::lightgrey, DocumentWindow::allButtons), deletionFlag(false)
{
    centreWithSize(560, 200);
    setVisible(true);
}

PlugInWindow::~PlugInWindow()
{
}

void PlugInWindow::closeButtonPressed()
{
    deletionFlag = true;
}

bool PlugInWindow::shouldBeDeleted()
{
    return deletionFlag;
}

void PlugInWindow::process(const AudioSourceChannelInfo& bufferToFill)
{
}
