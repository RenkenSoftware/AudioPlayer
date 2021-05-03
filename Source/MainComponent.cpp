#include <JuceHeader.h>
#include "TrackPlayer.h"
#include "PlugIn.h"
#include "EQBand.h"
#include "SpectralAnalyser.h"
#include "PlugInWindow.h"
#include "EQBandWindow.h"
#include "SpectralAnalyserWindow.h"
#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent() : mainPlayer()
{
    for (int i = 0; i < 10; i++)
    {
        plugInsActive[i] = false;
    }

    addMouseListener(this, true);

    setSize (800, 600);

    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0.0, 5.0);
    volumeSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    volumeSlider.setSkewFactor(0.5, false);
    volumeSlider.setValue(1.0);
    volumeSlider.addListener(this);

    addAndMakeVisible(transportSlider);
    transportSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    transportSlider.addListener(this);

    addAndMakeVisible(addEQBandButton);
    addEQBandButton.setButtonText("Add EQ Band");
    addEQBandButton.setEnabled(true);
    addEQBandButton.addListener(this);

    addAndMakeVisible(addAnalyserButton);
    addAnalyserButton.setButtonText("Add Analyser");
    addAnalyserButton.setEnabled(true);
    addAnalyserButton.addListener(this);

    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Load Audio File");
    loadButton.addListener(this);

    addAndMakeVisible(playButton);
    playButton.setButtonText("Play");
    playButton.addListener(this);

    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop");
    stopButton.addListener(this);

    addAndMakeVisible(pauseButton);
    pauseButton.setButtonText("Pause");
    pauseButton.addListener(this);

    addAndMakeVisible(volumeLabel);
    volumeLabel.setText("Volume", dontSendNotification);
    volumeLabel.attachToComponent(&volumeSlider, true);

    addAndMakeVisible(plugInList);
    plugInList.setName("PlugIns");
    plugInList.onChange = [this] { plugInSelected(); };

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    Timer::stopTimer();

    shutdownAudio();

    for (int i = 0; i < 10; i++)
    {
        if (plugInsActive[i] == true)
        {
            delete plugIns[i].value();
            plugIns[i].reset();
        }
    }
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    sampleRateValue = sampleRate;

    mainPlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);

    Timer::startTimer(10);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (mainPlayer.isPlaying())
    {
        mainPlayer.getNextAudioBlock(bufferToFill);

        for (int i = 0; i < 10; i++)
        {
            if (plugInsActive[i] == true)
            {
                plugIns[i].value()->process(bufferToFill);
            }
        }
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    mainPlayer.releaseResources();
}

void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setOpacity(1.0f);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    addEQBandButton.setBounds(700, 90, 90, 30);
    addAnalyserButton.setBounds(700, 130, 90, 30);
    loadButton.setBounds(10, 10, 100, 30);
    playButton.setBounds(120, 560, 70, 30);
    stopButton.setBounds(280, 560, 70, 30);
    pauseButton.setBounds(200, 560, 70, 30);
    volumeSlider.setBounds(420, 560, 370, 30);
    transportSlider.setBounds(40, 520, 750, 30);

    plugInList.setBounds(150, 10, 200, 20);
}

void MainComponent::buttonClicked(Button* pButton)
{
    if (pButton == &loadButton) {
        loadButtonClicked();
    }
    else if (pButton == &playButton) {
        playButtonClicked();
    }
    else if (pButton == &stopButton) {
        stopButtonClicked();
    }
    else if (pButton == &pauseButton) {
        pauseButtonClicked();
    }
    else if (pButton == &addEQBandButton)
    {
        addEQBandButtonClicked();
    }
    else if (pButton == &addAnalyserButton)
    {
        addAnalyserButtonClicked();
    }
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    if (slider == &volumeSlider)
    {
        volumeSliderValueChanged();
    }
}

void MainComponent::sliderDragEnded(Slider* slider)
{
    if (slider == &transportSlider)
    {
        transportSliderDragEnded();
    }
}

void MainComponent::timerCallback()
{
    if (mainPlayer.isPlaying())
    {
        if (!transportSlider.isMouseButtonDown())
        {
            transportSlider.setValue(mainPlayer.getTransportPosition());
        }
    }

    for (int i = 0; i < 10; i++)
    {
        if (plugInsActive[i] == true)
        {
            if (plugIns[i].value()->shouldBeDeleted())
            {
                plugInsActive[i] = false;
                delete plugIns[i].value();
                plugIns[i].reset();

                plugInList.clear();

                for (int i = 0; i < 10; i++)
                {
                    if (plugInsActive[i] == true)
                    {
                        plugInList.addItem(plugIns[i].value()->getName(), i + 1);
                    }
                }
            }
        }
    }
}

void MainComponent::mouseDoubleClick(const MouseEvent& event)
{
    if (volumeSlider.isMouseOver())
    {
        volumeSlider.setValue(1.0);
    }
}

void MainComponent::loadButtonClicked()
{
    FileChooser chooser("Select an audio file.", {}, "*.wav;*.mp3");
    if (chooser.browseForFileToOpen())
    {

        if (mainPlayer.loadAudioFile(&chooser.getResult()))
        {
            transportSlider.setRange(0.0, mainPlayer.getLength());
            transportSlider.setValue(0.0);
        }
    }
}

void MainComponent::playButtonClicked()
{
    mainPlayer.play();
}

void MainComponent::stopButtonClicked()
{
    mainPlayer.stop();
    transportSlider.setValue(0.0);
}

void MainComponent::pauseButtonClicked()
{
    mainPlayer.pause();
}

void MainComponent::addEQBandButtonClicked()
{
    for (int i = 0; i < 10; i++)
    {
        if (plugInsActive[i] == false)
        {
            plugIns[i] = new EQBandWindow("EQ Band", sampleRateValue);
            plugInsActive[i] = true;
            plugInList.addItem(plugIns[i].value()->getName(), i + 1);
            break;
        }
    }
}

void MainComponent::addAnalyserButtonClicked()
{
    for (int i = 0; i < 10; i++)
    {
        if (plugInsActive[i] == false)
        {
            plugIns[i] = new SpectralAnalyserWindow("Spectral Analyser");
            plugInsActive[i] = true;
            plugInList.addItem(plugIns[i].value()->getName(), i + 1);
            break;
        }
    }
}

void MainComponent::volumeSliderValueChanged()
{
    mainPlayer.setGain(volumeSlider.getValue());
}

void MainComponent::transportSliderDragEnded()
{
    mainPlayer.setTransportPosition(transportSlider.getValue());
}

void MainComponent::plugInSelected()
{
    plugIns[plugInList.getSelectedId() - 1].value()->setVisible(true);
}
