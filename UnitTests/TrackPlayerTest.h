#pragma once

#include <JuceHeader.h>
#include "..\Source\TrackPlayer.h"

using namespace juce;

class TrackPlayerTest : public UnitTest
{
public:
    TrackPlayerTest() : UnitTest("TrackPlayerTest") {}

    void runTest() override
    {
        TrackPlayer player;

        beginTest("Load an audio file");
        String cwd = File::getCurrentWorkingDirectory().getFullPathName();
        cwd = cwd.replace("\\", "/");
        cwd = cwd += "/UnitTests/pinknoise.wav";
        File file(cwd);
        expect(player.loadAudioFile(&file));

        beginTest("Check transprt position after loading file");
        expect(player.getTransportPosition() == 0.0);

        beginTest("Check Play functions");
        expect(player.play());
        expect(!player.play());

        beginTest("Check Stop functions");
        expect(player.stop());
        expect(!player.stop());

        beginTest("Check Pause functions");
        expect(!player.pause());
        player.play();
        expect(player.pause());

        beginTest("isPlaying return test");
        expect(!player.isPlaying());
        player.play();
        expect(player.isPlaying());
    }
};

// Creating a static instance will automatically add the instance to the array
// returned by UnitTest::getAllTests(), so the test will be included when you call
// UnitTestRunner::runAllTests()
static TrackPlayerTest test;