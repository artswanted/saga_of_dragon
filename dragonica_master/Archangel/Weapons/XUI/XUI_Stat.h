#ifndef WEAPON_XUI_XUI_STAT_H
#define WEAPON_XUI_XUI_STAT_H

#include "BM/Common.h"

namespace XUI
{
#ifdef CHECK_XUI_PERFORMANCE
#define XUI_STAT(x)	x;
#else
#define XUI_STAT(x)	__noop;
#endif

	typedef struct _XStatInfoF
	{
		_XStatInfoF() { iCount = 0; fTotalTime = 0.0f; fMaxTime = 0.0f; fMinTime = 1000.0f; }
		unsigned long iCount;	
		float fTotalTime;
		float fMaxTime;
		float fMinTime;
		float fLastTime;
	} XStatInfoF;

	typedef struct _XFrameStat
	{
		_XFrameStat() { iCountPerFrame = 0; iCurrentFrame = 0; fCurrentTotalTime = 0.0f; iMaxCount = 0; iLastCount = 0; }
		unsigned long iCountPerFrame;
		unsigned long iCurrentFrame;
		unsigned long iMaxCount;
		unsigned long iLastCount;
		float fCurrentTotalTime;
		XStatInfoF frameStat;
		XStatInfoF totalStat;
	} XFrameStat;

	void UpdateStatInfo(XStatInfoF& info, float fTime);
	void UpdateStatInfo(XStatInfoF* info, float fTime);	
	void PrintStatInfo(XStatInfoF& info, char const* header = NULL, char const* footer = NULL);

	void UpdateFrameCount(XFrameStat& info, unsigned long iFrame, float fTime);
	void XUI_PrintCSVHeader();
	void PrintFrameCount(XFrameStat& info, char const* header = NULL, char const* footer = NULL, float fGroupTotalTime = 0.0f);
	void PrintFrameCountForCSV(XFrameStat& info, char const* header = NULL, char const* footer = NULL, float fGroupTotalTime = 0.0f);

	inline void UpdateStatInfo(XStatInfoF& info, float fTime)
	{
	//#ifndef EXTERNAL_RELEASE
		info.iCount++;
		info.fTotalTime += fTime;
		if (fTime > info.fMaxTime)
			info.fMaxTime = fTime;
		else if (fTime < info.fMinTime)
			info.fMinTime = fTime;
		info.fLastTime = fTime;
	//#endif
	}

	inline void UpdateStatInfo(XStatInfoF* info, float fTime)
	{
	//#ifndef EXTERNAL_RELEASE
		if (info == NULL)
			return;

		info->iCount++;
		info->fTotalTime += fTime;
		if (fTime > info->fMaxTime)
			info->fMaxTime = fTime;
		else if (fTime < info->fMinTime)
			info->fMinTime = fTime;
		info->fLastTime = fTime;
	//#endif
	}

	inline void PrintStatInfo(XStatInfoF& info, char const* header, char const* footer)
	{
	//#ifndef EXTERNAL_RELEASE
		std::string headerString;
		std::string footerString;
		if (header)
			headerString = header;
		if (footer)
			footerString = footer;

		char buf[1024];

		sprintf_s(buf, 1024, "%s (%d times called, min %f, max %f, average %f, total %f, last %f) %s\n", headerString.c_str(), 
			info.iCount, info.iCount > 0 ? info.fMinTime : 0.0f, info.fMaxTime, info.iCount > 0 ? info.fTotalTime / (float)info.iCount : 0.0f, info.fTotalTime, info.fLastTime,
			footerString.c_str());
		OutputDebugStringA(buf);
	//#endif
	}

	inline void UpdateFrameCount(XFrameStat& info, unsigned long iFrame, float fTime)
	{
	//#ifndef EXTERNAL_RELEASE
		if (info.iCurrentFrame == iFrame)
		{
			info.iCountPerFrame++;
			info.fCurrentTotalTime += fTime;
		}
		else
		{
			if (info.fCurrentTotalTime < 2.0f)
			{
				UpdateStatInfo(info.frameStat, info.fCurrentTotalTime);
				if (info.frameStat.fMaxTime == info.fCurrentTotalTime)
				{
					info.iMaxCount = info.iCountPerFrame;
				}
			}
			info.iLastCount = info.iCountPerFrame;
			info.iCurrentFrame = iFrame;
			info.iCountPerFrame = 1;
			info.fCurrentTotalTime = fTime;
		}
		UpdateStatInfo(info.totalStat, fTime);
	//#endif
	}

	inline void PrintFrameCount(XFrameStat& info, char const* header, char const* footer, float fGroupTotalTime)
	{
		//#ifndef EXTERNAL_RELEASE
		std::string headerString;
		std::string footerString;
		if (header)
			headerString = header;
		if (footer)
			footerString = footer;

		char buf[1024];
		
		sprintf_s(buf, 1024, "%s Total(%d times called, min %f, max %f, average %f, total %f, last %f) Frame(%d times called, min %f, max %f(%d), average %f, total %f, last %f(%d)) %s\n", headerString.c_str(), 
			info.totalStat.iCount, info.totalStat.iCount > 0 ? info.totalStat.fMinTime : 0.0f, info.totalStat.fMaxTime, info.totalStat.iCount > 0 ? info.totalStat.fTotalTime / (float)info.totalStat.iCount : 0.0f, info.totalStat.fTotalTime, fGroupTotalTime > 0.0f ? info.totalStat.fTotalTime / fGroupTotalTime * 100.0f : 100.0f, info.totalStat.fLastTime,
			info.frameStat.iCount, info.frameStat.iCount > 0 ? info.frameStat.fMinTime : 0.0f, info.frameStat.fMaxTime, info.iMaxCount, info.frameStat.iCount > 0 ? info.frameStat.fTotalTime / (float)info.frameStat.iCount : 0.0f, info.frameStat.fTotalTime, info.frameStat.fLastTime, info.iLastCount,
			footerString.c_str());
		OutputDebugStringA(buf);
		//#endif
	}

