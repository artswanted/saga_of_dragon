#ifndef MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGBSTIMEEVENT_H
#define MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGBSTIMEEVENT_H

//
template< typename _ENUM_T >
struct STimeEvent
{
	STimeEvent(_ENUM_T const& reType, BM::GUID const& rkCharGuid, DWORD const& rdwTickBase)
		: eType(reType), kCharGuid(rkCharGuid), dwTickBase(rdwTickBase), dwCurEleapsedTime(0)
	{
	}
	STimeEvent(STimeEvent const& rhs)
		: eType(rhs.eType), kCharGuid(rhs.kCharGuid), dwTickBase(rhs.dwTickBase), dwCurEleapsedTime(rhs.dwCurEleapsedTime)
	{
	}

	bool operator ==(STimeEvent const& rhs) const
	{
		return (eType == rhs.eType) && (kCharGuid == rhs.kCharGuid);
	}
	bool Tick(DWORD const& rkEleapsedTime)
	{
		dwCurEleapsedTime += rkEleapsedTime;
		if( dwTickBase <= dwCurEleapsedTime )
		{
			dwCurEleapsedTime -= dwTickBase;
			return true;
		}
		return false;
	}

	_ENUM_T eType;
	BM::GUID kCharGuid;
	DWORD const dwTickBase;
	DWORD dwCurEleapsedTime;
};

//
template< typename _ENUM_T >
class PgTimeEventMgr
{
	typedef std::list< STimeEvent< _ENUM_T > > CONT_TIME_EVENT;
	typedef PgTimeEventMgr< _ENUM_T > _INHERIT_TYPE;
public:
	PgTimeEventMgr()
		: m_kContTimeEvent()
	{
	}
	~PgTimeEventMgr()
	{
	}

	typedef std::map<BM::GUID, int> CONT_EVENTCOUNT;
	void Clear()
	{
		m_kContTimeEvent.clear();
	}


	void Add(BM::GUID const& rkCharGuid, _ENUM_T const eType, DWORD const dwTick)
	{
		m_kContTimeEvent.push_back( CONT_TIME_EVENT::value_type(eType, rkCharGuid, dwTick) );
	}
	void DelAll(BM::GUID const& rkCharGuid)
	{
		CONT_TIME_EVENT::iterator find_iter = m_kContTimeEvent.begin();
		while( m_kContTimeEvent.end() != find_iter )
		{
			if( (*find_iter).kCharGuid == rkCharGuid )
			{
				find_iter = m_kContTimeEvent.erase( find_iter );
			}
			else
			{
				++find_iter;
			}
		}
	}
	void DelType(BM::GUID const& rkCharGuid, _ENUM_T const eType)
	{
		CONT_TIME_EVENT::iterator find_iter = m_kContTimeEvent.begin();
		while( m_kContTimeEvent.end() != find_iter )
		{
			if( (*find_iter).eType == eType
			&&	(*find_iter).kCharGuid == rkCharGuid )
			{
				find_iter = m_kContTimeEvent.erase( find_iter );
			}
			else
			{
				++find_iter;
			}
		}
	}
	template< typename _T_TickFunc >
	void Tick(DWORD const dwEleapsedTime, _T_TickFunc kEventFunc)
	{
		CONT_TIME_EVENT::iterator iter = m_kContTimeEvent.begin();
		while( m_kContTimeEvent.end() != iter )
		{
			CONT_TIME_EVENT::value_type& rkTimeEvent = (*iter);
			if( rkTimeEvent.Tick( dwEleapsedTime ) )
			{
				if( kEventFunc(rkTimeEvent) )
				{
					iter = m_kContTimeEvent.erase( iter ); // 삭제
					continue;
				}
			}
			++iter;
		}
	}

	template< typename T_OUT_SET >
	void CountEvent(_ENUM_T const eType, T_OUT_SET& rkOut) const
	{
		CONT_TIME_EVENT::const_iterator itor_event = m_kContTimeEvent.begin();
		while(m_kContTimeEvent.end() != itor_event)
		{
			CONT_TIME_EVENT::value_type const& rkTimeEvent = (*itor_event);
			if (rkTimeEvent.eType == eType)
			{
				T_OUT_SET::iterator itor_out = rkOut.find(rkTimeEvent.kCharGuid);
				if (itor_out == rkOut.end())
				{
					rkOut.insert(std::make_pair(rkTimeEvent.kCharGuid, 1));
				}
				else
				{
					(*itor_out).second++;
				}
			}
			++itor_event;
		}
	}

private:
	CONT_TIME_EVENT m_kContTimeEvent;
};


//
typedef enum eBattleSquareTimeEventType
{
	BSTET_REVIVE		= 1,
	BSTET_TEAM_POINT	= 2,
} EBattleSquareTimeEventType;

//
class PgBSTimeEventMgr : public PgTimeEventMgr< EBattleSquareTimeEventType >
{
public:
	typedef std::map<BM::GUID, int> CONT_EVENTCOUNT;
	struct SBSTimeEventTickFunc
	{
		SBSTimeEventTickFunc(ContGuidSet& rkContRevive, ContGuidSet& rkContTeamPoint);
		bool operator ()(CONT_TIME_EVENT::value_type& rkTimeEvent);
	private:
		ContGuidSet& m_rkContRevive;
		ContGuidSet& m_rkContTeamPoint;
	};

public:
	PgBSTimeEventMgr();
	~PgBSTimeEventMgr();
};

#endif // MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGBSTIMEEVENT_H