
function Act_Duel_SlowMotion_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	actorID = actor:GetID()
	action:SetDoNotBroadCast(true);

	action:SetParamInt(4,0)	--	Step
	action:SetParamInt(5,0);	--	SlowMotion Step

	actor:StopJump();
	actor:SetNoWalkingTarget(false);
	
	actor:StartGodTime(0);
	action:SetParamFloat(9,GetAccumTime());
	
	actor:SetCanHit(false);
	actor:Concil(false)
	
	actor:AttachParticle(9112,"p_ef_heart","ef_end_dem_01");
	actor:AttachParticle(9113,"p_ef_heart","ef_end_spot_01");
	
	g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
	, g_fChargeFocusFilterValue[2]
	, g_fChargeFocusFilterValue[3]
	, 0.2
	,true
	,true)	
	
	actor:ResetAnimation();
	
	g_world:SetUpdateSpeed(0.0008)
	
	if actor:IsMyActor() then
		LockPlayerInput(23)
	end

	--action:SetParamInt(6, 0) --알파 빼는 단계로 쓰자
	--action:SetParamInt(14, 0)
	
	return true
end
function Act_Duel_SlowMotion_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	currentSlot = action:GetCurrentSlot()
	local	param = action:GetParam(0)
	if param == "end" then
		return	true
	end
	
	local	iStep = action:GetParamInt(4)
	if iStep == 0 then
	
		if actor:IsAnimationDone() then
			--action:StartTimer(1000,0.25,0);
			--actor:DoDropItems()
			
			g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
			, g_world:GetWorldFocusFilterAlpha()
			, 0
			, 0.2
			,false
			,true);
			
			g_world:SetUpdateSpeed(1)

			action:SetParamInt(5,2)

			if action:GetScriptParam("TYPE") == "CHANGE_KFM" then
				actor:SetUseSubAnimation(true)
			end
			local	die_particle_id = actor:GetDieParticleID();
			if nil ~= die_particle_id and 0 < string.len(die_particle_id) then
				local heartPos ="char_root"
				local DieParticleNode = actor:GetDieParticleNode()
				if nil~=DieParticleNode and 0<string.len(DieParticleNode) then
					heartPos = DieParticleNode
				end
				actor:AttachParticleS(9114,heartPos,die_particle_id, actor:GetDieParticleScale())
			end
			
			if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
				action:SetParam(0,"end");
				action:SetParamInt(4,2)
				action:SetParamFloat(14,accumTime)
				return	true
			end
			actor:PlayNext();
			action:SetParamInt(4,1)
			
			if actor:IsMyActor() then
				UnLockPlayerInput(23)
			end
			return	true
		end	
		
	elseif iStep == 1 then
		action:SetParam(0,"end");
		action:SetParamInt(4,2)
		if 0==action:GetParamFloat(14) then
			action:SetParamFloat(14,accumTime)
		end
		return	true
	else
		return	true;
	end
	
	local	iSlowMotionStep = action:GetParamInt(5)


	--ODS("iStep : "..iStep.." iSlowMotionStep:"..iSlowMotionStep.." g_fBlowUpHorizonSpeed:"..g_fBlowUpHorizonSpeed.."\n");	
	
	--	Slow Motion Processing
	if iSlowMotionStep == 0 then
		local	fElapsedTime = GetAccumTime() - action:GetParamFloat(9);
		
		if fElapsedTime > 1 then
			action:SetParamFloat(9,GetAccumTime());
			action:SetParamInt(5,1)
		end
		return	true;
	
	elseif iSlowMotionStep == 1 then
			
		local	fElapsedTime = GetAccumTime() - action:GetParamFloat(9);
		local	fMaxSpeedTime = 4.0
		local	fRate = fElapsedTime/fMaxSpeedTime;
		if fRate> 1 then
			fRate = 1
		end
		
		local fSpeed = fRate;
		
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
			,true);
			
			g_world:SetUpdateSpeed(1)
			action:SetParamInt(5,2)
		end
	end
	return true
end

function Act_Duel_SlowMotion_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
			
	if( CheckNil(action == nil) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	
	g_world:SetUpdateSpeed(1)
	g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
				, g_world:GetWorldFocusFilterAlpha()
				, 0
				, 0.2
				,false
				,true);	
	
	actor:Concil(true)
	actor:SetBlowUp(false);
	
	actor:SetDownState(false);

	actor:FreeMove(false);
end

function Act_Duel_SlowMotion_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
		
	if actor:IsMyActor() then
		UnLockPlayerInput(23)
	end
	return	true;
end
