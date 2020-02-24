#include <fstream>
#include <algorithm>
#include <assert.h>

#include "WavParser.h"
#include "WavLog.h"
#include "WavUtility.h"

//=================================================================================================

void WavParser::Initialize(std::ifstream& in_File)
{
	assert(in_File);

	m_HeaderTypeGroup = WavParserHelper::GetWavHeaderType(in_File);

	m_DataType = WavParserHelper::GetWavDataType(in_File);

	if (m_HeaderTypeGroup.MajorType == WavHeaderType::Invalid || m_DataType == WavDataType::Invalid)
		return;

	m_File.reset(new WavFile());

	switch(m_HeaderTypeGroup.MajorType)
	{
	case WavHeaderType::Standard:
		m_File->Header = new StandardWavHead();
		break;
	case WavHeaderType::NonPCM:
		if (m_HeaderTypeGroup.SubType == WavHeaderSubType::Normal)
			m_File->Header = new NonPcmHead();
		//else if (m_HeaderTypeGroup.SubType == WavHeaderSubType::Extensible)
		//	m_File->Header = new ExtensibleNonPCMHead();
		break;
	case WavHeaderType::Extensible:
		if (m_HeaderTypeGroup.SubType == WavHeaderSubType::Normal)
			m_File->Header = new ExtensibleWavHead();
		else if (m_HeaderTypeGroup.SubType == WavHeaderSubType::NoFact)
			m_File->Header = new ExtensibleNoFactHead();
		break;
	case WavHeaderType::Bwf:
		if (m_HeaderTypeGroup.SubType == WavHeaderSubType::Normal)
			m_File->Header = new StandardBWFHead();
		else if (m_HeaderTypeGroup.SubType == WavHeaderSubType::Extensible)
			m_File->Header = new ExtensibleBWFHead();
		else if (m_HeaderTypeGroup.SubType == WavHeaderSubType::RF64)
			m_File->Header = new RF64Header();
		break;
	default:
		m_File->Header = nullptr;
	}

	return;
}

//=======================================================================================================================

std::string InitPath = "Init Path";
std::ifstream InitStream = std::ifstream(InitPath, std::ios_base::binary);

//========================================================================================================================
WavParser::WavParser() : m_File(nullptr),m_DataType(WavDataType::Invalid), 
m_Path(InitPath), m_Stream(InitStream)
{

}

bool WavParser::LoadFile(const std::string& path)
{
	if (path.empty())
		return false;

	m_Path = path;

	m_Stream = std::ifstream(m_Path, std::ios_base::binary);

	if (m_Stream)
	{
		Initialize(m_Stream);
		return true;
	}
	else
	{
		return false;;
	}
}

