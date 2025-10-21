#ifndef WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSION_H
#define WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSION_H

#include "Lohengrin/dbtables3.h"
#include "Variant/AlramMissionActionDef.h"

class PgAlramMission
{
public:
	enum eState
	{
		STATE_NONE,
		STATE_IDLE,
		STATE_CHECKTIME,
		STATE_PACKET,
		STATE_SUCCEED,
		STATE_FAILED,
	};

	enum eFailType
	{
		TYPE_SUCCESS			= 0,// 달성시 성공
		TYPE_FAIL				= 1,// 달성시 실패
		TYPE_FAIL2				= 2,// 달성시 실패, 달성안하면 성공
	};

	typedef VEC_INT		VEC_PARAMS;

public:
	static DWORD const ms_dwDelayTime = 1000;

public:
	PgAlramMission();
	~PgAlramMission();

	HRESULT Begin( TBL_DEF_ALRAM_MISSION const * pkDef, DWORD const dwBeginTime, bool const bIsRebegin=false );
	HRESULT OnTick( DWORD const dwCurTime );

	BYTE GetState()const{return m_byState;}
	void SetState( BYTE const byState ){m_byState = byState;}

	int GetID()const{return (m_pkDef ? m_pkDef->iAlramID : 0);}
	int GetNextID()const{return (m_pkDef ? m_pkDef->iAlramID_Next : 0);}
	DWORD GetDefTime()const{return (m_pkDef ? static_cast<DWORD>(m_pkDef->iTime) * 1000 : 0 );}
	DWORD GetBeginTime()const{return m_dwBeginTime;}
	int GetExtValue()const{return m_iExtValue;}
	TBL_DEF_ALRAM_MISSION const * const GetDef()const{return m_pkDef;}

	bool GetParam( size_t const iIndex, int &iOutCount )const;
	bool SetParam( size_t const iIndex, int const iCount );

	void WriteToPacket( BM::Stream &kPacket )const;
	void ReadFromPacket( BM::Stream &kPacket );

	HRESULT WriteToPacket_Client( BM::Stream &kPacket )const;

//	클라이언트에서만 사용
	size_t GetSuccessCount()const;

private:
	TBL_DEF_ALRAM_MISSION const *m_pkDef;
	BYTE						m_byState;
	int							m_iExtValue;
	DWORD						m_dwBeginTime;
	VEC_PARAMS					m_kParamList;
};

#endif // WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSION_H