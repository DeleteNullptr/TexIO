#pragma once

#include "Common.h"
#include "FormatIO.h"

#include "TGA_IO.h"

namespace TexIO
{
	class TexIO
	{
	  public:
		bool ReadHeader(string filename, ImageDesc& desc, bool keepOpen = true)
		{
			// Determine Image type
			string ext = GetExtension(filename, desc);

			fstream* pFile;
			bool result = false;

			auto it = mOpenFiles.find(filename);
			if(it != mOpenFiles.end())
			{
				pFile = &it->second;
				pFile->seekg(0);
			}
			else
			{
				mOpenFiles[filename] = fstream(filename, ios::binary | ios::in);
				pFile = &mOpenFiles[filename];
				if(pFile->fail()) return false;
			}

			desc.MipLevels = 1;

			if (desc.FileFormat == FileType::TGA || ext == "tga")
			{
				result = TGA_IO::ReadHeader(*pFile, desc);
			}
			else if (desc.FileFormat == FileType::PNG || ext == "png")
			{
				//TGA_IO::ReadHeader(filename, keepOpen);
			}
			else if (desc.FileFormat == FileType::HDR || ext == "hdr")
			{
				//TGA_IO::ReadHeader(filename, keepOpen);
			}

			desc.Update();

			if(!keepOpen)
			{
				pFile->close();
				mOpenFiles.erase(filename);
			}

			return result;
		}

		bool WriteHeader(string filename, ImageDesc desc)
		{
			// Determine Image type
			string ext = GetExtension(filename, desc);

			auto file = fstream(filename, ios::binary | ios::out | ios::trunc);

			bool result = false;

			if (desc.FileFormat == FileType::TGA || ext == "tga")
			{
				result = TGA_IO::WriteHeader(file, desc);
			}
			else if (desc.FileFormat == FileType::PNG || ext == "png")
			{
				//TGA_IO::ReadHeader(filename, keepOpen);
			}
			else if (desc.FileFormat == FileType::HDR || ext == "hdr")
			{
				//TGA_IO::ReadHeader(filename, keepOpen);
			}
			else
			{
				return result;
			}

			file.close();
			return result;
		}

		uint ReadData(string filename, byte* pData, ImageDesc desc,uint pixelOffset, uint pixelsToRead)
		{
			// Determine Image type
			string ext = GetExtension(filename, desc);
			fstream* pFile;

			auto it = mOpenFiles.find(filename);
			if(it != mOpenFiles.end())
			{
				pFile = &it->second;
				pFile->seekg(0);
			}
			else
			{
				mOpenFiles[filename] = fstream(filename, ios::binary | ios::in);
				pFile = &mOpenFiles[filename];
			}

			uint pixelsRead{0};

			if (desc.FileFormat == FileType::TGA || ext == "tga")
			{
				pFile->seekg(sizeof(TGA_IO::TargaHeader) + desc.Signature.length());
				pixelsRead = TGA_IO::ReadData(*pFile, desc, pData, pixelOffset, pixelsToRead);
			}
			else if (desc.FileFormat == FileType::PNG || ext == "png")
			{
				//TGA_IO::ReadHeader(filename, keepOpen);
			}
			else if (desc.FileFormat == FileType::HDR || ext == "hdr")
			{
				//TGA_IO::ReadHeader(filename, keepOpen);
			}

			pFile->close();
			mOpenFiles.erase(filename);

			return pixelsRead;
		}

		uint WriteData(string filename, ImageDesc desc, byte* pData, uint pixelOffset, uint pixelsToWrite)
		{
			uint pixelsWritten{0};

			// Determine Image type
			string ext = GetExtension(filename, desc);

			auto file = fstream(filename, ios::binary | ios::out | ios::app);

			if (desc.FileFormat == FileType::TGA || ext == ".tga")
			{
				pixelsWritten = TGA_IO::WriteData(file, desc, pData, pixelOffset, pixelsToWrite);
			}
			else if (desc.FileFormat == FileType::PNG || ext == ".png")
			{
				//TGA_IO::ReadHeader(filename, keepOpen);
			}
			else if (desc.FileFormat == FileType::HDR || ext == ".hdr")
			{
				//TGA_IO::ReadHeader(filename, keepOpen);
			}
			file.close();
			mOpenFiles.erase(filename);

			return pixelsWritten;
		}

	  private:
		// Returns extension based on filename or description, adds it to filename if not present.
		string GetExtension(string& filename, ImageDesc desc)
		{
			auto extIdx = filename.rfind('.');
			string ext;
			if(extIdx != string::npos)
			{
				ext = filename.substr(extIdx + 1);
				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
			}
			else
			{
				switch(desc.FileFormat)
				{
					case FileType::TGA:
					{
						ext = ".tga";
						break;
					}
					case FileType::PNG:
					{
						ext = ".png";
						break;
					}
					case FileType::HDR:
					{
						ext = ".hdr";
						break;
					}
					default:
						break;
				}
				filename += ext;
			}
			return ext;
		}
		map<string, fstream> mOpenFiles;
	};
}