void WavParser::Parse()
{
	m_Stream.seekg(0, std::ios_base::end);
	long long fileSize = m_Stream.tellg();
	long long rawDataSize = 0;
	size_t IWavHeaderSize = sizeof(IWavHeader);

#ifdef DEBUG
	std::cout << "FileSize: " << fileSize << std::endl;
#endif // DEBUG

	//WavHeaderType type = m_WavData->WavHeader->type;
	m_Stream.seekg(0, std::ios_base::beg);

	std::streampos position = 0;

	//read header data for different header type
	switch (m_HeaderTypeGroup.MajorType)
	{
	case WavHeaderType::Standard:
		//IWavHeader is 16 bytes, so we start at begin + 16
		m_Stream.read(reinterpret_cast<char*>(m_File->Header) + IWavHeaderSize, sizeof(StandardWavHead) - IWavHeaderSize);
		rawDataSize = fileSize - (sizeof(StandardWavHead) - IWavHeaderSize);
#ifdef DEBUG
		std::cout << "StandardPCM HeadSize: " << sizeof(StandardWavHead) << std::endl
			<< "RawDataSize(fileSize - headerSize): " << rawDataSize << std::endl 
			<< "Pointer(finish read header): " << m_Stream.tellg() << std::endl;
#endif // DEBUG
		break;

	case WavHeaderType::Bwf:
		if (m_HeaderTypeGroup.SubType == WavHeaderSubType::Normal)
		{
			//IWavHeader is 16 bytes, so we start at begin + 16
			m_Stream.read(reinterpret_cast<char*>(m_File->Header) + IWavHeaderSize,
				sizeof(RIFFChunk) + sizeof(StandardPCMFmtChunk));

			position = m_Stream.tellg();

			m_Stream.seekg(sizeof(RIFFChunk) + sizeof(StandardPCMFmtChunk));

			//read first part of bext chunk to avoid align problem
			m_Stream.read(reinterpret_cast<char*>(m_File->Header) + IWavHeaderSize + position, 346);

			position = m_Stream.tellg();

			m_Stream.seekg(sizeof(RIFFChunk) + sizeof(StandardPCMFmtChunk) + 346);

			//read second part of bext chunk and first part of junk chunk to avoid align problem
			m_Stream.read(reinterpret_cast<char*>(m_File->Header) + IWavHeaderSize + position + 2, 264 + 82);

			position = m_Stream.tellg();

			//align byte
			m_Stream.seekg(-2, std::ios_base::cur);
			//read data chunk
			m_Stream.read(reinterpret_cast<char*>(m_File->Header) + IWavHeaderSize + position + 2,
				sizeof(DataChunk));

			rawDataSize = fileSize - (sizeof(StandardBWFHead) - IWavHeaderSize);
#ifdef DEBUG
			std::cout << " StandardBWF HeadSize: " << sizeof(StandardBWFHead) << std::endl
				<< "RawDataSize(fileSize - headerSize): " << rawDataSize << std::endl
				<< "Pointer(finish read header): " << m_Stream.tellg() << std::endl;
#endif // DEBUG
		}
		else if (m_HeaderTypeGroup.SubType == WavHeaderSubType::RF64)
		{
 			m_Stream.read(reinterpret_cast<char*>(m_File->Header) + IWavHeaderSize,
				sizeof(RF64Chunk) + sizeof(DS64Chunk));

			m_Stream.seekg(-4, std::ios_base::cur);
			long TableLength = 0;
			m_Stream.read(reinterpret_cast<char*>(&TableLength), sizeof(TableLength));
			m_Stream.seekg((std::streampos)TableLength, std::ios_base::cur);

			position = m_Stream.tellg();

			m_Stream.read(reinterpret_cast<char*>(m_File->Header) + IWavHeaderSize + position,
				sizeof(ExtensibleFmtChunk) + sizeof(BextChunk));

			PadChunk PAD;
			m_Stream.read(reinterpret_cast<char*>(&PAD), sizeof(PadChunk));
			if (!strncmp(PAD.ckID, "PAD", 4));
			{
				//unsigned long ckSize = 0;
				//m_Stream.read(reinterpret_cast<char*>(&ckSize), sizeof(ckSize));

				m_Stream.seekg((std::streampos)PAD.ckSize, std::ios_base::cur);

			}

			position = m_Stream.tellg();

			m_Stream.read(reinterpret_cast<char*>(m_File->Header) + IWavHeaderSize + position, 
				sizeof(DataChunk));



#ifdef DEBUG
			std::cout << " RF64 HeadSize: " << sizeof(RF64Header) << std::endl
				<< "RawDataSize(fileSize - headerSize): " << rawDataSize << std::endl
				<< "Pointer(finish read header): " << m_Stream.tellg() << std::endl;
#endif // DEBUG
		}
		
		break;

	case WavHeaderType::NonPCM:
		if (m_HeaderTypeGroup.SubType == WavHeaderSubType::Normal)
		{
			//IWavHeader is 16 bytes, so we start at begin + 16
			m_Stream.read(reinterpret_cast<char*>(m_File->Header) + IWavHeaderSize, sizeof(NonPcmHead) - IWavHeaderSize);
			rawDataSize = fileSize - (sizeof(NonPcmHead) - IWavHeaderSize);
		}

#ifdef DEBUG
		std::cout << "NonPCM HeadSize: " << sizeof(NonPcmHead) << std::endl 
			<< "RawDataSize(fileSize - headerSize): " << rawDataSize << std::endl 
			<< "Pointer(finish read header): " << m_Stream.tellg() << std::endl;
#endif // DEBUG
		break;

	case WavHeaderType::Extensible:
		//IWavHeader is 16 bytes, so we start at begin + 16
		if (m_HeaderTypeGroup.SubType == WavHeaderSubType::Normal)
		{
			m_Stream.read(reinterpret_cast<char*>(m_File->Header) + IWavHeaderSize, sizeof(ExtensibleWavHead) - IWavHeaderSize);
			rawDataSize = fileSize - (sizeof(ExtensibleWavHead) - IWavHeaderSize);
		}
		else if (m_HeaderTypeGroup.SubType == WavHeaderSubType::NoFact)
		{
			m_Stream.read(reinterpret_cast<char*>(m_File->Header) + IWavHeaderSize, sizeof(ExtensibleNoFactHead) - IWavHeaderSize);
			rawDataSize = fileSize - (sizeof(ExtensibleNoFactHead) - IWavHeaderSize);
		}
#ifdef DEBUG
		std::cout << "Extensible HeaderSize: " << sizeof(ExtensibleWavHead) << std::endl 
			<< "RawDataSize(fileSize - headerSize): " << rawDataSize << std::endl 
			<< "Pointer(finish read header): " << m_Stream.tellg() << std::endl;
#endif // DEBUG
		break;
	}
	

	WavParserHelper::PrintWavHeader(m_File->Header, m_HeaderTypeGroup.SubType);

//=================================Copy Data================================================
	auto pRawData = std::vector<unsigned char>(rawDataSize);
	m_Stream.read(reinterpret_cast<char*>(pRawData.data()), rawDataSize);

	m_File->WavData.resize(rawDataSize);
	std::move(pRawData.begin(), pRawData.end(), m_File->WavData.begin());
#ifdef DEBUG
	std::cout << "Pointer(finish read data): " << m_Stream.tellg() << std::endl
		<< "RawDataSize(rawData.size()): " << m_File->WavData.size() << std::endl;
	std::cout << std::endl;
#endif // DEBUG

	
	WavParserHelper::PrintDataType(m_DataType);

	return;
	
//	switch (m_DataType)
//	{
//	case WavDataType::WavData8bit:
//
//		m_File->WavData8bit.resize(rawDataSize);
//		std::move(pRawData.begin(), pRawData.end(), m_File->WavData8bit.begin());
//
//#ifdef DEBUG
//		std::cout << "Pointer(finish read data): " << m_Stream.tellg() << std::endl
//			<< "RawDataSize(rawData.size()): " << m_File->WavData8bit.size() << std::endl;
//		std::cout << std::endl;
//#endif // DEBUG
//		break;

//	case WavDataType::WavData12bit:
//		auto pRawData = std::vector<int12>(rawDataSize);
//		m_Stream.read(reinterpret_cast<int12*>(pRawData.data()), rawDataSize);
//		m_File->WavData12bit.resize(rawDataSize);
//		std::move(pRawData.begin(), pRawData.end(), m_File->WavData12bit.begin());
//
//#ifdef DEBUG
//		std::cout << "Pointer(finish read data): " << m_Stream.tellg() << std::endl
//			<< "RawDataSize(rawData.size()): " << m_File->WavData12bit.size() << std::endl;
//		std::cout << std::endl;
//#endif // DEBUG
//		break;
////
//	case WavDataType::WavData16bit:
//		m_File->WavData16bit.resize(rawDataSize);
//		std::move(pRawData.begin(), pRawData.end(), m_File->WavData16bit.begin());
//
//#ifdef DEBUG
//		std::cout << "Pointer(finish read data): " << m_Stream.tellg() << std::endl
//			<< "RawDataSize(rawData.size()): " << m_File->WavData16bit.size() << std::endl;
//		std::cout << std::endl;
//#endif // DEBUG
//		break;
//
//	case WavDataType::WavData24bit:
//		m_File->WavData24bit.resize(rawDataSize);
//		std::move(pRawData.begin(), pRawData.end(), m_File->WavData24bit.begin());
//
//#ifdef DEBUG
//		std::cout << "Pointer(finish read data): " << m_Stream.tellg() << std::endl
//			<< "RawDataSize(rawData.size()): " << m_File->WavData24bit.size() << std::endl;
//		std::cout << std::endl;
//#endif // DEBUG
//		break;
//
//	case WavDataType::WavData32bit:
//		m_File->WavData32bit.resize(rawDataSize);
//		std::move(pRawData.begin(), pRawData.end(), m_File->WavData32bit.begin());
//
//#ifdef DEBUG
//		std::cout << "Pointer(finish read data): " << m_Stream.tellg() << std::endl
//			<< "RawDataSize(rawData.size()): " << m_File->WavData32bit.size() << std::endl;
//		std::cout << std::endl;
//#endif // DEBUG
//		break;
//
//	case WavDataType::WavData64bit:
//		m_File->WavData64bit.resize(rawDataSize);
//		std::move(pRawData.begin(), pRawData.end(), m_File->WavData64bit.begin());
//
//#ifdef DEBUG
//		std::cout << "Pointer(finish read data): " << m_Stream.tellg() << std::endl
//			<< "RawDataSize(rawData.size()): " << m_File->WavData64bit.size() << std::endl;
//		std::cout << std::endl;
//#endif // DEBUG
//		break;

	//default:
	//	return;
	//}

}