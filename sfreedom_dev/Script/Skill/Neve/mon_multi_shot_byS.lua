
function Act_Mon_Multi_Shot_byS_OnCheckCanEnter(actor, action)
end

function Act_Mon_Multi_Shot_byS_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local kPacket = action:GetParamAsPacket()
	if kPacket:IsNil() then
		return true
	end
	
	action:SetParamFloat(91, kPacket:PopFloat())
	action:SetParamFloat(92, kPacket:PopFloat())
	action:SetParamFloat(93, kPacket:PopFloat())

	local	prevAction = actor:GetAction()
	if prevAction:GetID() ~= "a_jump" then
		actor:Stop()
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Multi_Shot_byS_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
	end
	
	return true
end
function Act_Mon_Multi_Shot_byS_OnCastingCompleted(actor,action)	

	Act_Mon_Melee_SetState(actor,action,"FIRE")
	
	Act_Mon_Multi_Shot_byS_Fire(actor,action);
	
end

function Act_Mon_Multi_Shot_byS_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--GetActionResultCS(action, actor)

end

function Act_Mon_Multi_Shot_byS_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then
		
			local iHitCount = action:GetParamInt(1)
			local iMax = action:GetParamInt(2)
			if iMax == 0 then
				iMax = 10
			end
			return iHitCount<=iMax
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Multi_Shot_byS_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mon_Multi_Shot_byS_OnCleanUp(actor, action)
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	-- action:DeleteParamPacket()
end

function Act_Mon_Multi_Shot_byS_OnLeave(actor, action)

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
function Act_Mon_Multi_Shot_byS_OnTargetListModified(actor,action,bIsBefore)
	
	if bIsBefore == false then
		Act_Mon_Melee_DoDamage(actor,action);
	end
end

function Act_Mon_Multi_Shot_byS_OnTimer(actor,accumtime,action,iTimerID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local iMax = action:GetParamInt(2)
	local iHitCount = action:GetParamInt(1)
	if iHitCount == iMax then
		return	false
	end
	
	local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
	if fireEffect == nil or "" == fireEffect then
		fireEffect = "ef_BoneDragon_skill_01_02_char_root"
	end	
	local scale = actor:GetAnimationInfo("RANDOM_SCALE")
	if nil == scale then
		scale = 1
	end
	
	local	kSoundID = actor:GetAnimationInfo("HIT_SOUND_ID");
	
	local fDistance = action:GetParamFloat(211)
	local iPCount = action:GetParamInt(3)
	
	local iCount = 1
	while iCount <= iPCount do
		local ptStartPos = action:GetParamAsPoint(0)
		local kDir = action:GetParamAsPoint(iCount)
		local kNextPos = ptStartPos:_Add(kDir:_Multiply(iHitCount*fDistance))
		
		actor:AttachParticleToPoint(3000+iHitCount+(iCount*100), kNextPos ,fireEffect)
		if kSoundID~="" then
			actor:AttachSound(2700+iHitCount,kSoundID)
		end
		iCount = iCount + 1
	end

	iHitCount = iHitCount +1
	action:SetParamInt(1,iHitCount)
	
	return	iHitCount<=iMax

end

function Act_Mon_Multi_Shot_byS_OnEvent(actor,textKey)

    if textKey == "hit" or textKey == "fire" then
		
		CheckNil(nil==actor);
		CheckNil(actor:IsNil());		
	
		local FireEffect = actor:GetAnimationInfo("FIRE_EFFECT2")
		if nil~=FireEffect and ""~=FireEffect then
			local FireEffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE2")
			if nil==FireEffectNode or ""==FireEffectNode then
				FireEffectNode = "char_root"
			end
			actor:AttachParticle(3000, FireEffectNode ,FireEffect)
		end
		
		Act_Mon_QuakeCamera(actor)
			
        local   action = actor:GetAction()
		
		CheckNil(nil==action)
		CheckNil(action:IsNil())
		
		local kSkill = GetSkillDef(action:GetActionNo())
		if nil==kSkill or kSkill:IsNil() then
			return false
		end

		local kSpeed = kSkill:GetAbil(AT_MOVESPEED)
		
		if kSpeed <= 0 then
			kSpeed = 100
		end
		local kRange = kSkill:GetAbil(AT_ATTACK_RANGE)
		if kRange <= 0 then
			kRange = 1000
		end

		local kTotalTime = kRange/kSpeed

		
		local kTimeInter = kTotalTime*0.1
		local kMove = kTimeInter * kSpeed
		action:SetParamFloat(211, kMove)
		action:SetParamInt(2,kTotalTime/kTimeInter)

		local ptStartPos = actor:GetTranslate()
		action:SetParamAsPoint(0, ptStartPos)
		action:SetParamInt(1,1)

		local kTargetPos = Point3(action:GetParamFloat(91), action:GetParamFloat(92), action:GetParamFloat(93))
		local kDir = kTargetPos:_Subtract(ptStartPos)
		kDir:Unitize()

		local iPCount = kSkill:GetAbil(AT_MON_PROJECTILE_NUM)
		local iAngle = kSkill:GetAbil(AT_MON_PROJECTILE_ANGLE)
		
		local iFirstAngle = (iPCount - 1) * iAngle / 2
		
		local fFirstAngle = math.pi * iFirstAngle / 180.0
		local fAngle = math.pi * iAngle / 180.0
		
		local fFirstCos = math.cos(fFirstAngle)
		local fFirstSin = math.sin(fFirstAngle)
		
		local kFirst = Point3(0,0,0)
		kFirst:SetX((kDir:GetX() * fFirstCos) + (kDir:GetY() * fFirstSin))
		kFirst:SetY((kDir:GetX() * fFirstSin) + (kDir:GetY() * fFirstCos))
		kFirst:Unitize()
		
		action:SetParamInt(3,iPCount)
		action:SetParamAsPoint(1, kFirst)

		local iCount = 2
		while iCount <= iPCount do
			local fCos = math.cos(-(fAngle*iCount))
			local fSin = math.sin(-(fAngle*iCount))
			local kRot = Point3(0,0,0)
			kRot:SetX((kFirst:GetX() * fCos) + (kFirst:GetY() + fSin))
			kRot:SetY((kFirst:GetX() + fSin) + (kFirst:GetY() + fCos))
			kRot:Unitize()
			action:SetParamAsPoint(iCount, kRot)
			
			iCount = iCount + 1
		end
		
	    action:StartTimer(kTotalTime,kTimeInter,0)
	end
	
	return true;
end
