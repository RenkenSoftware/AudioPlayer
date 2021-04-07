#pragma once

#include <JuceHeader.h>

using namespace juce;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public AudioAppComponent,
    public Button::Listener,
    public Slider::Listener,
    public ChangeListener,
    public Timer
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

    enum class TransportState {
        Playing,
        Stopped,
        Paused,
        NoFileLoaded,
        FileLoaded,
    };

    TransportState state;

    TextButton loadButton;
    TextButton playButton;
    TextButton stopButton;
    TextButton pauseButton;
    Slider volumeSlider;
    Slider transportSlider;
    Label volumeLabel;

    Label message;

    void buttonClicked(Button* pButton) override;
    void sliderValueChanged(Slider* slider) override;
    void sliderDragEnded(Slider* slider) override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void timerCallback() override;

    void loadButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void pauseButtonClicked();
    void volumeSliderValueChanged();
    void transportSliderDragEnded();
    void changeTransportState(TransportState newState);
    void doDSP(AudioSourceChannelInfo& bufferToFill);

    AudioFormatManager formatManager;
    AudioFormatReader* reader{};

    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
