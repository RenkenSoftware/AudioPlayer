#pragma once

#include "pch.h"
#include "CppUnitTest.h"
#include "../TrackPlayer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TrackPlayerTests
{
	TEST_CLASS(TrackPlayerTests)
	{
	public:
		
		TEST_METHOD(MainTest)
		{
			TrackPlayer player;
			Assert::AreEqual(player.stop(), false);
			Assert::AreEqual(player.play(), true);
			Assert::AreEqual(player.stop(), true);
			delete &player;
		}
	};
}
