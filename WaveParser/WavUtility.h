#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "WavParser.h"




namespace WavParserHelper
{
	WavHeaderTypeGroup GetWavHeaderType(std::ifstream& in_File);

	WavDataType GetWavDataType(std::ifstream& in_File);

	inline void EndianSwap(unsigned short& x)
	{
		x = (x >> 8) | (x << 8);
	}

	void PrintHeaderMemory();

	std::string GetTestFilePath(TestFileType FileType);

	std::string GetAudioFormatTag(unsigned short wFormatTag);

}