#include "../inc/TexIO.h"

using namespace TexIO;

int main()
{
	ImageDesc info;

	auto texIO = TexIO::TexIO();

	string horGrad = "../Resources/Gradient_RGB_horizontal.tga";
	string RLEtest = "../Resources/RLE.tga";
	string RLEgrayTest = "../Resources/RLE_Gray.tga";
	string RLEgradTest = "../Resources/Gradient_RGB_Vertical_RLE.tga";
	string RLEnoiseTest = "../Resources/RLE_Noise.tga";
	string RLEnoiseSmallTest = "../Resources/RLE_Noise_Small.tga";
	string horGradCopy = "gradient_copy.tga";
	string horGradCopy2 = "gradient_copy2";

	string in = RLEgradTest;
	string out = horGradCopy2;

	// Read
	if(texIO.ReadHeader(in,info,false))
	{
		uint pixelsToRead = info.Width * info.Height;
		vector<byte> data = vector<byte>(info.Width * info.Height * info.PixelSize);

		uint pixelsRead = texIO.ReadData(in, data.data(), info, 0, pixelsToRead);

		// Write

		uint pixelsWritten{0};

		info.Compression = CompressionType::None;

		texIO.WriteHeader(out,info);

		texIO.WriteData(out, info, data.data(), 0, pixelsToRead);
	}

	return 0;
}

