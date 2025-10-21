-- Idle

function Act_Trace_Ground2_OnEnter(actor, action)

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
		iDist = 40
	else
		iDist = tonumber(iDist)
		if 0>=iDist then iDist = 40 end
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

function Act_Trace_Ground2_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());

	local currentSlot = action:GetCurrentSlot()
	local param = action:GetParam(0)
	local iIdleType = action:GetParamInt(6);
	local iDist = action:GetParamInt(7);
	if 0>=iDist then iDist = 40 end

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
	-- Next
	local bForceNextSlot = false
	if 0 <= iNowSlot and 6 >= iNowSlot then -- Idle
		local fDistance = actor:TraceGround( 0.5, frameTime, iDist, 15.0, true )
		if fDistance > FarDist then
			bForceNextSlot = true
			iNextSlot = 7
		else
			if fDistance <= iDist then
				local bLookCaller = actor:GetAnimationInfoFromAction("LOOK_CALLER", action, 0);	
				if nil~=bLookCaller and 0~=tonumber(bLookCaller) then
					actor:LookAt(kPlayerPos, true)
				end

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
		local fDistance = actor:TraceGround( 0.8, frameTime, iDist, 15.0, true )		
		if fDistance > FarDist then
		else
			bForceNextSlot = true
			iNextSlot = 6
		end
	end
	if 8 == iNowSlot then -- opening
		local fDistance = actor:TraceGround( 1.0, frameTime, 30.0, 4.0, false )		
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

	local kParam = "MOVE"
	if 0<=iNextSlot and 6>iNextSlot then
		kParam = "STOP"
	end

	action:SetParam(0, kParam)
	
	return true

end
function Act_Trace_Ground2_OnCleanUp(actor, action)
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());

	actor:DetachFrom(1996)
	return true
end

function Act_Trace_Ground2_OnLeave(actor, action)
	return true
end

function ChoosePetAni(iNextSlot, actor, accumTime)
	local kCon = iNextSlot
	if 3>iNextSlot and 0<=iNextSlot then --idle
		--actor:GetAction():SetParamInt(7,iNextSlot)
		kCon = actor:UpdatePetActionQueue(accumTime)
	end
	return kCon
end

g_PetParticle = {"ef_Pet_emo_hungry_01_char_root", "ef_Pet_emo_sleep_01_char_root", "ef_Pet_emo_angry_01_char_root"}

function AttachBaloonParticle(actor, iNextSlot)
	if 3>iNextSlot or 5<iNextSlot then
		actor:DetachFrom(1996)
	else
		local kPath = g_PetParticle[iNextSlot-2]
		if nil~=kPath then
			actor:AttachParticle(1996, "p_ef_star", kPath)
		end
	end
end
