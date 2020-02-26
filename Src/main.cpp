#include <iostream>
#include <string>
#include "../Inc/WavParser.h"
#include "../Inc/WavUtility.h"

int main()
{
	std::string Path = WavParserHelper::GetTestFilePath(TestFileType::Int16Stereo);

	auto MyParser = new WavParser();

	if (MyParser->LoadFile(Path))
	{
		MyParser->Parse();
	}

	std::cin.get();

	delete MyParser;
	return 0;
}