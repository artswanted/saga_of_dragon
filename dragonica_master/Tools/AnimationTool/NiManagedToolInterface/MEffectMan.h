// Barunson Interactive Propritary Information

#pragma once
#include "MEventMan.h"
#include "PgEffect.H"

class	NiAnimationEventEffect;
namespace NiManagedToolInterface
{
	public __gc class MEffectMan : public MEventMan
	{
	public:


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
		PgEffect *GetEffect(NiAnimationEventEffect *pkEffectData);

		PgEffect *FindEffectWithTextKey(std::string const &kTextKey);

		//! 이펙트를 Target Point에 Attach한다.
		bool AttachTo(PgEffect *pkEffect, NiNode *pkTargetPoint);

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