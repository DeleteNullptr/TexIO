#pragma once

#include "Common.h"

#pragma pack(push, 1)
namespace TexIO
{
	// Base classes
	template <typename Tr>
	class CF_BASE1
	{
	  public:
		explicit CF_BASE1(Tr r=0):
			R(r)
		{}

		bool operator ==(const CF_BASE1& lhs) const
		{
			return (R == lhs.R);
		};

		Tr R{0};
	};

	template <typename Tr,typename Tg,typename Tb>
	class CF_BASE3
	{
	  public:
		explicit CF_BASE3(Tr r=0, Tg g=0, Tb b=0):
			R(r), G(g), B(b)
		{}

		bool operator ==(const CF_BASE3& lhs) const
		{
			return ((R == lhs.R) && (G == lhs.G) && (B == lhs.B));
		};

		Tr R{0};
		Tg G{0};
		Tb B{0};
	};

	template <typename Tr,typename Tg,typename Tb,typename Ta>
	class CF_BASE4
	{
	  public:
		explicit CF_BASE4(Tr r = 0, Tg g = 0, Tb b = 0, Ta a = 0):
		R(r), G(g), B(b), A(a)
		{}

		bool operator ==(const CF_BASE4& lhs) const
		{
			return ((R == lhs.R) && (G == lhs.G) && (B == lhs.B) && (A == lhs.A));
		};

		Tr R{0};
		Tg G{0};
		Tb B{0};
		Ta A{0};
	};

	// Derived formats

	using CF_R8_UINT = CF_BASE1<byte>;
	using CF_R16G16B16_UINT = CF_BASE3<ushort,ushort,ushort>;
	using CF_R8G8B8_UINT = CF_BASE3<byte,byte,byte>;
	using CF_R8G8B8A8_UINT = CF_BASE4<byte,byte,byte,byte>;

	enum ColorFormat
	{
		UNKNOWN,
		R16G16B16_UINT,
		R8_UINT,
		R8G8B8_UINT,
		R8G8B8A8_UINT,
	};
}
#pragma pack(pop)