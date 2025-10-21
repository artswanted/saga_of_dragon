#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGBSGAME_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGBSGAME_H

#include "bm/twrapper.h"

//
namespace BSGameUtil
{
	//
	class PgVerify
	{
	public:
		PgVerify();
		~PgVerify();

		bool Check();

	protected:
		bool CheckEffect(int const iItemNo, int const iEffectNo);
	private:
		CONT_DEFEFFECT const* m_pkDefEffect;
		CONT_DEFITEM const* m_pkDefItem;
	};

	//
	bool Q_DQT_LOAD_BS_GAME(CEL::DB_RESULT &rkResult);
	bool Q_DQT_BATTLE_SQUARE_COMMON(CEL::DB_RESULT &rkResult);

	//
	class PgBSStatusMng
	{
	public:
		PgBSStatusMng();
		~PgBSStatusMng();
		void Init();
		void Update(int const iGameIDX, int const iChannelNo, EBS_GAME_STATUS const eStatus);
		void Update(SBSGame const& rkBSGame, EBS_GAME_STATUS const eStatus);

	private:
		CLASS_DECLARATION_S_NO_SET(BM::GUID, SQLGuid);
	};
};

//
class PgBSContentsGameExt
	: public PgBSContentsGame
{
	class PgBSGameUserControl
	{
	public:
		PgBSGameUserControl();
		~PgBSGameUserControl();

		EBS_JOIN_RETURN Reserve(SBSGame const& rkGameInfo, EBS_GAME_STATUS const eStatus, BM::GUID const& rkGuid, int const iLevel);
		bool Join(BM::GUID const& rkGuid);
		void Leave(BM::GUID const& rkGuid);
		int CurUserCount() const;
		void ClearUser();
	private:
		ContGuidSet m_kContReserveUser;
		ContGuidSet m_kContPlayer;
	};

public:
	PgBSContentsGameExt();
	explicit PgBSContentsGameExt(PgBSGame const& rhs);
	~PgBSContentsGameExt();

	using PgBSContentsGame::Status;
	void Status(EBS_GAME_STATUS const& eStatus, BSGameUtil::PgBSStatusMng& rkStatusMng);
	bool MakeTime(EBS_GAME_STATUS const eStatus, BSGameUtil::PgBSStatusMng& rkStatusMng, bool const bForce = false);

	EBS_JOIN_RETURN Reserve(BM::GUID const& rkGuid, int const iLevel)		{ return m_kUser.Reserve(m_kGameInfo, m_kStatus, rkGuid, iLevel); }
	bool Join(BM::GUID const& rkGuid)										{ return m_kUser.Join(rkGuid); }
	void Leave(BM::GUID const& rkGuid)										{ return m_kUser.Leave(rkGuid); }
	int CurUserCount() const												{ return m_kUser.CurUserCount(); }
	void ClearUser()														{ m_kUser.ClearUser(); }

private:
	PgBSGameUserControl m_kUser;
};
typedef std::list< PgBSContentsGameExt > CONT_BS_CONTENTS_GAME_EXT;


//
class PgBSGameMngImpl
{
	//
	class PgBSUserControl
	{
		typedef std::map< BM::GUID, int > CONT_GUID_IDX; // User Guid -> GameIDX
	public:
		PgBSUserControl();
		~PgBSUserControl();

		bool Add(BM::GUID const& rkGuid, int const iGameIDX);
		void Del(BM::GUID const& rkGuid);
		void DelGame(int const iGameIDX);
		int Get(BM::GUID const& rkGuid) const;

	private:
		CONT_GUID_IDX m_kContGuidIdx;
	};

	//
	class PgBSGroundGameMng
	{
		typedef std::map< int, int > CONT_BS_GND_GAME;
	public:
		PgBSGroundGameMng();
		~PgBSGroundGameMng();

		bool Add(PgBSContentsGame const& rkBSGame);
		bool Del(PgBSContentsGame const& rkBSGame);
		bool IsEmptyGround(PgBSContentsGame const& rkBSGame) const;
		bool IsGroundGame(PgBSContentsGame const& rkBSGame) const;
		int IsGroundGame(int const iGroundNo) const; // Ground -> IDX
	private:
		CONT_BS_GND_GAME m_kGndGame;
	};

public:
	PgBSGameMngImpl();
	~PgBSGameMngImpl();

	void OnTick();
	bool ProcessMsg(BM::Stream& rkPacket);

protected:
	void NfyBSGameInfo(PgBSContentsGameExt const& rkBSGame);
	void BuildTime(bool const bForce = false);
	void NfyAllNotice(PgBSContentsGame const& rkBSGame, BM::Stream& rkOut, EBS_GAME_STATUS const eTempStatus = BSGS_NONE);
	void NfyGndNotice(SYSTEMTIME const& rkNow, __int64 const iNowSecTime, PgBSContentsGame& rkBSGame, bool const bForce = false);

	void ProcessGMCommand(BM::Stream& rkPacket);

private:
	CONT_BS_CONTENTS_GAME_EXT m_kContBSGame;
	SYSTEMTIME m_kDateTime;
	BSGameUtil::PgBSStatusMng m_kStatusMng;
	PgBSGroundGameMng m_kGndGameMng;
	PgBSUserControl m_kUsers;
	ContGuidSet m_kContLoginedUser;
};


//
class PgBSGameMng : public TWrapper< PgBSGameMngImpl, Loki::Mutex >
{
public:
	PgBSGameMng();
	virtual ~PgBSGameMng();

	bool ProcessMsg(BM::Stream& rkPacket);
	void OnTick();
};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGBSGAME_H