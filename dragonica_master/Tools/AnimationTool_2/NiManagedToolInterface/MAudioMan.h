// Barunson Interactive Propritary Information

#pragma once

#include "MEventMan.h"

namespace NiManagedToolInterface
{
	public __gc class MAudioMan : public MEventMan
	{
	public:
		typedef std::map<std::string, NiAudioSourcePtr> EventContainer;

		MAudioMan();
		~MAudioMan();

		//! 오디오 시스템을 초기화한다.
		void Init();

		//! 오디오 시스템 종료
		bool ShutDown();

		//! 오디오 시스템을 생성한다.
		bool CreateAudioSystem(IntPtr hWnd);

		//! 음향을 얻는다.
		NiAudioSourcePtr GetAudioSource(const char *pcSoundName, float fVolume, float fDistMin, float fDistMax);

		//! 컨텐츠를 삭제한다.
		void DeleteContents();

	protected:
		EventContainer *m_pkEventContainer;
	};

}