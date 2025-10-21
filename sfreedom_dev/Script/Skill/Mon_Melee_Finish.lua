-- skill 
 
function Act_Mon_Melee_Finish_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C) 
end 
 
function Act_Mon_Melee_Finish_SetState(actor,action,kState)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	ODS("Act_Mon_Melee_Finish_SetState " .. kState .. "\n", false, 1509)
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
		 
			ODS("Act_Mon_Melee_Finish_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n"); 
		 
			action:SetSlot(6);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자. 
		end 
		iNewState = 1; 
		 
	elseif kState == "FIRE" then 
	 
		action:SetSlot(2) 
		iNewState = 2; 
	elseif kState == "RETURN2" then 
--		ODS("RETURN2 AniName : " ..action:GetSlotAnimName(3).."\n", false, 1509)
		if actor:GetAnimationLength(action:GetSlotAnimName(3)) ~= 0 then
			action:SetSlot(3)
			iNewState = 3
		else
			return false
		end
	elseif kState == "RETURN3" then 
		if actor:GetAnimationLength(action:GetSlotAnimName(4)) ~= 0 then
			action:SetSlot(4)
			iNewState = 4
		else
			return false
		end
	elseif kState == "RETURN4" then 
--		ODS("Act_Mon_Melee_Finish_SetState SlotName["..action:GetSlotAnimName(5).."]\n", false, 1509)
		if actor:GetAnimationLength(action:GetSlotAnimName(5)) ~= 0 then
			action:SetSlot(5)
			iNewState = 5
		else
			return false
		end
	elseif kState == "RETURN5" then
--		ODS("Act_Mon_Melee_Finish_SetState SlotName["..action:GetSlotAnimName(5).."]\n", false, 1509)
	end
	 
	actor:ResetAnimation(); 
	actor:PlayCurrentSlot(); 
	action:SetParamInt(0,iNewState) 
	
	return	true
end 
 
function Act_Mon_Melee_Finish_OnEnter(actor, action)
 
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	ODS("Act_Mon_Melee_Finish_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n"); 
	 
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if nil~=prevAction and prevAction:GetID() ~= "a_jump" then
			actor:Stop() 
		end 
	end
	 
	actor:ResetAnimation() 
	 
	if action:GetActionParam() == AP_FIRE then 
		Act_Mon_Melee_Finish_OnCastingCompleted(actor,action) 
	else 
	 
		if( Act_Mon_Melee_Finish_SetState(actor,action,"BATTLEIDLE_START") == false) then 
			Act_Mon_Melee_Finish_SetState(actor,action,"BATTLEIDLE_LOOP") 
		end	 
		 
	    ODS("Act_Mon_Melee_Finish_OnEnter Start Casting\n"); 
		 
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
function Act_Mon_Melee_Finish_OnCastingCompleted(actor,action)	 
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	Act_Mon_Melee_Finish_SetState(actor,action,"FIRE") 
	 
	Act_Mon_Melee_Finish_Fire(actor,action)
end 
 
function Act_Mon_Melee_Finish_Fire(actor,action) 
 
 	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	--GetActionResultCS(action, actor) 
 
	-- 타겟리스트의 첫번째 놈을 바라보도록 한다. 
	local	kTargetList = action:GetTargetList(); 
	local	iTargetCount = kTargetList:size(); 
	 
	ODS("Act_Mon_Melee_Finish_Fire iTargetCount:"..iTargetCount.."\n"); 
	 
	 
	if iTargetCount>0 then 
		local	kTargetInfo = kTargetList:GetTargetInfo(0); 
		if kTargetInfo:IsNil() == false then 
		 
			local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID()); 
			if kTargetPilot:IsNil() == false then 
			 
				local	kTargetActor = kTargetPilot:GetActor(); 
				if kTargetActor:IsNil() == false then 
				 
					ODS("Act_Mon_Melee_Finish_Fire LookAtBidirection Attacker:"..actor:GetID().." Target:"..kTargetActor:GetID().."\n"); 
					--actor:LookAtBidirection(kTargetActor:GetPos()); 
					actor:LookAt(kTargetActor:GetPos(),true,true); 
				end 
			end 
		end 
	end 
	Skill_ChainLightning_Fire(actor,action)
	actor:SetParam("lightning", "false")
