-- skill

function Act_Mon_Earthquake_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mon_Earthquake_SetState(actor,action,kState)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iNewState = -1;
	if kState == "BATTLEIDLE_START" then
		local CastEffect = actor:GetAnimationInfoFromAniName("CAST_EFFECT", "earthquake_battleidle"):GetStr()
		if nil~=CastEffect and ""~=CastEffect then
			local CastEffectNode = actor:GetAnimationInfoFromAniName("CAST_EFFECT_NODE","earthquake_battleidle"):GetStr()
			if nil==CastEffectNode or ""==CastEffectNode then
				CastEffectNode = "Bip01 Prop1"
			end
			actor:AttachParticle(3251,CastEffectNode ,CastEffect)
--칼 번쩍이는거
		end
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
	
		if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then
			return	false
		end
		
		action:SetSlot(4)
		iNewState = 3;

	end
	
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,iNewState)
	
	return	true;

end

function Act_Mon_Earthquake_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Earthquake_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_Earthquake_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Earthquake_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList();
		local	iTargetCount = kTargetList:size();
	
		if iTargetCount>0 then
			local	kTargetInfo = kTargetList:GetTargetInfo(0);
			if kTargetInfo:IsNil() == false then
			
				local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
				if kTargetPilot:IsNil() == false then
				
					local	kTargetActor = kTargetPilot:GetActor();
					if kTargetActor:IsNil() == false then
					
						--actor:LookAtBidirection(kTargetActor:GetPos());
						actor:LookAt(kTargetActor:GetPos(),true,true);
					
					end
				end
			end
		end
	end
	

	return true
end
function Act_Mon_Earthquake_OnCastingCompleted(actor,action)	

	Act_Mon_Earthquake_SetState(actor,action,"FIRE")
	
	Act_Mon_Earthquake_Fire(actor,action);
	
end

function Act_Mon_Earthquake_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--GetActionResultCS(action, actor)

	-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
	local	kTargetList = action:GetTargetList();
	local	iTargetCount = kTargetList:size();
	
	if iTargetCount>0 then
		local	kTargetInfo = kTargetList:GetTargetInfo(0);
		if kTargetInfo:IsNil() == false then
		
			local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then
			
				local	kTargetActor = kTargetPilot:GetActor();
				if kTargetActor:IsNil() == false then
					--actor:LookAtBidirection(kTargetActor:GetPos());
					actor:LookAt(kTargetActor:GetPos(),true,true);
				end
			end
		end
	end
	
end

function Act_Mon_Earthquake_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if iState == 3 then
			if actor:IsAnimationDone() == true then
				return false;
			end
		else
			if actor:IsAnimationDone() == true then
			
				local	iHitCount = action:GetParamInt(1)
			
				if iHitCount>=5 then
					return Act_Mon_Earthquake_SetState(actor,action,"RETURN")
				end
			end
		end
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Earthquake_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Earthquake_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mon_Earthquake_OnCleanUp(actor, action)

	actor:DetachFrom(3251);

end

function Act_Mon_Earthquake_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
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
function Act_Mon_Earthquake_OnTargetListModified(actor,action,bIsBefore)

	if bIsBefore == false then
	
		Act_Mon_Melee_DoDamage(actor,action);
	
	end

end

function Act_Mon_Earthquake_OnTimer(actor,accumtime,action,iTimerID)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local	iHitCount = action:GetParamInt(1)
	if iHitCount == 5 then
		return	false
	end
	
	local	kLookDir = actor:GetLookingDir()
	local	fPreDistance = 100	-- 이펙트가 나오기 시작할 거리
	local	fDistance = 30.0	-- 이펙스 사이의 간격 
	local	kSkillDef = GetSkillDef(action:GetActionNo())
	if nil~=kSkillDef and false==kSkillDef:IsNil() then
		fPreDistance = kSkillDef:GetAbil(AT_SKILL_MIN_RANGE)
		
		local iSkillRange = kSkillDef:GetAbil(AT_ATTACK_RANGE)
		if 0 < iSkillRange then
			fDistance = (iSkillRange-fPreDistance)*0.2--5로 나눈것
		end
	end

	local kFirePos = nil
	local FireNode = actor:GetAnimationInfo("FIRE_START_NODE")
	if nil~=FireNode and ""~=FireNode then
		kFirePos = actor:GetNodeTranslate(FireNode)
	end
	if nil==kFirePos then
		kFirePos = actor:GetPos()
	end

	local	kNextPos = kFirePos:_Add(kLookDir:_Multiply(fPreDistance+iHitCount*fDistance))
	
	local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
	if fireEffect ~= nil and "" ~= fireEffect then
		local rot = 0--Random()%(math.pi)
		local scale = actor:GetAnimationInfo("RANDOM_SCALE")
		if nil == scale then
			scale = 1
		else
			scale = 1 + (Random()%(scale*100)/100-scale*0.5) -- scale값이 랜덤 범위
			rot = Random()%(math.pi)
		end

		local quat = Quaternion(rot, Point3(0,0,1))
		actor:AttachParticleToPointWithRotate(31+iHitCount,kNextPos,fireEffect,quat,scale)
	end				

	iHitCount = iHitCount +1;
	action:SetParamInt(1,iHitCount);
	
	return	iHitCount<5;

end

function Act_Mon_Earthquake_OnEvent(actor,textKey)

    if textKey == "hit" or textKey == "fire" then
		
		if( CheckNil(nil==actor) ) then return false end
		if( CheckNil(actor:IsNil()) ) then return false end		

		local   action = actor:GetAction();

		if( CheckNil(nil==action) ) then return false end
		if( CheckNil(action:IsNil()) ) then return false end

		if action:GetActionParam() == AP_CASTING then
			return
		end
	
		local FireEffect = actor:GetAnimationInfo("FIRE_EFFECT2")
		if nil~=FireEffect and ""~=FireEffect then
			local FireEffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE2")
			if nil==FireEffectNode or ""==FireEffectNode then
				FireEffectNode = "char_root"
			end
			actor:AttachParticle(3252, FireEffectNode ,FireEffect)
		end
		local kQuakeFactor = actor:GetAnimationInfo("QUAKE_FACTOR")
		if nil==kQuakeFactor or ""==kQuakeFactor then
			kQuakeFactor = 5
		else
			kQuakeFactor = tonumber(kQuakeFactor)
		end
		
		local kQuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
		if nil==kQuakeTime or ""==kQuakeTime then
			kQuakeTime = 1
		elseif "NONE"==kQuakeTime then
		else
			kQuakeTime = tonumber(kQuakeTime)
		end
		
		if "NONE"~=kQuakeTime then
			QuakeCamera(kQuakeTime,kQuakeFactor,0,0.1,5);
		end
		
       
		local	kSkillDef = GetSkillDef(action:GetActionNo())
		local kTotalTime = 1.0
		if nil~=kSkillDef and false==kSkillDef:IsNil() then
			local kSpeed = kSkillDef:GetAbil(AT_MOVESPEED)
			if kSpeed <= 0 then
				kSpeed = 190
			end
			local kRange = kSkillDef:GetAbil(AT_ATTACK_RANGE)
			if kRange <= 0 then
				kRange = 300
			end

			kTotalTime = kRange/kSpeed
		end
		local kTimeInter = kTotalTime*0.2
	    action:StartTimer(kTotalTime,kTimeInter,0)
--	    action:StartTimer(1.0,0.1,0)
	    
	end
	
	return true;
end
