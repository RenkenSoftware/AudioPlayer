#include "TrackPlayer.h"

TrackPlayer::TrackPlayer()
{
    transportSource.addChangeListener(this);
    formatManager.registerBasicFormats();
    fileState = FileState::NoFileLoaded;
    transportState = TransportState::Stopped;
    transportPosition = 0.0;
}

TrackPlayer::~TrackPlayer()
{
	if (fileState == FileState::FileLoaded)
	{
		unloadAudioFile();
	}
}

bool TrackPlayer::loadAudioFile(File* file)
{
    if (fileState == FileState::FileLoaded)
    {
        unloadAudioFile();
    }

    reader = formatManager.createReaderFor(*file);

    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
        fileState = FileState::FileLoaded;
        changeTransportState(TransportState::Stopped);
        return true;
    }
    else {
        delete reader;
        return false;
    }
}

bool TrackPlayer::play()
{
    return changeTransportState(TransportState::Playing);
}

bool TrackPlayer::pause()
{
    return changeTransportState(TransportState::Paused);
}

bool TrackPlayer::stop()
{
    return changeTransportState(TransportState::Stopped);
}

void TrackPlayer::setTransportPosition(double newPosition)
{
    transportSource.setPosition(newPosition);
}

void TrackPlayer::setGain(float gain)
{
    transportSource.setGain(gain);
}

double TrackPlayer::getTransportPosition()
{
    return transportSource.getCurrentPosition();
}

bool TrackPlayer::isPlaying()
{
    if (transportState == TransportState::Playing)
    {
        return true;
    }
    
    return false;
}

double TrackPlayer::getLength()
{
    return transportSource.getLengthInSeconds();
}

void TrackPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    if (transportState == TransportState::Playing)
    {
        transportSource.getNextAudioBlock(bufferToFill);
    }
}

void TrackPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void TrackPlayer::releaseResources()
{
    transportSource.releaseResources();
}

//================================================================================================

bool TrackPlayer::unloadAudioFile()
{
    if (fileState == FileState::FileLoaded)
    {
        fileState = FileState::NoFileLoaded;
        return true;
    }

    return false;
}

bool TrackPlayer::changeTransportState(TransportState state)
{
    if (transportState == state)
    {
        return false;
    }

    switch (state)
    {
    case TransportState::Stopped:
        transportSource.setPosition(0.0);
        transportSource.stop();
        break;

    case TransportState::Playing:
        if (fileState == FileState::NoFileLoaded)
        {
            return false;
        }

        if (transportState == TransportState::Stopped)
        {
            transportSource.start();
        }
        break;

    case TransportState::Paused:
        if (transportState == TransportState::Stopped)
        {
            return false;
        }
        break;
    }

    transportState = state;
    return true;
}

void TrackPlayer::changeListenerCallback(ChangeBroadcaster* source)
{
    if (transportSource.hasStreamFinished())
    {
        changeTransportState(TransportState::Stopped);
    }
}
