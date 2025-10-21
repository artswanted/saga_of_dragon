--	주의!!! 이 루아를 사용하는 액터가 맵에 반드시 동시에 하나만 존재해야한다.

function Act_Die_SlowMotion_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	actorID = actor:GetID()
	CloseUI("FRM_WARNING")

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
	ChangeDieAnimation(actor, action)
	
	g_world:SetUpdateSpeed(0.0008)

	action:SetParamInt(6, 0) --알파 빼는 단계로 쓰자
	action:SetParamInt(14, 0)
	
	action:SetParamFloat(2, GetDyingTime(actor,action))	--	알파를 빼는 시간

	return true
end
function Act_Die_SlowMotion_OnTimer(actor,accumTime,action,timerid)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	ODS("Act_Die_SlowMotion_OnTimer\n");
	actor:DoDropItems(1,15.0);
	return	true;
end

function Act_Die_SlowMotion_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	currentSlot = action:GetCurrentSlot()
	local	param = action:GetParam(0)
	local 	totalalphafadeoutTime = action:GetParamFloat(2)
	if param == "end" then
		--if false == g_world:IsHaveWorldAttr(GATTR_MISSION+GATTR_SUPER_GROUND) then
			local deltatime = accumTime - action:GetParamFloat(14)
			if g_dyingAlphaDelayTime < deltatime then
				if 0 == action:GetParamInt(6) then
					actor:SetTargetAlpha(actor:GetAlpha(), 0.0, totalalphafadeoutTime);
					action:SetParamInt(6, 1)
				elseif g_dyingAlphaDelayTime+totalalphafadeoutTime < deltatime then
					g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
				end
			end
		--end
		return	true
	end
	
	local	iStep = action:GetParamInt(4)
	if iStep == 0 then
	
		if actor:IsAnimationDone() then
			--action:StartTimer(1000,0.25,0);
			actor:DoDropItems()
			
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
			ChangeDieAnimation(actor, action)
			action:SetParamInt(4,1)
			return	true
		end	
		
	elseif iStep == 1 then
		if  true==actor:IsAnimationLoop() or actor:IsAnimationDone() or false == g_world:IsHaveWorldAttr(GATTR_MISSION+GATTR_SUPER_GROUND) then
			action:SetParam(0,"end");
			action:SetParamInt(4,2)
			if 0==action:GetParamFloat(14) then
				action:SetParamFloat(14,accumTime)
			end
			return	true
		end	
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

function Act_Die_SlowMotion_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(action == nil) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	
	g_world:SetUpdateSpeed(1)
	if actor:GetAbil(AT_GRADE) == EMGRADE_BOSS then
		g_world:OnBossDie()
	end
	
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

function Act_Die_SlowMotion_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	ODS("Act_Die_SlowMotion_OnLeave action:"..action:GetID().."\n");
	
	if action:GetActionType() ~= "EFFECT" then
		
		actor:SetSendBlowStatus(false);
		
		local	kEffectDef = GetEffectDef(actor:GetAction():GetActionNo());
		local	fGodTime = 1;
		if kEffectDef:IsNil() == false then
			fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0;
		end
		actor:StartGodTime(fGodTime);
		
	end
	
	local	kCurAction = actor:GetAction();
	if( CheckNil(kCurAction == nil) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	
	if actor:GetPilot():IsPlayerPilot() == false then
		
		if actor:GetAbil(AT_MANUAL_DIEMOTION) == 0 then	-- 0은 서버에서 죽임, 101은 죽이지 않음(후처리 필요)
			if( CheckNil(g_world == nil) ) then return true end
			if( CheckNil(g_world:IsNil()) ) then return true end
			g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
		end
		
	end	

	return	true;
end

function Act_Die_SlowMotion_OnEvent(actor,textKey)
	local action = actor:GetAction()
    if textKey == "hit" or textKey == "fire" then
		local alpha = actor:GetAnimationInfo("DIE_ALPHA")
		if nil ~= alpha then
			alpha = tonumber(alpha)
			actor:SetTargetAlpha(actor:GetAlpha(),0,alpha)
		end
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
	end
end
