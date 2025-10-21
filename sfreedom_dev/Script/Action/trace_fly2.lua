-- Idle

function Act_Trace_Fly2_OnEnter(actor, action)

	--ODS("Act_Trace_Fly2_OnEnter\n", false, 1509)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	CheckNil(action == nil);
	CheckNil(action:IsNil());
	
	action:SetDoNotBroadCast(true);
	actor:MakePetActionQueue(-1)
	action:SetParamInt(6,0);

	local kMyPilot = actor:GetPilot()
	if kMyPilot:IsNil() then return true end
	local kMyUnit = kMyPilot:GetUnit()
	if kMyUnit:IsNil() then return true end
	local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
	if kCallerPilot:IsNil() then return true end
	local kCallerActor = kCallerPilot:GetActor()
	if kCallerActor:IsNil() then return true end
	
	actor:SetMovingDir(kCallerActor:GetMovingDir());
	actor:FindPathNormal();

	local iDist = actor:GetAnimationInfoFromAniName("MIN_DIST", "walk"):GetStr()
	if nil==iDist or ""==iDist then
		iDist = 42
	else
		iDist = tonumber(iDist)
		if 0>=iDist then iDist = 42 end
	end

	local iFarDist = actor:GetAnimationInfoFromAniName("MAX_DIST", "run"):GetStr()
	if nil==iFarDist or ""==iFarDist then
		iFarDist = 60
	else
		iFarDist = tonumber(iFarDist)
		if 0>=iFarDist then iFarDist = 60 end
	end

	action:SetParamInt(7,iDist);
	action:SetParamInt(8,iFarDist);

	return true
end

function Act_Trace_Fly2_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());

	local currentSlot = action:GetCurrentSlot()
	local param = action:GetParam(0)
	local iIdleType = action:GetParamInt(6);
	local iDist = action:GetParamInt(7);
	if 0>=iDist then iDist = 42 end

	local FarDist = action:GetParamInt(8);
	if 0>=FarDist then FarDist = 60 end
	
	local kMyPilot = actor:GetPilot()
	if kMyPilot:IsNil() then return true end
	local kMyUnit = kMyPilot:GetUnit()
	if kMyUnit:IsNil() then return true end
	local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
	if kCallerPilot:IsNil() then return true end
	local kCallerActor = kCallerPilot:GetActor()
	if kCallerActor:IsNil() then return true end
	local kPlayerPos = kCallerActor:GetPos()

	--Pos
	actor:SetTraceFlyTargetLoc( kPlayerPos )
	
	local iNowSlot = action:GetCurrentSlot()
	local iNextSlot = iNowSlot
--	ODS("name : "..action:GetCurrentSlotAnimName().." NowSlot : "..iNowSlot.."\n", false, 1509)
	
	-- Next
	local bForceNextSlot = false
	if 0 <= iNowSlot and 6 >= iNowSlot then -- Idle
		local fDistance = actor:TraceFly( 1.5, frameTime, iDist, 10.0, 5.25, 0.0, true )
		--ODS("59 fDistance : "..fDistance.."\n", false, 1509)
		if fDistance > FarDist then
			bForceNextSlot = true
			iNextSlot = 7
		else
			if fDistance < iDist then
				bForceNextSlot = 5<iNowSlot
				if true==bForceNextSlot then	--움직였다가 멈출 때
					actor:MakePetActionQueue(-1)
				end
				iNextSlot = actor:UpdatePetActionQueue(accumTime)
			else
				bForceNextSlot = true
				iNextSlot = 6
			end
		end
	end
	if 7 == iNowSlot then -- Run
		local fDistance = actor:TraceFly( 1.8, frameTime, iDist, 7.0, 5.25, 20.0, true )
		--ODS("75 fDistance : "..fDistance.."\n", false, 1509)
		if fDistance > FarDist then
		else
			bForceNextSlot = true
			iNextSlot = 6
		end
	end
	if 8 == iNowSlot then -- opening
		local fDistance = actor:TraceFly( 1.0, frameTime, 30.0, 4.0, 0.25, 20.0, true )
		if iDist < fDistance then
			bForceNextSlot = true
		end
		iNextSlot = 0
	end
	
	local kNew = ChoosePetAni(iNextSlot, actor, accumTime)
	if kNew~=iIdleType then
		iNextSlot = kNew
		bForceNextSlot = true
		action:SetParamInt(6,iNextSlot)
		if kCallerActor:IsMyActor() then
			AttachBaloonParticle(actor, iNextSlot)
		end
	end

	if actor:IsAnimationDone() or bForceNextSlot then
		action:SetSlot(iNextSlot)
		actor:PlayCurrentSlot(false)
	end
	
	return true

end
function Act_Trace_Fly2_OnCleanUp(actor, action)
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());

	actor:DetachFrom(1996)
	return true
end

function Act_Trace_Fly2_OnLeave(actor, action)
	return true
end

