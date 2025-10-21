#pragma once

#include "Lohengrin/GMCommand.h"
#include "Variant/PgEmporia.h"
#include "PgTask_Contents.h"
#include "PgEmporiaPack.hpp"

class PgEmporiaMgr
{
public:
	typedef PgEmporiaPack< MAX_EMPORIA_GRADE >		T_EMPORIA_PACK;
	typedef std::map< BM::GUID, T_EMPORIA_PACK* >	CONT_EMPORIA_PACK;// first: EmporiaID

public:
	PgEmporiaMgr();
	~PgEmporiaMgr();

	static bool LoadEmporiaBattleTime(void);

	// Database
	void LoadDB(void)const;
	bool Locked_Q_DQT_LOAD_EMPORIA( CEL::DB_RESULT &rkResult );
	bool Locked_Q_DQT_CREATE_EMPORIA( CEL::DB_RESULT &rkResult );
	void Locked_Q_DQT_SAVE_EMPORIA(CEL::DB_RESULT &rkResult);
	HRESULT Locked_Q_DQT_SWAP_EMPORIA( CEL::DB_RESULT_TRAN &rkContResult );

	bool Locked_Build( CONT_EMPORIA_PACK & rkContEmporiaPack, CONT_EMPORIA_FROMGUILD &kContEmporiaFromGuild );
	void Locked_OnTick(void);
	void Locked_ConnectPublicCenter( bool const bConnect );

	void Locked_GetEmporiaInfo( BM::GUID const &kGuildID, SGuildEmporiaInfo& rkEmporiaInfo )const;
	BYTE Locked_GetEmporiaKey( SEmporiaKey const &kEmporiaKey, BM::GUID const &kGuildGuid, SGroundKey &kOutGndKey )const;
	BYTE Locked_GetEmporiaKeyFromMercenary( BM::GUID const &kEmporiaID, BM::GUID const &kGuildGuid, BM::GUID const &kCharGuid, PgLimitClass const &kClassInfo, size_t const iJoinIndex, bool const bIsAttack, SGroundKey &kOutGndKey )const;

//	Msg
	bool ProcessMsg(SEventMessage *pkMsg);
	bool Locked_RecvGMCommand( EGMCmdType const eCommandType, BM::Stream * const pkPacket );
	void Locked_Recv_PT_N_N_REQ_EMPORIA_BATTLE_RESERVE( BM::Stream * const pkPacket );
	bool Locked_Recv_PT_T_N_ANS_CREATE_PUBLICMAP( BM::Stream * const pkPacket );
	void Locked_Recv_PT_T_N_ANS_DELETE_PUBLICMAP( BM::Stream * const pkPacket );
	bool Locked_Recv_PT_M_N_NFY_EMPORIA_BATTLE_RESULT( BM::Stream * const pkPacket );
	void Locked_Recv_PT_N_N_NFY_GUILD_REMOVE( BM::Stream * const pkPacket );
	void Locked_Recv_PT_M_N_NFY_EMPORIA_BATTLE_USERCOUNT_FULL( BM::Stream * const pkPacket );
	void Locked_Recv_PT_M_N_REQ_EMPORIA_PORTAL_INFO( BM::Stream * const pkPacket );
	void Locked_Recv_PT_N_N_ANS_GET_SETTING_MERCENARY( BM::Stream * const pkPacket );
	void Locked_Recv_PT_M_N_NFY_GAME_READY( BM::Stream * const pkPacket );
	void Locked_Recv_Administrator( BM::Stream * const pkPacket, bool const bAdmin );

private:
	void Release( CONT_EMPORIA_PACK &rkContEmporiaPack );
	bool CloseEmporia( BM::GUID const &kEmporiaID, bool const bDelete );

	bool IsEmporiaHaveGuild( BM::GUID const &kGuildID )const;
	bool GetEmporiaHaveGuild( BM::GUID const &kGuildID, SEmporiaHaveInfo &rkOutInfo )const;

	T_EMPORIA_PACK* GetEmporiaPack( BM::GUID const &kID )const;
	T_EMPORIA_PACK* GetEmporiaPackByGuild( BM::GUID const &kGuild, size_t &iOutIndex )const;

private:
	mutable ACE_RW_Thread_Mutex		m_kMutex;
	CONT_EMPORIA_PACK				m_kContEmporiaPack;
	CONT_EMPORIA_FROMGUILD			m_kContEmporiaFromGuild;
};

#define g_kEmporiaMgr SINGLETON_STATIC(PgEmporiaMgr)

