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

	struct ImageDesc
	{
		uint Width = 1;
		uint Height = 1;
		ColorFormat PixelFormat = ColorFormat::UNKNOWN;
		byte PixelSize = 0;
		byte NumChannels = 0;
		bool HasAlphaChannel = false;
		FileType  FileFormat = Unknown;
		CompressionType Compression = None;
		string Signature = "Created using TexIO";

		void Update()
		{
			switch(PixelFormat)
			{
				case ColorFormat::R8_UINT:
				{
					PixelSize = 1;
					NumChannels = 1;
					HasAlphaChannel = false;
					break;
				}
				case ColorFormat::R8G8B8_UINT:
				{
					PixelSize = 3;
					NumChannels = 3;
					HasAlphaChannel = false;
					break;
				}
				case ColorFormat::R8G8B8A8_UINT:
				{
					PixelSize = 4;
					NumChannels = 4;
					HasAlphaChannel = true;
					break;
				}
				case ColorFormat::R16G16B16_UINT:
				{
					PixelSize = 6;
					NumChannels = 3;
					HasAlphaChannel = false;
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