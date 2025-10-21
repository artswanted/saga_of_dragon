function Skill_Meteor_Strike_OnEnter(actor, action)
	actorID = actor:GetID()
	actionID = action:GetID()
	weapontype = actor:GetEquippedWeaponType()

	if actor:IsMyActor() then
		actor:StartSkillCoolTime(action:GetActionNo())
		action:LockCurrentTarget(900, 10)
	end

	-- 서버로 부터 타겟에 대한 데미지를 요청한다.
	guid = GUID("123")
	guid:Generate()
	
--	pilot = g_pilotMan:NewPilotWithActorID(guid, "c_meteor_spirit", "npc")
	spiritActor = pilot:GetActor()

	actorPos = actor:GetTranslate()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:AddActor(guid, spiritActor, actorPos, 1)
	action:SetParam(100, guid:GetString())
	spiritActor:ClearActionState()
	spiritActor:SetTargetScale(1.2)
	spiritActor:ReserveTransitAction("a_spawn")
	actor:SeeFront(true)

	return true
end

function Skill_Meteor_Strike_OnUpdate(actor, accumTime, frameTime)
	actorID = actor:GetID()
	action = actor:GetAction()
	animDone = actor:IsAnimationDone()
	nextActionName = action:GetNextActionName()
	weapontype = actor:GetEquippedWeaponType()
	actionID = action:GetID()
	if action:GetParamFloat(45) == "null" then
		action:SetParamFloat(45, accumTime)
	end
	elapsedTime = accumTime - action:GetParamFloat(45)
	kThreshold = elapsedTime * 400 

	currentSlot = action:GetCurrentSlot()
	if currentSlot == 0 then
		if animDone == true then
			actor:PlayNext()
			-- 시전이 끝났으면 필요한 이펙트를 붙인다.
			actor:AttachParticle(15, "char_root", "e_fireball")
			QuakeCamera(0.6, 1.0)
		end
	elseif currentSlot == 1 then
		idx = 0
		targetCount = action:GetTargetCount()
		while idx < targetCount do
			kTargetGuid = action:GetTargetGUID(idx)
			kTargetPilot = g_pilotMan:FindPilot(kTargetGuid)
			kPeerActor = kTargetPilot:GetActor()
			Skill_Fire_Buff(kPeerActor, true)
			kPeerActor:AttachParticle(16, "char_root", "e_dmg_fire")
			kPeerActor:AddStateScript(0, "State_Fire")
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

function Skill_Meteor_Strike_OnCleanUp(actor, action)
end

function Skill_Meteor_Strike_OnLeave(actor, action)
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

function Skill_Meteor_Strike_OnEvent(actor, textKey)
	if textKey == "fire" or textKey=="hit" then

	end
end
