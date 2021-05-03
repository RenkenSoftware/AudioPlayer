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

private:

    double sampleRateValue;

    TextButton addEQBandButton;
    TextButton addAnalyserButton;

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
    void addEQBandButtonClicked();
    void addAnalyserButtonClicked();
    void volumeSliderValueChanged();
    void transportSliderDragEnded();

    void plugInSelected();

    TrackPlayer mainPlayer{};

    std::array<std::optional<PlugInWindow*>, 10> plugIns;
    std::array<bool, 10> plugInsActive;

    ComboBox plugInList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};