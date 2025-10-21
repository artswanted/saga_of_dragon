

function Skill_Fire_Ball_OnCheckCanEnter(actor,action)
	local myWeaponType = actor:GetEquippedWeaponType();

--	if myWeaponType ~= 9 then
--		return false
--	end
	
	if actor:IsMeetFloor() == false then
		return false
	end

	return true
end

function Skill_Fire_Ball_OnCastingCompleted(actor, action)
	Skill_Fire_Ball_Fire(actor,action)
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Skill_Fire_Ball_Fire(actor,action)
	actor:DetachFrom(126)
	actor:AttachParticle(127, "p_wp_l_hand", "ef_mag_spell_finish")

	if actor:IsMyActor() == true then
		local kTargets = CreateActionTargetList(actor:GetPilotGuid(), action:GetActionInstanceID(),action:GetActionNo())
		local iTargetCount = Skill_Fire_Ball_OnFindTarget(actor,action,kTargets);
	
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

function Skill_Fire_Ball_OnEnter(actor, action)
	local prevActionID = actor:GetAction():GetID()

	if prevActionID == "a_Fire Ball" then
		action:SetParam(911, "end")
		action:SetDoNotBroadCast(true)
		return true 
	end

	actor:AttachParticle(126, "p_wp_l_hand", "ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand")

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Fire_Ball_OnCastingCompleted(actor,action);	
	
	return true
end

function Skill_Fire_Ball_OnFindTarget(actor,action,kTargets)
	kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetTranslate(), actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor), 30, 0, 0);
	kParam:SetParam_3(true, FTO_NORMAL);
	
	return	action:FindTargets(TAT_RAY_DIR, kParam, kTargets, kTargets);
end

function Skill_Fire_Ball_OnUpdate(actor, accumTime, frameTime)
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

function Skill_Fire_Ball_OnCleanUp(actor, action)
end

function Skill_Fire_Ball_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local nextActionID = action:GetID()
	local endParam = curAction:GetParam(911)

	if endParam == "end" then
		return true
	end

	return false
end

function Skill_Fire_Ball_OnEvent(actor, textKey)
	if textKey == "hit" then
		Skill_Fire_Ball_DoEffect(actor)
	elseif textKey == "fire" then
		local action = actor:GetAction()
		local kProjectileMan = GetProjectileMan()
		local iTargetCount = action:GetTargetCount()
		local kTargetDir = actor:GetTranslate()
		local kMovingDir = actor:GetLookingDir()
		local iAttackRange = action:GetSkillRange(0,actor)
		local kHelperPos = Point3(action:GetParamFloat(2001), action:GetParamFloat(2002), action:GetParamFloat(2003))
		local kActionTargetInfo = nil
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
				kMovingDir:Multiply(500)
				kTargetDir:Add(kMovingDir)
				kMagicMissile:SetTargetLoc(kTargetDir)
			else
				ODS("______Attack Range_____ : " .. iAttackRange .. "\n")
				kMagicMissile:SetTargetObject(kActionTargetInfo)
			end
	
			kMagicMissile:Fire()
		end
		action:ClearTargetList();
	end
	return true
end

function Skill_Fire_Ball_DoEffect(actor)
	local action = actor:GetAction()
	local kTargetDir = actor:GetTranslate()
	local kMovingDir = actor:GetLookingDir()
	local guid = GUID("123")
	
	actor:AttachSound(7283,"FireBall");

	ODS("________용 등장_______\n")

	-- 용 등장, 이펙트 사운드 연출
	guid:Generate()
	local pilot = g_pilotMan:NewPilot(guid, 900006, 0)
	if pilot:IsNil() == true then
		return true
	end

	local kNewActor = pilot:GetActor()
	kMovingDir:Multiply(15)
	kTargetDir:Add(kMovingDir)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:AddActor(guid, kNewActor, kTargetDir, 2)
	kNewActor:SetTranslate(kTargetDir)
	--kNewActor:FindPathNormal()
	kNewActor:ToLeft(actor:IsToLeft(), true)
	kNewActor:FreeMove(true)
	
	kNewActor:ReserveTransitAction("a_FireBall_Dragon")
	kNewActor:ClearActionState()

	--kMovingDir:Multiply(100)
	--kTargetDir:Add(kMovingDir)
	--kNewActor:LookAt(kTargetDir)
	local kNewAction = kNewActor:GetReservedTransitAction()

	-- 기존 액터를 설정해 준다.
	kNewAction:SetParam(1945, actor:GetPilotGuid():GetString())

	--action:GetTargetList():ApplyActionEffects()
end

function Skill_FireBall_Dragon_OnEnter(actor, action)
	action:SetDoNotBroadCast(true)
	return true
end

function Skill_FireBall_Dragon_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local world = g_world

	if actor:IsAnimationDone() == true and
		world:IsNil() == false then
		world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
		return false
	end
	return true
end

function Skill_FireBall_Dragon_OnLeave(actor, action)
	return true
end

function Skill_FireBall_Dragon_OnEvent(actor, textkey)

	if textkey == "fire" then
		local kHelperPos = actor:GetNodeWorldPos("Dummy03")
		local kAction = actor:GetAction()
		local kMasterGuidStr = kAction:GetParam(1945)
		local kMasterPilot = g_pilotMan:FindPilot(GUID(kMasterGuidStr))
		local kMasterActor = kMasterPilot:GetActor()
		local kMasterAction = kMasterActor:GetAction()

		kMasterAction:SetParamFloat(2001, kHelperPos:GetX())
		kMasterAction:SetParamFloat(2002, kHelperPos:GetY())
		kMasterAction:SetParamFloat(2003, kHelperPos:GetZ())

		ODS(" X : " .. kHelperPos:GetX() .. " Y : " .. kHelperPos:GetY() .. " Z : " .. kHelperPos:GetZ() .. "\n")

		Skill_Fire_Ball_OnEvent(kMasterActor, "fire")
	end

	return true
end
