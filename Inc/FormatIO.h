#pragma once

#include "Common.h"
#include "ColorFormats.h"

namespace TexIO
{
	enum CompressionType
	{
		None = 0,
		RLE = 1,
		LZW = 2,
	};

	enum FileType
	{
		Unknown,
		TGA,
		PNG,
		HDR,
	};

	// TODO: seperate Format into on-disk and on-device formats for clarity ?
	struct ImageDesc
	{
		uint Width = 1;
		uint Height = 1;
		ColorFormat PixelFormat = ColorFormat::UNKNOWN;
		byte PixelSize = 0;
		// Number of channels actually containing data, regardless of actual format
		byte NumChannels = 0;
		// Indicates Alpha channel contains usefull data
		bool HasAlphaChannel = false;
		byte MipLevels = 0;
		FileType  FileFormat = Unknown;
		CompressionType Compression = None;
		string Signature = "Created using TexIO";

		void Update()
		{
			switch(NumChannels)
			{
				case 1:
				{
					PixelSize = 1;
					PixelFormat = R8_UINT;
					HasAlphaChannel = false;
					break;
				}
				case 3:
				{
					PixelSize = 4;
					PixelFormat = R8G8B8A8_UINT;
					HasAlphaChannel = false;
					break;
				}
				case 4:
				{
					PixelSize = 4;
					PixelFormat = R8G8B8A8_UINT;
					HasAlphaChannel = true;
					break;
				}
			}
		}
	};

	class FormatIO
	{
	  public:
		virtual ImageDesc ReadHeader(istream istream, bool keepOpen = true) = 0;
		virtual uint ReadData(istream istream, void*& pData, uint pixelCount) = 0;
		virtual bool WriteHeader(ostream istream, ImageDesc ) = 0;
		virtual uint WriteData(ostream istream, void*& pData,uint pixelCount) = 0;
	};
}