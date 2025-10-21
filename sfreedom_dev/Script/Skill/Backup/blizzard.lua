
function Skill_Blizzard_OnCheckCanEnter(actor,action)
	local myWeaponType = actor:GetEquippedWeaponType();

--	if myWeaponType ~= 9 then
--		return false
--	end
	
	if actor:IsMeetFloor() == false then
		return false
	end

	return true
end

function Skill_Blizzard_OnCastingCompleted(actor, action)
	actor:DetachFrom(126)
	actor:AttachParticle(127, "p_wp_l_hand", "ef_mag_spell_finish")
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end

function Skill_Blizzard_OnEnter(actor, action)
	local prevActionID = actor:GetAction():GetID()

	if prevActionID == "a_Blizzard" then
		action:SetParam(911, "end")
		action:SetDoNotBroadCast(true)
		return true 
	end

	actor:Stop()

	if actor:IsMyActor() == true then

		local kTarget = CreateActionTargetList(actor:GetPilotGuid(), action:GetActionInstanceID(),action:GetActionNo())
		local iFound = Skill_Ground_Upper_OnFindTarget(actor,action,kTarget);

		if iFound > 0 then
			local i = 0 
			while i < iFound do
				local kTargetInfo = kTarget:GetTargetInfo(i)
				local kTargetGUID = kTargetInfo:GetTargetGUID()
				local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
				if kTargetPilot:IsNil() == false then
					local kTargetActor = kTargetPilot:GetActor()
					if kTargetActor:IsNil() == false then
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
				end
				i = i + 1
			end
		end
		DeleteActionTargetList(kTarget)
	else
		GetActionResultCS(action, actor)
	end	
	
	actor:AttachParticle(126, "p_wp_l_hand", "ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand")
	return true
end
function Skill_Blizzard_OnFindTarget(actor,action,kTargets)
	local kPos = actor:GetPos()
	local kDir = actor:GetLookingDir()
	local fBackDistance = -15
	local kParam = FindTargetParam()

	kDir:Multiply(fBackDistance)
	kPos:Add(kDir)
	kParam:SetParam_1(kPos, actor:GetLookingDir())
	kParam:SetParam_2(action:GetSkillRange(0,actor), 5, 0, 10)
	kParam:SetParam_3(true, FTO_NORMAL)

	return action:FindTargets(TAT_BAR, kParam, kTargets, kTargets);
end

function Skill_Blizzard_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	local bIsAnimDone = actor:IsAnimationDone()
	local nextActionID = action:GetNextActionName()
	local endParam = action:GetParam(911)
	local iSlot = action:GetCurrentSlot()

	if endParam == "end" then
		return false
	end

	if bIsAnimDone == true then
		if iSlot == 1 and nextActionID == "a_battle_idle" then
			action:SetParam(911, "end")
			return false
		end
	end

	return true
end

function Skill_Blizzard_OnCleanUp(actor, action)
end

function Skill_Blizzard_OnLeave(actor, action)
	local curAction = actor:GetAction()
	local nextActionID = action:GetID()
	local endParam = curAction:GetParam(911)

	if endParam == "end" then
		return true
	end

	return false	
end

function Skill_Blizzard_OnEvent(actor, textKey)

	if textKey == "hit" then
		Skill_Blizzard_DoEffect(actor)
	end

	return true
end

function Skill_Blizzard_DoEffect(actor)
	local action = actor:GetAction()
	local kGuid = actor:GetPilotGuid()
	local kMovingDir = actor:GetLookingDir()
	local kHelperPos = actor:GetNodeWorldPos("char_root")
	local iTargetCount = action:GetTargetCount()
	local kAxis = Point3(0,0,1)
	local iDegree = math.acos(kMovingDir:Dot(Point3(0,1,0)))

	kMovingDir:Multiply(60)
	kHelperPos:Add(kMovingDir)
	kHelperPos:SetZ(kHelperPos:GetZ() + 5)

	if actor:IsToLeft() then
		kAxis = Point3(0,0,-1)
	end

	actor:AttachParticleToPointWithRotate(128, kHelperPos, "ef_skill_blizzard", Quaternion(iDegree, kAxis))
	
	-- TargetCount는 자기 자신뿐이다.
	if iTargetCount > 0 then
		local kActionResult = action:GetTargetActionResult(0)
		local pt = actor:GetABVShapeWorldPos(action:GetTargetABVShapeIndex(0))

		action:ClearTargetList()
	end
end
