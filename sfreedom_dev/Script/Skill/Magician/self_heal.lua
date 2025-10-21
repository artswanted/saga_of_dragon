
function Skill_Self_Heal_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;

end
function Skill_Self_Heal_Fire(actor,action)
	actor:DetachFrom(126)
	actor:AttachParticle(127, "p_wp_l_hand", "ef_mag_spell_finish")
	actor:AttachParticle(9125, "char_root", "ef_skill_heal_self")

    action:CreateActionTargetList(actor);
    
end

function Skill_Self_Heal_OnCastingCompleted(actor, action)

	Skill_Self_Heal_Fire(actor,action);

	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	
end

function Skill_Self_Heal_OnEnter(actor, action)
	local prevActionID = actor:GetAction():GetID()

	if prevActionID == "a_Self Heal" then
		action:SetParam(911, "end")
		action:SetDoNotBroadCast(true)
		return true 
	end

	actor:Stop()

	actor:AttachParticle(126, "p_wp_l_hand", "ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand")

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Self_Heal_OnCastingCompleted(actor,action);	
	return true
end
function Skill_Self_Heal_OnFindTarget(actor,action,kTargets)
	local kParam = FindTargetParam()
	kParam:SetParam_1(actor:GetTranslate(), actor:GetLookingDir())
	kParam:SetParam_2(action:GetSkillRange(0,actor), 30, 0, 0)
	kParam:SetParam_3(true, FTO_NORMAL)

	return	action:FindTargets(TAT_RAY_DIR, kParam, kTargets, kTargets);
end

function Skill_Self_Heal_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	local bIsAnimDone = actor:IsAnimationDone()
	local nextActionID = action:GetNextActionName()
	local endParam = action:GetParam(911)
	local iSlot = action:GetCurrentSlot()

	if endParam == "end" then
		return false
	end

	if bIsAnimDone == true then
		if iSlot == 1 and nextActionID == "a_idle" then
			action:SetParam(911, "end")
			return false
		end
	end

	return true
end

function Skill_Self_Heal_OnCleanUp(actor, action)
end

function Skill_Self_Heal_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local nextActionID = action:GetID()
	local endParam = curAction:GetParam(911)

	if endParam == "end" then
		return true
	end

	return false	
end

function Skill_Self_Heal_OnEvent(actor, textKey)

	if textKey == "hit" or textKey == "fire" then
		Skill_Self_Heal_DoEffect(actor)
	end

	return true
end

function Skill_Self_Heal_DoEffect(actor)
	local action = actor:GetAction()
	local kGuid = actor:GetPilotGuid()

	local kHelperPos = actor:GetNodeWorldPos("char_root")
	local iTargetCount = action:GetTargetCount()
	
	actor:AttachSound(7283,"SelfHeal");

	-- TargetCount는 자기 자신뿐이다.
	ODS("Heal's Target Count = " .. iTargetCount .. "\n")
	if iTargetCount > 0 then

		local kActionResult = action:GetTargetActionResult(0)
		local pt = actor:GetABVShapeWorldPos(action:GetTargetABVShapeIndex(0))

		action:ClearTargetList()
	end
end
