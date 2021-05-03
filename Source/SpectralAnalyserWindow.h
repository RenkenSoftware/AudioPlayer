#pragma once

using namespace juce;

class SpectralAnalyserWindow : public PlugInWindow
{
public:
	SpectralAnalyserWindow(String name);

	~SpectralAnalyserWindow() override;

	void process(const AudioSourceChannelInfo& bufferToFill) override;

private:

	SpectralAnalyser* analyser;
};