-- Idle
--g_kOpeingActor = nil

function CreateOpeningActor(iNo, kPos)
	local guid = GUID("123")
	guid:Generate();
	
	local pilot = g_pilotMan:NewPilot(guid, iNo, 0,"NPC")
	
	if pilot == nil or pilot:IsNil() == true then
		return nil
	end
	
	local kActor = pilot:GetActor()
	if kActor == nil or kActor:IsNil() == true then
		return nil
	end
	
	if( CheckNil(nil == g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	if IsSingleMode() == false then
		g_pilotMan:InsertPilot(guid,pilot);
	end

	local	kSpawnPos = Point3(0, 0, 4000)
	if nil~=kPos and false==kPos:IsZero() then
		kSpawnPos = kPos
	end
	
	kActor:FreeMove(true)
	kActor:SetUseLOD(false)
	kActor:SetUseSmoothShow(false)
	g_world:AddActor(guid, pilot:GetActor(), kSpawnPos, 6)
	
	pilot:GetActor():ClearReservedAction()
	local kAction = pilot:GetActor():ReserveTransitAction("a_opening_movie")
	kAction:SetSlot(0)
	kActor:ResetAnimation();

	pilot:GetActor():ClearActionState()
	pilot:SetAbil(AT_HP, 1000000);
	pilot:SetAbil(AT_UNIT_SIZE,2);
	pilot:SetAbil(AT_DAMAGEACTION_TYPE,0);
	pilot:SetAbil(AT_NOT_SEE_PLAYER, 0)
	pilot:SetAbil(AT_DAMAGEACTION_TYPE, 0)	
	
	g_world:SetCameraModeMovie2(kActor, 2, 1000000, "nop()")

	return kActor
end

function RemoveOpeningActor(iNo)
	if( CheckNil(nil == g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local kActor = g_pilotMan:FindActorByClassNo(iNo);
	
	if kActor == nil or kActor:IsNil() then
		return
	end
	g_world:RemoveActorOnNextUpdate(kActor:GetPilotGuid())
	g_world:RemoveCameraAM()
	g_kOpeingActor = nil
end

function Act_Opening_Manual2_OnEnter(actor, action)
	if( CheckNil(nil == actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	CheckNil(nil == action);
	if( CheckNil(action:IsNil()) ) then return false end

	if( CheckNil(nil == g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local actorID = actor:GetID()
	local loc = actor:GetNodeWorldPos("char_root")
	loc:SetZ(loc:GetZ() + 0.1)
	action:SetParamInt(82, 0)

	local iActor = actor:GetAnimationInfoFromAction("OPENING_ACTOR", action, 1000082)
	if nil==iActor then
		iActor = 6105100
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

function Act_Opening_Manual2_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(nil == actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	if( CheckNil(nil == g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local kOpenActor = actor

	if nil~=g_kOpeingActor and false==g_kOpeingActor:IsNil() then
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
function Act_Opening_Manual2_OnCleanUp(actor, action)
    Common_ManualOpening_Leave(actor)
--	SetBreak()
	if( CheckNil(nil == actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	actor:SetCanHit(true)

	local CurAction = actor:GetAction()
	if( CheckNil(CurAction == nil) ) then return false end	
	if( CheckNil(CurAction:IsNil()) ) then return false end
	RemoveOpeningActor(CurAction:GetParamInt(2))
end

function Act_Opening_Manual2_OnLeave(actor, action)
	if( CheckNil(nil == actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	actor:DetachFrom(50)
	actor:RestoreLockBidirection()
	return true
end

function Act_Opening_Manual2_OnEvent(actor,textKey)
	if( CheckNil(nil == actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

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
	
		if( CheckNil(nil == action) ) then return false end
		if( CheckNil(action:IsNil()) ) then return false end

		if nil~=action and false == action:IsNil() then
			Common_ManualOpening_End(actor, actor:GetAction())
			action:SetParam(15, "opening_end")
		end
	end
end
