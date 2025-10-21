-- skill

function Act_Mon_EnergyExplosion_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mon_EnergyExplosion_SetState(actor,action,kState)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iNewState = -1;
	if kState == "BATTLEIDLE_START" then
	
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false	
		end
				
		action:SetSlot(0)
		iNewState = 0;
	
	elseif kState == "BATTLEIDLE_LOOP" then

		local FireEffect = actor:GetAnimationInfoFromAniName("FIRE_EFFECT",action:GetSlotAnimName(2))
		if nil==FireEffect or FireEffect:IsNil() then
			FireEffect = "ef_deathmaster_skill_02_01_p_ef_head"
		else
			FireEffect = FireEffect:GetStr()
		end

		if "NONE"~=FireEffect then
			actor:AttachParticle(1204,"p_ef_head",FireEffect);
		end
		
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
		
			ODS("Act_Mon_EnergyExplosion_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
		
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

function Act_Mon_EnergyExplosion_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Mon_EnergyExplosion_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	actor:ResetAnimation()
	
	--	attack target pos
	local	ptTargetPos = 0;
	
	if IsSingleMode() then
		ptTargetPos = GetMyActor():GetPos();
	else
		local	kPacket = action:GetParamAsPacket();
		if kPacket == nil or kPacket:IsNil() then
			ODS("[SkillError] Act_Mon_EnergyExplosion_OnEnter, kPacket is not valid. actor : "..actor:GetPilotGuid():GetString().." action:"..action:GetID().."\n");
			ptTargetPos = actor:GetPos()
		else
			ptTargetPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat());
		end
	end	
	
	local	fSkillRange = action:GetSkillRange(0,actor)
	if fSkillRange == 0 then
		fSkillRange = 100
	end

	local effect = actor:GetAnimationInfoFromAniName("RANGE_EFFECT",action:GetCurrentSlotAnimName())	
	if nil~=effect then
		effect = effect:GetStr()
		if ""==effect then
			effect = "ef_deathmaster_skill_02_05_char_root"
		end
		if "NONE"~=effect then
			actor:AttachParticleToPointS(81190,ptTargetPos, effect, fSkillRange*0.0043);
		end
	end
	
	action:SetParamAsPoint(0,ptTargetPos);
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_EnergyExplosion_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_EnergyExplosion_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_EnergyExplosion_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
	    ODS("Act_Mon_EnergyExplosion_OnEnter Start Casting\n");
		
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
function Act_Mon_EnergyExplosion_OnCastingCompleted(actor,action)	

	Act_Mon_EnergyExplosion_SetState(actor,action,"FIRE")
	
	Act_Mon_EnergyExplosion_Fire(actor,action);
	
end

function Act_Mon_EnergyExplosion_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--GetActionResultCS(action, actor)

	-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
	local	kTargetList = action:GetTargetList();
	local	iTargetCount = kTargetList:size();
	
	ODS("Act_Mon_EnergyExplosion_Fire iTargetCount:"..iTargetCount.."\n");
	
	
	if iTargetCount>0 then
		local	kTargetInfo = kTargetList:GetTargetInfo(0);
		if kTargetInfo:IsNil() == false then
		
			local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then
			
				local	kTargetActor = kTargetPilot:GetActor();
				if kTargetActor:IsNil() == false then
				
					ODS("Act_Mon_EnergyExplosion_Fire LookAtBidirection Attacker:"..actor:GetID().." Target:"..kTargetActor:GetID().."\n");
					--actor:LookAtBidirection(kTargetActor:GetPos());
					actor:LookAt(kTargetActor:GetPos(),true,true);
				end
			end
		end
	end
	
end

function Act_Mon_EnergyExplosion_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end	
	
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then

			if iState == 2 then
				if actor:GetAnimationLength(action:GetSlotAnimName(4)) ~= 0 then
					Act_Mon_EnergyExplosion_SetState(actor,action,"RETURN")
					return true
				end
			end

			return false
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_EnergyExplosion_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > 2.0 then -- action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_EnergyExplosion_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mon_EnergyExplosion_OnCleanUp(actor, action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	actor:DetachFrom(81190);
end

function Act_Mon_EnergyExplosion_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Act_Mon_EnergyExplosion_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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

function Act_Mon_EnergyExplosion_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local   action = actor:GetAction();
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
		
    ODS("Act_Mon_EnergyExplosion_OnEvent actor:"..actor:GetID().." textKey:"..textKey.."\n");
       
    if	textKey == "attk_ground" then
		
		local Effect = actor:GetAnimationInfo("GROUND_EFFECT")
		if nil==Effect or ""==Effect then
			Effect = "ef_deathmaster_skill_02_03_char_root"
		end
		
		local	kActorPos = actor:GetPos()
		local	fSkillRange = action:GetSkillRange(0,actor)
		if fSkillRange == 0 then
			fSkillRange = 100
		end

		local	kLookDir = actor:GetLookingDir()
		kLookDir:Multiply(fSkillRange);
		
		local	kTargetPos = action:GetParamAsPoint(0);
		if "NONE"~=Effect then
			actor:AttachParticleToPointS(4381,kTargetPos,Effect, fSkillRange*0.0043);
		end
    
	    Act_Mon_Melee_DoDamage(actor,action);    
		action:SetParam(2,"HIT")
		
	elseif	textKey == "hit" then
	
		Act_Mon_Melee_DoDamage(actor,action);
		action:SetParam(2,"HIT")

    elseif  textKey == "fire" then
    
		local FireEffect = actor:GetAnimationInfoFromAniName("FIRE_EFFECT",action:GetSlotAnimName(2))
		if nil==FireEffect or FireEffect:IsNil() then
			FireEffect = "ef_deathmaster_skill_02_02_p_ef_head"
		else
			FireEffect = FireEffect:GetStr()
		end

		if "NONE"~=FireEffect then
			actor:AttachParticle(1204,"p_ef_head",FireEffect);
		end

	    
	end
	
	return true;
end

function Act_Mon_EnergyExplosion_OnTargetListModified(actor,action,bIsBefore)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	if bIsBefore == false then
		if action:GetParam(2) == "HIT" then
        
           --  hit 키 이후라면
           --  바로 대미지 적용시켜준다.
			Act_Mon_Melee_DoDamage(actor,action);    

		end    
	end        
end
