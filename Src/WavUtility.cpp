#include "../Inc/WavUtility.h"
#include <cstring>

WavDataType WavParserHelper::GetWavDataType(std::ifstream &in_File)
{
	if (in_File)
	{
		short wavDataType;

		in_File.seekg(0, std::ios_base::beg);
		char ckID[4];
		in_File.read(ckID, sizeof(ckID));

		if (!std::strncmp(ckID, "RIFF", 4))
		{
			in_File.seekg(34, std::ios_base::beg);
			in_File.read(reinterpret_cast<char *>(&wavDataType), sizeof(wavDataType));
		}
		else if (!std::strncmp(ckID, "RF64", 4))
		{
			long ckSize;
			in_File.seekg(16, std::ios_base::beg);
			in_File.read(reinterpret_cast<char *>(&ckSize), sizeof(ckSize));
			in_File.seekg(ckSize + 22, std::ios_base::cur);
			in_File.read(reinterpret_cast<char *>(&wavDataType), sizeof(wavDataType));
		}

		switch (wavDataType)
		{
		case 8:
			return WavDataType::WavData8bit;
			break;
		case 12:
			return WavDataType::WavData12bit;
			break;
		case 16:
			return WavDataType::WavData16bit;
			break;
		case 24:
			return WavDataType::WavData24bit;
			break;
		case 32:
			return WavDataType::WavData32bit;
			break;
		case 64:
			return WavDataType::WavData64bit;
			break;
		default:
			return WavDataType::Invalid;
		}
	}

	return WavDataType::Invalid;
}

WavHeaderTypeGroup WavParserHelper::GetWavHeaderType(std::ifstream &in_File)
{
	WavHeaderTypeGroup HeaderTypeGroup;

	if (in_File)
	{

		long wavHeaderType;
		in_File.seekg(16, std::ios_base::beg);
		in_File.read(reinterpret_cast<char *>(&wavHeaderType), sizeof(wavHeaderType));

		short wavHeaderSubType;
		in_File.read(reinterpret_cast<char *>(&wavHeaderSubType), sizeof(wavHeaderSubType));

		//Three Big Type : PCM / Non-PCM / Extensible
		if (wavHeaderType == 16)
		{
			in_File.seekg(36, std::ios_base::beg);
			char ckID[4];
			in_File.read(ckID, sizeof(ckID));

			//Standard PCM
			if (!std::strncmp(ckID, "data", 4))
			{
				HeaderTypeGroup.MajorType = WavHeaderType::Standard;
			}
			//StandardBWF
			else if (!std::strncmp(ckID, "bext", 4))
			{
				HeaderTypeGroup.MajorType = WavHeaderType::Bwf;

				HeaderTypeGroup.SubType = WavHeaderSubType::Normal;
			}
			else
			{
				HeaderTypeGroup.MajorType = WavHeaderType::Invalid;
			}
		}
		else if (wavHeaderType == 18)
		{
			HeaderTypeGroup.MajorType = WavHeaderType::NonPCM;

			in_File.seekg(38, std::ios_base::beg);
			char ckID[4];
			in_File.read(ckID, sizeof(ckID));
			if (!std::strncmp(ckID, "fact", 4))
				HeaderTypeGroup.SubType = WavHeaderSubType::Normal;
		}
		else if (wavHeaderType == 28)
		{
			HeaderTypeGroup.MajorType = WavHeaderType::Bwf;

			in_File.seekg(0, std::ios_base::beg);
			char ckID[4];
			in_File.read(ckID, sizeof(ckID));

			if (!std::strncmp(ckID, "RF64", 4))
				HeaderTypeGroup.SubType = WavHeaderSubType::RF64;
			else
				HeaderTypeGroup.SubType = WavHeaderSubType::Normal;
		}
		else if (wavHeaderType == 40)
		{
			in_File.seekg(36, std::ios_base::beg);
			char ckID[4];
			in_File.read(ckID, sizeof(ckID));
			if (!std::strncmp(ckID, "data", 4) || !std::strncmp(ckID, "bext", 4))
			{
				HeaderTypeGroup.MajorType = WavHeaderType::Bwf;
			}
			else
			{
				HeaderTypeGroup.MajorType = WavHeaderType::Extensible;

				in_File.seekg(60, std::ios_base::beg);
				char ckID[4];
				in_File.read(ckID, sizeof(ckID));
				if (!std::strncmp(ckID, "fact", 4))
					HeaderTypeGroup.SubType = WavHeaderSubType::Normal;
				else
					HeaderTypeGroup.SubType = WavHeaderSubType::NoFact;
			}
		}
		else
		{
			HeaderTypeGroup.MajorType = WavHeaderType::Invalid;
		}

		if (wavHeaderSubType == 0xFFFE)
			HeaderTypeGroup.SubType = WavHeaderSubType::Extensible;
	}

	return HeaderTypeGroup;
}

