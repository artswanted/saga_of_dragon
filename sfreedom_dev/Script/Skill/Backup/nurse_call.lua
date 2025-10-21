

function Skill_Nurse_Call_OnCheckCanEnter(actor, action)
	local myWeaponType = actor:GetEquippedWeaponType();

--	if myWeaponType ~= 9 then
--		return false
--	end
	
	if actor:IsMeetFloor() == false then
		return false
	end

	return true
end

function Skill_Nurse_Call_OnCastingCompleted(actor, action)
	Skill_Nurse_Call_Fire(actor,action)
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end
function Skill_Nurse_Call_Fire(actor,action)
	actor:DetachFrom(126)
	actor:AttachParticle(127, "p_wp_l_hand", "ef_mag_spell_finish")

	if actor:IsMyActor() == true then
		local kTarget = CreateActionTargetList(actor:GetPilotGuid(), action:GetActionInstanceID(),action:GetActionNo())
		local iFound = Skill_Transformation_OnFindTarget(actor,action,kTarget);

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
	
end

function Skill_Nurse_Call_OnEnter(actor, action)
	local prevActionID = actor:GetAction():GetID()

	if prevActionID == "a_Nurse Call" then
		action:SetParam(911, "end")
		action:SetDoNotBroadCast(true)
		return false 
	end

	actor:Stop()

	actor:AttachParticle(126, "p_wp_l_hand", "ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand")

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Nurse_Call_OnCastingCompleted(actor,action);	
	return true
end
function Skill_Nurse_Call_OnFindTarget(actor,action,kTargets)
	local kParam = FindTargetParam()
	kParam:SetParam_1(actor:GetTranslate(), actor:GetLookingDir())
	kParam:SetParam_2(action:GetSkillRange(0,actor), 30, 0, 0)
	kParam:SetParam_3(true, FTO_NORMAL)
	
	return	action:FindTargets(TAT_RAY_DIR, kParam, kTargets, kTargets);
end

function Skill_Nurse_Call_OnUpdate(actor, accumTime, frameTime)
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
			nextActionID = "a_battle_idle"
			return false
		end
	end

	return true
end

function Skill_Nurse_Call_OnCleanUp(actor, action)
end

function Skill_Nurse_Call_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local nextActionID = action:GetID()
	local endParam = curAction:GetParam(911)

	if endParam == "end" then
		return true
	end

	return false
end

function Skill_Nurse_Call_OnEvent(actor, textKey)

	if textKey == "hit" then
		Skill_Nurse_Call_DoEffect(actor)
	end

	return true
end

function Skill_Nurse_Call_DoEffect(actor)
	local action = actor:GetAction()
	local kHelperPos = actor:GetNodeWorldPos("p_wp_l_hand")
	local kTargetDir = actor:GetTranslate()
	local kMovingDir = actor:GetLookingDir()
	local guid = GUID("123")

	ODS("________간호사 등장_______\n")

	-- 간호사 등장, 이펙트 사운드 연출
	--[[
	guid:Generate()
	local pilot = g_pilotMan:NewPilot(guid, 900003, 0)
	if pilot:IsNil() == false then
		local kNewActor = pilot:GetActor()
		kMovingDir:Multiply(40)
		kTargetDir:Add(kMovingDir)
		g_world:AddActor(guid, kNewActor, kTargetDir, 2)
		
		local pt = kNewActor:GetTranslate()
		pt:SetZ(pt:GetZ() - 10)
		kNewActor:AttachParticleToPoint(192, pt, "ef_bang")
		kNewActor:ReserveTransitAction("a_Nurse_Kiss")
		kNewActor:ClearActionState()
		kTargetDir:Add(kMovingDir)
		kNewActor:LookAt(kTargetDir)
	end
	]]

	action:GetTargetList():ApplyActionEffects()
end

function Skill_Nurse_Call_DoUpdate(actor, accumTime, frameTime)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local world = g_world

	if actor:IsAnimationDone() == true and
		world:IsNil() == false then
		local pt = actor:GetTranslate()
		pt:SetZ(pt:GetZ() - 30)
		actor:AttachParticleToPoint(192, pt, "ef_bang")
		world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
	end
end


function Skill_Nurse_Kiss_OnEnter(actor, action)
	--action:SetDoNotBroadCast(true)
	actor:SeeFront(true)
	return true
end

function Skill_Nurse_Kiss_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local world = g_world

	if actor:IsAnimationDone() == true and
		world:IsNil() == false then
		local pt = actor:GetTranslate()
		pt:SetZ(pt:GetZ() - 30)
		--actor:AttachParticleToPoint(192, pt, "ef_bang")
		--world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
		--actor:GetAction():SetNextActionName("a_skill_Nurse_Call")
		return false
	end
	return true
end

function Skill_Nurse_Kiss_OnLeave(actor, action)
	return true
end
