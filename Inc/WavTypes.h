#pragma once
#pragma pack(1)

#include <memory>
#include <vector>

enum class WavHeaderType
{
	Invalid,
	Standard,
	NonPCM,
	Extensible,
	Bwf,
};

enum class WavHeaderSubType
{
	Normal,
	Extensible,
	NoFact,
	RF64
};

struct WavHeaderTypeGroup
{
	WavHeaderType MajorType;
	WavHeaderSubType SubType;
};

enum class WavDataType
{
	Invalid,
	//usigned int8
	WavData8bit,
	//int12
	WavData12bit,
	//int16
	WavData16bit,
	//int24
	WavData24bit,
	//float32
	WavData32bit,
	//float64
	WavData64bit
};
//===========================Basic Chunk==================================
struct RIFFChunk
{
	//Chunk ID : RIFF
	char ckID[4];
	//RIFF Chunk Size
	unsigned long ckSize;
	//Wav ID: Wav
	char WavID[4];
};

struct FactChunk
{
	//Chunk ID: fact
	char ckID[4];
	//factChunk Size
	unsigned long ckSize;
	//Number of samples (per channel)
	unsigned long dwSampleLength;
};

//================================FMT Chunk=================================
struct StandardPCMFmtChunk
{
	//Chunk ID: fmt
	char ckID[4];
	//fmt Chunk Size
	unsigned long ckSize;
	//Wav_FORMAT_PCM
	unsigned short wFormatTag;
	//Number of channels 1=Mono 2=Sterio
	unsigned short nChannels;
	//Sampling Frequency in Hz
	unsigned long nSamplesPerSec;
	//bytes per second
	unsigned long nAvgBytesPerSec;
	//2=16-bit mono, 4=16-bit stereo
	unsigned short nBlockAlign;
	//Number of bits per sample
	unsigned short wBitsPerSample;
};

struct NonPcmFmtChunk
{
	StandardPCMFmtChunk standardFmtChunk;
	//Size of the extension
	unsigned short cbSize;
};

struct ExtensibleFmtChunk
{
	StandardPCMFmtChunk standardFmtChunk;
	//Size of the extension
	unsigned short cbSize;
	//at most 8*M
	unsigned short wValidBitsPerSample;
	//Speaker position mask
	unsigned long dwChannelMask;
	//GUID (first two bytes are the data format code)
	char SubFormat[16];
};

struct DataChunk
{
	//Chunk ID: data
	char ckID[4];
	//Chunk size: n
	unsigned long ckSize;
};

//==============================Extensional Chunk===========================
struct BextChunk
{
	//bext
	char ckID[4];
	unsigned long ckSize;
	char Description[256];
	char Originator[32];
	char OriginatorReference[32];
	char OriginationDate[10];
	char OriginationTime[8];
	//align byte
	unsigned short Align = 0;
	unsigned long TimeReferenceLow;
	unsigned long TimeReferenceHigh;
	unsigned short Version;
	char UMID[64];
	short LoudnessValue;
	short LoudnessRange;
	short MaxTruePeakLevel;
	short MaxMomentaryLoudness;
	short MaxShortTermLoudness;
	char Reserved[180];
};

struct JunkChunk
{
	//junk
	char ckID[4];
	unsigned long ckSize;
	char initialData[74];
};

struct RF64Chunk
{
	// Chunk ID = "RF64"
	char ckID[4];
	// -1 = 0xFFFFFFFF means dont use this data, use
	// riffSizeHigh and riffSizeLow in ds64 chunk instead
	unsigned long ckSize;
	// Type ID = "WAVE"
	char rf64Type[4];
};

struct CS64Chunk
{
	char ckID[4];
	unsigned long ckSizeLow;
	unsigned long ckSizeHigh;
};

struct DS64Chunk
{
	// 'DS64'
	char ckID[4];
	unsigned long ckSize;
	unsigned long RIFFSizeLow;
	unsigned long RIFFSizeHigh;
	unsigned long DataSizeLow;
	unsigned long DataSizeHigh;
	unsigned long SampleCountLow;
	unsigned long SampleCountHigh;
	unsigned long TableLength;
};

struct PadChunk
{
	char ckID[4];
	unsigned long ckSize;
};

//==============================Wav Header=================================================
struct IWavHeader
{
	IWavHeader(WavHeaderType type) : type(type) {}
	virtual ~IWavHeader() {}

	WavHeaderType type;
};
//RAII for IWavHeader
//typedef std::shared_ptr<IWavHeader> p_IWavHeader;

