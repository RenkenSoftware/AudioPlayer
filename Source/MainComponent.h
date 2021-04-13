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
    public Timer,
    public KeyListener
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

    static constexpr auto fftOrder = 10;
    static constexpr auto fftSize = 1 << fftOrder;

private:

    enum class TransportState {
        Playing,
        Stopped,
        Paused,
        NoFileLoaded,
        FileLoaded
    };

    enum class SpecState {
        Spectrogram,
        FreqMag
    };

    TransportState transportState;
    SpecState specState;
   
    TextButton specButton;
    TextButton freqMagButton;
    TextButton loadButton;
    TextButton playButton;
    TextButton stopButton;
    TextButton pauseButton;
    Slider volumeSlider;
    Slider transportSlider;
    Slider bassEqSlider;
    Slider midEqSlider;
    Slider highEqSlider;
    Label volumeLabel;
    Label bassLabel;
    Label midLabel;
    Label highLabel;
    Label message;

    void buttonClicked(Button* pButton) override;
    void sliderValueChanged(Slider* slider) override;
    void sliderDragEnded(Slider* slider) override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void timerCallback() override;
    bool keyPressed(const KeyPress& key, Component* component) override;

    void loadButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void pauseButtonClicked();
    void specButtonClicked();
    void freqMagButtonClicked();
    void volumeSliderValueChanged();
    void bassEqSliderValueChanged();
    void midEqSliderValueChanged();
    void highEqSliderValueChanged();
    void transportSliderDragEnded();
    void changeTransportState(TransportState newState);
    void pushNextSampleIntoFifo(float sample) noexcept;
    void drawNextLineOfSpectrogram();

    AudioFormatManager formatManager;
    AudioFormatReader* reader{};
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;

    IIRFilter bassEqL;
    IIRFilter bassEqR;
    IIRFilter midEqL;
    IIRFilter midEqR;
    IIRFilter highEqL;
    IIRFilter highEqR;

    double sampleRateValue;

    dsp::FFT specFFT;
    Image specImage;
    std::array<float, fftSize> fifo;
    std::array<float, fftSize * 2> fftData;
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};