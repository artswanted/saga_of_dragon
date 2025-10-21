function Act_PAREL_Die_OnEnter(actor, action)
	if actor == nil or actor:IsNil() then
		return false
	end

	
	CallUI("FRM_MOVIE_IN")
	action:SetParamInt(0,0) --  ActionStep

	
	-- Slow Motion
	local	fVelocity = action:GetScriptParamAsInt("VELOCITY")
	local	fInitVelocity = math.sqrt(2*-g_fGravity*fVelocity)
	action:SetParamInt(1,0)	--	SlowMotion Step
	action:SetParamFloat(2,fInitVelocity)	-- Initial Velocity
	actor:FreeMove(true)
	actor:StopJump()
	actor:SetNoWalkingTarget(false)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local fAccumTime = g_world:GetAccumTime()
	action:SetParamFloat(9,fAccumTime)
	action:SetParamFloat(10,fAccumTime)
	action:SetParamFloat(11,fAccumTime)
	
--	actor:StartGodTime(0)
	actor:SetMovingDelta(Point3(0,0,0))
	actor:SetCanHit(false)
	actor:Concil(false)

	actor:AttachParticle(9112,"p_ef_heart","ef_end_dem_01");
	actor:AttachParticle(9113,"p_ef_heart","ef_end_spot_01");

	g_world:SetUpdateSpeed(1)

	g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
	, g_fChargeFocusFilterValue[2]
	, g_fChargeFocusFilterValue[3]
	, 0.2
	,true
	,true)

	actor:ResetAnimation()
	g_world:SetUpdateSpeed(0.0008)

--	actor:AttachParticle(99, "p_ef_center", "e_ef_parel_skill_04")
--	actor:AttachParticleToPoint(100, kCenter, "e_ef_parel_die_01")
	return true
end