struct StandardWavHead : public IWavHeader
{
	StandardWavHead() : IWavHeader(WavHeaderType::Standard) {}

	RIFFChunk RIFFChunk;
	StandardPCMFmtChunk fmtChunk;
	DataChunk DataChunk;
};

struct NonPcmHead : public IWavHeader
{
	NonPcmHead() : IWavHeader(WavHeaderType::NonPCM) {}

	RIFFChunk RIFFChunk;
	NonPcmFmtChunk fmtChunk;
	FactChunk FactChunk;
	DataChunk DataChunk;
};

struct ExtensibleWavHead : public IWavHeader
{
	ExtensibleWavHead() : IWavHeader(WavHeaderType::Extensible) {}

	RIFFChunk RIFFChunk;
	ExtensibleFmtChunk fmtChunk;
	FactChunk FactChunk;
	DataChunk DataChunk;
};

//struct ExtensibleNonPCMHead : public IWavHeader
//{
//	ExtensibleNonPCMHead() : IWavHeader(WavHeaderType::NonPCM) {}
//
//	RIFFChunk RIFFChunk;
//	ExtensibleFmtChunk fmtChunk;
//	FactChunk FactChunk;
//	DataChunk DataChunk;
//};

struct ExtensibleNoFactHead : public IWavHeader
{
	ExtensibleNoFactHead() : IWavHeader(WavHeaderType::Extensible) {}

	RIFFChunk RIFFChunk;
	ExtensibleFmtChunk fmtChunk;
	DataChunk DataChunk;
};

struct StandardBWFHead : public IWavHeader
{
	StandardBWFHead() : IWavHeader(WavHeaderType::Bwf) {}

	RIFFChunk RIFFChunk;
	StandardPCMFmtChunk fmtChunk;
	BextChunk BextChunk;
	JunkChunk JunkChunk;
	DataChunk DataChunk;
};

struct RF64Header : public IWavHeader
{
	RF64Header() : IWavHeader(WavHeaderType::Bwf) {}

	RF64Chunk RF64Chunk;
	DS64Chunk DS64Chunk;
	//std::vector<CS64Chunk> TableChunk;
	ExtensibleFmtChunk fmtChunk;
	BextChunk BextChunk;
	//JunkChunk JunkChunk;
	DataChunk DataChunk;
};

struct ExtensibleBWFHead : public IWavHeader
{
	ExtensibleBWFHead() : IWavHeader(WavHeaderType::Bwf) {}

	RIFFChunk RIFFChunk;
	ExtensibleFmtChunk fmtChunk;
	FactChunk FactChunk;
	BextChunk BextChunk;
	JunkChunk JunkChunk;
	DataChunk DataChunk;
};

//=================================Header + Data==============================================
struct int12
{
	unsigned uint12 : 12;
};

struct int24
{
	unsigned uint24 : 24;
};

struct WavFile
{
	WavFile() : Header(nullptr) {}

	//Header
	IWavHeader *Header;

	//Only Use One Type
	std::vector<unsigned char> WavData;
};
//RAII for IWavParser
typedef std::shared_ptr<WavFile> p_WavFile;

//================================================================================
/* Windows WAVE File Encoding Tags */
#define WAVE_FORMAT_UNKNOWN 0x0000	 /* Unknown Format */
#define WAVE_FORMAT_PCM 0x0001		   /* PCM */
#define WAVE_FORMAT_ADPCM 0x0002	   /* Microsoft ADPCM Format */
#define WAVE_FORMAT_IEEE_FLOAT 0x0003  /* IEEE Float */
#define WAVE_FORMAT_ALAW 0x0006		   /* ALAW */
#define WAVE_FORMAT_MULAW 0x0007	   /* MULAW */
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE  /* SubFormat */
#define WAVE_FORMAT_DEVELOPMENT 0xFFFF /* Development */

//================================================================================
enum class TestFileType
{
	Uint8Stereo,
	Uint8ALawStereo,
	UInt8MuLawStereo,
	Int12Stereo,
	Int16Stereo,
	Int24Stereo,
	Int32Stereo,
	Float32Stereo,
	Float64Stereo,
	Uint8StereoExt,
	UInt8ALawStereoExt,
	UInt8MuLawStereoExt,
	Int12StereoExt,
	Int16StereoExt,
	Int24StereoExt,
	Int32StereoExt,
	Float32StereoExt,
	Float64StereoExt,
	Int16Channel_6,
	Int24Channel_8,
	Int16Bwf,
	Int24RF64
};
