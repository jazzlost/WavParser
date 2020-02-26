#pragma once

#include <string>
#include <fstream>

#include "WavParser.h"

namespace WavParserHelper
{
	void PrintLog(const std::string& rhs, std::ostream& out_Stream = std::cout);

	void PrintChar(const char* ch, size_t size);

	void PrintWavHeaderType(IWavHeader* wavHeader);

	void PrintDataType(WavDataType DataType);

	void PrintWavHeader(IWavHeader* wavHeader, WavHeaderSubType SubType);

	void PrintRIFFChunk(RIFFChunk rhs);

	void PrintStandardFmtChunk(StandardPCMFmtChunk rhs);

	void PrintNonPcmFmtChunk(NonPcmFmtChunk rhs);

	void PrintExtensibleFmtChunk(ExtensibleFmtChunk rhs);

	void PrintFactChunk(FactChunk rhs);

	void PrintBextChunk(BextChunk rhs);

	void PrintRF64Chunk(RF64Chunk rhs);

	void PrintDS64Chunk(DS64Chunk rhs);

	void PrintCS64Chunk(CS64Chunk rhs, int Index);

	void PrintTableChunk(std::vector<CS64Chunk>& rhs);

	void PrintJunkChunk(JunkChunk rhs);

	void PrintDataChunk(DataChunk rhs);

	void PrintStandardWavHeader(StandardWavHead* wavHeader);

	void PrintStandardBwfHeader(StandardBWFHead* wavHeader);
	
	void PrintExtensibleBwfHeader(ExtensibleBWFHead* wavHeader);

	void PrintNonPcmWavHeader(NonPcmHead* wavHeader);

	//void PrintExtensibleNonPcmWavHeader(ExtensibleNonPCMHead* wavHeader);

	void PrintExtensibleWavHeader(ExtensibleWavHead* wavHeader);

	void PrintExtensibleNoFactWavHeader(ExtensibleNoFactHead* wavHeader);

	void PrintRF64Header(RF64Header* wavHeader);

}