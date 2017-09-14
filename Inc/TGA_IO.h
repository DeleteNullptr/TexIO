#pragma once

#include "Common.h"
#include "FormatIO.h"
#include <array>
#include <cstring>
#include <iostream>

#pragma pack (push,1)

namespace TexIO
{
	class TGA_IO : public FormatIO
	{
	  public:
		// Only True color is supported
		enum class ColorType : byte
		{
			None = 0,
			Map,
			True,
			Mono,
		};

		enum class PixelDepth : byte
		{
			R8 = 8,
			R5G5B5A1 = 16,
			R8G8B8 = 24,
			R8G8B8A8 = 32,
		};

		enum class ImageType : byte
		{
			None = 0,
			ColorMap,
			TrueColor,
			MonoChrome,
			ColorMapEnc = 9,
			TrueColorEnc,
			MonoChromeEnc,
		};

		struct TargaHeader
		{
			byte IDLength = 0;                        /* 00h  Size of Image ID field */
			byte ColorMapType = 0;                    /* 01h  Color map type */
			ImageType ImageType = ImageType::None;    /* 02h  Image type code */
			short CMapStart = 0;                     /* 03h  Color map origin */
			short CMapLength = 0;                    /* 05h  Color map length */
			byte CMapDepth = 0;                       /* 07h  Depth of color map entries */
			short XOffset = 0;                       /* 08h  X origin of image */
			short YOffset = 0;                       /* 0Ah  Y origin of image */
			ushort Width = 0;                         /* 0Ch  Width of image */
			ushort Height = 0;                        /* 0Eh  Height of image */
			PixelDepth PixelSize = PixelDepth::R8G8B8;/* 10h  Image pixel size */
			byte ImageDescriptor = 0;                 /* 11h  Image descriptor byte */
		};

		// Functions

		static bool CheckDesc(ImageDesc& desc)
		{
			if(desc.Compression != CompressionType::None) return false;
			switch(desc.PixelFormat)
			{
				case ColorFormat::R8G8B8_UINT:
					break;
				case ColorFormat::R8G8B8A8_UINT:
					break;
				default:
					return false;
			}

			return true;
		}

		static bool ReadHeader(istream& stream, ImageDesc& desc)
		{
			TargaHeader tgaInfo;
			stream.readsome(reinterpret_cast<char*>(&tgaInfo),sizeof(TargaHeader));

			desc.FileFormat = FileType::TGA;
			desc.Width = (uint)tgaInfo.Width;
			desc.Height = (uint)tgaInfo.Height;
			switch(tgaInfo.PixelSize)
			{
				case PixelDepth::R8G8B8:
					desc.PixelFormat = ColorFormat::R8G8B8A8_UINT;
					desc.NumChannels = 3;
					break;
				case PixelDepth::R8G8B8A8:
					desc.PixelFormat = ColorFormat::R8G8B8A8_UINT;
					desc.NumChannels = 4;
					break;
				case PixelDepth::R5G5B5A1:
					return false;
				default:
					break;
			}
			desc.Compression = (tgaInfo.ImageType == ImageType::ColorMapEnc || tgaInfo.ImageType == ImageType::MonoChromeEnc ||
					tgaInfo.ImageType == ImageType::TrueColorEnc) ? CompressionType ::RLE : CompressionType::None;
			//if(desc.Compression != CompressionType::None) return false;
			desc.Signature.resize(tgaInfo.IDLength);
			stream.readsome(&desc.Signature[0], tgaInfo.IDLength);

			return true;
		}

