#pragma once

using namespace juce;

class SpectralAnalyser : public PlugIn, public Button::Listener, public Timer
{
public:
	SpectralAnalyser();

	~SpectralAnalyser();

	void resized() override;

	void paint(juce::Graphics& g) override;

	void processNextBlock(const AudioSourceChannelInfo& bufferToFill) override;

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

	void buttonClicked(Button* pButton) override;
	void timerCallback() override;

	void specButtonClicked();
	void freqMagButtonClicked();
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

};