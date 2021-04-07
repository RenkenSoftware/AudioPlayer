#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()

{
    // Make sure you set the size of the component after
    // you add any child components.

    setSize (800, 600);

    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0.0f, 5.0f);
    volumeSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    volumeSlider.addListener(this);

    addAndMakeVisible(transportSlider);
    transportSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    transportSlider.addListener(this);

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

    addAndMakeVisible(message);

    addAndMakeVisible(volumeLabel);
    volumeLabel.setText("Volume", dontSendNotification);
    volumeLabel.attachToComponent(&volumeSlider, true);

    transportSource.addChangeListener(this);

    changeTransportState(TransportState::NoFileLoaded);

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

    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.

    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr || state != TransportState::Playing)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    transportSource.releaseResources();
}

void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    loadButton.setBounds(10, 10, 100, 30);
    playButton.setBounds(120, 560, 70, 30);
    stopButton.setBounds(280, 560, 70, 30);
    pauseButton.setBounds(200, 560, 70, 30);
    message.setBounds(10, 560, 100, 30);
    volumeSlider.setBounds(420, 560, 370, 30);
    transportSlider.setBounds(40, 520, 750, 30);
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

void MainComponent::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (transportSource.hasStreamFinished())
        {
            stopButtonClicked();
        }
    }
}

void MainComponent::timerCallback()
{
    if (!transportSlider.isMouseButtonDown())
    {
        transportSlider.setValue(transportSource.getCurrentPosition());
    }
}

void MainComponent::loadButtonClicked()
{
    FileChooser chooser("Select an audio file.", {}, "*.wav;*.mp3");
    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        reader = formatManager.createReaderFor(file);
        if (reader != nullptr)
        {
            std::unique_ptr<juce::AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
            transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
            readerSource.reset(newSource.release());
            changeTransportState(TransportState::FileLoaded);
        }
        else {
            message.setText("Invalid file...", dontSendNotification);
            delete reader;
        }
    }
}

void MainComponent::playButtonClicked()
{
    changeTransportState(TransportState::Playing);
}

void MainComponent::stopButtonClicked()
{
    changeTransportState(TransportState::Stopped);
}

void MainComponent::pauseButtonClicked()
{
    changeTransportState(TransportState::Paused);
}

void MainComponent::volumeSliderValueChanged()
{
    transportSource.setGain((float)volumeSlider.getValue());
}

void MainComponent::transportSliderDragEnded()
{
    transportSource.setPosition(transportSlider.getValue());
}

void MainComponent::changeTransportState(TransportState newState)
{
    if (state == newState)
    {
        return;
    }
    state = newState;

    switch (state)
    {
    case TransportState::Stopped:
        message.setText("Stopped", dontSendNotification);
        stopButton.setEnabled(false);
        playButton.setEnabled(true);
        pauseButton.setEnabled(false);
        transportSource.setPosition(0.0);
        transportSlider.setValue(0.0);
        stopTimer();
        transportSource.stop();
        break;

    case TransportState::Playing:
        message.setText("Playing", dontSendNotification);
        stopButton.setEnabled(true);
        pauseButton.setEnabled(true);
        playButton.setEnabled(false);
        startTimer(100);
        transportSource.start();
        break;

    case TransportState::Paused:
        message.setText("Paused", dontSendNotification);
        playButton.setEnabled(true);
        pauseButton.setEnabled(false);
        stopButton.setEnabled(false);
        stopTimer();
        break;

    case TransportState::FileLoaded:
        message.setText("File loaded!", dontSendNotification);
        playButton.setEnabled(true);
        stopButton.setEnabled(false);
        pauseButton.setEnabled(false);
        transportSlider.setRange(0.0, transportSource.getLengthInSeconds());
        transportSlider.setEnabled(true);
        volumeSlider.setEnabled(true);
        break;

    case TransportState::NoFileLoaded:
        message.setText("No file loaded...", dontSendNotification);
        playButton.setEnabled(false);
        stopButton.setEnabled(false);
        pauseButton.setEnabled(false);
        transportSlider.setEnabled(false);
        transportSlider.setRange(0.0, 1.0);
        transportSlider.setValue(0.5);
        volumeSlider.setEnabled(false);
        volumeSlider.setValue(1.0f);
        break;
    }
}

void MainComponent::doDSP(AudioSourceChannelInfo& bufferToFill)
{
    for (int i = 0; i < bufferToFill.buffer->getNumChannels(); i++)
    {
        for (int j = 0; j < bufferToFill.numSamples; j++)
        {
            bufferToFill.buffer->setSample(i, j, bufferToFill.buffer->getSample(i, j));
        }
    }
}