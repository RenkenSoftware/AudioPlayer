#include <JuceHeader.h>
#include "PlugInWindow.h"

PlugInWindow::PlugInWindow(String name) : DocumentWindow(name, Colours::lightgrey, DocumentWindow::closeButton), deletionFlag(false)
{
    setVisible(true);
}

PlugInWindow::~PlugInWindow()
{
}

void PlugInWindow::closeButtonPressed()
{
    setVisible(false);
}

bool PlugInWindow::shouldBeDeleted()
{
    return deletionFlag;
}

void PlugInWindow::process(const AudioSourceChannelInfo& bufferToFill)
{
}
