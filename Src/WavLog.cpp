#include "../Inc/WavLog.h"
#include "../Inc/WavUtility.h"

void WavParserHelper::PrintLog(const std::string &rhs, std::ostream &out_Stream)
{
	/*std::ofstream out_File("123.txt", std::ios_base::app);*/
	out_Stream << rhs << std::endl;
	/*out_File << rhs << std::endl;*/
}

void WavParserHelper::PrintChar(const char *ch, size_t size)
{
	for (int i = 0; i < size; i++)
	{
		std::cout << ch[i];
	}
	std::cout << std::endl;
}

void WavParserHelper::PrintWavHeaderType(IWavHeader *wavHeader)
{
	switch (wavHeader->type)
	{
	case WavHeaderType::Standard:

		WavParserHelper::PrintLog("Standard Wav Header");
		break;

	case WavHeaderType::NonPCM:

		WavParserHelper::PrintLog("NonPCM Wav Header");
		break;

	case WavHeaderType::Extensible:
		WavParserHelper::PrintLog("Extensible Wav Header");
		break;
	}
}

void WavParserHelper::PrintDataType(WavDataType DataType)
{
	switch (DataType)
	{
	case WavDataType::WavData8bit:
		std::cout << "WavData : uint 8bit" << std::endl;
		break;
	case WavDataType::WavData12bit:
		std::cout << "WavData : int 12bit" << std::endl;
		break;
	case WavDataType::WavData16bit:
		std::cout << "WavData : int 16bit" << std::endl;
		break;
	case WavDataType::WavData24bit:
		std::cout << "WavData : int 24bit" << std::endl;
		break;
	case WavDataType::WavData32bit:
		std::cout << "WavData : float 32bit" << std::endl;
		break;
	case WavDataType::WavData64bit:
		std::cout << "WavData : double 64bit" << std::endl;
		break;
	default:
		std::cout << "WavData : Invalid" << std::endl;
	}

	return;
}

void WavParserHelper::PrintWavHeader(IWavHeader *wavHeader, WavHeaderSubType SubType)
{
#ifdef DEBUG
	std::cout << std::endl;
#endif // DEBUG

	switch (wavHeader->type)
	{
	case WavHeaderType::Standard:
		PrintStandardWavHeader(dynamic_cast<StandardWavHead *>(wavHeader));
		break;

	case WavHeaderType::Bwf:
		if (SubType == WavHeaderSubType::Normal)
			PrintStandardBwfHeader(dynamic_cast<StandardBWFHead *>(wavHeader));
		else if (SubType == WavHeaderSubType::Extensible)
			PrintExtensibleBwfHeader(dynamic_cast<ExtensibleBWFHead *>(wavHeader));
		else if (SubType == WavHeaderSubType::RF64)
			PrintRF64Header(dynamic_cast<RF64Header *>(wavHeader));
		break;

	case WavHeaderType::NonPCM:
		if (SubType == WavHeaderSubType::Normal)
			PrintNonPcmWavHeader(dynamic_cast<NonPcmHead *>(wavHeader));
		//else if (SubType == WavHeaderSubType::Extensible)
		//	PrintExtensibleNonPcmWavHeader(dynamic_cast<ExtensibleNonPCMHead*>(wavHeader));
		break;

	case WavHeaderType::Extensible:
		if (SubType == WavHeaderSubType::Normal)
			PrintExtensibleWavHeader(dynamic_cast<ExtensibleWavHead *>(wavHeader));
		else if (SubType == WavHeaderSubType::NoFact)
			PrintExtensibleNoFactWavHeader(dynamic_cast<ExtensibleNoFactHead *>(wavHeader));
		break;

	default:
		break;
	}

#ifdef DEBUG
	std::cout << std::endl;
#endif // DEBUG
}

void WavParserHelper::PrintRIFFChunk(RIFFChunk rhs)
{
	//if not restrict size, there problem on \0 with char convert to string
	std::string RIFF(rhs.ckID);
	RIFF.resize(4);
	std::string WAVE(rhs.WavID);
	WAVE.resize(4);

	PrintLog("ChunkID: " + RIFF);
	PrintLog("ChunkSize: " + std::to_string(rhs.ckSize));
	PrintLog("WAVE: " + WAVE);

	std::cout << std::endl;
}

