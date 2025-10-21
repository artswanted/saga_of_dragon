-- Idle
--g_kOpeingActor = nil

function Act_Opening_Manual3_OnEnter(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local actorID = actor:GetID()
	local loc = actor:GetNodeWorldPos("char_root")
	loc:SetZ(loc:GetZ() + 0.1)
	action:SetParamInt(82, 0)

	local iActor = actor:GetAnimationInfoFromAction("OPENING_ACTOR", action, 1000082)
	if nil==iActor then
		iActor = 1028320
	end

	local kPos = Point3(0,0,4000)
	local kX = actor:GetAnimationInfoFromAction("OPENING_ACTOR_X", action, 1000082)
	local kY = actor:GetAnimationInfoFromAction("OPENING_ACTOR_Y", action, 1000082)
	local kZ = actor:GetAnimationInfoFromAction("OPENING_ACTOR_Z", action, 1000082)
	if nil~=kX and ""~=kX then
		kPos:SetX(tonumber(kX))
	end
	if nil~=kY and ""~=kY then
		kPos:SetY(tonumber(kY))
	end
	if nil~=kZ and ""~=kZ then
		kPos:SetZ(tonumber(kZ))
	end

	action:SetParamInt(2, iActor)
	local kOpeningActor = CreateOpeningActor(iActor,kPos)
	if nil==kOpeningActor or kOpeningActor:IsNil() then
		kOpeningActor = actor
	end
	g_kOpeingActor = kOpeningActor
	CloseUI("FRM_LOADING_IMG")
	CallUI("FRM_MOVIE_IN")

	actor:SetCanHit(false)
	if actor:GetAbil(AT_MONSTER_TYPE) == 1 then -- 공중형이다.
		local fHeight = actor:GetAbil(AT_HEIGHT)
		loc:SetZ(loc:GetZ() - fHeight)
	end

	action:SetParamFloat(0,-1);
	if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then	--	Opening 애니가 없을 경우 Idle01 애니로 전환한다.
		action:SetSlot(1);
		action:SetParamFloat(0,g_world:GetAccumTime()); -- Idle 은 Loop 애니여서 IsAnimationDone() 이 무의미해진다. 따라서 흐른 시간으로 종료처리해야한다. 시작시간을 기록.
	end

	local MyActor = GetMyActor();
	
	CheckNil(nil == MyActor);
	CheckNil(MyActor:IsNil());

	MyActor:SetMovingDelta(Point3(0,0,0))
	MyActor:SetMovingDir(Point3(0,0,0))

	if EMGRADE_ELITE == actor:GetAbil(AT_GRADE) then
		action:SetParamFloat(9, -0.8)
		actor:LockBidirection(false)
	end

	action:SetParam(15, "")
	return true
end

function Act_Opening_Manual3_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if actor:IsNil() then
		return false
	end
	CheckNil(nil == g_kOpeingActor);
	CheckNil(g_kOpeingActor:IsNil());
	local kOpenActor = actor

	if false==g_kOpeingActor:IsNil() then
		kOpenActor = g_kOpeingActor
	end

	local action = actor:GetAction()


	if action:GetParamInt(81) == 1 then
	    if true == g_world:IsEndShowWorldFocusFilter() then
	        return false
	    end
	    return true
	end

	if kOpenActor:IsAnimationDone() == true and action:GetParam(15) == "" then
		action:SetParam(15, "AnimDone")
		return false
	end

	if KeyIsDown(KEY_ESC) then
	    Common_ManualOpening_End(actor,action)
	end

	return true
end
function Act_Opening_Manual3_OnCleanUp(actor, action)
    Common_ManualOpening_Leave(actor)
--	SetBreak()
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());
	
	actor:SetCanHit(true)

	local CurAction = actor:GetAction()
	CheckNil(nil == CurAction);
	CheckNil(CurAction:IsNil());
	RemoveOpeningActor(CurAction:GetParamInt(2))
end

function Act_Opening_Manual3_OnLeave(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	actor:DetachFrom(50)
	actor:RestoreLockBidirection()
	return true
end

function Act_Opening_Manual3_OnEvent(actor,textKey)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	if textKey == "effect_00001" or textKey == "effect_00002" then	--마할카 도끼
		QuakeCamera(0.3, 0.7)
	elseif textKey == "mahalka_01" then	-- 마할카 괴성
		QuakeCamera(0.7, 0.7)
	elseif textKey == "DeathMaster_01" then
		QuakeCamera(0.7, 1.0)
	elseif textKey == "cam" then
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
	elseif textKey == "opening_end" then
		local action = actor:GetAction() 
	
		CheckNil(nil == action);
		CheckNil(action:IsNil());

		if nil~=action and false == action:IsNil() then
			Common_ManualOpening_End(actor, actor:GetAction())
			action:SetParam(15, "opening_end")
		end
	end
end
