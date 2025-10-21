-- Idle

function Act_trace_Idle_OnEnter(actor, action)
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());
	
	action:SetDoNotBroadCast(true);
	--action:SetDoNotPlayAnimation(true);
	--action:SetParamInt(0,0);

	return true
end

function Act_trace_Idle_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());

	local currentSlot = action:GetCurrentSlot()
	local param = action:GetParam(0)
	local iIdleType = action:GetParamInt(6);
	
	local kPlayerPilot = g_pilotMan:GetPlayerPilot()
	if kPlayerPilot:IsNil() then return true end
	local kPlayerPos = kPlayerPilot:GetActor():GetPos()
	
	--Pos
	actor:SetTraceFlyTargetLoc( kPlayerPos )
	
	-- (0)idle1, (1)idle1, (2)idle1, (3)idle1, (4)run, (5)talk, (6)opening
	local iNowSlot = action:GetCurrentSlot()
	local iNextSlot = iNowSlot
	
	-- Next
	local bForceNextSlot = false
	if 0 <= iNowSlot and 3 >= iNowSlot then -- Idle
		local fDistance = actor:TraceFly( 0.5, frameTime, 40.0, 5.0, 0.25, 0.0, true )
		if fDistance > 60 then
			bForceNextSlot = true
			iNextSlot = 4
		else
			if actor:IsAnimationDone() then
				--[[if iNowSlot == 0 then
					iNextSlot = math.random(0, 3)
				else
					iNextSlot = 0
				end]]
			end
		end
	end
	if 4 == iNowSlot then -- Run
		local fDistance = actor:TraceFly( 0.8, frameTime, 40.0, 7.0, 0.25, 15.0, true )
		if fDistance > 50 then
		else
			bForceNextSlot = true
			iNextSlot = 0
		end
	end
	if 5 == iNowSlot then -- Talk
		local fDistance = actor:TraceFly( 1.0, frameTime, 30.0, 4.0, 0.25, 15.0, true )
		if fDistance > 30 then
			iNextSlot = 4
		else
			iNextSlot = 0
		end
	end
	if 6 == iNowSlot then -- opening
		actor:TraceFly( 1.0, frameTime, 30.0, 4.0, 0.25, 15.0, false )
		iNextSlot = 0
	end
	
	if actor:IsAnimationDone() or bForceNextSlot then
		action:SetSlot(iNextSlot)
		actor:PlayCurrentSlot(true)
	end
	
	return true

end
function Act_trace_Idle_OnCleanUp(actor, action)
	return true
end

function Act_trace_Idle_OnLeave(actor, action)
	return true
end

