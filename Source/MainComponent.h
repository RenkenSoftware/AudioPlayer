#pragma once
#include <optional>

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

    double sampleRateValue;

    TextButton addPlugInButton;
   
    TextButton specButton;
    TextButton freqMagButton;
    TextButton loadButton;
    TextButton playButton;
    TextButton stopButton;
    TextButton pauseButton;
    Slider volumeSlider;
    Slider transportSlider;
    Label volumeLabel;

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
    void addPlugInButtonClicked();
    void volumeSliderValueChanged();
    void transportSliderDragEnded();
    void pushNextSampleIntoFifo(float sample) noexcept;
    void drawSpecImage();
    void drawFreqMagImage();
    void drawSpectralImage();
    void drawLine(Image* image, int fromX, int fromY, int toX, int toY, Colour colour);

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

    std::array<std::optional<PlugInWindow*>, 10> plugIns;
    std::array<bool, 10> plugInsActive;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};