//============================================================================
void WavParserHelper::PrintHeaderMemory()
{
	IWavHeader *wavHeader = new IWavHeader(WavHeaderType::Standard);
	/*StandardWavHead* standardHeader = new StandardWavHead();*/
	StandardBWFHead *standardBWFHeader = new StandardBWFHead();
	RIFFChunk *riffChunk = new RIFFChunk();
	StandardPCMFmtChunk *standardPCMFmtChunk = new StandardPCMFmtChunk();
	//BextChunk* bextChunk = new BextChunk();
	JunkChunk *junkChunk = new JunkChunk();
	DataChunk *dataChunk = new DataChunk();

	std::cout << "IWavHeader: " << sizeof(*wavHeader) << std::endl;
	std::cout << "StandardBWFHeader: " << sizeof(*standardBWFHeader) << std::endl;
	std::cout << "RIFFChunk: " << sizeof(*riffChunk) << std::endl;
	std::cout << "StandardPCMFmtChunk: " << sizeof(*standardPCMFmtChunk) << std::endl;
	std::cout << "BextChunk: " << sizeof(*new BextChunk()) << std::endl;
	std::cout << "JunkChunk: " << sizeof(*junkChunk) << std::endl;
	std::cout << "DataChunk: " << sizeof(*dataChunk) << std::endl;
}

std::string WavParserHelper::GetTestFilePath(TestFileType FileType)
{
	std::string BasePath = "../../Resource/";
	std::string FileName;
	std::string FilePath;
	switch (FileType)
	{
	case TestFileType::Uint8Stereo:
		FileName = "M1F1-uint8-AFsp.wav";
		break;
	case TestFileType::Uint8ALawStereo:
		FileName = "M1F1-Alaw-AFsp.wav";
		break;
	case TestFileType::UInt8MuLawStereo:
		FileName = "M1F1-mulaw-AFsp.wav";
		break;
	case TestFileType::Int12Stereo:
		FileName = "M1F1-int12-AFsp.wav";
		break;
	case TestFileType::Int16Stereo:
		FileName = "M1F1-int16-AFsp.wav";
		break;
	case TestFileType::Int24Stereo:
		FileName = "M1F1-int24-AFsp.wav";
		break;
	case TestFileType::Int32Stereo:
		FileName = "M1F1-int32-AFsp.wav";
		break;
	case TestFileType::Float32Stereo:
		FileName = "M1F1-float32-AFsp.wav";
		break;
	case TestFileType::Float64Stereo:
		FileName = "M1F1-float64-AFsp.wav";
		break;
	case TestFileType::Uint8StereoExt:
		FileName = "M1F1-uint8WE-AFsp.wav";
		break;
	case TestFileType::UInt8ALawStereoExt:
		FileName = "M1F1-AlawWE-AFsp.wav";
		break;
	case TestFileType::UInt8MuLawStereoExt:
		FileName = "M1F1-mulawWE-AFsp.wav";
		break;
	case TestFileType::Int12StereoExt:
		FileName = "M1F1-int12WE-AFsp.wav";
		break;
	case TestFileType::Int16StereoExt:
		FileName = "M1F1-int16WE-AFsp.wav";
		break;
	case TestFileType::Int24StereoExt:
		FileName = "M1F1-int24WE-AFsp.wav";
		break;
	case TestFileType::Int32StereoExt:
		FileName = "M1F1-int32WE-AFsp.wav";
		break;
	case TestFileType::Float32StereoExt:
		FileName = "M1F1-float32WE-AFsp.wav";
		break;
	case TestFileType::Float64StereoExt:
		FileName = "M1F1-float64WE-AFsp.wav";
		break;
	case TestFileType::Int16Channel_6:
		FileName = "6_Channel_ID.wav";
		break;
	case TestFileType::Int24Channel_8:
		FileName = "8_Channel_ID.wav";
		break;
	case TestFileType::Int16Bwf:
		FileName = "M1F1-Bwf-16bit.wav";
		break;
	case TestFileType::Int24RF64:
		FileName = "M1F1-Bwf-RF64-24bit.wav";
		break;
	default:
		FileName = "Invalid";
	}

	FilePath = BasePath + FileName;

	return FilePath;
}

std::string WavParserHelper::GetAudioFormatTag(unsigned short wFormatTag)
{
	std::string FormatTag;
	switch (wFormatTag)
	{
	case 0x0000:
		FormatTag = "WAVE_FORMAT_UNKNOWN";
		break;
	case 0x0001:
		FormatTag = "WAVE_FORMAT_PCM";
		break;
	case 0x0002:
		FormatTag = "WAVE_FORMAT_ADPCM";
		break;
	case 0x0003:
		FormatTag = "WAVE_FORMAT_IEEE_FLOAT";
		break;
	case 0x0006:
		FormatTag = "WAVE_FORMAT_ALAW";
		break;
	case 0x0007:
		FormatTag = "WAVE_FORMAT_MULAW";
		break;
	case 0xFFFE:
		FormatTag = "WAVE_FORMAT_EXTENSIBLE";
		break;
	case 0xFFFF:
		FormatTag = "WAVE_FORMAT_DEVELOPMENT";
		break;
	default:
		FormatTag = "WAVE_FORMAT_UNKNOWN";
	}

	return FormatTag;
}
