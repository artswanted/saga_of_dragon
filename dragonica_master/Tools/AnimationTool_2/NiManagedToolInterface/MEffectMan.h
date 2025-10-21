// Barunson Interactive Propritary Information

#pragma once
#include "MEventMan.h"

namespace NiManagedToolInterface
{
	public __gc class MEffectMan : public MEventMan
	{
	public:
		__nogc class PgEffect
		{
		public:
			PgEffect();
			PgEffect(NiAVObjectPtr spEffect);
			~PgEffect();

			NiAVObject *GetEffectRoot();
			void SetEffectRoot(NiAVObject *pkParitlce);

			float GetEndTime();
			void SetEndTime(float fTime);

			float GetBeginTime();
			void SetBeginTime(float fTime);

			bool GetLoop();
			void SetLoop(bool bLoop);

			//! 이펙트를 떼어내야 하는지 체크
			bool Expired(float fAccumTime);

			//! 이펙트를 끝내야 하는 시간을 알아낸다.
			void Analyze(NiObjectNET *pkRoot);

			//! Clone Function
			PgEffect *Clone();

		protected:
			NiAVObjectPtr m_spEffectRoot;
			bool m_bLoop;
			float m_fEndTime;
			float m_fBeginTime;
		};

		typedef std::list<PgEffect *> AttachedEffectSlot;
		typedef std::map<std::string, PgEffect *> EventContainer;

		MEffectMan();
		~MEffectMan();

		//! 이펙트 시스템을 초기화한다.
		void Init();

		//! 컨텐츠를 모두 삭제한다.
		void DeleteContents();

		//! 이펙트 시스템 종료
		bool ShutDown();

		//! 이펙트를 로딩한다.
		PgEffect *LoadEffect(const char *pcEffectPath);

		//! 이펙트를 얻는다.
		PgEffect *GetEffect(NiKFMTool::Effect *pkEffectData);

		//! 이펙트를 Target Point에 Attach한다.
		bool AttachTo(PgEffect *pkEffect, NiNode *pkTargetPoint);

		//! 이펙트를 Target Point 의 좌표에 Attach한다.
		bool AttachToPoint(PgEffect *pkEffect, NiNode *pkTargetPoint, bool bUseLock);

		//! 이펙트를 Detach한다.
		void DetachFrom(PgEffect *pkEffect);

		//! 모든 이펙트를 Detach한다.
		void DetachAllEffects();

		//! AttachedList에 있는 이펙트를 움직이거나, 때야 한다.
		void Update(float fFrameTime);

		//! AttachedEffectSlot에 PgEffect를 등록한다.
		void AddToAttachedEffectSlot(PgEffect *pkEffect);

		//! AttachedEffectSlot에 PgEffect를 등록해제한다.
		void RemoveFromAttachedEffectSlot(PgEffect *pkEffect);

	protected:
		EventContainer *m_pkEventContainer;
		AttachedEffectSlot *m_pkAttachedEffectSlot;
	};
}