end 
 
function Act_Mon_Melee_Finish_OnUpdate(actor, accumTime, frameTime) 
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction() 
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0) 
--	ODS("Act_Mon_Melee_Finish_OnUpdate iState : " .. iState .." AParma : " .. action:GetActionParam() .. "\n", false, 1509)

	if action:GetActionParam() == AP_FIRE then 
	 
		if actor:IsAnimationDone() == true then
			if iState>=2 then	--fire이면
				if iState>=5 then return false end
				local bRet = Act_Mon_Melee_Finish_SetState(actor,action,"RETURN"..iState)
				if bRet==false then
--					ODS("Act_Mon_Melee_Finish_SetState false. iState : " .. iState .. "\n", false, 1509)
				else
					local 	bEnd = actor:GetAnimationInfo("IS_LIGHTING_END")
					if nil~=bEnd and "TRUE"==bEnd then
						Act_Mon_Melee_Finish_ReleasePlane(actor)
					end
				end
				return bRet
			end
		end
		if iState<=2 and "true"==actor:GetParam("lightning") then
			local   fLastUpdateTime = action:GetParamFloat(3);
			
			if IsShowTargetLightning(actor, action) then
				if accumTime - fLastUpdateTime > 0.5 then
					local path = actor:GetAnimationInfo("LIGHTNING_PATH")
					Skill_ChainLightning_CreateLinkedPlane(actor,action,path)
					action:SetParamFloat(3,accumTime);
				end
				local node = actor:GetAnimationInfo("LIGHTNING_START_NODE")
				Skill_ChainLightning_UpdatePlanePos(actor, action, node)
			end
		end
		 
	elseif action:GetActionParam() == AP_CASTING then
		 
		if iState == 0 then 
			if actor:IsAnimationDone() then 
				Act_Mon_Melee_Finish_SetState(actor,action,"BATTLEIDLE_LOOP") 
			end 
		end 
	 
		if IsSingleMode() then 
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then 
			 
				action:SetActionParam(AP_FIRE) 
				Act_Mon_Melee_Finish_OnCastingCompleted(actor,action) 
			 
			end 
		end 
	 
	end 
	return true 
end

function Act_Mon_Melee_Finish_ReleasePlane(actor)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local   iGroupNum = actor:GetAction():GetParamInt(2);
	if iGroupNum ~= -1 then
		local   kMan = GetLinkedPlaneGroupMan();
		kMan:ReleaseLinkedPlaneGroup(iGroupNum);
		actor:GetAction():SetParamInt(2,-1);
		actor:GetAction():ClearTargetList();
	end
end

