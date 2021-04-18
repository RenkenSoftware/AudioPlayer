#pragma once

#include <JuceHeader.h>
#include "../TrackPlayer.h"

using namespace juce;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public AudioAppComponent,
    public Button::Listener,
    public Slider::Listener,
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

    static constexpr auto fftOrder = 10;
    static constexpr auto fftSize = 1 << fftOrder;
    static constexpr auto scopeSize = 512;

private:

    enum class SpecState {
        Spectrogram,
        FreqMag
    };

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

    void buttonClicked(Button* pButton) override;
    void sliderValueChanged(Slider* slider) override;
    void sliderDragEnded(Slider* slider) override;
    void timerCallback() override;
    void mouseDoubleClick(const MouseEvent& event) override;

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
    void pushNextSampleIntoFifo(float sample) noexcept;
    void drawSpecImage();
    void drawFreqMagImage();
    void drawSpectralImage();
    void drawLine(Image* image, int fromX, int fromY, int toX, int toY, Colour colour);

    IIRFilter bassEqL;
    IIRFilter bassEqR;
    IIRFilter midEqL;
    IIRFilter midEqR;
    IIRFilter highEqL;
    IIRFilter highEqR;

    double sampleRateValue;

    dsp::FFT specFFT;
    std::array<float, fftSize> fifo;
    std::array<float, fftSize * 2> fftData;
    std::array<float, scopeSize> scopeData;
    int fifoIndex;
    bool nextFFTBlockReady;
    Image specImage;
    int specImageX;
    int specImageY;

    TrackPlayer mainPlayer{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};