function Act_PAREL_Die_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end

	local action = actor:GetAction()		-- 지금 엑션 객체를 반환
	if action:IsNil() == true then
		ODS("[PAREL] Action Error")
		return false
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local ActionStep = action:GetParamInt(0)
	local SlowStep = action:GetParamInt(1)
	
	local curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번
	
	local	fTotalElapsedTime = action:GetParamFloat(9)
	local	fTotalElapsedTime2 = accumTime - action:GetParamFloat(10)
	local	fTotalPlayTime = 10
	action:SetParamFloat(9,fTotalElapsedTime+frameTime)

	if fTotalElapsedTime2 > fTotalPlayTime then
		return false
	end
	
	--	Slow Motion Processing
	if 1 == SlowStep then
	    local	fElapsedTime = GetAccumTime() - action:GetParamFloat(10)
		local	fMaxSpeedTime = 4.0
		local	fRate = fElapsedTime/fMaxSpeedTime;
		if fRate> 1 then
			fRate = 1
		end

		local fSpeed = fRate

		if fRate<3.5 then
			fSpeed = fSpeed*0.3
		end

		g_world:SetUpdateSpeed(fSpeed*0.995+0.005)

		if fRate == 1.0 then
			g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
			, g_world:GetWorldFocusFilterAlpha()
			, 0
			, 0.2
			,false
			,true)
			
			g_world:SetUpdateSpeed(1)
			action:SetParamInt(1,2)
		end
	elseif 0 == SlowStep then
	    local	fElapsedTime = GetAccumTime() - action:GetParamFloat(10)

		if fElapsedTime > 1 then
			action:SetParamFloat(10,GetAccumTime())
			action:SetParamInt(1,1)
		end
		return true
	end
	
	if 0 == ActionStep then
	    if actor:IsAnimationDone() then
			actor:PlayNext()
			action:SetParamInt(0,1)
		end
	end
	
	if 2 > ActionStep then
	    local	kMovingDir = actor:GetLookingDir()
		local	fZ_Velocity_Init = action:GetParamFloat(2)
		local	fZ_Velocity = g_fGravity*(0.5*frameTime+fTotalElapsedTime)+fZ_Velocity_Init
		kMovingDir:SetZ(fZ_Velocity);

		if kMovingDir:GetX() > 0 then
			g_meleedrop = g_meleedrop+1
			if g_meleedrop == 2 then
				g_meleedrop = 0
			end
		end

		if action:GetScriptParam("NO_HORI_MOVE") ~= "TRUE" then
			kMovingDir:SetX(kMovingDir:GetX()*-1*g_fBlowUpHorizonSpeed)
			kMovingDir:SetY(kMovingDir:GetY()*-1*g_fBlowUpHorizonSpeed)
		end
		actor:SetMovingDelta(kMovingDir)
	elseif 2 == ActionStep then
	    local	kMovingDir = actor:GetLookingDir()
		local	fZ_Velocity_Init = action:GetParamFloat(2)
		local	fZ_Velocity = g_fGravity*(0.5*frameTime+fTotalElapsedTime)+fZ_Velocity_Init
		kMovingDir:SetZ(fZ_Velocity)
		
		if action:GetScriptParam("NO_HORI_MOVE") ~= "TRUE" then
			kMovingDir:SetX(kMovingDir:GetX()*-1*g_fBlowUpHorizonSpeed*0.2);
			kMovingDir:SetY(kMovingDir:GetY()*-1*g_fBlowUpHorizonSpeed*0.2);

		end
		actor:SetMovingDelta(kMovingDir)
	elseif 3 == ActionStep then
	    actor:SetDownState(false);
		action:StartTimer(100000,0.5,0)
		action:SetParamFloat(5,accumTime)
		action:SetParamInt(0,4)
		return	true
	elseif 4 == ActionStep then
	
	    	-- 큐브 파렐로 변신
	    	local kTargetPos= actor:GetTranslate()
			local guid = GUID("")
			guid:Generate()
			local kCubeParel = g_pilotMan:NewPilot(guid, 100057, 0)
			local kCubeParelActor = kCubeParel:GetActor()
			if kCubeParelActor:IsNil() == true then
				ODS("[PAREL] CubeParel Generate Error\n")
				return false
			end
			g_world:AddActor(guid, kCubeParelActor, Point3(kTargetPos:GetX(),kTargetPos:GetY(), 150 ), 6) -- NPC Create
			kCubeParelActor:SetClonePos(actor)
			kCubeParelActor:ClearActionState()
			kCubeParelActor:SetTargetAnimation("die")
			kCubeParelActor:AttachParticle(99, "char_root", "e_ef_parel_die_01")

			action:SetParamFloat(5, accumTime)	-- 시작시간 저장
			action:SetParamInt(0,5)
	elseif 5 == ActionStep then
	    local fTime = accumTime - action:GetParamFloat(5)
	    if fTime > 8.0 then
   			return false
		elseif fTime > 0.5 then
		    actor:SetHide(true)
		    actor:SetCanHit(false)
		end
	end

 	if actor:IsMeetFloor() == true then

		if 1 == ActionStep then

			local	fVelocity = action:GetScriptParamAsInt("VELOCITY")*g_fBlowUpSecondJumpPower;
			local	fInitVelocity = math.sqrt(2*-g_fGravity*fVelocity);

			action:SetParamFloat(2,fInitVelocity);	-- Initial Velocity
			action:SetParamFloat(9,0);
			action:SetParamInt(0,2);
			
			local pt = actor:GetTranslate()
			pt:SetZ(pt:GetZ() - 30)
			actor:AttachParticleToPoint(200, pt, "e_jump")
			actor:SetBlowUp(false)
		elseif 2 == ActionStep then

			local pt = actor:GetTranslate()
			pt:SetZ(pt:GetZ() - 30)
			actor:AttachParticleToPoint(201, pt, "e_jump")
			action:SetParamInt(0,3);
			actor:SetSendBlowStatus(false, true)
			actor:SetDownState(true)
		end
		
	end

	return true
end

function Act_PAREL_Die_OnCleanUp(actor, action)
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	
    g_world:SetUpdateSpeed(1)
	g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
				, g_world:GetWorldFocusFilterAlpha()
				, 0
				, 0.2
				,false
				,true)

	actor:Concil(true)
	actor:SetBlowUp(false)
	actor:SetDownState(false)
	actor:FreeMove(false)
end

function Act_PAREL_Die_OnLeave(actor, action)
	CallUI("FRM_MOVIE_OUT")
	
	actor:SetSendBlowStatus(false)
	
	-- 지워 
--	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
	return true
end

function Act_PAREL_Die_OnTimer(actor,accumTime,action,timerid)
	return	true
end
