#pragma once

#include <JuceHeader.h>

using namespace juce;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public AudioAppComponent,
                       public Button::Listener,
                       public Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...

    TextButton loadButton;
    TextButton playButton;
    TextButton stopButton;
    TextButton pauseButton;
    Slider volumeSlider;
    Slider transportSlider;
    Label volumeLabel;

    Label message;

    File resourceFile;

    void buttonClicked(Button* pButton);

    void sliderValueChanged(Slider* slider);

    void sliderDragEnded(Slider* slider);

    AudioFormatManager formatManager;

    bool playing;
    bool fileLoaded;
    double volume;

    AudioFormatReader* reader;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
