-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [FIRE_SOUND_ID] : 스킬이 FIRE 될때 플레이 할 사운드 ID
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

-- Melee
function Skill_DemolitionCharge1_OnCheckCanEnter(actor,action)
	return 	Act_Melee_Default_OnCheckCanEnter(actor,action)
end
function Skill_DemolitionCharge1_OnCastingCompleted(actor, action)
	Act_Melee_Default_OnCastingCompleted(actor,action)
end
function Skill_DemolitionCharge1_Fire(actor,action)
	Act_Melee_Default_Fire(actor,action)
end

function Skill_DemolitionCharge1_OnEnter(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	actor:DetachFrom(123)
	return Act_Melee_Default_OnEnter(actor, action)
end

function Skill_DemolitionCharge1_OnUpdate(actor, accumTime, frameTime)
	return Act_Melee_Default_OnUpdate(actor, accumTime, frameTime)
end

function Skill_DemolitionCharge1_OnCleanUp(actor)
	return Act_Melee_Default_OnCleanUp(actor)
end

function Skill_DemolitionCharge1_OnLeave(actor, action)
	return Act_Melee_Default_OnLeave(actor, action)
end

function Skill_DemolitionCharge1_OnEvent(actor,textKey)
	return	Skill_DemolitionCharge_OnEvent(actor,textKey)
end

function Skill_DemolitionCharge1_OnTargetListModified(actor,action,bIsBefore)
	Act_Melee_Default_OnTargetListModified(actor,action,bIsBefore)
end
