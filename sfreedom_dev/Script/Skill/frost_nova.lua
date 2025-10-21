function Skill_Frost_Nova_OnCastingCompleted(actor,action)
end

function Skill_Frost_Nova_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType()

	if actor:IsMyActor() then
		actor:StartSkillCoolTime(action:GetActionNo())
		action:LockCurrentTarget(900, 10)
	end

	-- 서버로 부터 타겟에 대한 데미지를 요청한다.
--	guid = GUID("123")
--	guid:Generate()
	
--	pilot = g_pilotMan:NewPilotWithActorID(guid, "c_frost_spirit", "npc")
--	spiritActor = pilot:GetActor()

--	actorPos = actor:GetTranslate()
--	g_world:AddActor(guid, spiritActor, actorPos, 1)
--	action:SetParam(100, guid:GetString())
--	spiritActor:ClearActionState()
--	spiritActor:SetTargetScale(1.2)
--	spiritActor:ReserveTransitAction("a_spawn")
	actor:SeeFront(true)
	actor:AttachParticle(102, "char_root", "e_skill_frost_start")


	return true
end

function Skill_Frost_Nova_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local weapontype = actor:GetEquippedWeaponType()
--	local actionID = action:GetID()
	if action:GetParamFloat(45) == "null" then
		action:SetParamFloat(45, accumTime)
		ODS("Start AccumTime : " .. GetAccumTime .. "\n")
	end
	local elapsedTime = accumTime - action:GetParamFloat(45)
	local kThreshold = elapsedTime * 400 

	local currentSlot = action:GetCurrentSlot()
	if currentSlot == 0 then
		if animDone == true then
			actor:PlayNext()
			-- 시전이 끝났으면 필요한 이펙트를 붙인다.
			QuakeCamera(0.6, 1.0)
		end
	elseif currentSlot == 1 then
		local idx = 0
		local targetCount = action:GetTargetCount()
		while idx < targetCount do
			local kTargetGuid = action:GetTargetGUID(idx)
			local kTargetPilot = g_pilotMan:FindPilot(kTargetGuid)
			local kPeerActor = kTargetPilot:GetActor()
			kPeerActor:AddEffect(101100401, 0, nil, 0, 0, true)
			kPeerActor:AttachParticleToPoint(16, kPeerActor:GetHitPoint(), "e_dmg_ice")
			Net_ReqDamage(action:GetActionNo(), action:GetActionInstanceID(), kTargetGuid)
--			if kPeerActor:IsNil() == false then
--				ODS("Between actors Distance = " .. actor:GetDistance(kPeerActor) .. " Threshold : " .. kThreshold .. "\n")
--				if actor:GetDistance(kPeerActor) <= kThreshold then
--					Skill_Frost_Buff(kPeerActor, true)
--					kPeerActor:AttachParticle(16, "char_root", "e_dmg")
--				else
--					break
--				end
--			end
			idx = idx + 1
		end
		return false
	end
		
	return true
end

function Skill_Frost_Nova_OnCleanUp(actor, action)
end

function Skill_Frost_Nova_OnLeave(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	curAction = actor:GetAction()
	actionID = action:GetID()

	guidStr = curAction:GetParam(100)
	if  guidStr ~= "null" then
		guid = GUID(guidStr)
	--	g_world:RemoveObject(guid)
	end

	if 	actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true
	end
	return false 
end

function Skill_Frost_Nova_OnEvent(actor, textKey)
	if textKey == "fire" or textKey=="hit" then

	end
end
