
#include <JuceHeader.h>
#include "TrackPlayer.h"
#include "PlugInWindow.h"
#include "MainComponent.h"
#include "../UnitTests/TrackPlayerTest.h"

/** Uncomment this to run all unit tests. The test results will be written into UnitTestLog.txt in the UnitTests folder.
*/
//#define TEST_MODE

class AudioPlayerApplication : public juce::JUCEApplication
{
public:
    AudioPlayerApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    void initialise (const juce::String& commandLine) override
    {
#ifdef TEST_MODE
        String cwd = File::getCurrentWorkingDirectory().getFullPathName();
        cwd = cwd.replace("\\", "/");
        cwd = cwd += "/UnitTests/UnitTestLog.txt";

        File logFile(cwd);
        logFile.deleteFile();
        logFile.create();
        FileLogger logger(logFile, "Log file for Unit tests");

        Logger::setCurrentLogger(&logger);
        UnitTestRunner testRunner;
        testRunner.runAllTests();
        Logger::setCurrentLogger(nullptr);
        quit();
#else
        mainWindow.reset (new MainWindow (getApplicationName()));
#endif
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (false, true);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.

START_JUCE_APPLICATION (AudioPlayerApplication)