function Act_Mon_Melee_Finish_OnCleanUp(actor, action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	Act_Mon_Melee_Finish_ReleasePlane(actor)
end 
 
function Act_Mon_Melee_Finish_OnLeave(actor, action) 

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end	
 
	local	kCurAction = actor:GetAction(); 
	
	if( CheckNil(nil==kCurAction) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	
	local	actionID = action:GetID() 
	ODS("Act_Mon_Melee_Finish_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n"); 
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
function Act_Mon_Melee_Finish_DoDamage(actor,action) 

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    local	kTargetList = action:GetTargetList(); 
    local	iTargetCount = kTargetList:size()
	--ODS("Act_Mon_Melee_Finish_DoDamage iTargetCount : " .. iTargetCount .. "\n", false, 1509) 
	 
    local iCount = 0 
    while iCount<iTargetCount do 
	    local	kTargetInfo = kTargetList:GetTargetInfo(iCount); 
	    if kTargetInfo:IsNil() == false then 
		 
		    local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID()); 
		    if kTargetPilot:IsNil() == false then 
			 
			    local	kTargetActor = kTargetPilot:GetActor(); 
			    if kTargetActor:IsNil() == false then 
			     
                    local targetEffectNode = actor:GetAnimationInfo("TARGET_EFFECT_NODE")
                    local targetEffect = actor:GetAnimationInfo("TARGET_EFFECT")
                	
                    if targetEffectNode ~= nil and targetEffect ~= nil then
                    
--						ODS("TARGET_EFFECT:"..targetEffect.." TARGET_EFFECT_NODE : "..targetEffectNode.."\n", false, 1509);

						kTargetActor:AttachParticle(124, targetEffectNode, targetEffect) 
                     
                    end 
				 
			    end 
		    end 
	    end 
	     
	    iCount=iCount+1; 
    end 

	if action:GetTargetList():IsNil() == false then
		action:GetTargetList():ApplyActionEffects();
	end
	if "true"~=actor:GetParam("lightning") then
		action:ClearTargetList();	     
	end
end 
 
function Act_Mon_Melee_Finish_OnEvent(actor,textKey) 
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local FireEffect_TextKey = actor:GetAnimationInfo("FIRE_TEXTKEY")
	if nil~=FireEffect_TextKey and FireEffect_TextKey~="" then
		if textKey == FireEffect_TextKey then
			local Fire_TextKey_Effect = actor:GetAnimationInfo("FIRE_TEXTKEY_EFFECT")
			local Fire_TextKey_Node = actor:GetAnimationInfo("FIRE_TEXTKEY_NODE")
			local Fire_TextKey_Scale = actor:GetAnimationInfo("FIRE_TEXTKEY_SCALE")
			actor:AttachParticleWithRotate(3851,Fire_TextKey_Node, Fire_TextKey_Effect, Fire_TextKey_Scale)
		end
	end
	
	if textKey == "hit" or textKey == "fire" then 
--       ODS("Act_Mon_Melee_Finish_OnEvent actor:"..actor:GetID().." textKey:"..textKey.."\n", false, 1509)
        local   action = actor:GetAction();
		if( CheckNil(nil==action) ) then return false end
		if( CheckNil(action:IsNil()) ) then return false end

		local path = actor:GetAnimationInfo("LIGHTNING_PATH")
		if nil~=path and ""~=path then
			if IsShowTargetLightning(actor, action) then
				Skill_ChainLightning_CreateLinkedPlane(actor,action, path)
			end
			actor:SetParam("lightning", "true")
		end

		local kDoDamage = actor:GetAnimationInfo("DO_DAMAGE")
		if nil ~= kDoDamage and "TRUE" == kDoDamage then
			Act_Mon_Melee_Finish_DoDamage(actor,action); 
		end
		
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
		local EffectFront = actor:GetAnimationInfo("FIRE_EFFECT_FRONT")
		if nil==EffectScale or ""==EffectScale then
			EffectScale = 1
		else
			EffectScale = tonumber(EffectScale)
		end

		if nil~=EffectNode and nil~=Effect then
			actor:AttachParticleS(125, EffectNode, Effect, EffectScale)
		elseif nil~=EffectFront and nil~=Effect then
			EffectFront = tonumber(EffectFront)
			local kLook = actor:GetLookingDir()
			kLook:Unitize()
			kLook:Multiply(EffectFront)
			local kPos = actor:GetTranslate()
			kPos:Add(kLook)
			actor:AttachParticleToPointS(125, kPos, Effect, EffectScale)
		end

	end
	 
	return true; 
end

function Act_Mon_Melee_Finish_OnTargetListModified(actor,action,bIsBefore)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
    local	kTargetList = action:GetTargetList()
    local	iTargetCount = kTargetList:size()
	Act_Mon_Melee_Finish_DoDamage(actor,action)
--	ODS("Act_Mon_Melee_Finish_OnTargetListModified iTargetCount : " .. iTargetCount .. "\n", false, 1509) 
end

function IsShowTargetLightning(actor, action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	bShow = actor:GetAnimationInfo("IS_SHOW_TARGET_LIGHTNING")
	if nil==bShow or ""==bShow or "TRUE"==bShow then
		bShow = true
	else
		bShow = action:GetTargetList():size() > 0
	end
	return bShow
end
