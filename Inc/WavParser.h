#pragma once

#include <vector>
#include <string>
#include <iostream>
#include "WavTypes.h"

#define DEBUG

class WavParser
{
public:
	WavParser();
	virtual ~WavParser() = default;

	virtual bool LoadFile(const std::string& path);
	virtual void Parse();

private:
	void Initialize(std::ifstream& in_File);

private:
	//File Path
	std::string& m_Path;

	//File Stream
	std::ifstream& m_Stream;

	//File Complete Wav Data
	p_WavFile m_File;
	
	//File Header Types
	WavHeaderTypeGroup m_HeaderTypeGroup;

	//File RawData Types
	WavDataType m_DataType;
};