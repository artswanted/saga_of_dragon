
function Act_Mon_SummonBone2_OnCheckCanEnter(actor, action)
end

function Act_Mon_SummonBone2_SetState(actor,action,kState)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iNewState = -1;
	if kState == "BATTLEIDLE_START" then
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false	
		end
				
		action:SetSlot(0)
		iNewState = 0;
	
	elseif kState == "BATTLEIDLE_LOOP" then
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
		
			action:SetSlot(3);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1;
		
	elseif kState == "FIRE" then
	
		action:SetSlot(2)
		iNewState = 2;
	elseif kState == "RETURN" then
	
		action:SetSlot(4)
		iNewState = 3;

	end
	
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,iNewState)
	
	return	true;

end

function Act_Mon_SummonBone2_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local kPacket = action:GetParamAsPacket()
	if kPacket:IsNil() then
		return true
	end
	
	local iStartPos = kPacket:PopInt()
	local kStartPos = "bone_start"..iStartPos
	
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local ptStartPos = g_world:GetObjectPosByName(kStartPos)
	
	action:SetParamAsPoint(1, ptStartPos)
	
	local	prevAction = actor:GetAction()
	if prevAction:GetID() ~= "a_jump" then
		actor:Stop()
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_SummonBone2_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_SummonBone2_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_SummonBone2_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
	end
	
	action:DeleteParamPacket()
	return true
end
function Act_Mon_SummonBone2_OnCastingCompleted(actor,action)	

	Act_Mon_SummonBone2_SetState(actor,action,"FIRE")
	
	Act_Mon_SummonBone2_Fire(actor,action);
	
end

function Act_Mon_SummonBone2_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--GetActionResultCS(action, actor)

end

function Act_Mon_SummonBone2_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then
		
			local	iHitCount = action:GetParamInt(1)
		
			return iHitCount<10
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_SummonBone2_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_SummonBone2_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mon_SummonBone2_OnCleanUp(actor, action)

end

function Act_Mon_SummonBone2_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	if action:GetActionType()=="EFFECT" or
		actionID == "a_freeze" or
		actionID == "a_reverse_gravity" or
		actionID == "a_blow_up" or
		actionID == "a_blow_up_small" or
		actionID == "a_knock_back" or
		actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_run_right" or
		actionID == "a_run_left" or
		actionID == "a_handclaps" then
		return true
	end
	return false 
end
function Act_Mon_SummonBone2_OnTargetListModified(actor,action,bIsBefore)

	if bIsBefore == false then
	
		Act_Mon_Melee_DoDamage(actor,action);
	
	end

end

function Act_Mon_SummonBone2_OnTimer(actor,accumtime,action,iTimerID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local	iHitCount = action:GetParamInt(1)
	if iHitCount == 10 then
		return	false
	end
	
	local	kLookDir = Point3(-1,0,0)
	local	fPreDistance = 200
	local	fDistance = 30.0
	local	kSkillDef = GetSkillDef(action:GetActionNo())
	if nil~=kSkillDef and false==kSkillDef:IsNil() then
		--fPreDistance = kSkillDef:GetAbil(AT_SKILL_MIN_RANGE)
		
		local iSkillRange = kSkillDef:GetAbil(AT_ATTACK_RANGE)
		-- if 0 < iSkillRange then
			-- fDistance = iSkillRange*0.1 --10으로 나눈것
		-- end
		
		fDistance = action:GetParamFloat(211)
	end
	local ptStartPos = action:GetParamAsPoint(1)
	local	kNextPos = ptStartPos:_Add(kLookDir:_Multiply(fPreDistance+iHitCount*fDistance))
	
	local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
	if fireEffect == nil or "" == fireEffect then
		fireEffect = "ef_BoneDragon_skill_01_02_char_root"
	end	
	if fireEffect ~= nil and "" ~= fireEffect then
		local rot = 0--Random()%(math.pi)
		local scale = actor:GetAnimationInfo("RANDOM_SCALE")
		if nil == scale then
			scale = 1
		end

		actor:AttachParticleToPoint(3251, kNextPos ,fireEffect)
	end				

	local	kSoundID = actor:GetAnimationInfo("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784+iHitCount,kSoundID)
	end

	iHitCount = iHitCount +1;
	action:SetParamInt(1,iHitCount);
	
	return	iHitCount<10;

end

function Act_Mon_SummonBone2_OnEvent(actor,textKey)

    if textKey == "hit" or textKey == "fire" then
		
		CheckNil(nil==actor);
		CheckNil(actor:IsNil());		
	
		local FireEffect = actor:GetAnimationInfo("FIRE_EFFECT2")
		if nil~=FireEffect and ""~=FireEffect then
			local FireEffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE2")
			if nil==FireEffectNode or ""==FireEffectNode then
				FireEffectNode = "char_root"
			end
			--actor:AttachParticle(3252, FireEffectNode ,FireEffect)
		end
		
		Act_Mon_QuakeCamera(actor)
			
        local   action = actor:GetAction();
		
		CheckNil(nil==action);
		CheckNil(action:IsNil());
		local	kSkillDef = GetSkillDef(action:GetActionNo())
		local kTotalTime = 1.0
		
		local kSpeed = kSkillDef:GetAbil(AT_MOVESPEED)
		
		if nil~=kSkillDef and false==kSkillDef:IsNil() then
			
			--ODS("speed : "..kSpeed.."\n",false,987)
			if kSpeed <= 0 then
				kSpeed = 200
			end
			local kRange = kSkillDef:GetAbil(AT_ATTACK_RANGE)
			if kRange <= 0 then
				kRange = 300
			end

			kTotalTime = kRange/kSpeed
		end
		local kTimeInter = kTotalTime*0.1
	    action:StartTimer(kTotalTime,kTimeInter,0)
		
		local kMove = kTimeInter * kSpeed
		action:SetParamFloat(211, kMove)
		--ODS("Move : "..kMove.." inter :"..kTimeInter.." speed : "..kSpeed.."\n",false,987)
--	    action:StartTimer(1.0,0.1,0)
	    
	end
	
	return true;
end