void WavParserHelper::PrintStandardFmtChunk(StandardPCMFmtChunk rhs)
{
	//if not restrict size, there problem on \0 with char convert to string
	std::string FMT(rhs.ckID);
	FMT.resize(4);

	PrintLog("ChunkID: " + FMT);
	PrintLog("ChunkSize: " + std::to_string(rhs.ckSize));
	PrintLog("AudioFormat: " + WavParserHelper::GetAudioFormatTag(rhs.wFormatTag));
	PrintLog("NumOfChan: " + std::to_string(rhs.nChannels));
	PrintLog("SamplesPerSec: " + std::to_string(rhs.nSamplesPerSec));
	PrintLog("bytesPerSec: " + std::to_string(rhs.nAvgBytesPerSec));
	PrintLog("blockAlign: " + std::to_string(rhs.nBlockAlign));
	PrintLog("bitsPerSample: " + std::to_string(rhs.wBitsPerSample));

	std::cout << std::endl;
}

void WavParserHelper::PrintNonPcmFmtChunk(NonPcmFmtChunk rhs)
{
	//if not restrict size, there problem on \0 with char convert to string
	std::string FMT(rhs.standardFmtChunk.ckID);
	FMT.resize(4);

	PrintLog("ChunkID: " + FMT);
	PrintLog("ChunkSize: " + std::to_string(rhs.standardFmtChunk.ckSize));
	PrintLog("AudioFormat: " + WavParserHelper::GetAudioFormatTag(rhs.standardFmtChunk.wFormatTag));
	PrintLog("NumOfChan: " + std::to_string(rhs.standardFmtChunk.nChannels));
	PrintLog("SamplesPerSec: " + std::to_string(rhs.standardFmtChunk.nSamplesPerSec));
	PrintLog("bytesPerSec: " + std::to_string(rhs.standardFmtChunk.nAvgBytesPerSec));
	PrintLog("blockAlign: " + std::to_string(rhs.standardFmtChunk.nBlockAlign));
	PrintLog("bitsPerSample: " + std::to_string(rhs.standardFmtChunk.wBitsPerSample));
	//PrintLog("ExtensionSize: " + std::to_string(rhs.cbSize));

	std::cout << std::endl;
}

void WavParserHelper::PrintExtensibleFmtChunk(ExtensibleFmtChunk rhs)
{
	//if not restrict size, there problem on \0 with char convert to string
	std::string FMT(rhs.standardFmtChunk.ckID);
	FMT.resize(4);
	std::string SubFormat(rhs.SubFormat);
	SubFormat.resize(16);

	PrintLog("ChunkID: " + FMT);
	PrintLog("ChunkSize: " + std::to_string(rhs.standardFmtChunk.ckSize));
	PrintLog("AudioFormat: " + WavParserHelper::GetAudioFormatTag(rhs.standardFmtChunk.wFormatTag));
	PrintLog("NumOfChan: " + std::to_string(rhs.standardFmtChunk.nChannels));
	PrintLog("SamplesPerSec: " + std::to_string(rhs.standardFmtChunk.nSamplesPerSec));
	PrintLog("bytesPerSec: " + std::to_string(rhs.standardFmtChunk.nAvgBytesPerSec));
	PrintLog("blockAlign: " + std::to_string(rhs.standardFmtChunk.nBlockAlign));
	PrintLog("bitsPerSample: " + std::to_string(rhs.standardFmtChunk.wBitsPerSample));
	PrintLog("ExtensionSize: " + std::to_string(rhs.cbSize));
	PrintLog("ValidBitsPerSample: " + std::to_string(rhs.wValidBitsPerSample));
	PrintLog("ChannelMask: " + std::to_string(rhs.dwChannelMask));
	PrintLog("SubFormat: " + SubFormat);

	std::cout << std::endl;
}

