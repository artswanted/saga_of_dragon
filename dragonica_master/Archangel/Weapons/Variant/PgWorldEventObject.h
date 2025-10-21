#ifndef WEAPON_VARIANT_WORLDEVENT_PGWORLDEVENTOBJECT_H
#define WEAPON_VARIANT_WORLDEVENT_PGWORLDEVENTOBJECT_H

//
class PgWEClientObjectState
{
public:
	explicit PgWEClientObjectState();
	explicit PgWEClientObjectState(PgWEClientObjectState const& rhs);
	virtual ~PgWEClientObjectState();

	virtual bool SetStatus(int const iNewState);
	virtual bool SetShow(bool const bShow);
	virtual bool Parse(TiXmlElement const* pkNode);

	void Update(PgWEClientObjectState const& rhs);

	void WriteToPacket(BM::Stream& rkPacket) const;
	bool ReadFromPacket(BM::Stream& rkPacket);

	CLASS_DECLARATION_S_NO_SET(WORD, ID);
	CLASS_DECLARATION_S_NO_SET(int, State);
	CLASS_DECLARATION_S_NO_SET(__int64, SyncTime);
	CLASS_DECLARATION_S_NO_SET(bool, Show);

protected:
	virtual void OnStateChange();

private:
	void operator =(PgWEClientObjectState const& rhs);
private:
	mutable Loki::Mutex m_kMutex;
};



//
template< typename _ObjectState >
class PgWEClientObjectBaseMgr
{
	typedef std::map< WORD, _ObjectState > ContWEObjectState;
protected:
	PgWEClientObjectBaseMgr()
	{
	}
	virtual ~PgWEClientObjectBaseMgr()
	{
	}

public:
	bool ParseWEClientObjectList(TiXmlElement const* pkListElementNode)
	{
		std::string const kWEListElementName( "WORLDEVENT_OBJECT_LIST" );
		if( kWEListElementName != pkListElementNode->Value() )
		{
			return false;
		}

		bool bRet = true;
		TiXmlElement const* pkWEClientObjectNode = pkListElementNode->FirstChildElement();
		while( pkWEClientObjectNode )
		{
			bRet = ParseWEClientObject(pkWEClientObjectNode) && bRet;
			pkWEClientObjectNode = pkWEClientObjectNode->NextSiblingElement();
		}
		return bRet;
	}

protected:
	virtual bool ParseWEClientObject(TiXmlElement const* /*pkWEObjectNode*/)
	{
		return true;
	}

	ContWEObjectState m_kContState;
};


//
class PgWEClientObjectServerMgr : private PgWEClientObjectBaseMgr< PgWEClientObjectState >
{
	typedef PgWEClientObjectBaseMgr< PgWEClientObjectState > _MyBaseType;
protected:
	PgWEClientObjectServerMgr();
	virtual ~PgWEClientObjectServerMgr();

	void ClonseWEClientOjbect(PgWEClientObjectServerMgr& rkTo) const;

	bool SetWEClientObjectState(WORD const kID, int const iNewState);
	bool GetWEClientObjectState(WORD const kID, PgWEClientObjectState const* &pkOut);
	bool SetWEClientObjectShow(WORD const kID, bool const bShow);

	virtual void NfyWEClientOjbectToGroundUser(PgWEClientObjectState const& rkState); // PgGround에서 구현
	void WEClientObjectWriteToPacket(BM::Stream& rkPacket) const;

	virtual bool ParseWEClientObject(TiXmlElement const* pkWEObjectNode);
public:
	bool ParseWEClientObjectList(TiXmlElement const* pkListElementNode);
};

#endif // WEAPON_VARIANT_WORLDEVENT_PGWORLDEVENTOBJECT_H