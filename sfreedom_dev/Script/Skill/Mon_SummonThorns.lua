function Act_Mon_SummonThorns_OnCheckCanEnter(actor, action)
	return true
end

function Act_Mon_SummonThorns_SetState(actor,action,kState) 
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	local	iNewState = -1
--	ODS("Act_Mon_SummonThorns_SetState " ..kState .. "\n", false, 1509)
	if kState == "BATTLEIDLE_START" then
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false
		end
				
		action:SetSlot(0)
		iNewState = 0
	elseif kState == "BATTLEIDLE_LOOP" then
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
		if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then
			return false
		end
		action:SetSlot(4)
	end

	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	action:SetParamInt(0,iNewState)
	
	return	true
end

function Act_Mon_SummonThorns_OnEnter(actor, action)
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
	action:SetParamInt(1,1)
	action:SetParamInt(2,0)	--범위이펙트를 붙인적이 있느냐
	
	if IsSingleMode() then
		action:AddNewGUID(GetMyActor():GetPos())
	else
		local	kPacket = action:GetParamAsPacket()
		if kPacket == nil or kPacket:IsNil() then	--패킷이 잘못되었을 때
			if iTargetCount>0 then
				for i=0,iTargetCount-1 do
					local	kTargetInfo = kTargetList:GetTargetInfo(i)
					if kTargetInfo:IsNil() == false then
						action:AddNewGUID(kTargetInfo:GetTargetGUID())
					end
				end
			else
				action:AddNewGUID(GetMyActor():GetPos())
			end
		else
			local byPacketType = kPacket:PopByte()
			local iCount = kPacket:PopInt()
			action:SetParamInt(1,iCount)
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			for i=1,iCount do
				local kGuid = kPacket:PopGuid()
				local kTarget = g_world:FindActor(kGuid)
				if nil~=kTarget and false==kTarget:IsNil() then
					action:AddNewGUID(kGuid)
				end
			end
		end
	end
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_SummonThorns_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_SummonThorns_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_SummonThorns_SetState(actor,action,"BATTLEIDLE_LOOP")
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
function Act_Mon_SummonThorns_OnCastingCompleted(actor,action)
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	Act_Mon_SummonThorns_SetState(actor,action,"FIRE")
	
	Act_Mon_SummonThorns_Fire(actor,action);
end

function Act_Mon_SummonThorns_Fire(actor,action)
	
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

function Act_Mon_SummonThorns_OnUpdate(actor, accumTime, frameTime)
--	ODS("Act_Mon_SummonThorns_OnUpdate\n", false, 1509)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	local	action = actor:GetAction()
	local	iState = action:GetParamInt(0)
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	if 0==action:GetParamInt(2) then	--범위이펙트를 붙인적이 없다면
		action:SetParamInt(2,1)
		local RangeEffect = actor:GetAnimationInfo("RANGE_EFFECT")
		if nil == RangeEffect then	--범위 이펙트 이름
			RangeEffect = "ef_valgon_skill_03_03_char_root"
		end
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local iTotal = action:GetTotalGUIDInContainer()
		for i=0,iTotal-1 do
			local kTarget = g_world:FindActor(action:GetGUIDInContainer(i))
			if nil~=kTarget and false==kTarget:IsNil() then
				kTarget:AttachParticle(81190, "char_root", RangeEffect)
			end
		end
	end
	
	if action:GetActionParam() == AP_FIRE then
		if actor:IsAnimationDone() == true then
			if iState == 2 then	--Fire인 경우
				return Act_Mon_SummonThorns_SetState(actor, action, "RETURN")
			else
				return false
			end
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_SummonThorns_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > 2.0 then -- action:GetAbil(AT_CAST_TIME)/1000.0 then
				action:SetActionParam(AP_FIRE)
				Act_Mon_SummonThorns_OnCastingCompleted(actor,action)			
			end
		end
	end
	return true
end

function Act_Mon_SummonThorns_OnCleanUp(actor, action)
--	ODS("Act_Mon_SummonThorns_OnCleanUp\n", false, 1509)
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	local	kCurAction = actor:GetAction()
	if nil~=kCurAction and false==kCurAction:IsNil() then
		local iTargetCount = kCurAction:GetParamInt(1)
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		for i=0,iTargetCount do
			local kGuid = kCurAction:GetGUIDInContainer(i)
			if false==kGuid:IsNil() then
				local kTarget = g_world:FindActor(kGuid)
				if nil~=kTarget and false==kTarget:IsNil() then
					kTarget:DetachFrom(81190)
				end
			end
		end
	end
end

function Act_Mon_SummonThorns_OnLeave(actor, action)
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	local	kCurAction = actor:GetAction()
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

function Act_Mon_SummonThorns_OnEvent(actor,textKey)
   	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
    if	textKey == "fire" or textKey == "hit" then
--		ODS("Act_Mon_SummonThorns_OnEvent " .. textKey .. "\n", false, 1509)
        local   action = actor:GetAction()
		if( CheckNil(nil==action) ) then return end
		if( CheckNil(action:IsNil()) ) then return end
		local	kActorPos = actor:GetPos()
		local	fSkillRange = action:GetSkillRange(0,actor)
		if fSkillRange == 0 then
			fSkillRange = 100
		end
		
		local	kLookDir = actor:GetLookingDir()
		kLookDir:Multiply(fSkillRange)
	
		local	kTargetList = action:GetTargetList()
		local	iTargetCount = action:GetTotalGUIDInContainer()
--		ODS("action:GetGUIDInContainer " .. iTargetCount .. "\n", false, 1509)
		--여기쯤에서 소환된다고 생각하고 붙여줬던 범위 파티클을 떼자
		local bSoundplay = false
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		for i=0,iTargetCount do	--0부터
--			ODS("Act_Mon_SummonThorns_OnEvent " .. iTargetCount .. "\n", false, 1509)
			local kGuid = action:GetGUIDInContainer(i)
			if false==kGuid:IsNil() then
				local kTarget = g_world:FindActor(kGuid)
				if nil~=kTarget and false==kTarget:IsNil() then
					kTarget:DetachFrom(81190)
					if bSoundplay == false then
						local	kSoundID = actor:GetAnimationInfo("HIT_SOUND_ID");
						if kSoundID ~= "" then
							kTarget:AttachSound(986,kSoundID);
						end	
						bSoundplay = true
					end
				end
			end
		end

		local QuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
		if nil ~= QuakeTime and "" ~= QuakeTime then
			local QuakeFactor = actor:GetAnimationInfo("QUAKE_FACTOR")	
			if  nil == QuakeFactor then
				QuakeFactor = 1.0
			else
				QuakeFactor = tonumber(QuakeFactor)
			end
			QuakeCamera(QuakeTime, QuakeFactor)
		end
	end
	return true
end

function Act_Mon_SummonThorns_OnTargetListModified(actor,action,bIsBefore)
end
