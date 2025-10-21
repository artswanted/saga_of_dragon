--	주의!!! 이 루아를 사용하는 액터가 맵에 반드시 동시에 하나만 존재해야한다.

-- [VELOCITY] : Speed to blow up
-- [NO_HORI_MOVE] : 횡 이동을 할 것인가.

g_fGravity = -800.0
g_meleedrop = 0

function Act_Die_BlowUp_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	actorID = actor:GetID()
	--ODS("Act_Die_BlowUp_OnEnter actionID : "..action:GetID().."\n");

	CloseUI("FRM_WARNING")
	local	fVelocity = action:GetScriptParamAsInt("VELOCITY");
	local	fInitVelocity = math.sqrt(2*-g_fGravity*fVelocity);

	action:SetParamFloat(1,fInitVelocity);	-- Initial Velocity
	action:SetParamInt(2,-1);	-- Step
	action:SetParamInt(5,0);	--	SlowMotion Step
	action:SetParamFloat(11,0);
	actor:FreeMove(true);	
	actor:StopJump();
	actor:SetNoWalkingTarget(false);
	action:SetParamFloat(10,g_world:GetAccumTime());
	action:SetParamFloat(13,g_world:GetAccumTime());	
	actor:StartGodTime(0);
	actor:SetMovingDelta(Point3(0,0,0));
	action:SetParamFloat(9,GetAccumTime());
	
	actor:SetCanHit(false);
	actor:Concil(false)
	
	--actor:AttachParticleToPoint(9112,actor:GetNodeWorldPos("p_ef_heart"),"ef_end_dem_01");
	--actor:AttachParticleToPoint(9113,actor:GetNodeWorldPos("p_ef_heart"),"ef_end_spot_01");
	actor:AttachParticle(9112,"p_ef_heart","ef_end_dem_01");
	actor:AttachParticle(9113,"p_ef_heart","ef_end_spot_01");
	
	g_world:SetUpdateSpeed(1)
	
	action:SetParamFloat(14,-1);
	
	g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
	, g_fChargeFocusFilterValue[2]
	, g_fChargeFocusFilterValue[3]
	, 0.2
	,true
	,true)	
	
	actor:ResetAnimation();
	
	g_world:SetUpdateSpeed(0.0008)

	action:SetParamInt(6, 0) --알파 빼는 단계로 쓰자
	action:SetParamInt(14, 0)

	return true
end
function Act_Die_BlowUp_OnTimer(actor,accumTime,action,timerid)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	-- if( CheckNil(action == nil) ) then return false end
	-- if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Die_BlowUp_OnTimer\n");
	actor:DoDropItems(1,15.0);
	return	true;
end

function Act_Die_BlowUp_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	currentSlot = action:GetCurrentSlot()
	local	param = action:GetParam(0)

	if param == "end" then -- change kfm 인 애들을 위해
--		if false == g_world:IsHaveWorldAttr(GATTR_MISSION+GATTR_SUPER_GROUND) then
			local deltatime = accumTime - action:GetParamFloat(14)
			if g_dyingAlphaDelayTime < deltatime then
				if 0 == action:GetParamInt(6) then
					actor:SetTargetAlpha(actor:GetAlpha(), 0.0, g_dyingTime);
					action:SetParamInt(6, 1)
				elseif g_dyingAlphaDelayTime+g_dyingTime < deltatime then
					g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
				end
			end
--		end
		
		g_world:SetUpdateSpeed(1)
		return	false
	end
	
	local	iStep = action:GetParamInt(2);
	local	fTotalElapsedTime = action:GetParamFloat(11);
	local	fTotalElapsedTime2 = accumTime - action:GetParamFloat(13);
	local	fTotalPlayTime = 10;
	
	local	iSlowMotionStep = action:GetParamInt(5)
	
	action:SetParamFloat(11,fTotalElapsedTime+frameTime);

	if fTotalElapsedTime2>fTotalPlayTime then
