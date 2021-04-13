#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : specFFT (fftOrder),
                                 specImage (Image::RGB, 760, 300, true)

{
    // Make sure you set the size of the component after
    // you add any child components.

    addKeyListener(this);

    setSize (800, 600);

    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0.0f, 5.0f);
    volumeSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    volumeSlider.setSkewFactor(0.5, false);
    volumeSlider.addListener(this);

    addAndMakeVisible(transportSlider);
    transportSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    transportSlider.addListener(this);

    addAndMakeVisible(bassEqSlider);
    bassEqSlider.setRange(0.01f, 2.0f);
    bassEqSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    bassEqSlider.setValue(1.0f);
    bassEqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 30);
    bassEqSlider.addListener(this);

    addAndMakeVisible(midEqSlider);
    midEqSlider.setRange(0.01f, 2.0f);
    midEqSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    midEqSlider.setValue(1.0f);
    midEqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 30);
    midEqSlider.addListener(this);

    addAndMakeVisible(highEqSlider);
    highEqSlider.setRange(0.01f, 2.0f);
    highEqSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    highEqSlider.setValue(1.0f);
    highEqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 30);
    highEqSlider.addListener(this);

    addAndMakeVisible(specButton);
    specButton.setButtonText("Spectogram mode");
    specButton.setEnabled(true);
    specButton.addListener(this);

    addAndMakeVisible(freqMagButton);
    freqMagButton.setButtonText("Frequency-magnitude mode");
    freqMagButton.setEnabled(false);
    freqMagButton.addListener(this);

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

    addAndMakeVisible(bassLabel);
    bassLabel.setText("Bass", dontSendNotification);
    bassLabel.attachToComponent(&bassEqSlider, true);

    addAndMakeVisible(midLabel);
    midLabel.setText("Mid", dontSendNotification);
    midLabel.attachToComponent(&midEqSlider, true);

    addAndMakeVisible(highLabel);
    highLabel.setText("High", dontSendNotification);
    highLabel.attachToComponent(&highEqSlider, true);

    transportSource.addChangeListener(this);

    changeTransportState(TransportState::NoFileLoaded);

    specState = SpecState::FreqMag;

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
    sampleRateValue = sampleRate;

    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    bassEqL.setCoefficients(IIRCoefficients::makeLowShelf(sampleRate, 300, 1.0, 1.0f));
    bassEqR.setCoefficients(IIRCoefficients::makeLowShelf(sampleRate, 300, 1.0, 1.0f));
    midEqL.setCoefficients(IIRCoefficients::makePeakFilter(sampleRate, 2000, 1.0, 1.0f));
    midEqR.setCoefficients(IIRCoefficients::makePeakFilter(sampleRate, 2000, 1.0, 1.0f));
    highEqL.setCoefficients(IIRCoefficients::makeHighShelf(sampleRate, 5000, 1.0, 1.0f));
    highEqR.setCoefficients(IIRCoefficients::makeHighShelf(sampleRate, 5000, 1.0, 1.0f));
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr || transportState != TransportState::Playing)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    transportSource.getNextAudioBlock(bufferToFill);

    bassEqL.processSamples(bufferToFill.buffer->getWritePointer(0), bufferToFill.numSamples);
    bassEqR.processSamples(bufferToFill.buffer->getWritePointer(1), bufferToFill.numSamples);
    midEqL.processSamples(bufferToFill.buffer->getWritePointer(0), bufferToFill.numSamples);
    midEqR.processSamples(bufferToFill.buffer->getWritePointer(1), bufferToFill.numSamples);
    highEqL.processSamples(bufferToFill.buffer->getWritePointer(0), bufferToFill.numSamples);
    highEqR.processSamples(bufferToFill.buffer->getWritePointer(1), bufferToFill.numSamples);

    const float* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

    for (int i = 0; i < bufferToFill.numSamples; i++)
    {
        pushNextSampleIntoFifo(channelData[i]);
    }
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
    g.setOpacity(1.0f);
    g.drawImage(specImage, 20, 200, 760, 300, 0, 0, 760, 300, false);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    specButton.setBounds(700, 10, 90, 30);
    freqMagButton.setBounds(700, 50, 90, 30);
    loadButton.setBounds(10, 10, 100, 30);
    playButton.setBounds(120, 560, 70, 30);
    stopButton.setBounds(280, 560, 70, 30);
    pauseButton.setBounds(200, 560, 70, 30);
    message.setBounds(10, 560, 100, 30);
    volumeSlider.setBounds(420, 560, 370, 30);
    transportSlider.setBounds(40, 520, 750, 30);
    bassEqSlider.setBounds(150, 10, 150, 150);
    midEqSlider.setBounds(340, 10, 150, 150);
    highEqSlider.setBounds(530, 10, 150, 150);
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
    else if (pButton == &specButton)
    {
        specButtonClicked();
    }
    else if (pButton == &freqMagButton)
    {
        freqMagButtonClicked();
    }
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    if (slider == &volumeSlider)
    {
        volumeSliderValueChanged();
    }

    if (slider == &bassEqSlider)
    {
        bassEqSliderValueChanged();
    }

    if (slider == &midEqSlider)
    {
        midEqSliderValueChanged();
    }

    if (slider == &highEqSlider)
    {
        highEqSliderValueChanged();
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

    if (nextFFTBlockReady)
    {
        drawNextLineOfSpectrogram();
        nextFFTBlockReady = false;
        repaint();
    }
}

