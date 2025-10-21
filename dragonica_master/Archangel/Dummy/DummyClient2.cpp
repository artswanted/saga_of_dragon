#include "stdafx.h"
#include "DummyClient2.h"
#include "Network.h"

void DummyClient2::OnConnectFromLogin(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive)
{
}

void DummyClient2::OnDisconnectFromLogin(CEL::CSession_Base *pSession)
{

}

void DummyClient2::OnConnectFromSwitch(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive)
{

}

void DummyClient2::OnDisconnectFromSwitch(CEL::CSession_Base *pSession)
{

}

void CALLBACK DummyClient2::OnRecvFromLogin( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	unsigned short kPacketType;
	pkPacket->Pop(kPacketType);
	switch(kPacketType)
	{
	case PT_A_ENCRYPT_KEY:
		{
			std::vector<char> kKey;
			pkPacket->Pop(kKey);
			if(S_OK == pkSession->VSetEncodeKey(kKey))
			{
				// if(g_kNetwork.OnConnectFromLogin_sub(pkSession))
				// {
				// 	return;
				// }
				SClientTryLogin kCTL;
				kCTL.SetID( m_kID );
				kCTL.SetPW( m_kPW );
				kCTL.SetVersion( PACKET_VERSION_C, PACKET_VERSION_S );
				kCTL.PatchVersion(562949953421374);

				BM::Stream kPacket(PT_C_L_TRY_AUTH);
				kCTL.WriteToPacket(kPacket);
				SendToLogin(kPacket);
			}
			else
			{
				INFO_LOG( BM::LOG_LV4, _T("OnRecvFromLogin PT_A_ENCRYPT_KEY SetEncodeKey Failed"));
				pkSession->VTerminate();
			}
		}break;
	case PT_L_C_NFY_REALM_LIST:
		{
			BM::Stream kPacket(PT_C_L_REQ_CHANNEL_LIST);
			kPacket.Push((short)1);//렐름 번호 보냄.
			SendToLogin(kPacket);
		}break;
	case PT_L_C_NFY_CHANNEL_LIST:
		{
			const short nRealmNo = 1;
			const short nChannelNo = 1;
			BM::Stream kPacket(PT_C_L_TRY_LOGIN);
			kPacket.Push(nRealmNo);
			kPacket.Push(nChannelNo);
			SendToLogin(kPacket);
		}break;
	case PT_L_C_NFY_RESERVED_SWITCH_INFO:
		{// 100016 PT_L_C_NFY_RESERVED_SWITCH_INFO
			Sleep(50 + rand()%1000);

			SSwitchReserveResult kSRR;	
			kSRR.ReadFromPacket(*pkPacket);

			CEL::ADDR_INFO SwitchAddr;
			const struct in_addr addr = kSRR.addrSwitch.ip;
			std::string strIP = inet_ntoa(addr);

			SwitchAddr.Set(UNI(strIP), kSRR.addrSwitch.wPort );
			SwitchKey(kSRR.guidSwitchKey);

			_DETAIL_LOG INFO_LOG( BM::LOG_LV1, __FL__<<L"Switch Info Recved ["<<C2L(SwitchAddr)<<L"]");
			BM::GUID guid;

			m_kSwitchSessionKey.WorkerGuid(g_kNetwork.SwitchConnector());
			g_kNetwork.DoConnectSwitch(SwitchAddr, m_kSwitchSessionKey);
		}break;
	}
}

void CALLBACK DummyClient2::OnRecvFromSwitch(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	unsigned short kPacketType;
	pkPacket->Pop(kPacketType);
	switch(kPacketType)
	{
	case PT_A_ENCRYPT_KEY:
		{
			std::vector<char> kKey;
			pkPacket->Pop(kKey);
			
			if(S_OK == pkSession->VSetEncodeKey(kKey))
			{
				//스위치에 인증 해달라고 보냄.
				STryAccessSwitch kTAS;
				kTAS.SetVersion(PACKET_VERSION_C, PACKET_VERSION_S);
				kTAS.SetID( m_kID );
				kTAS.SetPW( m_kPW);
				kTAS.guidSwitchKey = SwitchKey();

				BM::Stream Packet(PT_C_S_TRY_ACCESS_SWITCH);
				kTAS.WriteToPacket(Packet);
				SendToSwitch( Packet );
			}
		}break;
	case PT_S_C_REQ_PING://핑에 캐릭 정보가 가는 이유는??
		{
			BM::GUID kGUID;
			pkPacket->Pop(kGUID);

			DWORD const dwTime = BM::GetTime32();//timegettime 으로 바꾸면, 시간이 5ms 내외에서 뒤로 돌아가거나 할 수도 있으니 주의.
			::GUID kTempGuid = kGUID;
			kTempGuid.Data1 ^= dwTime;//시간값 XOR 로 줌.

			BM::Stream kResPacket(PT_C_S_ANS_ONLY_PING, kTempGuid);
			SendToSwitch(kResPacket); 
		}break;
	case PT_S_C_TRY_ACCESS_SWITCH_RESULT:
		{
			BM::Stream kPacket(PT_C_S_REQ_CHARACTER_LIST);
			SendToSwitch(kPacket);
		}break;
	case PT_S_C_ANS_CHARACTER_LIST:
		{
			UNIT_PTR_ARRAY kUnitArray;
			kUnitArray.ReadFromPacket(*pkPacket);
			if(!kUnitArray.empty())
			{
				UNIT_PTR_ARRAY::iterator itor = kUnitArray.begin();
				const BM::GUID &kGuid = itor->pkUnit->GetID();
				BM::Stream kPacket(PT_C_S_REQ_SELECT_CHARACTER);
				kPacket.Push(kGuid);
				kPacket.Push(static_cast<bool>(false));

				SendToSwitch(kPacket);
			}
			else
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__<<L"No Character");
			}
		}break;
	case PT_T_C_ANS_SELECT_CHARACTER://진입불가 상태임
		{
			ESelectCharacterRet eCause;
			pkPacket->Pop(eCause);
			INFO_LOG(BM::LOG_LV0, __FL__<<L"Can't MapMove Cause["<<eCause<<L"]");
		}break;
	case PT_T_C_NFY_CHARACTER_MAP_MOVE://맵이동 완료
		{
			BYTE cMoveCause;
			SGroundKey kGroundKey;
			pkPacket->Pop(cMoveCause);
			pkPacket->Pop(kGroundKey);
			BM::Stream kPacket( PT_C_M_NFY_MAPLOADED, cMoveCause );
			kGroundKey.WriteToPacket(kPacket);
			SendToSwitch(kPacket);
		}break;
	}
}

bool DummyClient2::SendToLogin(const BM::Stream &rkPacket)const
{
	BM::CAutoMutex kLock(m_kMutex);
	g_kNetwork.SendToLogin(LoginSessionKey(), rkPacket);
	return true;
}

bool DummyClient2::SendToSwitch(const BM::Stream &rkPacket)const
{
	BM::CAutoMutex kLock(m_kMutex);
	g_kNetwork.SendToSwitch(SwitchSessionKey(), rkPacket);
	return true;
}
