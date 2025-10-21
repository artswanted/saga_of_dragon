-- Idle

function Act_Trace_Ground_OnEnter(actor, action)


	CheckNil(action == nil);
	CheckNil(action:IsNil());
	
	action:SetDoNotBroadCast(true);
	--action:SetDoNotPlayAnimation(true);
	--action:SetParamInt(0,0);

	return true
end

function Act_Trace_Ground_OnUpdate(actor, accumTime, frameTime)
	
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
		local fDistance = actor:TraceGround( 0.5, frameTime, 40.0, 15.0, true )
--		ODS("48 fDistance : "..fDistance.."\n", false, 1509)
		if fDistance > 60 then
			bForceNextSlot = true
			iNextSlot = 2
		else
			if fDistance < 40 then
				local bLookCaller = actor:GetAnimationInfoFromAction("LOOK_CALLER", action, 0);	
				if nil~=bLookCaller and 0~=tonumber(bLookCaller) then
					actor:LookAt(kPlayerPos, true)
				end
				bForceNextSlot = iNextSlot~=0
				iNextSlot = 0
			else
				actor:LookAt(kPlayerPos, true)
				bForceNextSlot = iNextSlot~=1
				iNextSlot = 1
			end
		end
	end
	if 2 == iNowSlot then -- Run
		local fDistance = actor:TraceGround( 0.8, frameTime, 40.0, 15.0, true )		
--		ODS("64 fDistance : "..fDistance.."\n", false, 1509)
		if fDistance > 60 then
		else
			bForceNextSlot = true
			iNextSlot = 1
		end
	end
	if 3 == iNowSlot then -- opening
		local fDistance = actor:TraceGround( 1.0, frameTime, 30.0, 4.0, false )		
		if 40 < fDistance then
			bForceNextSlot = true
		end
		iNextSlot = 0
	end
	
	if actor:IsAnimationDone() or bForceNextSlot then
		action:SetSlot(iNextSlot)
		actor:PlayCurrentSlot(false)

		actor:DetachFrom(3333,true)

		local kEffectID = actor:GetAnimationInfoFromAction("EFFECT_ID", action, 0);	
		local kAttachNode = actor:GetAnimationInfoFromAction("ATTACH_NODE", action, 0);	
		local kScale = actor:GetAnimationInfoFromAction("SCALE", action, 0);

		if nil==kEffectID or ""==kEffectID then
			return true;
		end

		if nil==kAttachNode or ""==kAttachNode then
			return true;
		end

		if nil==kScale or ""==kScale then
			kScale = 1.0
		else
			kScale = tonumber(kScale)
		end
		
		actor:AttachParticleS(3333, kAttachNode, kEffectID, kScale);

	end
	
	return true

end
function Act_Trace_Ground_OnCleanUp(actor, action)
	return true
end

function Act_Trace_Ground_OnLeave(actor, action)	
	return true
end

