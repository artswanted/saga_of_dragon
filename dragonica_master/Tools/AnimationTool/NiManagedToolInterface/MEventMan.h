// Barunson Interactive Propritary Information

#pragma once

using namespace System::Collections;

namespace NiManagedToolInterface
{
	public __gc class MEventMan
	{
	public:
		MEventMan();
		~MEventMan();

		// TODO : vector로 바꾸기.. 귀찮아서 일단.. --;
		typedef std::map<std::string, __int64> PathXmlContainer;
		typedef std::map<std::string, std::string> PathContainer;

		//! 이벤트 시스템을 초기화한다.
		virtual void Init() {}

		//! 이벤트 시스템 종료
		virtual bool ShutDown() { return true;}

		//! 지정한 파일을 파싱하여, 이벤트 경로를 추가한다.
		bool AddEventItems(String *pcPath, String *pkReqTag);

		//! 이벤트 경로 리스트를 얻는다.
		ArrayList *GetEventPathList();

		//! 이벤트의 경로를 얻는다.
		const char *GetEventPath(const char *pcEventName);

		//! 컨텐츠를 모두 삭제한다.
		void DeleteContents();

		//! 마지막에 추가한 Event Xml의 경로를 반환한다.
		String *GetLastEventPath();

	protected:

		String *m_pkLastAddedPath;
		PathContainer *m_pkPathContainer;
		PathXmlContainer *m_pkPathXmlContainer;
	};

}