	inline void XUI_PrintCSVHeader()
	{
		OutputDebugStringA("\n\ngroup, name, percentage(total), call(total), min time(total), max time(total), average(total), time(total), call(frame), min time(frame), max time(frame), max count(frame), average(frame), time(frame)\n");
	}

	inline void PrintFrameCountForCSV(XFrameStat& info, char const* header, char const* footer, float fGroupTotalTime)
	{
		std::string headerString;
		std::string footerString;
		if (header)
			headerString = header;
		if (footer)
			footerString = footer;

		char buf[1024];

		sprintf_s(buf, 1024, "%s, %s, %.2f, %d, %f, %f, %f, %f, %d, %f, %f, %d, %f, %f\n", headerString.c_str(), footerString.c_str(), 
			fGroupTotalTime > 0.0f ? info.totalStat.fTotalTime / fGroupTotalTime * 100.0f : 100.0f, info.totalStat.iCount, info.totalStat.iCount > 0 ? info.totalStat.fMinTime : 0.0f, info.totalStat.fMaxTime, info.totalStat.iCount > 0 ? info.totalStat.fTotalTime / (float)info.totalStat.iCount : 0.0f, info.totalStat.fTotalTime,
			info.frameStat.iCount, info.frameStat.iCount > 0 ? info.frameStat.fMinTime : 0.0f, info.frameStat.fMaxTime, info.iMaxCount, info.frameStat.iCount > 0 ? info.frameStat.fTotalTime / (float)info.frameStat.iCount : 0.0f, info.frameStat.fTotalTime);
		OutputDebugStringA(buf);
	}


	class XUIStatTimerF
	{
	public:
		XUIStatTimerF(XFrameStat* info, unsigned long iFrame)
		{
			if (info)
				pStatInfo = info;
			else
				pStatInfo = NULL;
			this->iFrame = iFrame;
			Start();
		}
		~XUIStatTimerF()
		{
			Stop();
		}

		void Start()
		{
			bStart = true;
			fTime = BM::GetTime32();
		}

		void Stop()
		{
			if (bStart && pStatInfo)
			{
				UpdateFrameCount(*pStatInfo, iFrame, (BM::GetTime32() - fTime) / 1000.0f);
				bStart = false;
			}
		}

	protected:
		DWORD fTime;
		bool bStart;
		unsigned long iFrame;
		XFrameStat* pStatInfo;
	};

	class XUIStatGroup
	{
	public:
		XUIStatGroup()
		{
		}
		
		XUIStatGroup(char* groupName)
		{
			if (groupName)	
				m_kStatGroupName = groupName;
		}

		XUIStatGroup(std::string groupName)
		{
			m_kStatGroupName = groupName;
		}

		~XUIStatGroup()
		{
			m_kStatInfoList.clear();
		}

		XFrameStat& GetStatInfo(std::string statName)
		{
			StatGroupMap::iterator iter = m_kStatInfoList.find(statName);
			if (iter == m_kStatInfoList.end())
			{
				auto ret = m_kStatInfoList.insert(std::make_pair(statName, XFrameStat()));
				iter = ret.first;
			}
			return iter->second;
		}

		void UpdateStat(std::string statName, float fTime, unsigned long iFrame)
		{
			UpdateFrameCount(GetStatInfo(statName), iFrame, fTime);
		}

		void PrintStatGroup()
		{
			StatGroupMap::iterator iter = m_kStatInfoList.begin();
			float totalTime = 0.0f;
			while (iter != m_kStatInfoList.end())
			{
				totalTime += iter->second.totalStat.fTotalTime;
				++iter;
			}

			iter = m_kStatInfoList.begin();
			while (iter != m_kStatInfoList.end())
			{
				PrintFrameCount(iter->second, m_kStatGroupName.c_str(), iter->first.c_str(), totalTime);
				++iter;
			}
		}

		void PrintStatGroupForCSV()
		{
			StatGroupMap::iterator iter = m_kStatInfoList.begin();
			float totalTime = 0.0f;
			while (iter != m_kStatInfoList.end())
			{
				totalTime += iter->second.totalStat.fTotalTime;
				++iter;
			}

			XUI_PrintCSVHeader();
			iter = m_kStatInfoList.begin();
			while (iter != m_kStatInfoList.end())
			{
				PrintFrameCountForCSV(iter->second, m_kStatGroupName.c_str(), iter->first.c_str(), totalTime);
				++iter;
			}
		}

	protected:
		typedef std::map<std::string, XFrameStat> StatGroupMap;
		StatGroupMap m_kStatInfoList;
		std::string m_kStatGroupName;
	};
}

#endif // WEAPON_XUI_XUI_STAT_H