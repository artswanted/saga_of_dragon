-- Idle

function Act_Trace_Fly_OnEnter(actor, action)

	ODS("Act_Trace_Fly_OnEnter\n", false, 1509)
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());
	
	action:SetDoNotBroadCast(true);
	--action:SetDoNotPlayAnimation(true);
	--action:SetParamInt(0,0);

	return true
end

function Act_Trace_Fly_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());

--[[	local fDelta = accumTime-action:GetParamFloat(5)
	if 0.05 > fDelta then
		return true
	end
	action:SetParamFloat(5, accumTime)
	frameTime = frameTime + fDelta]]

	local currentSlot = action:GetCurrentSlot()
	local param = action:GetParam(0)
	local iIdleType = action:GetParamInt(6);
	
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
	if 0 <= iNowSlot and 1 >= iNowSlot then -- Idle
		local fDistance = actor:TraceFly( 1.5, frameTime, 40.0, 10.0, 5.25, 0.0, true )
		--ODS("59 fDistance : "..fDistance.."\n", false, 1509)
		if fDistance > 50 then
			bForceNextSlot = true
			iNextSlot = 2
		else
			if fDistance < 42 then
				bForceNextSlot = iNextSlot~=0
				iNextSlot = 0
			else
				bForceNextSlot = iNextSlot~=1
				iNextSlot = 1
			end
		end
	end
	if 2 == iNowSlot then -- Run
		local fDistance = actor:TraceFly( 1.8, frameTime, 40.0, 7.0, 5.25, 20.0, true )
		--ODS("75 fDistance : "..fDistance.."\n", false, 1509)
		if fDistance > 42 then
		else
			bForceNextSlot = true
			iNextSlot = 1
		end
	end
	if 3 == iNowSlot then -- opening
		local fDistance = actor:TraceFly( 1.0, frameTime, 30.0, 4.0, 0.25, 20.0, true )
		if 40 < fDistance then
			bForceNextSlot = true
		end
		iNextSlot = 0
	end
	
	if actor:IsAnimationDone() or bForceNextSlot then
		action:SetSlot(iNextSlot)
		actor:PlayCurrentSlot(false)
	end
	
	return true

end
function Act_Trace_Fly_OnCleanUp(actor, action)
	return true
end

function Act_Trace_Fly_OnLeave(actor, action)
	return true
end