--		if false == g_world:IsHaveWorldAttr(GATTR_MISSION+GATTR_SUPER_GROUND) then
			local param = action:GetParamInt(6)
			if 0 == param then
				actor:SetTargetAlpha(actor:GetAlpha(), 0.0, g_dyingTime);
				action:SetParamInt(6,1);				
			elseif 1 == param and fTotalElapsedTime2>(fTotalPlayTime+g_dyingTime) then
				g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
			end
--		end
		
		g_world:SetUpdateSpeed(1)
		return	 false;
	end
	
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
	
	
	--	Position Moving Processing
	
	if iStep == -1 then
	
		if actor:IsAnimationDone() then
			actor:PlayNext()
			action:SetParamInt(2,0);
		end
		
	elseif iStep == 0 then
	
		if actor:IsAnimationDone() then
			actor:PlayNext()
			action:SetParamInt(2,1);
		end
			
	end

	if iStep == -1 or iStep == 0 or iStep == 1 then
	
		local	kMovingDir = actor:GetLookingDir();
		local	fZ_Velocity_Init = action:GetParamFloat(1);
		local	fZ_Velocity = g_fGravity*(0.5*frameTime+fTotalElapsedTime)+fZ_Velocity_Init;
		kMovingDir:SetZ(fZ_Velocity);
		
		if kMovingDir:GetX() > 0 then
			g_meleedrop = g_meleedrop+1
			if g_meleedrop == 2 then
				g_meleedrop = 0
			end
		end

		if action:GetScriptParam("NO_HORI_MOVE") ~= "TRUE" then
		
			kMovingDir:SetX(kMovingDir:GetX()*-1*g_fBlowUpHorizonSpeed);
			kMovingDir:SetY(kMovingDir:GetY()*-1*g_fBlowUpHorizonSpeed);
	
		end
		actor:SetMovingDelta(kMovingDir);
		
	elseif iStep == 2 then
		
		local	kMovingDir = actor:GetLookingDir();
		local	fZ_Velocity_Init = action:GetParamFloat(1);
		local	fZ_Velocity = g_fGravity*(0.5*frameTime+fTotalElapsedTime)+fZ_Velocity_Init;
		kMovingDir:SetZ(fZ_Velocity);
		
		if action:GetScriptParam("NO_HORI_MOVE") ~= "TRUE" then
			kMovingDir:SetX(kMovingDir:GetX()*-1*g_fBlowUpHorizonSpeed*0.2);
			kMovingDir:SetY(kMovingDir:GetY()*-1*g_fBlowUpHorizonSpeed*0.2);
			
		end
		
		actor:SetMovingDelta(kMovingDir);
		
	elseif iStep == 3 and actor:IsAnimationDone() == true then

		actor:SetDownState(false);
		--action:StartTimer(1000,0.25,0);
		actor:DoDropItems();

		g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
			, g_world:GetWorldFocusFilterAlpha()
			, 0
			, 0.2
			,false
			,true);
			
		g_world:SetUpdateSpeed(1)

		if action:GetScriptParam("TYPE") ~= "CHANGE_KFM" then
			if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then
				action:SetParam(0,"end");
				action:SetParamFloat(14,accumTime)
				
				g_world:SetUpdateSpeed(1)
				return	false
			end
		end
		
		action:SetParamFloat(12,accumTime);
		action:SetParamInt(2,4)
		
		return	true;	
	elseif iStep == 4 then	--	일정시간 누워있기
	
		local	fElapsedTime = accumTime - action:GetParamFloat(12)
		if fElapsedTime>2 then

			if action:GetScriptParam("TYPE") == "CHANGE_KFM" then
				actor:SetUseSubAnimation(true)	
				local	die_particle_id = actor:GetDieParticleID(); -- 다이 파티클이 있으면 붙여주기
			    if die_particle_id ~= "" then
				    local heartPos = actor:GetNodeTranslate("char_root")
					local DieParticleNode = actor:GetDieParticleNode()
					if nil~=DieParticleNode and 0<string.len(DieParticleNode) then
						heartPos = actor:GetNodeTranslate(DieParticleNode)
					end
			        actor:AttachParticleToPointS(436, heartPos, die_particle_id, actor:GetDieParticleScale())
			    end
			end
		
			actor:PlayNext();
			action:SetParamInt(2,5)
			action:SetParamFloat(12,accumTime);
		
		end
		return	true;
		
	elseif iStep == 5 then	--	일어나기
		if action:GetScriptParam("TYPE") == "CHANGE_KFM2" then
			local	fElapsedTime = accumTime - action:GetParamFloat(12)
			if fElapsedTime>2 then
				local heartPos = actor:GetNodeTranslate("char_root")
				actor:AttachParticleToPoint(436, heartPos, "ef_deathmaster_die_02_char_root");
				actor:DetachFrom(53911);
				actor:SetUseSubAnimation(true)	
				actor:PlayNext();
				action:SetParamInt(2,6)
			end		

			return	true;	
						
		elseif actor:IsAnimationDone() == true then
			if actor:GetAnimationLength(action:GetSlotAnimName(5)) == 0 then
				if action:GetScriptParam("TYPE") == "CHANGE_KFM" then
					local remain = actor:GetAnimationInfo("REMAIN_BODY")
					if nil == remain or "true" ~= remain then
						actor:SetHide(true)
					end
				end
				action:SetParam(0,"end");
				action:SetParamFloat(14,accumTime)
				
				g_world:SetUpdateSpeed(1)
				return	false
			end
			actor:PlayNext()
			action:SetParamInt(2,6)
			
			return	true;	
		end
	elseif iStep == 6 then
		
		g_world:SetUpdateSpeed(1)
		return false
	end

	if actor:IsMeetFloor() == true then
	
		if iStep == 1 then
		
			local	fVelocity = action:GetScriptParamAsInt("VELOCITY")*g_fBlowUpSecondJumpPower;
			local	fInitVelocity = math.sqrt(2*-g_fGravity*fVelocity);		
			
			action:SetParamFloat(1,fInitVelocity);	-- Initial Velocity
			action:SetParamFloat(11,0);
			action:SetParamInt(2,2);
			action:SetParamInt(4,0);
			local pt = actor:GetTranslate()
			pt:SetZ(pt:GetZ() - 30)
			actor:AttachParticleToPoint(200, pt, "e_jump")
			actor:PlayNext();
			actor:SetBlowUp(false);	
			
		elseif iStep == 2 then
		
			if action:GetParamInt(4) == 0 then
				action:SetParamInt(4,1);
			else
		
				local pt = actor:GetTranslate()
				pt:SetZ(pt:GetZ() - 30)
				actor:AttachParticleToPoint(201, pt, "e_jump")
				action:SetParamInt(2,3);
				actor:ResetAnimation();
				action:SetSlot(3);
				actor:SetTargetAnimation(action:GetCurrentSlotAnimName());
				actor:SetSendBlowStatus(false, true);
				

				actor:SetDownState(true);
			
			end
		end
	
	end
	return true
end

function Act_Die_BlowUp_OnCleanUp(actor, action)
	
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
	if false == g_world:IsHaveWorldAttr(GATTR_FLAG_MISSION) then
		g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
	end
end

function Act_Die_BlowUp_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
			
	if( CheckNil(action == nil) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	
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
			g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
		end
		
	end	
	return	true;
end

function Act_Die_BlowUp_OnEvent(actor,textKey)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	local action = actor:GetAction()
	if( CheckNil(action == nil) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
    if textKey == "hit" or textKey == "fire" then
		local alpha = actor:GetAnimationInfo("DIE_ALPHA")
		if nil ~= alpha then
			alpha = tonumber(alpha)
			actor:SetTargetAlpha(actor:GetAlpha(),0, alpha)
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
