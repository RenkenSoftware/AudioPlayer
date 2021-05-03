#include <JuceHeader.h>
#include "PlugIn.h"
#include "SpectralAnalyser.h"

SpectralAnalyser::SpectralAnalyser() : specFFT(fftOrder),
														specImage(Image::RGB, 760, 300, true),
														fifoIndex(0),
														nextFFTBlockReady(false),
														specImageX(10),
														specImageY(10)
{
	setSize(880, 350);

	addAndMakeVisible(specButton);
	specButton.setButtonText("Spectogram mode");
	specButton.setEnabled(true);
	specButton.addListener(this);

	addAndMakeVisible(freqMagButton);
	freqMagButton.setButtonText("Frequency-magnitude mode");
	freqMagButton.setEnabled(false);
	freqMagButton.addListener(this);

	specState = SpecState::FreqMag;

	Timer::startTimer(10);
}

SpectralAnalyser::~SpectralAnalyser()
{
	Timer::stopTimer();
}

void SpectralAnalyser::resized()
{
	specButton.setBounds(780, 10, 90, 30);
	freqMagButton.setBounds(780, 50, 90, 30);
}

void SpectralAnalyser::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.setOpacity(1.0f);
	g.drawImageAt(specImage, specImageX, specImageY);
}

void SpectralAnalyser::processNextBlock(const AudioSourceChannelInfo& bufferToFill)
{
	const float* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

	for (int i = 0; i < bufferToFill.numSamples; i++)
	{
		pushNextSampleIntoFifo(channelData[i]);
	}
}

void SpectralAnalyser::buttonClicked(Button* pButton)
{
	if (pButton == &specButton)
	{
		specButtonClicked();
	}
	else if (pButton == &freqMagButton)
	{
		freqMagButtonClicked();
	}
}

void SpectralAnalyser::timerCallback()
{
	if (nextFFTBlockReady)
	{
		specFFT.performFrequencyOnlyForwardTransform(fftData.data());
		nextFFTBlockReady = false;
		drawSpecImage();
		repaint();
	}
}

void SpectralAnalyser::specButtonClicked()
{
	freqMagButton.setEnabled(true);
	specButton.setEnabled(false);
	specState = SpecState::Spectrogram;
}

void SpectralAnalyser::freqMagButtonClicked()
{
	freqMagButton.setEnabled(false);
	specButton.setEnabled(true);
	specState = SpecState::FreqMag;
}

void SpectralAnalyser::pushNextSampleIntoFifo(float sample) noexcept
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

void SpectralAnalyser::drawSpecImage()
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

void SpectralAnalyser::drawFreqMagImage()
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

void SpectralAnalyser::drawSpectralImage()
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

void SpectralAnalyser::drawLine(Image* image, int fromX, int fromY, int toX, int toY, Colour colour)
{
	for (int i = 0; i <= 100; i++)
	{
		float lambda = ((float)i / 100.0f);
		image->setPixelAt((int)((lambda * (float)fromX) + ((1.0f - lambda) * (float)toX)), (int)((lambda * (float)fromY) + ((1.0f - lambda) * (float)toY)), colour);
	}
}


