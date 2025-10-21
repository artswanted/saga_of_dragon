-- skill

function Act_BoneDragon_Summon_Monster_OnCheckCanEnter(actor, action)
	
end

function Act_BoneDragon_Summon_Monster_SetState(actor,action,kState)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iNewState = -1;
	if kState == "START" then
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false	
		end
		action:SetSlot(0)
		iNewState = 0
	elseif kState == "IDLE" then
		action:SetSlot(1)
		iNewState = 1
	elseif kState == "ATTACK" then
		action:SetSlot(2)
		iNewState = 2
	elseif kState == "LOOP" then
		action:SetSlot(3)
		iNewState = 3
	elseif kState == "FINISH" then
		action:SetSlot(4)
		iNewState = 4
	end
	ODS("state : "..iNewState.."\n",false,987)
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,iNewState)
	
	return	true;

end

function Act_BoneDragon_Summon_Monster_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsEqualObjectName("B_cleef_BoneDragon") == true then
		CallUI("FRM_GOGO_LEFT")
		AddWarnDataTT(403300)
	end
	
	local kAniTime = action:GetAbil(AT_ANIMATION_TIME)
	if kAniTime == 0 then
		kAniTime = 8000
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(1, g_world:GetAccumTime())
	action:SetParamFloat(2, kAniTime*0.001)
	
	ODS("Act_Mon_Melee_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
	
	local	prevAction = actor:GetAction()
	if prevAction:GetID() ~= "a_jump" then
		actor:Stop()
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Melee_OnCastingCompleted(actor,action)
	else
	
		if( Act_BoneDragon_Summon_Monster_SetState(actor,action,"START") == false) then
			Act_BoneDragon_Summon_Monster_SetState(actor,action,"IDLE")
		end	
		
	    ODS("Act_Mon_Melee_OnEnter Start Casting\n");
		
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList();
		local	iTargetCount = kTargetList:size();
	
		ODS("Act_Mon_Melee Casting Start iTargetCount :"..iTargetCount.."\n");

		
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
function Act_BoneDragon_Summon_Monster_OnCastingCompleted(actor,action)	

	Act_BoneDragon_Summon_Monster_SetState(actor,action,"ATTACK")
	Act_BoneDragon_Summon_Monster_FireEffect(actor)
	Act_Mon_Melee_Fire(actor,action);
	
end

function Act_BoneDragon_Summon_Monster_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local bAnimationDone = actor:IsAnimationDone()
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if iState == 2 then
			if bAnimationDone == true then
				Act_BoneDragon_Summon_Monster_SetState(actor,action,"LOOP")
			end
		elseif iState == 3 then
			local kAccumTime = action:GetParamFloat(1)
			local kAniTime = action:GetParamFloat(2)
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			if g_world:GetAccumTime() - kAccumTime > kAniTime then
				Act_BoneDragon_Summon_Monster_SetState(actor,action,"FINISH")
			end
		elseif iState == 4 then
			if bAnimationDone == true then
				return false
			end
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if bAnimationDone then
				Act_BoneDragon_Summon_Monster_SetState(actor,action,"IDLE")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Melee_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end

function Act_BoneDragon_Summon_Monster_OnCleanUp(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(125)
end

function Act_BoneDragon_Summon_Monster_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	--ODS("Act_Mon_Melee_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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

function Act_BoneDragon_Summon_Monster_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
       ODS("Act_Mon_Melee_OnEvent actor:"..actor:GetID().." textKey:"..textKey.."\n");
       
    if textKey == "hit" or textKey == "fire" then
        local   action = actor:GetAction();
		
		CheckNil(nil==action);
		CheckNil(action:IsNil());
	
		Act_Mon_Melee_DoDamage(actor,action);
		Act_BoneDragon_Summon_Monster_FireEffect(actor)
	end
	
	return true;
end

function Act_BoneDragon_Summon_Monster_FireEffect(actor)
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

	local EffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
	local Effect = actor:GetAnimationInfo("FIRE_EFFECT")
	local EffectScale = actor:GetAnimationInfo("FIRE_EFFECT_SCALE")
	if nil==EffectScale or ""==EffectScale then
		EffectScale = 1
	else
		EffectScale = tonumber(EffectScale)
	end

	if nil~=EffectNode and nil~=Effect then
		actor:AttachParticleS(125, EffectNode, Effect, EffectScale)
	end
end

function Act_BoneDragon_Summon_Monster_OnTargetListModified(actor,action,bIsBefore)
end
