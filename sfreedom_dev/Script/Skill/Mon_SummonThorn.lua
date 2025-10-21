function Act_Mon_SummonThorn_OnCheckCanEnter(actor, action)
	return true
end

function Act_Mon_SummonThorn_SetState(actor,action,kState)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iNewState = -1
	if kState == "BATTLEIDLE_START" then
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false
		end
				
		action:SetSlot(0)
		iNewState = 0
	elseif kState == "BATTLEIDLE_LOOP" then
--		actor:AttachParticle(1204,"p_ef_head","ef_deathmaster_skill_02_01_p_ef_head");
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
			action:SetSlot(3)	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자
		end
		iNewState = 1
	elseif kState == "FIRE" then
		action:SetSlot(2)
		iNewState = 2
	elseif kState == "RETURN" then
		iNewState = 3
		if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then	--리턴동작 없을 땐 그냥 끝
			return false
		end
		action:SetSlot(4)
	end

	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	action:SetParamInt(0,iNewState)
	
	return	true
end

function Act_Mon_SummonThorn_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	actor:ResetAnimation()
	
	--	attack target pos
	local	ptTargetPos = 0;

	local	kTargetList = action:GetTargetList()
	local	iTargetCount = kTargetList:size()
	
	if IsSingleMode() then
		ptTargetPos = GetMyActor():GetPos();
	else
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
	end

--[[	ptTargetPos = g_world:ThrowRay(ptTargetPos,Point3(0,0,-1),500)
	ptTargetPos:SetZ(ptTargetPos:GetZ() + 2)	-- 안올리면 땅에 파뭍힌다
	
	local iAbil = action:GetAbil(AT_2ND_AREA_PARAM1)
	if iAbil <= 0 then
		iAbil = 60
	end
	actor:AttachParticleToPointS(81190,ptTargetPos,"ef_Umai_skill_03_02_char_root", iAbil/64*2)	-- 반지름임]]
	
	action:SetParamAsPoint(0,ptTargetPos)
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_SummonThorn_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_SummonThorn_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_SummonThorn_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
		
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
	
		
		if iTargetCount>0 then
			local	kTargetInfo = kTargetList:GetTargetInfo(0)
			if kTargetInfo:IsNil() == false then
			
				local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID())
				if kTargetPilot:IsNil() == false then
				
					local	kTargetActor = kTargetPilot:GetActor()
					if kTargetActor:IsNil() == false then
						actor:LookAt(kTargetActor:GetPos(),true,true)
					end
				end
			end
		end
	end
	
	return true
end
function Act_Mon_SummonThorn_OnCastingCompleted(actor,action)
	Act_Mon_SummonThorn_SetState(actor,action,"FIRE")
	
	Act_Mon_SummonThorn_Fire(actor,action);
end

function Act_Mon_SummonThorn_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
	local	kTargetList = action:GetTargetList()
	local	iTargetCount = kTargetList:size()
	
	if iTargetCount>0 then
		local	kTargetInfo = kTargetList:GetTargetInfo(0)
		if kTargetInfo:IsNil() == false then
			local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID())
			if kTargetPilot:IsNil() == false then
				local	kTargetActor = kTargetPilot:GetActor()
				if kTargetActor:IsNil() == false then
					actor:LookAt(kTargetActor:GetPos(),true,true)
				end
			end
		end
	end
	
end

function Act_Mon_SummonThorn_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then
			if iState==2 then	-- 파이어 일때
				return Act_Mon_SummonThorn_SetState(actor,action,"RETURN")
			else
				return false
			end
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_SummonThorn_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > 2.0 then -- action:GetAbil(AT_CAST_TIME)/1000.0 then
				action:SetActionParam(AP_FIRE)
				Act_Mon_SummonThorn_OnCastingCompleted(actor,action)			
			end
		end
	end
	return true
end

function Act_Mon_SummonThorn_OnCleanUp(actor, action)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end

	actor:DetachFrom(81190)
end

function Act_Mon_SummonThorn_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
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

function Act_Mon_SummonThorn_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
     
    if	textKey == "fire" then
        local   action = actor:GetAction()
			
		if( CheckNil(nil==action) ) then return false end
		if( CheckNil(action:IsNil()) ) then return false end
	
		--local	kActorPos = actor:GetPos()
		local	fSkillRange = action:GetSkillRange(0,actor)
		if fSkillRange == 0 then
			fSkillRange = 100
		end
		
		local	kLookDir = actor:GetLookingDir()
		kLookDir:Multiply(fSkillRange)

		local	kTargetList = action:GetTargetList()
		local	iTargetCount = kTargetList:size()
	
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		if iTargetCount>0 then
			for i=0,iTargetCount-1 do
				local kTargetInfo = kTargetList:GetTargetInfo(i)	
				if kTargetInfo:IsNil() == false then
					local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID())
					if kTargetPilot:IsNil() == false then
--						local pos = kTargetPilot:GetPos()
						local apos = kTargetPilot:GetActor():GetPos()
						apos:SetZ(apos:GetZ()+50)
						local PartPos = g_world:ThrowRay(apos,Point3(0,0,-1),500)
						actor:AttachParticleToPoint(4381+i, PartPos, "ef_Umai_skill_03_03_char_root")
					end
				end
			end
		end
		
--	    Act_Mon_Melee_DoDamage(actor,action)
	end
	return true
end
