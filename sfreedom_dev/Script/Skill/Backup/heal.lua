
function Skill_Heal_OnCheckCanEnter(actor,action)
	local myWeaponType = actor:GetEquippedWeaponType();

--	if myWeaponType ~= 9 then
--		return false
--	end
	
	if actor:IsMeetFloor() == false then
		return false
	end

	return true
end
function Skill_Heal_Fire(actor,action)
	actor:DetachFrom(126)
	actor:AttachParticle(127, "p_wp_l_hand", "ef_mag_spell_finish")

	if actor:IsMyActor() == true then

		local kTarget = CreateActionTargetList(actor:GetPilotGuid(), action:GetActionInstanceID(),action:GetActionNo())
		local iFound = Skill_Heal_OnFindTarget(actor,action,kTarget);

		if iFound > 0 then
			local kTargetInfo = kTarget:GetTargetInfo(0)
			local kTargetGUID = kTargetInfo:GetTargetGUID()
			local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
			if kTargetPilot:IsNil() then
				return true
			end
	
			local kTargetActor = kTargetPilot:GetActor()
			if kTargetActor:IsNil() then
				return true
			end
	
			local kUnitArray = NewUNIT_PTR_ARRAY()
			kUnitArray:AddUnit(kTargetPilot:GetUnit())
			local kActionResult = NewActionResultVector()
	
			-- Action Result 만듬
			action:Call_Skill_Result_Func(actor:GetPilot(), kUnitArray, kActionResult)
			local kResult = kActionResult:GetResult(kTargetGUID, false)
			if kResult:IsNil() == false and kResult:GetInvalid() == false then
				action:AddTarget(kTargetGUID, kTargetActor:GetHitObjectABVIndex(), kResult)
			end
	
			DeleteActionResultVector(kActionResult)
			DeleteUNIT_PTR_ARRAY(kUnitArray)
		end

		DeleteActionTargetList(kTarget)
	else
		GetActionResultCS(action, actor)
	end
end

function Skill_Heal_OnCastingCompleted(actor, action)

	Skill_Heal_Fire(actor,action);

	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function Skill_Heal_OnEnter(actor, action)
	local prevActionID = actor:GetAction():GetID()

	if prevActionID == "a_Heal" then
		action:SetParam(911, "end")
		action:SetDoNotBroadCast(true)
		return true 
	end

	actor:Stop()
	actor:AttachParticle(126, "p_wp_l_hand", "ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand")

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Heal_OnCastingCompleted(actor,action);
		
	return true
end

function Skill_Heal_OnFindTarget(actor,action,kTargets)
	local kParam = FindTargetParam()
	kParam:SetParam_1(actor:GetTranslate(), actor:GetLookingDir())
	kParam:SetParam_2(0, 30, action:GetSkillRange(0,actor), 0)
	kParam:SetParam_3(true, FTO_NORMAL)

	return	action:FindTargets(TAT_SPHERE, kParam, kTargets, kTargets);
end

function Skill_Heal_OnUpdate(actor, accumTime, frameTime)
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

function Skill_Heal_OnCleanUp(actor, action)
end

function Skill_Heal_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local nextActionID = action:GetID()
	local endParam = curAction:GetParam(911)

	if endParam == "end" then
		return true
	end

	return false	
end

function Skill_Heal_OnEvent(actor, textKey)

	if textKey == "hit" then
		Skill_Heal_DoEffect(actor)
	end

	return true
end

function Skill_Heal_DoEffect(actor)
	local action = actor:GetAction()
	local kGuid = actor:GetPilotGuid()

	local kHelperPos = actor:GetNodeWorldPos("char_root")
	local iTargetCount = action:GetTargetCount()
	
	actor:AttachSound(7283,"Heal");

	ODS("Heal's Target Count = " .. iTargetCount .. "\n")
	if iTargetCount > 0 then

		local kActionResult = action:GetTargetActionResult(0)
		local pt = actor:GetABVShapeWorldPos(action:GetTargetABVShapeIndex(0))

		action:ClearTargetList()
	end
end
