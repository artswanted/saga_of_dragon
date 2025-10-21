-- spawn
function Skill_Regain_OnCheckCanEnter(actor, action)
	return SkillFunc_OnCheckCanEnter(actor, action)
end

function Skill_Regain_OnEnter(actor, action)
	if SkillFunc_OnEnter(actor, action)  then
		if action:GetActionParam() ~= AP_CASTING then
			Skill_Regain_OnCastingCompleted(actor, action);
		end
		return true
	end
	return false
end

function Skill_Regain_OnCastingCompleted(actor, action)
	SkillFunc_OnCastingCompleted(actor, action)
	return true
end

function Skill_Regain_OnUpdate(actor, accumTime, frameTime)
	return SkillFunc_OnUpdate(actor, actor:GetAction(), accumTime, frameTime)	
end
function Skill_Regain_OnCleanUp(actor, action)
	return SkillFunc_OnCleanUp(actor, action)
end

function Skill_Regain_OnTargetListModified(actor, action, bIsBefore)
	return SkillFunc_OnTargetListModified(actor, action, bIsBefore)
end

function Skill_Regain_OnLeave(actor, action)
	return SkillFunc_OnLeave(actor, action)
end

function Skill_Regain_OnEvent(actor,textKey)
	return SkillFunc_OnEvent(actor,textKey)
end

function Skill_Regain_OnFindTarget(actor,action,kTargets)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	CustomUISummoner_TargetList(kTargets)
	return kTargets:size()
end