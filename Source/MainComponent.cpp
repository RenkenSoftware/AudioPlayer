#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0.0f, 5.0f, 0.1f);
    volumeSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    volumeSlider.setValue(1.0f);
    volumeSlider.addListener(this);

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
    playing = false;
    fileLoaded = false;
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
    volume = 1.0f;
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
   
    if (readerSource.get() == nullptr || playing == false)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock(bufferToFill);

    for (int i = 0; i < bufferToFill.buffer->getNumChannels(); i++)
    {
        for (int j = 0; j < bufferToFill.numSamples; j++)
        {
            bufferToFill.buffer->setSample(i, j, bufferToFill.buffer->getSample(i, j) * volume);
        }
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    transportSource.releaseResources();
}

//==============================================================================
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
    
    loadButton.setBounds(50, 50, 200, 50);
    playButton.setBounds(50, 150, 200, 50);
    stopButton.setBounds(50, 250, 200, 50);
    pauseButton.setBounds(50, 350, 200, 50);
    message.setBounds(300, 50, 200, 50);
    volumeSlider.setBounds(50, 450, 200, 50);
}

void MainComponent::buttonClicked(Button* pButton)
{
    if (pButton == &loadButton) {
        FileChooser chooser("Select a .wav file to play.", {}, "*.wav;*.mp3");
        if (chooser.browseForFileToOpen())
        {
            auto file = chooser.getResult();
            reader = formatManager.createReaderFor(file);
            if (reader != nullptr)
            {
                std::unique_ptr<juce::AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
                transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                playButton.setEnabled(true);
                readerSource.reset(newSource.release());
                fileLoaded = true;
                message.setText("File loaded!", dontSendNotification);
            }
            else {
                message.setText("Invalid file...", dontSendNotification);
                delete reader;
            }
        }
    }
    else if (pButton == &playButton) {
        if (fileLoaded == true && playing == false)
        {
            message.setText("Playing", dontSendNotification);
            playing = true;
            transportSource.start();
        }
        
    }
    else if (pButton == &stopButton) {
        if (playing == true)
        {
            message.setText("Stopped", dontSendNotification);
            playing = false;
            transportSource.setPosition(0.0);
        }
    }
    else if (pButton == &pauseButton) {
        if (playing == true)
        {
            message.setText("Paused", dontSendNotification);
            playing = false;
            transportSource.stop();
        }
    }
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    if (slider == &volumeSlider)
    {
        volume = slider->getValue();
    }
}