		static uint ReadData(istream& stream, const ImageDesc& desc, byte* pData, uint pixelOffset, uint pixelCount)
		{
			uint pixelsRead = 0;
			uint maxPixelsRead = min(pixelCount, (desc.Width * desc.Height) - pixelOffset);
			byte rawPixel[4];
			uint byteOffset = pixelOffset * desc.PixelSize;

			if(desc.Compression == CompressionType::RLE)
			{
				pixelsRead = DecodePixels(stream, desc, pData, pixelOffset, maxPixelsRead);
			} else {
				// R5G5B5A1
				if(desc.PixelSize == 2) {
						while(pixelsRead < min(pixelCount, maxPixelsRead)) {
							ushort raw;
							pixelsRead += stream.readsome(reinterpret_cast<char *>(&raw), 2);
							pData[byteOffset] = (byte)((raw & 0b0000000000011111) * 255 / 63);
							pData[byteOffset + 1] = (byte)(((raw & 0b0000001111100000) >> 5) * 255 / 63);
							pData[byteOffset + 2] = (byte)(((raw & 0b0111110000000000) >> 10) * 255 / 63);
							pData[byteOffset + 3] = (byte)(((raw & 0b1000000000000000) >> 15) * 255);
							byteOffset += 4;
						}
				} else {
					switch(desc.NumChannels)
					{
						case 3:
						{
							for (; pixelsRead < maxPixelsRead; ++pixelsRead) {
								stream.read(reinterpret_cast<char *>(rawPixel), 3);
								pData[byteOffset++] = rawPixel[2];
								pData[byteOffset++] = rawPixel[1];
								pData[byteOffset++] = rawPixel[0];
								// Skip the Alpha byte to pad to 32-bit
								byteOffset++;
							}
							break;
						}
						case 4:
						{
							for (; pixelsRead < maxPixelsRead; ++pixelsRead)
							{
								stream.read(reinterpret_cast<char *>(rawPixel), desc.PixelSize);
								pData[byteOffset++] = rawPixel[2];
								pData[byteOffset++] = rawPixel[1];
								pData[byteOffset++] = rawPixel[0];
								pData[byteOffset++] = rawPixel[3];
							}
							break;
						}
						default:
							break;
					}
				}
			}
			return pixelsRead;
		}

		static bool WriteHeader(ostream& stream, ImageDesc& desc)
		{
			if(!CheckDesc(desc)) return false;

			TargaHeader header = TargaHeader();
			header.IDLength = 255 < desc.Signature.length() ? (byte) 255 : (byte) desc.Signature.length();

			if(desc.Width <= USHRT_MAX) header.Width = (ushort)desc.Width;
			else cerr << "Image too wide for TGA (max 2^8): " << desc.Width << endl;

			if(desc.Height <= USHRT_MAX) header.Height = (ushort)desc.Height;
			else cerr << "Image too high for TGA (max 2^8): " << desc.Height << endl;

			if (desc.Compression == CompressionType::RLE) {
				header.ImageType = ImageType::TrueColorEnc;
			} else {
				header.ImageType = ImageType::TrueColor;
			}

			header.PixelSize = (PixelDepth)(desc.NumChannels * 8);

			stream.write(reinterpret_cast<char*>(&header), sizeof(TargaHeader));
			stream.write(desc.Signature.c_str(), header.IDLength);

			return true;
		}

		static uint WriteData(ostream& stream, const ImageDesc& desc, byte* pData, uint& pixelOffset, uint pixelCount)
		{
			uint pixelsWritten = 0;
			const uint maxPixelsWritten =  min(pixelCount,(desc.Width * desc.Height) - pixelOffset);
			uint byteOffset = pixelOffset * desc.PixelSize;

			if(desc.Compression == CompressionType::RLE)
			{
				pixelsWritten = EncodePixels(stream, desc, pData, pixelOffset, pixelsWritten);
			} else {
				uint pixelByteOffset {byteOffset};

				switch(desc.NumChannels)
				{
					case 3: {
						for (; pixelsWritten < maxPixelsWritten; ++pixelsWritten)
						{
							stream.write(reinterpret_cast<char *>(pData + pixelByteOffset + 2), 1);
							stream.write(reinterpret_cast<char *>(pData + pixelByteOffset + 1), 1);
							stream.write(reinterpret_cast<char *>(pData + pixelByteOffset), 1);
							pixelByteOffset += desc.PixelSize;
						}
						break;
					}
					case 4:
					{
						for (; pixelsWritten < maxPixelsWritten; ++pixelsWritten)
						{
							stream.write(reinterpret_cast<char *>(pData + pixelByteOffset + 2), 1);
							stream.write(reinterpret_cast<char *>(pData + pixelByteOffset + 1), 1);
							stream.write(reinterpret_cast<char *>(pData + pixelByteOffset), 1);
							stream.write(reinterpret_cast<char *>(pData + pixelByteOffset + 3), 1);
							pixelByteOffset += desc.PixelSize;
						}
						break;
					}
					default:
						break;
				}
			}
			pixelOffset += pixelsWritten;

			return pixelsWritten;
		}

