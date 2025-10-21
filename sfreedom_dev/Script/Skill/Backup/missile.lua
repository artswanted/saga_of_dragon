

function Skill_Missile_OnCheckCanEnter(actor,action)
	local myWeaponType = actor:GetEquippedWeaponType();

--	if myWeaponType ~= 9 then
--		return false
--	end
	
	if actor:IsMeetFloor() == false then
		return false
	end

	return true
end
function Skill_Missile_Fire(actor,action)
	actor:DetachFrom(126)
	actor:AttachParticle(127, "p_wp_l_hand", "ef_mag_spell_finish")

	if actor:IsMyActor() == true then

		local kTargets = CreateActionTargetList(actor:GetPilotGuid(), action:GetActionInstanceID(),action:GetActionNo())
		local iTargetCount = Skill_Missile_OnFindTarget(actor,action,kTargets);
	
		if iTargetCount > 0 then
			local kTargetInfo = kTargets:GetTargetInfo(0)
			local kTargetGUID = kTargetInfo:GetTargetGUID()
			local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
			if kTargetPilot:IsNil() then
				return true
			end
	
			local kTargetActor = kTargetPilot:GetActor()
			if kTargetActor:IsNil() then
				return true
			end
	
			-- 타겟을 쳐다본다.
			local kTargetPt = kTargetActor:GetABVShapeWorldPos(action:GetTargetABVShapeIndex(0))	
			actor:LookAt(kTargetPt)
	
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
	
		DeleteActionTargetList(kTargets)
	else
		GetActionResultCS(action, actor)
	end
end

function Skill_Missile_OnCastingCompleted(actor, action)

	Skill_Missile_Fire(actor,action);

	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function Skill_Missile_OnEnter(actor, action)
	local prevActionID = actor:GetAction():GetID()
	
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then	
			
			-- 공중에서 공격을 했다면 종료
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end
	end
	
		
	actor:AttachParticle(126, "p_wp_l_hand", "ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand")

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Missile_OnCastingCompleted(actor,action);
	
		
	return true
end

function Skill_Missile_OnFindTarget(actor,action,kTargets)
	kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetTranslate(), actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor), 30, 0, 0);
	kParam:SetParam_3(true, FTO_NORMAL);
	
	return	action:FindTargets(TAT_RAY_DIR, kParam, kTargets, kTargets);
end

function Skill_Missile_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	local iSlot = action:GetCurrentSlot()
	local bIsAnimDone = actor:IsAnimationDone()
	local endParam = action:GetParam(911)
	local nextActionID = action:GetNextActionName()

	if endParam == "end" then
		return false
	end

	if bIsAnimDone == true then
		if iSlot == 1 then
			action:SetParam(911, "end")
			if nextActionID == "a_idle" then
				action:SetNextActionName("a_battle_idle")
			end
			return false
		end
	end

	return true
end

function Skill_Missile_OnCleanUp(actor, action)
end

function Skill_Missile_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local nextActionID = action:GetID()
	local endParam = curAction:GetParam(911)

	if endParam == "end" then
		return true
	end

	return false
end

function Skill_Missile_OnEvent(actor, textKey)

	if textKey == "hit" then
		Skill_Missile_DoEffect(actor)
	end

	return true
end

function Skill_Missile_DoEffect(actor)
	local action = actor:GetAction()
	local kProjectileMan = GetProjectileMan()
	local kHelperPos = actor:GetNodeWorldPos("p_wp_l_hand")
	local iTargetCount = action:GetTargetCount()
	local kTargetDir = actor:GetTranslate()
	local kMovingDir = actor:GetLookingDir()
	local iAttackRange = action:GetSkillRange(0,actor)
	local kActionTargetInfo = nil
	
	actor:AttachSound(7283,"Missile");

	local kMagicMissile = kProjectileMan:CreateNewProjectile(action:GetScriptParam("PROJECTILE_ID"), action, actor:GetPilotGuid())
	if kMagicMissile:IsNil() == false then

		ODS("iTargetCount : " .. iTargetCount .. "\n")
		kMagicMissile:SetWorldPos(kHelperPos)

		if iTargetCount > 0 then
			local kTargetPilot = g_pilotMan:FindPilot(action:GetTargetGUID(0))
			local kTargetPt = kTargetPilot:GetActor():GetABVShapeWorldPos(action:GetTargetABVShapeIndex(0))
			kActionTargetInfo = action:GetTargetInfo(0)
		end

		kMagicMissile:SetParentActionInfo(action:GetActionNo(), action:GetActionInstanceID(), action:GetTimeStamp())

		if kActionTargetInfo == nil then
			ODS("______Attack Range_____ : " .. iAttackRange .. "\n")
			kMovingDir:Multiply(iAttackRange)
			kTargetDir:Add(kMovingDir)
			kMagicMissile:SetTargetLoc(kTargetDir)
		else
			ODS("______Attack Range_____ : " .. iAttackRange .. "\n")
			kMagicMissile:SetTargetObjectList(action:GetTargetList())
		end

		kMagicMissile:Fire()
	end
	action:ClearTargetList();
end
