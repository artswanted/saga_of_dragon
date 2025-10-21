-- Idle

function Act_Opening_OnEnter(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local actorID = actor:GetID()

	local loc = actor:GetNodeWorldPos("char_root")
	loc:SetZ(loc:GetZ() + 0.1)
	actor:SeeFront(true)
	actor:SetCanHit(false)
	if actor:GetAbil(AT_MONSTER_TYPE) == 1 then -- 공중형이다.
		local fHeight = actor:GetAbil(AT_HEIGHT)
		loc:SetZ(loc:GetZ() - fHeight)
	end
	--actor:AttachParticleToPoint(50, loc, "e_ef_monster_openning")
	
	action:SetParamFloat(0,-1);

	if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then	--	Opening 애니가 없을 경우 Idle01 애니로 전환한다.
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		action:SetSlot(1);
		action:SetParamFloat(0,g_world:GetAccumTime()); -- Idle 은 Loop 애니여서 IsAnimationDone() 이 무의미해진다. 따라서 흐른 시간으로 종료처리해야한다. 시작시간을 기록.
	end

	local iTTno = actor:GetAnimationInfoFromAniName("TT_NO", action:GetCurrentSlotAnimName())
	if nil~=iTTno and false==iTTno:IsNil() then
		iTTno = tonumber(iTTno:GetStr())
		g_ChatMgrClient:Notice_Show_ByTextTableNo2(iTTno)
	end
	return true
end

function Act_Opening_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()

	CheckNil(nil == action);
	CheckNil(action:IsNil());
	
	if action:GetCurrentSlot() == 0 then
	
		if actor:IsAnimationDone() == true then
			action:SetParam(15, "AnimDone")		
			return false 
		end
		
	else
	
		local	fElapsedTime = accumTime - action:GetParamFloat(0);
		if fElapsedTime>2 then
			action:SetParam(15, "AnimDone")		
			return	false;
		end
	
	end
	
	return true
end
function Act_Opening_OnCleanUp(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	actor:SetCanHit(true)
end

function Act_Opening_OnLeave(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local nextActionID = action:GetID()
	local prevAction = actor:GetAction()

	CheckNil(nil == prevAction);
	CheckNil(prevAction:IsNil());
	
	actor:DetachFrom(50)

	if prevAction:GetParam(15) == "AnimDone" and
		nextActionID == "a_idle" then
		return true
	end
	return false
end

function Act_Opening_OnCastingCompleted(actor, action)
end

function Act_Opening_OnEvent(actor,textKey)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	if textKey == "cam" then
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
end