	  private:
		static const byte RLE_packetMask = 128;
		static const byte packetPixelMask = 127;

		template<uint8_t numbytes>
		struct RLE_Packet
		{
		  public:
			byte GetNumPixels() { return (byte) (mNumPixels & packetPixelMask); };
			void SetNumPixels(byte numPixels) { mNumPixels = (byte) ((numPixels - 1) | RLE_packetMask); };
			array<byte, numbytes> mData {};
		  private:
			byte mNumPixels = 255;
		};

		struct Raw_PacketHeader
		{
		  public:
			byte GetNumPixels() { return (byte) (mNumPixels & packetPixelMask); };
			void SetNumPixels(byte numPixels) { mNumPixels = (byte) ((numPixels - 1) & packetPixelMask); };
		  private:
			byte mNumPixels = 0;
		};

		static void FixSwizzleBGR(byte* pDest, byte* pSource)
		{
			pDest[0] = pSource[2];
			pDest[1] = pSource[1];
			pDest[2] = pSource[0];
		};

		static void FixSwizzleBGRA(byte* pDest, byte* pSource)
		{
			pDest[0] = pSource[2];
			pDest[1] = pSource[1];
			pDest[2] = pSource[0];
			pDest[3] = pSource[3];
		};

		/*
		static void EncodeR5G5B5A1(const byte* pData, uint& byteOffset, ostream& stream)
		{
			ushort data {0};
			data |= (pData[byteOffset] * 63 / 255);
			data |= (pData[++byteOffset] * 63 / 255) << 5;
			data |= (pData[++byteOffset] * 63 / 255) << 10;
			data |= (pData[++byteOffset] * 63 / 255) << 15;
			stream.write(reinterpret_cast<char *>(&data), 2);
		}
		*/

