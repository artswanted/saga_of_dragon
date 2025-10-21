-- spawn
function Skill_bw_combo_Z_3_2_OnCheckCanEnter(actor, action)
	return SkillFunc_OnCheckCanEnter(actor, action)
end

function Skill_bw_combo_Z_3_2_OnEnter(actor, action)
	if SkillFunc_OnEnter(actor, action)  then 
		if action:GetActionParam() ~= AP_CASTING then
			Skill_bw_combo_Z_3_2_OnCastingCompleted(actor, action);
		end
		return true
	end
	return false
end

function Skill_bw_combo_Z_3_2_OnCastingCompleted(actor, action)
	SkillFunc_OnCastingCompleted(actor, action)
	return true
end

function Skill_bw_combo_Z_3_2_OnUpdate(actor, accumTime, frameTime)
	return SkillFunc_OnUpdate(actor, actor:GetAction(), accumTime, frameTime)	
end
function Skill_bw_combo_Z_3_2_OnCleanUp(actor, action)
	return SkillFunc_OnCleanUp(actor, action)
end

function Skill_bw_combo_Z_3_2_OnTargetListModified(actor, action, bIsBefore)
	return SkillFunc_OnTargetListModified(actor, action, bIsBefore)
end

function Skill_bw_combo_Z_3_2_OnLeave(actor, action)
	return SkillFunc_OnLeave(actor, action)
end

function Skill_bw_combo_Z_3_2_OnEvent(actor,textKey)
	return SkillFunc_OnEvent(actor,textKey)
end

function Skill_bw_combo_Z_3_2_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	local	iAttackRange = action:GetSkillRange(0,actor)
	
	local	kTargetPos = actor:GetLookingDir();
	kTargetPos:Multiply(iAttackRange);
	kTargetPos:Add(actor:GetPos());

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = iAttackRange
	end

	local kParam = FindTargetParam();
	kParam:SetParam_1(kTargetPos,Point3(0,0,-1));
	kParam:SetParam_2(0,0,iParam2,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	action:FindTargets(TAT_RAY_DIR_COLL_PATH,kParam,kTargets,kTargets);
	
	return	kTargets:size();
end