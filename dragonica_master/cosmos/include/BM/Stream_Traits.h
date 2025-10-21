#pragma once

template< typename T > struct _Traits_MarshallingPacket;

template< >
struct _Traits_MarshallingPacket< std::map< std::string, BM::vstring > >
{
	static void WriteToPacket( BM::Stream &rkStream, std::map< std::string, BM::vstring > const & rkTbl )
	{
		PU::TWriteTable_AA( rkStream, rkTbl );
	}

	static bool ReadFromPacket( BM::Stream &rkStream, std::map< std::string, BM::vstring >& rkTbl )
	{
		return PU::TLoadTable_AA( rkStream, rkTbl );
	}
};

template< >
struct _Traits_MarshallingPacket< std::map< std::string, BM::PgVariables<std::string> > >
{
	static void WriteToPacket( BM::Stream &rkStream, std::map< std::string, BM::PgVariables<std::string> > const & rkTbl )
	{
		PU::TWriteTable_AM( rkStream, rkTbl );
	}

	static bool ReadFromPacket( BM::Stream &rkStream, std::map< std::string, BM::PgVariables<std::string> >& rkTbl )
	{
		return PU::TLoadTable_AM( rkStream, rkTbl );
	}
};

template< >
struct _Traits_MarshallingPacket< std::map< std::wstring, BM::vstring > >
{
	static void WriteToPacket( BM::Stream &rkStream, std::map< std::wstring, BM::vstring > const & rkTbl )
	{
		PU::TWriteTable_AA( rkStream, rkTbl );
	}

	static bool ReadFromPacket( BM::Stream &rkStream, std::map< std::wstring, BM::vstring >& rkTbl )
	{
		return PU::TLoadTable_AA( rkStream, rkTbl );
	}
};

template< >
struct _Traits_MarshallingPacket< std::map< std::wstring, BM::PgVariables<std::wstring> > >
{
	static void WriteToPacket( BM::Stream &rkStream, std::map< std::wstring, BM::PgVariables<std::wstring> > const & rkTbl )
	{
		PU::TWriteTable_AM( rkStream, rkTbl );
	}

	static bool ReadFromPacket( BM::Stream &rkStream, std::map< std::wstring, BM::PgVariables<std::wstring> >& rkTbl )
	{
		return PU::TLoadTable_AM( rkStream, rkTbl );
	}
};
