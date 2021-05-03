#include <JuceHeader.h>
#include "TrackPlayer.h"
#include "PlugIn.h"
#include "EQBand.h"
#include "PlugInWindow.h"
#include "EQBandWindow.h"
#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent() : specFFT (fftOrder),
                                 specImage(Image::RGB, 760, 300, true),
                                 mainPlayer(),
                                 fifoIndex(0),
                                 nextFFTBlockReady(false),
                                 specImageX(20),
                                 specImageY(200)
{
    // Make sure you set the size of the component after
    // you add any child components.
    
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

    addAndMakeVisible(addPlugInButton);
    addPlugInButton.setButtonText("Add plugin");
    addPlugInButton.setEnabled(true);
    addPlugInButton.addListener(this);

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

    addAndMakeVisible(volumeLabel);
    volumeLabel.setText("Volume", dontSendNotification);
    volumeLabel.attachToComponent(&volumeSlider, true);

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

        const float* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

        for (int i = 0; i < bufferToFill.numSamples; i++)
        {
            pushNextSampleIntoFifo(channelData[i]);
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
    g.drawImageAt(specImage, specImageX, specImageY);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    addPlugInButton.setBounds(700, 90, 90, 30);
    specButton.setBounds(700, 10, 90, 30);
    freqMagButton.setBounds(700, 50, 90, 30);
    loadButton.setBounds(10, 10, 100, 30);
    playButton.setBounds(120, 560, 70, 30);
    stopButton.setBounds(280, 560, 70, 30);
    pauseButton.setBounds(200, 560, 70, 30);
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
    else if (pButton == &specButton)
    {
        specButtonClicked();
    }
    else if (pButton == &freqMagButton)
    {
        freqMagButtonClicked();
    }
    else if (pButton == &addPlugInButton)
    {
        addPlugInButtonClicked();
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

        if (nextFFTBlockReady)
        {
            specFFT.performFrequencyOnlyForwardTransform(fftData.data());
            nextFFTBlockReady = false;
            drawSpecImage();
            repaint();
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

void MainComponent::specButtonClicked()
{
    freqMagButton.setEnabled(true);
    specButton.setEnabled(false);
    specState = SpecState::Spectrogram;
}

void MainComponent::freqMagButtonClicked()
{
    freqMagButton.setEnabled(false);
    specButton.setEnabled(true);
    specState = SpecState::FreqMag;
}

void MainComponent::addPlugInButtonClicked()
{
    for (int i = 0; i < 10; i++)
    {
        if (plugInsActive[i] == false)
        {
            plugIns[i] = new EQBandWindow("New Plugin", sampleRateValue);
            plugInsActive[i] = true;
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

void MainComponent::drawSpecImage()
{

    if (specState == SpecState::FreqMag)
    {
        drawFreqMagImage();
    }
    else
    {
        drawSpectralImage();
    }
}

void MainComponent::drawFreqMagImage()
{
    specImage.clear(specImage.getBounds(), Colour::greyLevel(0.0f));

    auto mindB = -100.0f;
    auto maxdB = 0.0f;

    for (int i = 0; i < scopeSize; ++i)
    {
        auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
        auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
        auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fftDataIndex])
            - juce::Decibels::gainToDecibels((float)fftSize)),
            mindB, maxdB, 0.0f, 1.0f);

        scopeData[i] = level;
    }

    for (int i = 0; i < scopeSize - 1; ++i)
    {
        drawLine(&specImage, juce::jmap(i, 0, scopeSize - 1, 0, specImage.getWidth()), juce::jmap(scopeData[i], 0.0f, 1.0f, (float)specImage.getHeight(), 0.0f), juce::jmap(i + 1, 0, scopeSize - 1, 0, specImage.getWidth()), juce::jmap(scopeData[i + 1], 0.0f, 1.0f, (float)specImage.getHeight(), 0.0f), Colour::greyLevel(1.0f));
    }
}

void MainComponent::drawSpectralImage()
{
    auto rightHandEdge = specImage.getWidth() - 1;
    auto imageHeight = specImage.getHeight();

    specImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);

    auto maxLevel = juce::FloatVectorOperations::findMinAndMax(fftData.data(), fftSize / 2);

    for (auto y = 1; y < imageHeight; ++y)
    {
        auto skewedProportionY = 1.0f - std::exp(std::log((float)y / (float)imageHeight) * 0.2f);
        auto fftDataIndex = (size_t)juce::jlimit(0, fftSize / 2, (int)(skewedProportionY * fftSize / 2));
        auto level = juce::jmap(fftData[fftDataIndex], 0.0f, juce::jmax(maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);

        specImage.setPixelAt(rightHandEdge, y, juce::Colour::fromHSV(level, 1.0f, level, 1.0f));
    }
}

void MainComponent::drawLine(Image* image, int fromX, int fromY, int toX, int toY, Colour colour)
{
    for (int i = 0; i <= 100; i++)
    {
        float lambda = ((float)i / 100.0f);
        image->setPixelAt((int)((lambda * (float)fromX) + ((1.0f - lambda) * (float)toX)), (int)((lambda * (float)fromY) + ((1.0f - lambda) * (float)toY)), colour);
    }
}