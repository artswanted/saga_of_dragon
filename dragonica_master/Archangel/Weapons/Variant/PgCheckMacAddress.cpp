#include "stdafx.h"
#include "BM/LocalMgr.h"
#include "PgCheckMacAddress.h"

// 바른손 내부 테스트 서버
// SetMacAddress() 내의 해당 국가에서만 호출 되어야 함!
void PgCheckMacAddress::setInternal()
{
	PushMacAddress(L"00-15-17-46-F4-7A");
	PushMacAddress(L"00-15-17-A7-47-44");
	PushMacAddress(L"00-15-17-AA-44-34");
	PushMacAddress(L"00-15-17-76-FF-00");
	PushMacAddress(L"00-30-48-7E-3E-B2");
	PushMacAddress(L"00-15-17-4F-B0-EC");
	PushMacAddress(L"00-30-48-7E-6F-F4");
	PushMacAddress(L"00-30-48-C0-89-1C");
	PushMacAddress(L"00-15-17-9F-5E-C8");
	PushMacAddress(L"00-15-17-7E-CA-A4");
	PushMacAddress(L"00-15-17-AA-49-84");
	PushMacAddress(L"00-30-48-C0-89-26");
	PushMacAddress(L"1C-6F-65-36-CB-52");
	PushMacAddress(L"1C-6F-65-88-3B-0F");
	PushMacAddress(L"90-2B-34-30-AF-F4");
	PushMacAddress(L"1C-6F-65-3F-D1-99");
	PushMacAddress(L"A8-5E-45-13-1C-C6");
	PushMacAddress(L"A4-BF-01-00-43-9E");
	PushMacAddress(L"70-54-D2-1A-83-18");
	PushMacAddress(L"D8-CB-8A-5D-88-CE");
	PushMacAddress(L"A4-BF-01-00-43-9E");
	PushMacAddress(L"00-25-90-7C-01-D6"); // Mercy Root mac
	PushMacAddress(L"70-54-D2-1A-83-18"); // Hunter Root mac
	PushMacAddress(L"D8-CB-8A-5D-88-CE"); // Feribell mac
	PushMacAddress(L"52-54-00-E5-CB-BC"); // Vlad mac
	PushMacAddress(L"A8-A1-59-2B-41-E1"); // iKode
}