void WavParserHelper::PrintFactChunk(FactChunk rhs)
{
	//if not restrict size, there problem on \0 with char convert to string
	std::string Data(rhs.ckID);
	Data.resize(4);

	PrintLog("ChunkID: " + Data);
	PrintLog("ChunkSize: " + std::to_string(rhs.ckSize));
	PrintLog("SampleLength: " + std::to_string(rhs.dwSampleLength));

	std::cout << std::endl;
}

void WavParserHelper::PrintBextChunk(BextChunk rhs)
{
	//if not restrict size, there problem on \0 with char convert to string
	std::string ckID(rhs.ckID);
	ckID.resize(4);
	std::string Description(rhs.Description);
	Description.resize(256);
	std::string Originator(rhs.Originator);
	Originator.resize(32);
	std::string OriginatorReference(rhs.OriginatorReference);
	OriginatorReference.resize(32);
	std::string OriginationDate(rhs.OriginationDate);
	OriginationDate.resize(10);
	std::string OriginationTime(rhs.OriginationTime);
	OriginationTime.resize(8);

	PrintLog("ChunkID: " + ckID);
	PrintLog("ChunkSize: " + std::to_string(rhs.ckSize));
	PrintLog("Description: " + Description);
	PrintLog("Originator: " + Originator);
	PrintLog("OriginatorReference: " + OriginatorReference);
	PrintLog("OriginationDate: " + OriginationDate);
	PrintLog("OtiginationTime: " + OriginationTime);
	PrintLog("TimeReferenceLow: " + std::to_string(rhs.TimeReferenceLow));
	PrintLog("TimeReferenceHigh: " + std::to_string(rhs.TimeReferenceHigh));
	PrintLog("Version: " + std::to_string(rhs.Version));
	PrintLog("LoudnessValue: " + std::to_string(rhs.LoudnessValue));
	PrintLog("LoudnessRange: " + std::to_string(rhs.LoudnessRange));
	PrintLog("MaxTruePeakLevel: " + std::to_string(rhs.MaxTruePeakLevel));
	PrintLog("MaxMomentaryLoudness: " + std::to_string(rhs.MaxMomentaryLoudness));
	PrintLog("MaxShortTermLoudness: " + std::to_string(rhs.MaxShortTermLoudness));

	std::cout << std::endl;
}

void WavParserHelper::PrintJunkChunk(JunkChunk rhs)
{
	std::string ckID(rhs.ckID);
	ckID.resize(4);

	PrintLog("ChunkID: " + ckID);
	PrintLog("ChunkSize: " + std::to_string(rhs.ckSize));
	std::cout << std::endl;
}

void WavParserHelper::PrintRF64Chunk(RF64Chunk rhs)
{
	std::string ckID(rhs.ckID);
	ckID.resize(4);

	std::string Rf64Type(rhs.rf64Type);
	Rf64Type.resize(4);

	PrintLog("ChunkID: " + ckID);
	PrintLog("ChunkSize: " + std::to_string(rhs.ckSize));
	PrintLog("RF64Type: " + Rf64Type);

	std::cout << std::endl;
}

void WavParserHelper::PrintDS64Chunk(DS64Chunk rhs)
{
	std::string ckID(rhs.ckID);
	ckID.resize(4);

	PrintLog("ChunkID: " + ckID);
	PrintLog("ChunkSize: " + std::to_string(rhs.ckSize));
	PrintLog("RIFFSizeLow: " + std::to_string(rhs.RIFFSizeLow));
	PrintLog("RIFFSizeHigh: " + std::to_string(rhs.RIFFSizeHigh));
	PrintLog("DataSizeLow: " + std::to_string(rhs.DataSizeLow));
	PrintLog("DataSizeHigh: " + std::to_string(rhs.DataSizeHigh));
	PrintLog("SampleCountLow: " + std::to_string(rhs.SampleCountLow));
	PrintLog("SampleCountHigh: " + std::to_string(rhs.SampleCountHigh));
	PrintLog("TableLength: " + std::to_string(rhs.TableLength));

	std::cout << std::endl;
}

