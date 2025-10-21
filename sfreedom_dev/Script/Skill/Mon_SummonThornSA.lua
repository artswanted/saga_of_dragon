-- Mon_SummonThorn for skill actor

function Act_Mon_SummonThornSA_OnCheckCanEnter(actor, action) 
	return true
end

function Act_Mon_SummonThornSA_SetState(actor,action,kState)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iNewState = -1
	if kState == "BATTLEIDLE_START" then
		action:SetSlot(0)
		iNewState = 0
	elseif kState == "BATTLEIDLE_LOOP" then
		action:SetSlot(1)
		iNewState = 1
	elseif kState == "FIRE" then
		action:SetSlot(2)
		iNewState = 2
	elseif kState == "RETURN" then
		action:SetSlot(4)
		iNewState = 3
	end
	return	true
end

function Act_Mon_SummonThornSA_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kTargetList = action:GetTargetList()
	local	iTargetCount = kTargetList:size()
	local	kPacket = action:GetParamAsPacket()
		if kPacket == nil or kPacket:IsNil() then
			if iTargetCount>0 then
				local	kTargetInfo = kTargetList:GetTargetInfo(0)
				if kTargetInfo:IsNil() == false then
					local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID())
					if kTargetPilot:IsNil() == false then			
						ptTargetPos = kTargetPilot:GetPos()
					end
				end
			else
				ptTargetPos = actor:GetPos()
			end
		else
			ptTargetPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat())
		end

	action:SetParamFloat(2, 0.0)--시간 저장
	action:SetParamAsPoint(3, actor:GetPos())--중앙 위치 저장
	action:SetParamInt(4,0)

	return true
end
function Act_Mon_SummonThornSA_OnCastingCompleted(actor,action)
	Act_Mon_SummonThornSA_SetState(actor,action,"FIRE")
	
	Act_Mon_SummonThornSA_Fire(actor,action);
end

function Act_Mon_SummonThornSA_Fire(actor,action)
	
end

function Act_Mon_SummonThornSA_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	action = actor:GetAction()
		
	if action:IsNil() then
		return true
	end
	
--	local	action = actor:GetAction()
--	local	iState = action:GetParamInt(0)
	local 	fTime = action:GetParamFloat(2)
	local 	fDelta = accumTime - fTime
	local 	kCenter = action:GetParamAsPoint(3)
	local	iAbil = action:GetAbil(AT_2ND_AREA_PARAM1)
	if iAbil <= 0 then
		iAbil = 80
	end
	local 	iIdx = action:GetParamInt(4)
	local kEffect = actor:GetAnimationInfo("FIRE_START_EFFECT")
	if nil==kEffect or ""==kEffect then
		kEffect = "ef_Umai_skill_03_03_char_root"
	end
	
	local kInterval = actor:GetAnimationInfo("INTER_TIME")
	if nil==kInterval then
		kInterval = 0.08
	else
		kInterval = tonumber(kInterval)
	end

	if fDelta > kInterval then
		local fValue = Random()
		fValue = (fValue%628)*0.01
		local kPos = Point3(math.sin(fValue), math.cos(fValue), 0)
		kPos:Unitize()
		kPos:Multiply(math.random(1,iAbil-10))
		kPos:Add(kCenter)
--		actor:AttachParticleToPoint(4381+iIdx, kPos, "ef_Umai_skill_03_03_char_root")	

		local rot = Random()%math.pi
		local quat = Quaternion(rot, Point3(0,0,1))
		local scale = Random()%5*0.1 + 0.8 --1.0~1.4
		actor:AttachParticleToPointWithRotate(4381+iIdx, kPos, kEffect, quat, scale)	

		action:SetParamFloat(2, accumTime)--시간 저장
		action:SetParamInt(4, iIdx+1)

	end

	return true
end

function Act_Mon_SummonThornSA_OnCleanUp(actor, action)
	actor:DetachFrom(81190)
end

function Act_Mon_SummonThornSA_OnLeave(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local	kCurAction = actor:GetAction()
	local	actionID = action:GetID()
	if action:GetActionType()=="EFFECT" or
		actionID == "a_freeze" or
		actionID == "a_reverse_gravity" or
		actionID == "a_blow_up" or
		actionID == "a_blow_up_small" or
		actionID == "a_knock_back" or
		actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_run_right" or
		actionID == "a_run_left" or
		actionID == "a_handclaps" then
		return true
	end

	return false 
end

function Act_Mon_SummonThornSA_OnEvent(actor,textKey)
	return true
end

function Act_Mon_SummonThornSA_OnTargetListModified(actor,action,bIsBefore)
end