bool MainComponent::keyPressed(const KeyPress& key, Component* component)
{
    if (key.isKeyCode(KeyPress::spaceKey))
    {
        switch (transportState)
        {
        case TransportState::Playing:
            pauseButtonClicked();
            break;
        case TransportState::Paused:
            playButtonClicked();
            break;
        case TransportState::Stopped:
            playButtonClicked();
            break;
        case TransportState::FileLoaded:
            playButtonClicked();
            break;
        }
    }
    return true;
}

void MainComponent::loadButtonClicked()
{
    if (transportState == TransportState::Playing || transportState == TransportState::Paused)
    {
        stopButtonClicked();
    }

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

void MainComponent::specButtonClicked()
{
    freqMagButton.setEnabled(true);
    specButton.setEnabled(false);
    specState = SpecState::Spectrogram;
    specImage.clear(specImage.getBounds());
}

void MainComponent::freqMagButtonClicked()
{
    freqMagButton.setEnabled(false);
    specButton.setEnabled(true);
    specState = SpecState::FreqMag;
    specImage.clear(specImage.getBounds());
}

void MainComponent::volumeSliderValueChanged()
{
    transportSource.setGain((float)volumeSlider.getValue());
}

void MainComponent::bassEqSliderValueChanged()
{
    bassEqL.setCoefficients(IIRCoefficients::makeLowShelf(sampleRateValue, 300, 1.0, (float)bassEqSlider.getValue()));
    bassEqR.setCoefficients(IIRCoefficients::makeLowShelf(sampleRateValue, 300, 1.0, (float)bassEqSlider.getValue()));
}

void MainComponent::midEqSliderValueChanged()
{
    midEqL.setCoefficients(IIRCoefficients::makePeakFilter(sampleRateValue, 2000, 1.0, (float)midEqSlider.getValue()));
    midEqR.setCoefficients(IIRCoefficients::makePeakFilter(sampleRateValue, 2000, 1.0, (float)midEqSlider.getValue()));
}

void MainComponent::highEqSliderValueChanged()
{
    highEqL.setCoefficients(IIRCoefficients::makeHighShelf(sampleRateValue, 5000, 1.0, (float)highEqSlider.getValue()));
    highEqR.setCoefficients(IIRCoefficients::makeHighShelf(sampleRateValue, 5000, 1.0, (float)highEqSlider.getValue()));
}

void MainComponent::transportSliderDragEnded()
{
    transportSource.setPosition(transportSlider.getValue());
}

void MainComponent::changeTransportState(TransportState newState)
{
    if (transportState == newState)
    {
        return;
    }
    transportState = newState;

    switch (transportState)
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
        transportSlider.setValue(0.0);
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

void MainComponent::pushNextSampleIntoFifo(float sample) noexcept
{
    if (fifoIndex == fftSize)
    {
        if (!nextFFTBlockReady)
        {
            std::fill(fftData.begin(), fftData.end(), 0.0f);
            std::copy(fifo.begin(), fifo.end(), fftData.begin());
            nextFFTBlockReady = true;
        }

        fifoIndex = 0;
    }

    fifo[(size_t)fifoIndex++] = sample;
}

void MainComponent::drawNextLineOfSpectrogram()
{
    auto rightHandEdge = specImage.getWidth() - 1;
    auto imageHeight = specImage.getHeight();
    auto imageWidth = specImage.getWidth();

    if (specState == SpecState::Spectrogram)
    {
        specImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);
    }

    specFFT.performFrequencyOnlyForwardTransform(fftData.data());

    auto maxLevel = juce::FloatVectorOperations::findMinAndMax(fftData.data(), fftSize / 2);

    switch (specState)
    {
    case SpecState::Spectrogram:

        for (auto y = 1; y < imageHeight; ++y)
        {
            auto skewedProportionY = 1.0f - std::exp(std::log((float)y / (float)imageHeight) * 0.2f);
            auto fftDataIndex = (size_t)jlimit(0, fftSize / 2, (int)(skewedProportionY * fftSize / 2));
            auto level = jmap(fftData[fftDataIndex], 0.0f, jmax(maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);
            specImage.setPixelAt(rightHandEdge, y, Colour::fromHSV(level, 1.0f, level, 1.0f));
        }
        break;

    case SpecState::FreqMag:

        for (int x = 1; x < imageWidth; ++x)
        {
            auto skewedProportionX = 1.0f - std::exp(std::log((float)x / (float)imageWidth) * 0.2f);
            auto fftDataIndex = (size_t)jlimit(0, fftSize / 2, (int)(skewedProportionX * fftSize / 2));
            auto level = jmap(fftData[fftDataIndex], 0.0f, 20000.0f, 0.0f, (float)specImage.getHeight());
            specImage.setPixelAt(x, (int)level, Colour::greyLevel(1.0f));
        }
        break;

    }
}