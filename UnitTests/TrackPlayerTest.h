#pragma once

#include <JuceHeader.h>
#include "../Builds/VisualStudio2019/Source/TrackPlayer.h"

using namespace juce;

class TrackPlayerTest : public UnitTest
{
public:
    TrackPlayerTest() : UnitTest("TrackPlayerTest") {}

    void runTest() override
    {
        TrackPlayer player;

        beginTest("Part 1");

        expect(player.getTransportPosition() == 0.0);

        beginTest("Part 2");
    }
};

// Creating a static instance will automatically add the instance to the array
// returned by UnitTest::getAllTests(), so the test will be included when you call
// UnitTestRunner::runAllTests()
static TrackPlayerTest test;