void PgCheckMacAddress::SetMacAddress( LOCAL_MGR::NATION_CODE const eNationCode )
{
	// !! 해당 브랜치의 국가만 추가하세요 !!
	switch( eNationCode )
	{
	case LOCAL_MGR::NC_KOREA:
		{
			setInternal(); // 바른손 내부 테스트 서버
		}break;
	case LOCAL_MGR::NC_DEVELOP:
		{
			setInternal(); // 바른손 내부 테스트 서버
		}break;
	}

	switch( eNationCode )
	{
	case LOCAL_MGR::NC_RUSSIA:
		{
			setInternal(); // 바른손 내부 테스트 서버
			// mac ext
			PushMacAddress(L"00-21-5E-98-29-38");
			PushMacAddress(L"00-21-5E-98-2B-3C");
			PushMacAddress(L"00-21-5E-98-2A-C0");
			PushMacAddress(L"00-21-5E-98-2F-20");
			PushMacAddress(L"00-21-5E-98-2E-D8");
			PushMacAddress(L"00-21-5E-98-34-24");
			PushMacAddress(L"00-21-5E-93-37-D4");
			PushMacAddress(L"00-21-5E-98-29-4C");
			PushMacAddress(L"00-21-5E-91-EA-DC");
			PushMacAddress(L"00-21-5E-91-EA-28");
			PushMacAddress(L"00-21-5E-98-28-C8");
			PushMacAddress(L"00-21-5E-98-2B-EC");
			PushMacAddress(L"00-21-5E-98-33-80");
			PushMacAddress(L"00-21-5E-98-2B-8C");
			PushMacAddress(L"00-21-5E-98-99-7C");
			PushMacAddress(L"00-21-5E-98-28-30");
			PushMacAddress(L"00-21-5E-98-2F-18");
			PushMacAddress(L"00-21-5E-91-EA-04");
			PushMacAddress(L"00-15-17-25-E2-4E");

			// mac int
			PushMacAddress(L"00-21-5E-98-29-3A");
			PushMacAddress(L"00-21-5E-98-2B-3E");
			PushMacAddress(L"00-21-5E-98-2A-C2");
			PushMacAddress(L"00-21-5E-98-2F-22");
			PushMacAddress(L"00-21-5E-98-2E-DA");
			PushMacAddress(L"00-21-5E-98-34-26");
			PushMacAddress(L"00-21-5E-93-37-D6");
			PushMacAddress(L"00-21-5E-98-29-4E");
			PushMacAddress(L"00-21-5E-91-EA-DE");
			PushMacAddress(L"00-21-5E-91-EA-2A");
			PushMacAddress(L"00-21-5E-98-28-CA");
			PushMacAddress(L"00-21-5E-98-2B-EE");
			PushMacAddress(L"00-21-5E-98-33-82");
			PushMacAddress(L"00-21-5E-98-2B-8E");
			PushMacAddress(L"00-21-5E-98-99-7E");
			PushMacAddress(L"00-21-5E-98-28-32");
			PushMacAddress(L"00-21-5E-98-2F-1A");
			PushMacAddress(L"00-21-5E-91-EA-06");
		}break;
	case LOCAL_MGR::NC_USA:
		{
			setInternal(); // 바른손 내부 테스트 서버

			//test			
			PushMacAddress(L"00-50-56-9A-44-36");
			PushMacAddress(L"00-50-56-9A-44-33");
			PushMacAddress(L"00-50-56-9A-44-35");
			PushMacAddress(L"00-50-56-9A-44-3C");
			PushMacAddress(L"00-19-B9-F2-3B-B6");
			PushMacAddress(L"00-19-B9-F2-3B-B8");
			PushMacAddress(L"00-50-56-8C-19-E0");

			PushMacAddress(L"1C-C1-DE-70-06-F4");
			PushMacAddress(L"1C-C1-DE-E5-BC-70");
			PushMacAddress(L"78-E7-D1-EC-24-2A");
			PushMacAddress(L"78-E7-D1-EC-5D-32");
			PushMacAddress(L"78-E7-D1-91-9C-A2");
			PushMacAddress(L"78-E7-D1-EC-5E-40");
			PushMacAddress(L"1C-C1-DE-E5-D1-08");
			PushMacAddress(L"1C-C1-DE-E5-BD-84");
			PushMacAddress(L"1C-C1-DE-E5-BC-D0");
			PushMacAddress(L"1C-C1-DE-E5-0C-E0");
			PushMacAddress(L"1C-C1-DE-E5-2C-94");
			PushMacAddress(L"1C-C1-DE-E4-6E-04");
			PushMacAddress(L"1C-C1-DE-E4-4B-7C");
			PushMacAddress(L"78-E7-D1-EC-62-6E");
			PushMacAddress(L"00-0C-29-E8-68-12");
			PushMacAddress(L"00-0C-29-FA-60-04");
			PushMacAddress(L"00-0C-29-FB-46-0A");
			PushMacAddress(L"00-0C-29-97-18-0D");
		}break;
	case LOCAL_MGR::NC_SINGAPORE:
		{
			// 싱가폴
			setInternal(); // 바른손 내부 테스트 서버
			PushMacAddress(L"00-50-56-82-00-99");
			PushMacAddress(L"00-50-56-82-00-9F");
			PushMacAddress(L"00-50-56-82-00-9A");
			PushMacAddress(L"00-19-BB-2C-1C-37");
			PushMacAddress(L"00-19-BB-23-76-F0");
			PushMacAddress(L"00-19-BB-2C-6B-B1");
			PushMacAddress(L"00-17-A4-3D-03-9E");
			PushMacAddress(L"00-17-A4-3D-03-DC");
			PushMacAddress(L"00-17-A4-3D-63-5C");
			PushMacAddress(L"00-19-BB-23-76-98");
			PushMacAddress(L"00-19-BB-23-F7-38");
			PushMacAddress(L"00-1C-C4-BE-C4-A8");
			PushMacAddress(L"00-1E-0B-60-4A-4C");
			PushMacAddress(L"00-1C-C4-77-1E-AC");
			PushMacAddress(L"00-1C-C4-7C-BE-D2");
			PushMacAddress(L"00-1C-C4-77-1E-94");
			PushMacAddress(L"00-1C-C4-78-09-A8");
			PushMacAddress(L"00-17-A4-3D-03-73");
			PushMacAddress(L"00-19-BB-23-C7-ED");
			PushMacAddress(L"00-15-60-A7-FD-CE");
			PushMacAddress(L"00-17-A4-3D-23-6D");
			PushMacAddress(L"00-19-BB-23-C7-F1");
			PushMacAddress(L"00-1E-0B-60-7A-F4");
			PushMacAddress(L"00-1E-0B-5E-3E-76");
			PushMacAddress(L"00-1C-C4-78-89-12");
			PushMacAddress(L"00-1E-0B-60-8B-EA");
			PushMacAddress(L"00-1C-C4-77-1E-E6");
			PushMacAddress(L"00-19-BB-23-D7-87");
			PushMacAddress(L"00-19-BB-23-C7-AC");
			PushMacAddress(L"00-19-BB-23-E7-84");
			PushMacAddress(L"00-19-BB-23-D7-6D");
			PushMacAddress(L"00-19-BB-23-D7-92");
			PushMacAddress(L"00-19-BB-23-E7-92");
			PushMacAddress(L"00-19-BB-23-76-B2");
			PushMacAddress(L"00-1E-0B-5F-A0-B4");
			PushMacAddress(L"00-1E-0B-60-BB-34");
			PushMacAddress(L"00-19-BB-32-E3-56");
			PushMacAddress(L"00-1E-0B-46-C9-FA");
			PushMacAddress(L"00-1A-4B-B2-68-C4");
			PushMacAddress(L"00-19-BB-23-C7-71");
			PushMacAddress(L"00-1C-C4-77-9D-40");
			PushMacAddress(L"00-19-BB-23-C7-47");
			PushMacAddress(L"00-1C-C4-7C-56-32");
			PushMacAddress(L"00-50-56-82-01-1E");
			PushMacAddress(L"00-50-56-82-01-14");
			PushMacAddress(L"00-50-56-82-01-10");
			PushMacAddress(L"00-50-56-82-01-0F");
			PushMacAddress(L"00-50-56-82-01-0D");
			PushMacAddress(L"00-50-56-82-01-0E");
			PushMacAddress(L"00-50-56-82-01-11");
			PushMacAddress(L"00-50-56-82-00-F2");
			PushMacAddress(L"00-50-56-82-00-F3");
			PushMacAddress(L"00-50-56-82-00-F4");
			PushMacAddress(L"00-50-56-82-00-F5");
			PushMacAddress(L"00-50-56-82-00-F6");
			PushMacAddress(L"00-50-56-82-00-F7");
			PushMacAddress(L"00-50-56-82-00-F8");
			PushMacAddress(L"00-50-56-82-00-F9");
			PushMacAddress(L"00-50-56-82-00-FA");
			PushMacAddress(L"00-50-56-82-00-FB");
			PushMacAddress(L"00-50-56-82-00-FC");
			PushMacAddress(L"00-50-56-82-00-FD");
			PushMacAddress(L"00-50-56-82-00-FE");
			PushMacAddress(L"00-50-56-82-00-FF");
			PushMacAddress(L"00-50-56-82-01-00");
			PushMacAddress(L"00-50-56-82-01-01");
			PushMacAddress(L"00-50-56-82-01-02");
			PushMacAddress(L"00-50-56-82-01-03");
			PushMacAddress(L"00-50-56-82-01-04");
			PushMacAddress(L"00-50-56-82-01-09");
			PushMacAddress(L"00-50-56-82-01-05");
			PushMacAddress(L"00-50-56-82-01-0B");
			PushMacAddress(L"00-50-56-82-01-06");
			PushMacAddress(L"00-50-56-82-01-1C");
			PushMacAddress(L"00-50-56-82-01-1D");
			PushMacAddress(L"00-50-56-82-01-19");
			PushMacAddress(L"00-50-56-82-01-1F");


			// 말레이시아
			PushMacAddress(L"00-18-FE-31-10-45");
			PushMacAddress(L"00-19-BB-23-C7-2E");
			PushMacAddress(L"00-17-A4-A5-08-E0");
			PushMacAddress(L"00-22-19-86-C3-5F");
			PushMacAddress(L"00-22-19-8E-92-C4");
			PushMacAddress(L"00-19-BB-23-98-4C");
			PushMacAddress(L"00-19-BB-23-77-69");
			PushMacAddress(L"00-16-35-3A-B4-30");
			PushMacAddress(L"00-19-BB-23-97-2F");
			PushMacAddress(L"00-19-BB-23-97-35");
			PushMacAddress(L"00-19-BB-23-87-BD");
			PushMacAddress(L"00-19-BB-23-B7-EA");
			PushMacAddress(L"00-0E-0C-4E-94-EB");
			PushMacAddress(L"00-0E-0C-4E-97-52");
			PushMacAddress(L"00-04-23-B1-15-E6");
			PushMacAddress(L"00-19-BB-23-A7-08");
			PushMacAddress(L"00-19-BB-23-87-BC");
			PushMacAddress(L"00-22-19-7A-1B-15");
			PushMacAddress(L"00-22-19-7A-1D-82");
			PushMacAddress(L"00-17-08-4E-9D-F9");
			PushMacAddress(L"00-19-BB-23-88-84");
			PushMacAddress(L"00-17-A4-8D-D0-D7");
			PushMacAddress(L"00-16-35-38-D3-33");
			PushMacAddress(L"00-19-BB-23-A7-A4");
			PushMacAddress(L"00-19-BB-23-C8-D0");
			PushMacAddress(L"00-17-A4-38-4B-DA");

			PushMacAddress(L"00-18-71-E6-0F-47");
			PushMacAddress(L"00-18-71-E6-0F-48");
			PushMacAddress(L"00-19-BB-23-98-6D");
			PushMacAddress(L"00-18-FE-87-31-27");
			PushMacAddress(L"00-18-FE-87-31-28");
			PushMacAddress(L"00-17-A4-3D-91-26");
			PushMacAddress(L"00-19-BB-23-08-38");
			PushMacAddress(L"00-19-BB-23-08-44");
			PushMacAddress(L"00-17-A4-38-4B-DA");
			PushMacAddress(L"00-18-71-E6-1C-C1");
			PushMacAddress(L"00-18-71-E6-1C-99");

		}break;
	case LOCAL_MGR::NC_EU:
		{
			// TestServer
			setInternal(); // 바른손 내부 테스트 서버
			PushMacAddress(L"00-25-64-3C-B1-F9");
			PushMacAddress(L"00-22-19-D6-43-27");
			PushMacAddress(L"00-22-19-D6-49-7E");
			PushMacAddress(L"1C-6F-65-3F-D1-99");

			// back mac
			PushMacAddress(L"00-22-19-BD-9F-09");
			PushMacAddress(L"00-22-19-BF-E0-BE");
			PushMacAddress(L"00-22-19-BF-D4-DE");
			PushMacAddress(L"00-22-19-BF-D5-42");
			PushMacAddress(L"00-22-19-BF-D4-2F");
			PushMacAddress(L"00-22-19-BF-E0-C3");
			PushMacAddress(L"00-22-19-BF-DA-50");
			PushMacAddress(L"00-22-19-BF-EA-72");
			PushMacAddress(L"00-24-E8-61-BA-B5");
			PushMacAddress(L"00-24-E8-61-CD-BC");
			PushMacAddress(L"00-22-19-BD-BA-D1");
			PushMacAddress(L"00-22-19-D4-C0-9D");
			PushMacAddress(L"00-22-19-BF-AE-B3");
			PushMacAddress(L"00-22-19-BF-DA-96");
			PushMacAddress(L"00-22-19-BF-DB-9E");
			PushMacAddress(L"00-22-19-BF-DA-5A");
			PushMacAddress(L"00-22-19-BF-D7-99");
			PushMacAddress(L"00-22-19-BF-D7-8F");
			PushMacAddress(L"00-22-19-BF-D9-88");
			PushMacAddress(L"00-22-19-BF-CC-72");
			PushMacAddress(L"00-24-E8-61-C5-9B");
			PushMacAddress(L"00-24-E8-61-C0-94");
			PushMacAddress(L"00-22-19-BE-13-85");
			PushMacAddress(L"00-22-19-D4-C7-04");
			PushMacAddress(L"00-22-19-BF-AF-00");
			PushMacAddress(L"00-22-19-BF-DF-94");
			PushMacAddress(L"00-22-19-BE-13-93");
			PushMacAddress(L"00-22-19-BF-C2-1C");
			PushMacAddress(L"00-22-19-BF-DF-8F");
			PushMacAddress(L"00-22-19-BF-D4-D9");
			PushMacAddress(L"00-22-19-BF-DE-00");
			PushMacAddress(L"00-22-19-BF-C6-D2");
			PushMacAddress(L"00-22-19-BF-DB-A3");
			PushMacAddress(L"00-22-19-BF-DF-76");
			PushMacAddress(L"00-22-19-BF-DF-85");
			PushMacAddress(L"00-22-19-BD-F7-55");
			PushMacAddress(L"00-22-19-BF-DE-05");
			PushMacAddress(L"00-22-19-BF-E0-E6");
			PushMacAddress(L"00-22-19-BF-E0-D2");
			PushMacAddress(L"00-22-19-BF-DF-99");
			PushMacAddress(L"00-22-19-BF-D9-79");
			PushMacAddress(L"00-24-E8-61-D5-4C");
			PushMacAddress(L"00-24-E8-61-D7-A7");
			PushMacAddress(L"00-22-19-BD-FE-14");
			PushMacAddress(L"00-24-E8-61-E3-CA");
			PushMacAddress(L"00-24-E8-60-AC-22");
			PushMacAddress(L"00-25-64-3C-B6-AC");

			// front mac
			PushMacAddress(L"00-15-17-AA-35-D5");
			PushMacAddress(L"00-15-17-AB-05-55");
			PushMacAddress(L"00-15-17-AA-35-BD");
			PushMacAddress(L"00-15-17-AB-02-C9");
			PushMacAddress(L"00-15-17-AA-35-93");
			PushMacAddress(L"00-15-17-AB-02-D7");
			PushMacAddress(L"00-22-19-BF-EA-70");
			PushMacAddress(L"00-22-19-D4-C0-9C");
			PushMacAddress(L"00-15-17-AA-35-99");
			PushMacAddress(L"00-15-17-A9-FD-9D");
			PushMacAddress(L"00-15-17-A9-FD-BD");
			PushMacAddress(L"00-15-17-A9-FD-F9");
			PushMacAddress(L"00-15-17-A9-FE-1F");
			PushMacAddress(L"00-15-17-A9-FD-B9");
			PushMacAddress(L"00-22-19-BF-CC-70");
			PushMacAddress(L"00-22-19-D4-C7-03");
			PushMacAddress(L"00-15-17-AA-35-A1");
			PushMacAddress(L"00-15-17-AB-00-33");
			PushMacAddress(L"00-15-17-AB-05-BB");
			PushMacAddress(L"00-15-17-AA-33-69");
			PushMacAddress(L"00-15-17-AA-FF-E1");
			PushMacAddress(L"00-15-17-A9-F8-87");
			PushMacAddress(L"00-15-17-A9-FD-99");
			PushMacAddress(L"00-15-17-AB-02-FD");
			PushMacAddress(L"00-15-17-AA-35-C1");
			PushMacAddress(L"00-15-17-1C-E2-1D");
			PushMacAddress(L"00-15-17-AA-35-D1");
			PushMacAddress(L"00-15-17-AB-00-11");
			PushMacAddress(L"00-15-17-AA-35-95");
			PushMacAddress(L"00-15-17-AB-05-D1");
			PushMacAddress(L"00-15-17-A9-FB-A9");
			PushMacAddress(L"00-25-64-3C-B6-AD");
			//origin test
			PushMacAddress(L"00-21-9B-FC-F2-2A");
			PushMacAddress(L"00-22-19-D6-44-C0");
			PushMacAddress(L"00-22-19-D6-44-BF");

		}break;
	case LOCAL_MGR::NC_INDONESIA:
		{
			setInternal(); // 바른손 내부 테스트 서버
			PushMacAddress(L"00-50-56-8C-01-12");
			PushMacAddress(L"00-50-56-8C-01-10");
			PushMacAddress(L"00-50-56-8C-01-13");
			PushMacAddress(L"00-50-56-8C-01-11");
			PushMacAddress(L"00-50-56-8C-01-0C");
			PushMacAddress(L"00-50-56-8C-01-15");
			PushMacAddress(L"00-50-56-8C-01-06");
			PushMacAddress(L"00-50-56-8C-01-08");
			PushMacAddress(L"00-50-56-8C-01-09");
			PushMacAddress(L"00-50-56-8C-01-17");
			PushMacAddress(L"00-50-56-8C-01-05");
			PushMacAddress(L"00-50-56-8C-01-16");
			PushMacAddress(L"00-50-56-8C-01-1D");
			PushMacAddress(L"00-50-56-8C-01-1E");
			PushMacAddress(L"00-17-A4-77-0C-08");
			PushMacAddress(L"00-17-A4-77-10-32");

			PushMacAddress(L"00-17-A4-77-0C-00");
			PushMacAddress(L"00-17-A4-77-0C-08");
			PushMacAddress(L"00-0C-29-AE-AA-A0");
			PushMacAddress(L"00-0C-29-59-24-5D");
			PushMacAddress(L"00-17-A4-77-10-10");
			PushMacAddress(L"00-17-A4-77-10-18");
			PushMacAddress(L"00-17-A4-77-10-20");
			PushMacAddress(L"00-17-A4-77-10-28");
			PushMacAddress(L"00-0C-29-50-9B-8E");
			PushMacAddress(L"00-17-A4-77-10-40");
			PushMacAddress(L"00-17-A4-77-10-48");
			PushMacAddress(L"00-17-A4-77-10-50");
			PushMacAddress(L"00-17-A4-77-10-58");
			PushMacAddress(L"00-17-A4-77-10-60");
			PushMacAddress(L"00-17-A4-77-10-68");
			PushMacAddress(L"00-17-A4-77-0C-40");
			PushMacAddress(L"00-17-A4-77-0C-48");
			PushMacAddress(L"00-17-A4-77-0C-50");
			PushMacAddress(L"00-17-A4-77-0C-58");
			PushMacAddress(L"00-17-A4-77-0C-60");
			PushMacAddress(L"00-17-A4-77-0C-68");
			PushMacAddress(L"00-17-A4-77-10-70");
			PushMacAddress(L"00-17-A4-77-0C-70");
			PushMacAddress(L"00-17-A4-77-10-78");
			PushMacAddress(L"00-17-A4-77-0C-78");
			PushMacAddress(L"00-17-A4-77-0C-28");
			PushMacAddress(L"00-0C-29-52-BF-92");
			PushMacAddress(L"00-50-56-8C-00-28");
			PushMacAddress(L"00-50-56-8C-00-29");
		}break;
	case LOCAL_MGR::NC_PHILIPPINES:
		{
			// Public
			setInternal(); // 바른손 내부 테스트 서버
			PushMacAddress(L"00-26-B9-4E-9F-0A");
			PushMacAddress(L"00-26-B9-4E-9F-17");
			PushMacAddress(L"00-26-B9-4E-9F-24");
			PushMacAddress(L"00-26-B9-4E-9F-31");
			PushMacAddress(L"00-26-B9-4E-9F-3E");
			PushMacAddress(L"00-26-B9-4E-9F-4B");
			PushMacAddress(L"00-26-B9-4D-D7-76");
			PushMacAddress(L"00-26-B9-4D-D7-83");
			PushMacAddress(L"00-26-B9-4D-D7-90");
			PushMacAddress(L"00-26-B9-4D-D7-9D");
			PushMacAddress(L"00-26-B9-4D-D7-AA");
			PushMacAddress(L"00-26-B9-4D-D7-B7");
			PushMacAddress(L"00-26-B9-4D-D7-C4");
			PushMacAddress(L"00-1B-78-5D-2E-EC");
			PushMacAddress(L"00-26-B9-4E-9F-C0");
			PushMacAddress(L"00-1B-78-72-8A-3E");
			PushMacAddress(L"00-26-B9-4E-9F-99");
			PushMacAddress(L"00-26-B9-4E-9F-7F");
			PushMacAddress(L"00-26-B9-4E-9F-58");
			PushMacAddress(L"00-26-B9-4E-9F-65");
			PushMacAddress(L"00-26-B9-4E-9F-B3");
			PushMacAddress(L"00-26-B9-4E-9F-CD");
			PushMacAddress(L"00-26-B9-4D-D7-28");
			PushMacAddress(L"00-26-B9-4D-D7-35");
			PushMacAddress(L"00-26-B9-4E-9F-72");
			PushMacAddress(L"00-26-B9-4E-9F-8C");
			PushMacAddress(L"00-26-B9-4E-9F-A6");
			PushMacAddress(L"00-19-B9-F7-1C-3C");
			PushMacAddress(L"00-15-C5-E4-70-AB");
			PushMacAddress(L"00-26-B9-4D-D7-D1");
			PushMacAddress(L"00-26-B9-4D-D7-DE");
			PushMacAddress(L"84-2B-2B-3E-17-1B");
			PushMacAddress(L"84-2B-2B-3E-17-28");
			PushMacAddress(L"84-2B-2B-3E-17-35");
			PushMacAddress(L"84-2B-2B-3E-17-42");
			PushMacAddress(L"84-2B-2B-3E-17-4F");
			PushMacAddress(L"84-2B-2B-3E-17-5C");
			PushMacAddress(L"84-2B-2B-3E-17-69");
			PushMacAddress(L"84-2B-2B-3E-17-76");

			// Private
			PushMacAddress(L"00-26-B9-4E-9F-0C");
			PushMacAddress(L"00-26-B9-4E-9F-19");
			PushMacAddress(L"00-26-B9-4E-9F-26");
			PushMacAddress(L"00-26-B9-4E-9F-33");
			PushMacAddress(L"00-26-B9-4E-9F-40");
			PushMacAddress(L"00-26-B9-4E-9F-4D");
			PushMacAddress(L"00-26-B9-4D-D7-78");
			PushMacAddress(L"00-26-B9-4D-D7-85");
			PushMacAddress(L"00-26-B9-4D-D7-92");
			PushMacAddress(L"00-26-B9-4D-D7-9F");
			PushMacAddress(L"00-26-B9-4D-D7-AC");
			PushMacAddress(L"00-26-B9-4D-D7-B9");
			PushMacAddress(L"00-26-B9-4D-D7-C6");
			PushMacAddress(L"00-1B-78-5D-2E-ED");
			PushMacAddress(L"00-26-B9-4E-9F-C2");
			PushMacAddress(L"00-1B-78-72-8A-28");
			PushMacAddress(L"00-26-B9-4E-9F-9B");
			PushMacAddress(L"00-26-B9-4E-9F-81");
			PushMacAddress(L"00-26-B9-4E-9F-5A");
			PushMacAddress(L"00-26-B9-4E-9F-67");
			PushMacAddress(L"00-26-B9-4E-9F-B5");
			PushMacAddress(L"00-26-B9-4E-9F-CF");
			PushMacAddress(L"00-26-B9-4D-D7-2A");
			PushMacAddress(L"00-26-B9-4D-D7-37");
			PushMacAddress(L"00-26-B9-4E-9F-74");
			PushMacAddress(L"00-26-B9-4E-9F-8E");
			PushMacAddress(L"00-26-B9-4E-9F-A8");
			PushMacAddress(L"00-19-B9-F7-1C-3E");
			PushMacAddress(L"00-15-C5-E4-70-AD");
			PushMacAddress(L"00-26-B9-4D-D7-D3");
			PushMacAddress(L"00-26-B9-4D-D7-E0");
			PushMacAddress(L"84-2B-2B-3E-17-19");
			PushMacAddress(L"84-2B-2B-3E-17-26");
			PushMacAddress(L"84-2B-2B-3E-17-33");
			PushMacAddress(L"84-2B-2B-3E-17-40");
			PushMacAddress(L"84-2B-2B-3E-17-4D");
			PushMacAddress(L"84-2B-2B-3E-17-5A");
			PushMacAddress(L"84-2B-2B-3E-17-67");
			PushMacAddress(L"84-2B-2B-3E-17-74");
													 
		}break;
	case LOCAL_MGR::NC_THAILAND:
		{
			setInternal(); // 바른손 내부 테스트 서버
			// NIC 1
			PushMacAddress(L"00-0C-29-25-E5-2A");
			PushMacAddress(L"00-0C-29-6C-E9-6B");
			PushMacAddress(L"00-0C-29-0B-6E-F3");
			PushMacAddress(L"00-21-5E-98-DF-6C");
			PushMacAddress(L"00-21-5E-98-DE-28");
			PushMacAddress(L"00-1A-4B-A5-E7-4C");
			PushMacAddress(L"00-21-5E-98-DE-EC");
			PushMacAddress(L"00-21-5E-98-FA-88");
			PushMacAddress(L"00-0C-29-3A-6B-FD");
			PushMacAddress(L"00-21-5E-98-F9-80");
			PushMacAddress(L"00-21-5E-98-70-90");
			PushMacAddress(L"00-21-5E-99-02-B0");
			PushMacAddress(L"00-21-5E-98-E9-D4");
			PushMacAddress(L"00-21-5E-98-D5-20");
			PushMacAddress(L"00-21-5E-98-CB-60");
			PushMacAddress(L"00-21-5E-98-F9-78");
			PushMacAddress(L"00-21-5E-98-72-68");
			PushMacAddress(L"00-21-5E-98-DD-9C");
			PushMacAddress(L"00-21-5E-98-D4-FC");
			PushMacAddress(L"00-21-5E-98-FD-34");
			PushMacAddress(L"00-21-5E-99-03-1C");
			PushMacAddress(L"00-0C-29-7F-47-1D");
			PushMacAddress(L"00-0C-29-0A-7C-CA");
			PushMacAddress(L"00-0C-29-65-63-56");
			PushMacAddress(L"00-21-5E-98-D0-E4");
			PushMacAddress(L"00-21-5E-98-DE-E8");
			PushMacAddress(L"00-21-5E-98-F9-D2");
			PushMacAddress(L"00-21-5E-98-FA-1E");
			PushMacAddress(L"00-21-5E-99-03-9C");
			PushMacAddress(L"00-21-5E-99-02-94");
			PushMacAddress(L"00-21-5E-98-D6-E8");
			PushMacAddress(L"00-0C-29-06-31-13");
			PushMacAddress(L"00-21-5E-98-FA-68");
			PushMacAddress(L"00-0C-29-7F-AF-37");
			PushMacAddress(L"00-0C-29-7F-76-85");
			PushMacAddress(L"00-21-5E-93-7D-26");
			PushMacAddress(L"00-21-5E-93-7D-76");
			PushMacAddress(L"00-21-5E-98-F9-64");
			PushMacAddress(L"00-21-5E-98-DE-E0");
			PushMacAddress(L"00-21-5E-98-F9-84");
			PushMacAddress(L"00-21-5E-98-FD-30");

			// NIC 2
			PushMacAddress(L"00-21-5E-98-DF-6E");
			PushMacAddress(L"00-21-5E-98-DE-2A");
			PushMacAddress(L"00-21-5E-98-DE-EE");
			PushMacAddress(L"00-21-5E-98-FA-8A");
			PushMacAddress(L"00-21-5E-98-F9-82");
			PushMacAddress(L"00-21-5E-98-70-92");
			PushMacAddress(L"00-21-5E-99-02-B2");
			PushMacAddress(L"00-21-5E-98-E9-D6");
			PushMacAddress(L"00-21-5E-98-D5-22");
			PushMacAddress(L"00-21-5E-98-CB-62");
			PushMacAddress(L"00-21-5E-98-F9-7A");
			PushMacAddress(L"00-21-5E-98-72-6A");
			PushMacAddress(L"00-21-5E-98-DD-9E");
			PushMacAddress(L"00-21-5E-98-D4-FE");
			PushMacAddress(L"00-21-5E-98-FD-36");
			PushMacAddress(L"00-21-5E-99-03-1E");
			PushMacAddress(L"00-21-5E-98-C9-2A");
			PushMacAddress(L"00-21-5E-98-70-A6");
			PushMacAddress(L"00-21-5E-98-F9-6E");
			PushMacAddress(L"00-21-5E-98-FA-82");
			PushMacAddress(L"00-21-5E-98-E9-BA");
			PushMacAddress(L"00-21-5E-98-D5-C6");
			PushMacAddress(L"00-21-5E-98-F9-66");
			PushMacAddress(L"00-21-5E-98-DE-E2");
			PushMacAddress(L"00-21-5E-98-F9-86");
			PushMacAddress(L"00-21-5E-98-FA-26");
			PushMacAddress(L"00-21-5E-98-F9-26");
			PushMacAddress(L"00-21-5E-98-DE-DA");
			PushMacAddress(L"00-21-5E-98-D0-E6");
			PushMacAddress(L"00-21-5E-98-DE-EA");
			PushMacAddress(L"00-21-5E-98-F9-D0");
			PushMacAddress(L"00-21-5E-98-FA-8E");
			PushMacAddress(L"00-21-5E-98-FD-32");
			PushMacAddress(L"00-21-5E-98-FA-1C");
			PushMacAddress(L"00-21-5E-99-03-9E");
			PushMacAddress(L"00-21-5E-98-6D-AE");
			PushMacAddress(L"00-21-5E-98-D3-30");
			PushMacAddress(L"00-21-5E-99-02-96");
			PushMacAddress(L"00-21-5E-98-D6-EA");
			PushMacAddress(L"00-21-5E-98-FA-6A");
			PushMacAddress(L"00-21-5E-93-7D-24");
			PushMacAddress(L"00-21-5E-93-7D-74");
	
			PushMacAddress(L"00-21-5E-98-C9-28");
			PushMacAddress(L"00-21-5E-98-70-A4");
			PushMacAddress(L"00-21-5E-98-F9-6C");
			PushMacAddress(L"00-21-5E-98-FA-80");
			PushMacAddress(L"00-21-5E-98-E9-B8");
			PushMacAddress(L"00-21-5E-98-D5-C4");
			PushMacAddress(L"00-21-5E-98-FA-8C");
			PushMacAddress(L"00-21-5E-98-6D-AC");

			PushMacAddress(L"00-21-5E-98-C9-2A");
			PushMacAddress(L"00-21-5E-98-70-A6");
			PushMacAddress(L"00-21-5E-98-F9-6E");
			PushMacAddress(L"00-21-5E-98-FA-82");
			PushMacAddress(L"00-21-5E-98-E9-BA");
			PushMacAddress(L"00-21-5E-98-D5-C6");
			PushMacAddress(L"00-21-5E-98-FA-8E");
			PushMacAddress(L"00-21-5E-98-6D-AE");
			//Test
			PushMacAddress(L"00-21-5E-98-FA-24");
			PushMacAddress(L"00-21-5E-98-F9-24");
			PushMacAddress(L"00-21-5E-98-DE-D8");
			PushMacAddress(L"00-21-5E-98-D3-32");
									  
			PushMacAddress(L"00-21-5E-98-FA-26");
			PushMacAddress(L"00-21-5E-98-F9-26");
			PushMacAddress(L"00-21-5E-98-DE-DA");
			PushMacAddress(L"00-21-5E-98-D3-30");
			
		}break;
	case LOCAL_MGR::NC_VIETNAM:
		{
			setInternal(); // 바른손 내부 테스트 서버
			//public
			PushMacAddress(L"00-50-56-B2-00-09");
			PushMacAddress(L"00-50-56-B2-00-13");
			PushMacAddress(L"00-50-56-B2-00-2E");
			PushMacAddress(L"00-50-56-B2-00-3E");
			PushMacAddress(L"00-26-B9-FA-B9-68");
			PushMacAddress(L"00-50-56-B2-00-A5");
			PushMacAddress(L"00-26-B9-FA-96-F8");
			PushMacAddress(L"00-26-B9-F9-94-4C");
			PushMacAddress(L"00-26-B9-F9-8B-B8");
			PushMacAddress(L"00-26-B9-FA-BF-10");
			PushMacAddress(L"00-26-B9-F9-69-44");
			PushMacAddress(L"00-26-B9-FA-95-98");
			PushMacAddress(L"00-26-B9-F9-8D-18");
			PushMacAddress(L"00-50-56-B5-00-21");
			PushMacAddress(L"00-50-56-B5-00-25");
			PushMacAddress(L"00-50-56-B5-00-27");
			PushMacAddress(L"00-50-56-B5-00-29");
			PushMacAddress(L"00-50-56-B5-00-2B");
			PushMacAddress(L"00-26-B9-F9-8C-48");
			PushMacAddress(L"00-26-B9-F9-68-6C");
			PushMacAddress(L"00-26-B9-FA-BC-20");
			PushMacAddress(L"00-26-B9-FA-BC-E0");
			PushMacAddress(L"00-26-B9-F9-6A-50");
			PushMacAddress(L"00-23-AE-FE-55-4C");
			PushMacAddress(L"00-26-B9-F9-6B-00");
			PushMacAddress(L"00-26-B9-F9-84-50");
			PushMacAddress(L"00-26-B9-FA-BF-1C");
			PushMacAddress(L"00-26-B9-F9-94-68");
			PushMacAddress(L"00-50-56-B2-00-31");
			PushMacAddress(L"D8-D3-85-A6-D0-72");
			PushMacAddress(L"00-26-B9-F9-8B-AC");
			PushMacAddress(L"00-50-56-B5-00-2D");
			PushMacAddress(L"00-26-B9-F9-68-60");
			PushMacAddress(L"00-26-B9-F9-8E-68");
			PushMacAddress(L"00-26-B9-FA-96-6C");
			
			//local							  
			PushMacAddress(L"00-50-56-B2-00-08");
			PushMacAddress(L"00-50-56-B2-00-49");
			PushMacAddress(L"00-50-56-B2-00-2F");
			PushMacAddress(L"00-50-56-B2-00-A7");
			PushMacAddress(L"00-50-56-B2-00-C9");
			PushMacAddress(L"00-50-56-B2-00-3F");
			PushMacAddress(L"00-26-B9-FA-B9-6A");
			PushMacAddress(L"00-50-56-B2-00-A6");
			PushMacAddress(L"00-26-B9-FA-96-FA");
			PushMacAddress(L"00-26-B9-F9-94-4E");
			PushMacAddress(L"00-26-B9-F9-8B-BA");
			PushMacAddress(L"00-26-B9-FA-BF-12");
			PushMacAddress(L"00-26-B9-F9-69-46");
			PushMacAddress(L"00-26-B9-FA-95-9A");
			PushMacAddress(L"00-26-B9-F9-8D-1A");
			PushMacAddress(L"00-50-56-B5-00-20");
			PushMacAddress(L"00-50-56-B5-00-24");
			PushMacAddress(L"00-50-56-B5-00-26");
			PushMacAddress(L"00-50-56-B5-00-28");
			PushMacAddress(L"00-50-56-B5-00-2A");
			PushMacAddress(L"00-26-B9-F9-8C-4A");
			PushMacAddress(L"00-26-B9-F9-68-6E");
			PushMacAddress(L"00-26-B9-FA-BC-22");
			PushMacAddress(L"00-26-B9-FA-BC-E2");
			PushMacAddress(L"00-26-B9-F9-6A-52");
			PushMacAddress(L"00-23-AE-FE-55-4E");
			PushMacAddress(L"00-26-B9-F9-6B-02");
			PushMacAddress(L"00-26-B9-F9-84-52");
			PushMacAddress(L"00-26-B9-FA-BF-1E");
			PushMacAddress(L"00-26-B9-F9-94-6A");
			PushMacAddress(L"D8-D3-85-A6-D6-42");
			PushMacAddress(L"18-A9-05-4D-5A-1E");
			PushMacAddress(L"18-A9-05-4B-37-BE");
			PushMacAddress(L"18-A9-05-65-C1-3E");
			PushMacAddress(L"18-D3-85-A6-CF-3A");
			PushMacAddress(L"00-50-56-B5-00-E1");
			PushMacAddress(L"00-50-56-B5-00-F1");
			PushMacAddress(L"00-50-56-B5-00-E5");
			PushMacAddress(L"00-50-56-B5-00-E7");
			PushMacAddress(L"D8-D3-85-5E-37-B8");
			PushMacAddress(L"00-50-56-B2-00-30");
			PushMacAddress(L"18-A9-05-4D-B4-46");
			PushMacAddress(L"D8-D3-85-A6-D0-70");
			PushMacAddress(L"00-26-B9-F9-8B-AE");
			PushMacAddress(L"00-50-56-B5-00-2C");
			PushMacAddress(L"00-26-B9-F9-68-62");
			PushMacAddress(L"00-26-B9-F9-8E-6A");
			PushMacAddress(L"00-26-B9-FA-96-6E");

			//test1
			PushMacAddress(L"00-50-56-A7-00-6A");
			PushMacAddress(L"00-50-56-A7-00-6B");
			PushMacAddress(L"00-50-56-A7-00-68");
			PushMacAddress(L"00-50-56-A7-00-66");
			PushMacAddress(L"00-50-56-A7-00-67");
			PushMacAddress(L"00-50-56-A7-00-69");
			PushMacAddress(L"00-50-56-85-00-8d");
			PushMacAddress(L"00-50-56-85-00-89");
			PushMacAddress(L"00-50-56-85-00-24");

			//test2
			PushMacAddress(L"00-50-56-A7-00-6A");
			PushMacAddress(L"00-50-56-85-00-1d");
			PushMacAddress(L"00-50-56-85-00-1e");
			PushMacAddress(L"00-50-56-85-00-83");
			PushMacAddress(L"00-50-56-85-00-85");
			PushMacAddress(L"00-50-56-85-00-1C");

		}break;
	case LOCAL_MGR::NC_JAPAN:
		{
			setInternal(); // 바른손 내부 테스트 서버

			PushMacAddress(L"E4-1F-13-30-BC-AC");
			PushMacAddress(L"E4-1F-13-30-87-CC");
			PushMacAddress(L"E4-1F-13-2F-51-60");
			PushMacAddress(L"E4-1F-13-30-7B-30");
			PushMacAddress(L"E4-1F-13-30-8A-08");
			PushMacAddress(L"E4-1F-13-30-72-4C");
			PushMacAddress(L"E4-1F-13-30-A9-14");
			PushMacAddress(L"E4-1F-13-30-79-BC");
			PushMacAddress(L"E4-1F-13-30-89-74");
			PushMacAddress(L"E4-1F-13-30-89-94");
			PushMacAddress(L"00-21-5E-DC-F8-9C");
			PushMacAddress(L"00-21-5E-DC-F7-F0");
			PushMacAddress(L"00-21-5E-DC-F7-0C");
			PushMacAddress(L"00-21-5E-DC-F7-C4");
			PushMacAddress(L"00-21-5E-DD-21-D8");
			PushMacAddress(L"00-21-5E-DC-F7-08");
			PushMacAddress(L"00-21-5E-DC-E9-0C");
			PushMacAddress(L"00-21-5E-DC-F1-38");
			PushMacAddress(L"E4-1F-13-69-62-44");
			PushMacAddress(L"00-21-5E-DC-F1-50");
			PushMacAddress(L"00-21-5E-DC-F5-78");
			PushMacAddress(L"00-21-5E-DB-C6-58");
			PushMacAddress(L"00-21-5E-DD-21-24");
			PushMacAddress(L"E4-1F-13-30-72-54");
			PushMacAddress(L"00-21-5E-70-7C-D4");
			PushMacAddress(L"00-21-5E-DC-F7-04");
			PushMacAddress(L"E4-1F-13-2D-C0-F2");
			PushMacAddress(L"E4-1F-13-30-5D-EA");
			PushMacAddress(L"E4-1F-13-2D-2F-92");
			PushMacAddress(L"E4-1F-13-2D-2D-56");
			PushMacAddress(L"E4-1F-13-30-BC-AE");
			PushMacAddress(L"E4-1F-13-30-87-CE");
			PushMacAddress(L"E4-1F-13-2F-51-62");
			PushMacAddress(L"E4-1F-13-30-7B-32");
			PushMacAddress(L"E4-1F-13-30-8A-0A");
			PushMacAddress(L"E4-1F-13-30-72-4E");
			PushMacAddress(L"E4-1F-13-30-A9-16");
			PushMacAddress(L"E4-1F-13-30-79-BE");
			PushMacAddress(L"E4-1F-13-30-89-76");
			PushMacAddress(L"E4-1F-13-30-89-96");
			PushMacAddress(L"00-21-5E-DC-F8-9E");
			PushMacAddress(L"00-21-5E-DC-F7-F2");
			PushMacAddress(L"00-21-5E-DC-F7-0E");
			PushMacAddress(L"00-21-5E-DC-F7-C6");
			PushMacAddress(L"00-21-5E-C8-35-0E");
			PushMacAddress(L"00-21-5E-DB-C5-66");
			PushMacAddress(L"00-21-5E-DB-C5-F2");
			PushMacAddress(L"00-21-5E-C8-32-DA");
			PushMacAddress(L"00-21-5E-C8-36-42");
			PushMacAddress(L"00-21-5E-DD-21-DA");
			PushMacAddress(L"00-21-5E-DC-F7-0A");
			PushMacAddress(L"00-21-5E-DC-E9-0E");
			PushMacAddress(L"00-21-5E-DC-F1-3A");
			PushMacAddress(L"E4-1F-13-69-62-46");
			PushMacAddress(L"00-21-5E-DC-F1-52");
			PushMacAddress(L"00-21-5E-DC-F5-7A");
			PushMacAddress(L"00-21-5E-DB-C6-5A");
			PushMacAddress(L"00-21-5E-DD-21-26");
			PushMacAddress(L"E4-1F-13-30-72-56");
			PushMacAddress(L"00-21-5E-70-7C-D6");
			PushMacAddress(L"00-21-5E-DD-20-6E");
			PushMacAddress(L"00-21-5E-DC-F7-06");
			PushMacAddress(L"E4-1F-13-68-12-3C");
			PushMacAddress(L"E4-1F-13-68-12-3E");
			PushMacAddress(L"00-50-56-AF-00-A6");
			PushMacAddress(L"00-50-56-AF-00-B5");
			PushMacAddress(L"00-50-56-AF-00-AA");
			PushMacAddress(L"00-50-56-AF-00-BD");

			//일본 가상화 서버
			PushMacAddress(L"00-50-56-AF-00-A5");
			PushMacAddress(L"00-50-56-AF-00-B4");
			PushMacAddress(L"00-50-56-AF-00-A9");
			PushMacAddress(L"00-50-56-AF-00-BC");
			PushMacAddress(L"00-50-56-AF-00-A6");
			PushMacAddress(L"00-50-56-AF-00-B5");
			PushMacAddress(L"00-50-56-AF-00-AA");
			PushMacAddress(L"00-50-56-AF-00-BD");
			PushMacAddress(L"00-50-56-AF-00-9D");
			PushMacAddress(L"00-50-56-AF-00-9F");
			PushMacAddress(L"00-50-56-AF-00-A3");
			PushMacAddress(L"00-50-56-AF-00-AE");
			PushMacAddress(L"00-50-56-AF-00-B0");
			PushMacAddress(L"00-50-56-AF-00-B2");
			PushMacAddress(L"00-50-56-AF-00-DB");
			PushMacAddress(L"00-50-56-AF-00-9B");
			PushMacAddress(L"00-50-56-AF-00-AB");
			PushMacAddress(L"00-50-56-AF-00-A7");
			PushMacAddress(L"00-50-56-AF-00-78");
			PushMacAddress(L"00-50-56-AF-00-80");
			PushMacAddress(L"00-50-56-AF-00-82");
			PushMacAddress(L"00-50-56-AF-00-89");
			PushMacAddress(L"00-50-56-AF-00-91");
			PushMacAddress(L"00-50-56-AF-00-87");
			PushMacAddress(L"00-50-56-AF-00-8D");
			PushMacAddress(L"00-50-56-AF-00-8F");
			PushMacAddress(L"00-50-56-AF-00-93");
			PushMacAddress(L"00-50-56-AF-00-95");
			PushMacAddress(L"00-50-56-AF-00-97");
			PushMacAddress(L"00-50-56-AF-00-9A");
			PushMacAddress(L"00-50-56-AF-00-A2");
			PushMacAddress(L"00-50-56-AF-00-9E");
			PushMacAddress(L"00-50-56-AF-00-A0");
			PushMacAddress(L"00-50-56-AF-00-A4");
			PushMacAddress(L"00-50-56-AF-00-AF");
			PushMacAddress(L"00-50-56-AF-00-B1");
			PushMacAddress(L"00-50-56-AF-00-B3");
			PushMacAddress(L"00-50-56-AF-00-DC");
			PushMacAddress(L"00-50-56-AF-00-9C");
			PushMacAddress(L"00-50-56-AF-00-AD");
			PushMacAddress(L"00-50-56-AF-00-A8");
			PushMacAddress(L"00-50-56-AF-00-79");
			PushMacAddress(L"00-50-56-AF-00-B9");
			PushMacAddress(L"00-50-56-AF-00-BB");
			PushMacAddress(L"00-50-56-AF-00-81");
			PushMacAddress(L"00-50-56-AF-00-86");
			PushMacAddress(L"00-50-56-AF-00-8C");
			PushMacAddress(L"00-50-56-AF-00-92");
			PushMacAddress(L"00-50-56-AF-00-88");
			PushMacAddress(L"00-50-56-AF-00-8E");
			PushMacAddress(L"00-50-56-AF-00-90");
			PushMacAddress(L"00-50-56-AF-00-94");
			PushMacAddress(L"00-50-56-AF-00-96");
			PushMacAddress(L"00-50-56-AF-00-98");
			PushMacAddress(L"00-50-56-AF-00-AA");
			PushMacAddress(L"00-50-56-AF-00-BD");
		}break;
	case LOCAL_MGR::NC_TAIWAN:
		{
			setInternal(); // 바른손 내부 테스트 서버

						// 대만 2.0 테스트 서버
			PushMacAddress(L"00-22-19-54-B2-5D");
			PushMacAddress(L"00-22-19-54-B2-3F");
			PushMacAddress(L"00-22-19-54-AE-C5");
			PushMacAddress(L"00-22-19-54-B1-CC");
			PushMacAddress(L"00-22-19-54-B2-58");
			PushMacAddress(L"00-22-19-54-B2-76");
			PushMacAddress(L"00-22-19-54-AC-E0");
			PushMacAddress(L"00-22-19-54-AF-B5");
			PushMacAddress(L"00-0D-61-1F-08-09");
			PushMacAddress(L"00-22-19-54-B2-5B");
			PushMacAddress(L"00-22-19-54-AE-C3");
			PushMacAddress(L"00-22-19-54-B1-CA");
			PushMacAddress(L"00-22-19-54-B2-56");
			PushMacAddress(L"00-22-19-54-B2-74");
			PushMacAddress(L"00-22-19-54-AC-DE");
			PushMacAddress(L"00-22-19-55-74-65");
			PushMacAddress(L"00-22-19-55-74-63");
			PushMacAddress(L"00-22-19-55-7C-C1");
			PushMacAddress(L"00-22-19-55-7C-BF");
			PushMacAddress(L"00-22-19-55-6B-DC");
			PushMacAddress(L"00-22-19-55-6B-DA");
			PushMacAddress(L"00-22-19-56-D9-52");
			PushMacAddress(L"00-22-19-56-D9-50");
			PushMacAddress(L"00-22-19-57-6C-04");
			PushMacAddress(L"00-22-19-57-6C-02");
			PushMacAddress(L"00-22-19-57-66-0F");
			PushMacAddress(L"00-22-19-57-66-0D");
			PushMacAddress(L"00-22-19-55-7D-A2");
			PushMacAddress(L"00-24-E8-4D-63-70");
			PushMacAddress(L"00-24-E8-4D-66-0D");

			//2.0 live
			PushMacAddress(L"00-22-19-A9-BD-32");
			PushMacAddress(L"00-22-19-52-F4-A8");
			PushMacAddress(L"00-22-19-22-0C-36");
			PushMacAddress(L"00-22-19-A9-BD-5F");
			PushMacAddress(L"00-22-19-53-56-CA");
			PushMacAddress(L"00-22-19-52-F4-02");
			PushMacAddress(L"00-22-19-53-56-DE");
			PushMacAddress(L"00-22-19-52-F2-FF");
			PushMacAddress(L"00-22-19-53-56-C5");
			PushMacAddress(L"00-22-19-52-F4-A6");
			PushMacAddress(L"00-22-19-53-56-C8");
			PushMacAddress(L"00-22-19-52-F4-00");
			PushMacAddress(L"00-22-19-53-56-DC");

			// 대만 테스트 서버
			PushMacAddress(L"00-19-B9-F0-FE-DD");
			PushMacAddress(L"00-19-B9-F1-0B-DA");
			PushMacAddress(L"00-19-B9-F0-FA-0A");
			PushMacAddress(L"00-19-B9-F1-0C-C0");
			PushMacAddress(L"00-19-B9-F1-7A-03");
			PushMacAddress(L"00-19-B9-F1-0C-4D");
			PushMacAddress(L"00-0D-61-66-69-D9");

			// 대만구
			PushMacAddress(L"00-22-19-52-F4-A8");
			PushMacAddress(L"00-22-19-52-F4-94");
			PushMacAddress(L"00-22-19-53-56-CA");
			PushMacAddress(L"00-22-19-54-77-34");
			PushMacAddress(L"00-22-19-52-F4-02");
			PushMacAddress(L"00-22-19-52-F3-DA");
			PushMacAddress(L"00-22-19-53-56-DE");
			PushMacAddress(L"00-22-19-52-F2-FF");
			PushMacAddress(L"00-22-19-53-56-C5");
			PushMacAddress(L"00-22-19-A7-D3-51");
			PushMacAddress(L"00-22-19-A9-BD-32");
			PushMacAddress(L"00-22-19-A7-CF-95");
			PushMacAddress(L"00-22-19-22-0C-36");
			PushMacAddress(L"00-22-19-A9-BD-5F");

			// 전구
			PushMacAddress(L"00-22-19-54-B2-5D");
			PushMacAddress(L"00-22-19-54-AE-C5");
			PushMacAddress(L"00-22-19-54-B1-CC");
			PushMacAddress(L"00-22-19-54-B2-58");
			PushMacAddress(L"00-22-19-54-B2-76");
			PushMacAddress(L"00-22-19-54-AC-E0");
			PushMacAddress(L"00-22-19-54-B2-3F");
			PushMacAddress(L"00-22-19-54-AF-B5");
			PushMacAddress(L"00-22-19-C8-88-26");
			PushMacAddress(L"00-22-19-C8-81-10");
			PushMacAddress(L"00-24-E8-4D-63-D9");
			PushMacAddress(L"00-22-19-CD-19-C6");
			
			//추가 서버
			//인터널
			PushMacAddress(L"00-22-19-6D-84-A3");
			PushMacAddress(L"00-1E-C9-AC-B9-7E");
			PushMacAddress(L"00-22-19-56-7A-6C");
			PushMacAddress(L"00-22-19-55-B0-C1");
			PushMacAddress(L"00-21-5E-70-4E-9A");
			PushMacAddress(L"00-1A-64-95-D6-7E");
			PushMacAddress(L"00-1A-64-95-E0-0A");
			PushMacAddress(L"00-23-8B-64-78-75");
			PushMacAddress(L"00-23-8B-64-78-E5");
			//익스터널
			PushMacAddress(L"00-22-19-6D-84-A1");
			PushMacAddress(L"00-1E-C9-AC-B9-7C");
			PushMacAddress(L"00-22-19-56-7A-6A");
			PushMacAddress(L"00-22-19-55-B0-BF");
			PushMacAddress(L"00-21-5E-70-4E-98");
			PushMacAddress(L"00-1A-64-95-D6-7C");
			//Realm 5
			PushMacAddress(L"00-1F-29-CA-BC-60");
			PushMacAddress(L"00-21-5A-A9-AE-5E");
			PushMacAddress(L"00-21-5A-AB-BE-4A");
			PushMacAddress(L"00-21-5A-A7-56-68");
			PushMacAddress(L"00-21-5A-A5-89-10");
			PushMacAddress(L"00-21-5A-AA-18-8E");
			PushMacAddress(L"00-21-5A-AB-89-04");
			PushMacAddress(L"00-23-8B-64-78-4B");
			PushMacAddress(L"00-23-8B-64-78-59");
			PushMacAddress(L"00-1F-29-CA-BC-68");
			PushMacAddress(L"00-21-5A-A9-BE-4E");
			PushMacAddress(L"00-21-5A-AB-BE-84");
			PushMacAddress(L"00-21-5A-A7-56-62");
			PushMacAddress(L"00-21-5A-A5-BA-38");
			PushMacAddress(L"00-21-5A-AA-18-8C");

			//추가 r1
			PushMacAddress(L"00-22-19-52-F2-FD");//ext
			PushMacAddress(L"00-22-19-55-72-B7");//in
			PushMacAddress(L"00-22-19-55-72-B5");//ext
			PushMacAddress(L"00-22-19-55-7D-39");//in
			PushMacAddress(L"00-22-19-55-7D-37");//ext
			//추가 r2
			PushMacAddress(L"00-22-19-54-B2-3D");//ext
		}break;
	case LOCAL_MGR::NC_CHINA:
		{
			setInternal(); // 바른손 내부 테스트 서버

			// 테스트 서버 public
			PushMacAddress(L"00-1A-64-CE-96-CA");
			PushMacAddress(L"00-1A-64-50-F8-B0");
			PushMacAddress(L"00-1A-64-50-F9-34");
			PushMacAddress(L"00-1A-64-50-FB-44");
			PushMacAddress(L"00-1A-64-CE-10-DE");
			PushMacAddress(L"00-1A-64-CE-1A-2C");
			PushMacAddress(L"00-1A-64-CE-18-A6");
			PushMacAddress(L"00-1A-64-CE-14-6E");

			// 테스트 서버 private
			PushMacAddress(L"00-1A-64-CE-96-C8");
			PushMacAddress(L"00-1A-64-50-F8-AE");
			PushMacAddress(L"00-1A-64-50-F9-32");
			PushMacAddress(L"00-1A-64-50-FB-42");
			PushMacAddress(L"00-1A-64-CE-10-DC");
			PushMacAddress(L"00-1A-64-CE-1A-2A");
			PushMacAddress(L"00-1A-64-CE-18-A4");
			PushMacAddress(L"00-1A-64-CE-14-6C");

			// 전신1구 public
			PushMacAddress(L"00-1A-64-CE-0F-70");
			PushMacAddress(L"00-1A-64-CE-0C-B8");
			PushMacAddress(L"00-1A-64-CE-16-9C");
			PushMacAddress(L"00-1A-64-CE-16-3C");
			PushMacAddress(L"00-1A-64-CE-0E-08");
			PushMacAddress(L"00-22-19-8A-D4-DA");
			PushMacAddress(L"00-22-19-8A-E3-47");
			PushMacAddress(L"00-1A-64-CE-92-68");
			PushMacAddress(L"00-18-8B-80-AD-53");
			PushMacAddress(L"00-1A-64-CE-11-62");
			PushMacAddress(L"00-1A-64-CE-0E-20");
			PushMacAddress(L"00-1A-64-CE-0E-C2");
			PushMacAddress(L"00-1A-64-CE-18-6A");

			// 전신1구 private
			PushMacAddress(L"00-1A-64-CE-0F-6E");
			PushMacAddress(L"00-1A-64-CE-0C-B6");
			PushMacAddress(L"00-1A-64-CE-16-9A");
			PushMacAddress(L"00-1A-64-CE-16-3A");			
			PushMacAddress(L"00-1A-64-CE-92-66");

			// 전신2구 public
			PushMacAddress(L"00-1A-64-CE-14-D4");
			PushMacAddress(L"00-1A-64-CE-96-FA");
			PushMacAddress(L"00-1A-64-CE-8F-50");
			PushMacAddress(L"00-1A-64-CE-19-42");
			PushMacAddress(L"00-1A-64-CE-8D-B2");
			PushMacAddress(L"00-22-19-8A-D4-DF");
			PushMacAddress(L"00-22-19-88-87-92");
			PushMacAddress(L"00-1A-64-8D-34-10");
			PushMacAddress(L"00-1A-64-CE-0C-28");
			PushMacAddress(L"00-1A-64-CE-8C-D4");
			PushMacAddress(L"00-1A-64-50-F9-76");
			PushMacAddress(L"00-1A-64-CE-0F-AC");
			PushMacAddress(L"00-1A-64-CE-14-98");
			PushMacAddress(L"00-1A-64-CE-0D-D8");
			PushMacAddress(L"00-22-19-8A-D6-3F");
			PushMacAddress(L"00-22-19-88-85-9C");
			PushMacAddress(L"00-1A-64-CE-11-7A");
			PushMacAddress(L"00-22-19-8A-D5-E5");
			PushMacAddress(L"00-1A-64-CE-0B-2C");
			PushMacAddress(L"00-1A-64-CE-1B-F4");
			PushMacAddress(L"00-1A-64-CE-0F-A6");

			// 전신2구 private
			PushMacAddress(L"00-1A-64-CE-14-D2");
			PushMacAddress(L"00-1A-64-CE-96-F8");
			PushMacAddress(L"00-1A-64-CE-8F-4E");
			PushMacAddress(L"00-1A-64-CE-19-40");			
			PushMacAddress(L"00-1A-64-8D-34-0E");
			PushMacAddress(L"00-1A-64-CE-0C-26");
			PushMacAddress(L"00-1A-64-CE-8C-D2");
			PushMacAddress(L"00-1A-64-50-F9-74");
			PushMacAddress(L"00-1A-64-CE-0F-AA");
			PushMacAddress(L"00-1A-64-CE-14-96");			
			PushMacAddress(L"00-1A-64-CE-11-78");			

			// 왕통1구 public
			PushMacAddress(L"00-19-B9-E7-2E-CE");
			PushMacAddress(L"00-19-B9-E7-2E-97");
			PushMacAddress(L"00-1D-09-64-95-CA");
			PushMacAddress(L"00-19-B9-DF-51-6F");
			PushMacAddress(L"00-19-B9-F4-0B-CA");
			PushMacAddress(L"00-22-19-8A-D0-6C");
			PushMacAddress(L"00-22-19-88-85-AB");
			PushMacAddress(L"00-19-B9-E7-65-55");
			PushMacAddress(L"00-22-19-8A-D3-2A");
			PushMacAddress(L"00-19-B9-E7-56-BD");
			PushMacAddress(L"00-19-B9-E7-CB-0E");
			PushMacAddress(L"00-22-19-50-97-E6");
			PushMacAddress(L"00-19-B9-E7-53-89");

			// 왕통2구 private
			PushMacAddress(L"00-19-B9-E7-2E-CC");
			PushMacAddress(L"00-19-B9-E7-2E-95");
			PushMacAddress(L"00-1D-09-64-95-C8");
			PushMacAddress(L"00-19-B9-DF-51-6D");			
			PushMacAddress(L"00-19-B9-E7-65-53");			
			PushMacAddress(L"00-19-B9-E7-53-87");
		}break;
	default:
		{
		}break;
	}
}

