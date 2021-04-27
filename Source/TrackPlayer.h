#pragma once

using namespace juce;

class TrackPlayer : public ChangeListener
{
public:

    /** Constructor.
    */
	TrackPlayer();

    /** Destructor. 
    
        Unloads audio file if audio file was loaded.
    */
    ~TrackPlayer();

    /** Unloads previously loaded audio file and loads new audio file. 
        Returns false, if passed file is invalid.

        @param file File to load.
    */
	bool loadAudioFile(File* file);

    /** Changes the TransportState to Playing. Returns false if the actual TransportState is already Playing
    */
    bool play();

    /** Changes the TransportState to Paused. Returns false if the actual TransportState is already Paused.
    */
    bool pause();

    /** Changes the TransportState to Stopped. Returns false if the actual TransportState is already Stopped.
    */
    bool stop();

    /**Sets the transport position. 
    
        @param newPosition New position to set the transport to.
    */
    void setTransportPosition(double newPosition);

    /** Sets gain for the transportSoure.
    
        @param gain New gain to set.
    */
    void setGain(float gain);

    /** Returns the current transport position.
    */
    double getTransportPosition();

    /** Returns true, if the player is playing.
    */
    bool isPlaying();

    /** Calls the getNextAudioBlock function on the transportSource, filling the passed buffer with audio data from the loaded file.
    
        @param bufferToFill Buffer of the getNextAudioBlock function of your audio application to be filled with audio data from the loaded audio file.
    */

    /** Returns the length of the loaded audio file in seconds.
    */
    double getLength();

    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);

    /** Calls the prepareToPlay function on the transportSource.
    
        @param samplesPerBlockExpected Passed from the prepareToPlay function in your audio application.

        @param sampleRate Passed from the prepareToPlay function in your audio application.
    */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);

    /** Calls the releaseResources function on the transportSource.
    */
    void releaseResources();

private:
    enum class FileState
    {
        FileLoaded,
        NoFileLoaded
    };
    enum class TransportState {
        Playing,
        Stopped,
        Paused
    };

    AudioFormatManager formatManager;
    AudioFormatReader* reader{};
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    FileState fileState;
    TransportState transportState;

    double transportPosition;

    /** Unloads loaded audio file and returns true.
    
        Returns false if no audio file is loaded.
    */
    bool unloadAudioFile();

    /** Changes the actual TransportState and returns true. Returns false if the passed TransportState is already the actual TransportState.
        
        @param state TransportState to change to.
    */
    bool changeTransportState(TransportState state);

    /** See changeListenerCallback function from ChangeListener class.
    */
    void changeListenerCallback(ChangeBroadcaster* source) override;
};