void WavParserHelper::PrintCS64Chunk(CS64Chunk rhs, int Index)
{
	std::string ckID(rhs.ckID);
	ckID.resize(4);

	PrintLog("Index: " + Index);
	PrintLog("ChunkID: " + ckID);
	PrintLog("ChunkSizeLow: " + std::to_string(rhs.ckSizeLow));
	PrintLog("ChunkSizeHigh: " + std::to_string(rhs.ckSizeHigh));

	std::cout << std::endl;
}

void WavParserHelper::PrintTableChunk(std::vector<CS64Chunk> &rhs)
{
	int Count = 1;
	for (auto Chunk : rhs)
	{
		PrintCS64Chunk(Chunk, Count++);
	}

	std::cout << std::endl;
}

void WavParserHelper::PrintDataChunk(DataChunk rhs)
{
	//if not restrict size, there problem on \0 with char convert to string
	std::string Data(rhs.ckID);
	Data.resize(4);

	PrintLog("ChunkID: " + Data);
	PrintLog("ChunkSize: " + std::to_string(rhs.ckSize));

	std::cout << std::endl;
}

void WavParserHelper::PrintStandardWavHeader(StandardWavHead *wavHeader)
{
	PrintRIFFChunk(wavHeader->RIFFChunk);
	PrintStandardFmtChunk(wavHeader->fmtChunk);
	PrintDataChunk(wavHeader->DataChunk);
}

void WavParserHelper::PrintStandardBwfHeader(StandardBWFHead *wavHeader)
{
	PrintRIFFChunk(wavHeader->RIFFChunk);
	PrintStandardFmtChunk(wavHeader->fmtChunk);
	PrintBextChunk(wavHeader->BextChunk);
	PrintJunkChunk(wavHeader->JunkChunk);
	PrintDataChunk(wavHeader->DataChunk);
}

void WavParserHelper::PrintNonPcmWavHeader(NonPcmHead *wavHeader)
{
	PrintRIFFChunk(wavHeader->RIFFChunk);
	PrintNonPcmFmtChunk(wavHeader->fmtChunk);
	PrintFactChunk(wavHeader->FactChunk);
	PrintDataChunk(wavHeader->DataChunk);
}

//void WavParserHelper::PrintExtensibleNonPcmWavHeader(ExtensibleNonPCMHead* wavHeader)
//{
//	PrintRIFFChunk(wavHeader->RIFFChunk);
//	PrintExtensibleFmtChunk(wavHeader->fmtChunk);
//	PrintFactChunk(wavHeader->FactChunk);
//	PrintDataChunk(wavHeader->DataChunk);
//}

void WavParserHelper::PrintExtensibleWavHeader(ExtensibleWavHead *wavHeader)
{
	PrintRIFFChunk(wavHeader->RIFFChunk);
	PrintExtensibleFmtChunk(wavHeader->fmtChunk);
	PrintFactChunk(wavHeader->FactChunk);
	PrintDataChunk(wavHeader->DataChunk);
}

void WavParserHelper::PrintExtensibleNoFactWavHeader(ExtensibleNoFactHead *wavHeader)
{
	PrintRIFFChunk(wavHeader->RIFFChunk);
	PrintExtensibleFmtChunk(wavHeader->fmtChunk);
	PrintDataChunk(wavHeader->DataChunk);
}

void WavParserHelper::PrintExtensibleBwfHeader(ExtensibleBWFHead *wavHeader)
{
	PrintRIFFChunk(wavHeader->RIFFChunk);
	PrintExtensibleFmtChunk(wavHeader->fmtChunk);
	PrintBextChunk(wavHeader->BextChunk);
	PrintJunkChunk(wavHeader->JunkChunk);
	PrintDataChunk(wavHeader->DataChunk);
}

void WavParserHelper::PrintRF64Header(RF64Header *wavHeader)
{
	PrintRF64Chunk(wavHeader->RF64Chunk);
	PrintDS64Chunk(wavHeader->DS64Chunk);
	PrintExtensibleFmtChunk(wavHeader->fmtChunk);
	PrintBextChunk(wavHeader->BextChunk);
	//PrintJunkChunk(wavHeader->JunkChunk);
	PrintDataChunk(wavHeader->DataChunk);
}