void PgCheckMacAddress::PushMacAddress(std::wstring const& szAdd)
{
	std::wstring::size_type spos = szAdd.find(L":");
	if(spos != std::wstring::npos)
	{
		__asm int 3;//들어오면 안됨
	}
	spos = szAdd.find(L"-");
	if(spos == std::wstring::npos)
	{
		__asm int 3;//들어오면 안됨
	}

	std::wstring uprAdd(szAdd);
	UPR(uprAdd);

	m_kContMacAddr.insert(uprAdd);
}

bool PgCheckMacAddress::CheckMacAddress()
{
	switch( g_kLocal.ServiceRegion() )
	{
	case LOCAL_MGR::NC_VIETNAM:
		{
			return true;
		}break;
	}

	if( m_kContMacAddr.empty() )
	{
		return false;	// 실제 서비스 국가
	}

	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	PIP_ADAPTER_INFO pInfo = NULL;
	DWORD dwSize = 0;

	if( ERROR_BUFFER_OVERFLOW == GetAdaptersInfo( pAdapterInfo, &dwSize ) )
	{	
		pAdapterInfo = (PIP_ADAPTER_INFO) malloc( dwSize );
		GetAdaptersInfo( pAdapterInfo, &dwSize );
	}

	if( NO_ERROR == GetAdaptersInfo( pAdapterInfo, &dwSize ) )
	{
		pInfo = pAdapterInfo;
		while( pInfo ) // 모든 Mac Address search
		{
			wchar_t kTemp[255] = {0,};
			wsprintf( kTemp, L"%02x-%02x-%02x-%02x-%02x-%02x",
				pInfo->Address[0],
				pInfo->Address[1],
				pInfo->Address[2],
				pInfo->Address[3],
				pInfo->Address[4],
				pInfo->Address[5] );

			std::wstring kMacAddress(kTemp);
			UPR(kMacAddress);			
			CONT_MACADDR::const_iterator mac_iter = m_kContMacAddr.find( kMacAddress );
			if( mac_iter != m_kContMacAddr.end() )
			{
				free( pAdapterInfo );
				return true;
			}

			pInfo = pInfo->Next;
		}
	}

	free( pAdapterInfo );
	return false;
}