		static uint
		EncodePixels(ostream& stream, const ImageDesc &desc, byte* pData, uint& pixelOffset, uint& pixelCount)
		{
			byte counterEqual(0);
			byte counterNEqual(0);
			streamsize packageHeaderPos;
			uint i = 0;
			auto pixelSize = desc.PixelSize;
			auto byteOffset = pixelOffset * pixelSize;
			uint totalPixels = desc.Width * desc.Height;
			uint maxPixelsToEncode = min(totalPixels, pixelCount);
			uint pixelsEncoded {0};

			// Write RLE packet
			auto RLE_write = [&]()
			{
				switch (desc.NumChannels) {
					case 1 : {
						RLE_Packet<(int) PixelDepth::R8 / 8> packet;
						packet.SetNumPixels(counterEqual);
						packet.mData[0] = (byte) (pData[pixelOffset]);
						stream.write(reinterpret_cast<char *>(&packet), sizeof(packet));
						break;
					}

					case 3: {
						RLE_Packet<(int) PixelDepth::R8G8B8 / 8> packet;
						packet.SetNumPixels(counterEqual);
						packet.mData[0] = (byte) (pData[byteOffset]);
						packet.mData[1] = (byte) (pData[++byteOffset]);
						packet.mData[2] = (byte) (pData[++byteOffset]);
						// Skip Alpha byte to pad to 32-bit
						++byteOffset;
						stream.write(reinterpret_cast<char *>(&packet), sizeof(packet));
						break;
					}

					case 4: {
						RLE_Packet<(int) PixelDepth::R8G8B8A8 / 8> packet;
						packet.SetNumPixels(counterEqual);
						packet.mData[0] = (byte) (pData[byteOffset]);
						packet.mData[1] = (byte) (pData[++byteOffset]);
						packet.mData[2] = (byte) (pData[++byteOffset]);
						packet.mData[3] = (byte) (pData[++byteOffset]);
						stream.write(reinterpret_cast<char *>(&packet), sizeof(packet));
						break;
					}
					default:
					{
						cerr << "Can't write ColorFormat: " << desc.PixelFormat << endl;
					}
				}
				counterEqual = 0;
			};

			// write Raw package
			auto Raw_write = [&]()
			{
				Raw_PacketHeader packet;
				packet.SetNumPixels(counterNEqual);
				stream.write(reinterpret_cast<char *>(&packet), sizeof(packet));
				stream.write(reinterpret_cast<char *>(pData), counterNEqual * pixelSize);

				counterNEqual = 0;
			};

			auto CompPixels = [&](uint byteOffset, int rhsOffset) {
				for (auto i = 0; i < desc.NumChannels; ++i)
				{
					if (pData[byteOffset + i] != pData[byteOffset + i + rhsOffset])
					{
						return false;
					}
				}
				return true;
			};

			// write either RLE or raw packets
			for (pixelsEncoded = 0; pixelsEncoded < maxPixelsToEncode; pixelsEncoded++) {
				if (pixelsEncoded != totalPixels && CompPixels(pixelsEncoded, pixelSize)) {
					counterEqual++;
					if (counterEqual == 128) {
						RLE_write();
						counterEqual = 0;
					} else if (counterNEqual > 0) {
						Raw_write();
						counterNEqual = 0;
					}
				} else {
					counterNEqual++;
					if (pixelsEncoded == totalPixels - 1) {
						//rawPixels.push_back(pData[i]);
						if (counterEqual > 0) {
							if (CompPixels(i, -pixelSize) && counterEqual < 128) ++counterEqual;
							RLE_write();
							if (counterEqual == 128) Raw_write();
						} else {
							Raw_write();
						}
						break;
					} else {
						if (counterNEqual == 128) {
							Raw_write();
							counterNEqual = 0;
						} else if (counterEqual > 0) {
							RLE_write();
							counterEqual = 0;
						}
					}
				}
				byteOffset += pixelSize;
			}
			return pixelsEncoded;
		}

		static uint
		DecodePixels(istream& stream, const ImageDesc &desc, byte* pData, uint& pixelOffset, uint pixelsToDecode)
		{
			auto byteOffset = pixelOffset * desc.PixelSize;
			uint pixelCount = desc.Width * desc.Height;
			uint maxPixelsToDecode = min(pixelCount - pixelOffset, pixelsToDecode);
			uint pixelsDecoded {0};
			void (*swizzler) (byte*, byte*);

			switch(desc.PixelSize) {
				case 3:
					swizzler = FixSwizzleBGR;
					break;
				case 4:
					swizzler = FixSwizzleBGRA;
					break;
				default:
					swizzler = nullptr;
			}

			byte pixel[4];
			while(pixelsDecoded < maxPixelsToDecode) {
				byte packet{0};
				stream.read(reinterpret_cast<char*>(&packet), 1);
				uint packetPixelCount = (packet & packetPixelMask) + 1u;
				if (packet & RLE_packetMask) {
					stream.read(reinterpret_cast<char *>(pixel), desc.NumChannels);
					for (auto i = 0; i < packetPixelCount; ++i) {
						swizzler(pData + byteOffset, pixel);
						byteOffset += desc.PixelSize;
					}
				} else {
					for (auto i = 0; i < packetPixelCount; ++i) {
						stream.read(reinterpret_cast<char *>(pixel), desc.NumChannels);
						swizzler(pData + byteOffset, pixel);
						byteOffset += desc.PixelSize;
					}
				}
				pixelsDecoded += packetPixelCount;
			}
			return pixelsDecoded;
		}
	};
}

#pragma pack (pop)