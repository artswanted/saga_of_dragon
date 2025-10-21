-- skill

function Act_Mon_Melee_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mon_Melee_SetState(actor,action,kState)
	
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
		
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
		
			ODS("Act_Mon_Melee_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
		
			action:SetSlot(3);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1;
			
		local iClassNo = actor:GetAbil(AT_CLASS)
		local iSeqID = actor:GetAniSequenceID()
		--ODS("iClassNo = "..iClassNo.." / iSeqID = "..iSeqID.."\n", false, 3851)
		if 2004001 == iClassNo and 1000207 == iSeqID then
			actor:AttachParticleS(3851, "char_root", "efx_dunamic_rolling_attack_02_02_hold", 1.0)
		end
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
	
	local DetachEffect = actor:GetAnimationInfo("DETACH_RANGE_EFFECT")
	if nil~=DetachEffect and "TRUE"==DetachEffect then
		actor:DetachFrom(81190)	--Act_Mon_MassiveProjectile
	end
	return	true;

end

function Act_Mon_Melee_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--ODS("Act_Mon_Melee_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n")
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	if( "FALSE" == action:GetScriptParam("APPLY_ACTION_EFFECTS") ) then
		action:SetParamInt(20110615,1)
	end
	
	actor:ResetAnimation()
	action:SetParamInt(88, 0)
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Melee_OnCastingCompleted(actor,action)
	else	
		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
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
function Act_Mon_Melee_OnCastingCompleted(actor,action)	

	Act_Mon_Melee_SetState(actor,action,"FIRE")
	
	Act_Mon_Melee_Fire(actor,action);
	
end

function Act_Mon_Melee_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--GetActionResultCS(action, actor)

	-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
	local	kTargetList = action:GetTargetList();
	local	iTargetCount = kTargetList:size();
	
	ODS("Act_Mon_Melee_Fire iTargetCount:"..iTargetCount.."\n");
	
	
	if iTargetCount>0 then
		local	kTargetInfo = kTargetList:GetTargetInfo(0);
		if kTargetInfo:IsNil() == false then
		
			local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then
			
				local	kTargetActor = kTargetPilot:GetActor();
				if kTargetActor:IsNil() == false then
				
					ODS("Act_Mon_Melee_Fire LookAtBidirection Attacker:"..actor:GetID().." Target:"..kTargetActor:GetID().."\n");
					--actor:LookAtBidirection(kTargetActor:GetPos());
					actor:LookAt(kTargetActor:GetPos(),true,true);
					
					local iClassNo = actor:GetAbil(AT_CLASS)
					local iSeqID = actor:GetAniSequenceID()
					if 2006001 == iClassNo then
						PlaySoundByID( "sum-blasser" )
					end
				end
			end
		end
	end
	Skill_ChainLightning_Fire(actor,action)
	actor:SetParam("lightning", "false")
end

function Act_Mon_Melee_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then
			local nextAction = actor:GetAnimationInfo("NEXT_ACTION")
			if nil~=nextAction and "NONE"~=nextAction then
				action:SetNextActionName(nextAction)
			end
			return false
		end
		if "true"==actor:GetParam("lightning") then
			local   fLastUpdateTime = action:GetParamFloat(3);
			if accumTime - fLastUpdateTime > 0.5 then
				local path = actor:GetAnimationInfo("LIGHTNING_PATH")
				Skill_ChainLightning_CreateLinkedPlane(actor,action,path)
				action:SetParamFloat(3,accumTime);
			end
			local node = actor:GetAnimationInfo("LIGHTNING_START_NODE")
			Skill_ChainLightning_UpdatePlanePos(actor, action, node)	
		end
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if(0 == action:GetParamInt(20110510) ) then -- 캐스팅시에 애니를 한번더 재생해주지 않으면 멈춰있는 현상이 있음
				actor:ResetAnimation();	
				actor:PlayCurrentSlot();
				action:SetParamInt(20110510, 1)
			end
			
			if actor:IsAnimationDone() then
				Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
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
function Act_Mon_Melee_OnCleanUp(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	local   iGroupNum = actor:GetAction():GetParamInt(2);
    if iGroupNum ~= -1 then
        local   kMan = GetLinkedPlaneGroupMan();
        kMan:ReleaseLinkedPlaneGroup(iGroupNum);
        actor:GetAction():SetParamInt(2,-1);
		actor:GetAction():ClearTargetList();
    end
	
	actor:DetachFrom(122)
end

function Act_Mon_Melee_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
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

function Act_Mon_Cast_Effect(actor,textKey)
	if actor == nil or textKey == nil then
		return
	end
	
	if textKey == "start" then
		local action = actor:GetAction()
		if action:IsNil() then
			return
		end
		
		local iFirst = action:GetParamInt(88)		
		
		if iFirst == 0 then
			if action:GetActionParam() ~= AP_FIRE then
				local EffectNode = actor:GetAnimationInfo("CAST_EFFECT_NODE")
				if EffectNode == nil then
					EffectNode = "char_root"
				end
				local Effect = actor:GetAnimationInfo("CAST_EFFECT")
				local EffectScale = actor:GetAnimationInfo("CAST_EFFECT_SCALE")
				if nil==EffectScale or ""==EffectScale then
					EffectScale = 1
				else
					EffectScale = tonumber(EffectScale)
				end
				
				if nil~=Effect then
					actor:AttachParticleS(122, EffectNode, Effect, EffectScale)
				end
				
			end
		end
		action:SetParamInt(88, 1)
	elseif textKey == "elga_jump" then
		actor:DetachFrom(122)
	end
end

function Act_Mon_Melee_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
    --ODS("Act_Mon_Melee_OnEvent actor:"..actor:GetID().." textKey:"..textKey.."\n");
	
	Act_Mon_Cast_Effect(actor,textKey)
	
    if textKey == "hit" or textKey == "fire" then
        local   action = actor:GetAction();
		
		if( CheckNil(nil==action) ) then return false end
		if( CheckNil(action:IsNil()) ) then return false end
	
		local path = actor:GetAnimationInfo("LIGHTNING_PATH")
		if nil~=path and ""~=path then
			Skill_ChainLightning_CreateLinkedPlane(actor,action, path)
			actor:SetParam("lightning", "true")
		end
		Act_Mon_Melee_DoDamage(actor,action);
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

		local kSoundID = actor:GetAnimationInfo("HIT_SOUND_ID")
		if nil ~= kSoundID and "" ~= kSoundID then
			--ODS("actor:GetAnimationInfo(HIT_SOUND_ID)\n")
			actor:AttachSound(2785,kSoundID);
		end

		local EffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
		local Effect = actor:GetAnimationInfo("FIRE_EFFECT")
		local EffectFront = actor:GetAnimationInfo("FIRE_EFFECT_FRONT")
		local EffectScale = nil
		local EffectAutoScale = actor:GetAnimationInfo("FIRE_EFFECT_AUTO_SCALE")
		if nil~=EffectAutoScale then
			EffectAutoScale = string.upper(EffectAutoScale)
			if "TRUE"==EffectAutoScale then
				EffectScale = GetEffectAutoScale(actor)
			end
		else
			EffectScale = actor:GetAnimationInfo("FIRE_EFFECT_SCALE")
		end

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
			
			local const_rotate = Quaternion(-90.0*math.pi/180.0, Point3(0,0,1))
			local Quat = actor:GetRotateQuaternion()
			Quat = Quat:Multiply(const_rotate)
			actor:AttachParticleToPointWithRotate(125, kPos, Effect, Quat, EffectScale)
		end
	elseif textKey == "start" then
		local   action = actor:GetAction();
		
		if( CheckNil(nil==action) ) then return false end
		if( CheckNil(action:IsNil()) ) then return false end
		
		if true==action:IsSlotEventScript() then
			local EventScriptNo = actor:GetAnimationInfo("EVENTSCRIPT_NO")
			local EventScriptViewDistance = actor:GetAnimationInfo("EVENTSCRIPT_VIEW_DISTANCE")
			if nil==EventScriptViewDistance or ""==EventScriptViewDistance then
				EventScriptViewDistance = 300
			else
				EventScriptViewDistance = tonumber(EventScriptViewDistance)
			end

			local kSubPos = actor:GetPos():_Subtract(GetMyActor():GetPos())
			--ODS("kSubPos x: "..kSubPos:GetX()..", y:"..kSubPos:GetY()..", z:"..kSubPos:GetZ().."\n")

			if nil == EventScriptNo or "" == EventScriptNo then
				EventScriptNo = 0
			end
			
			if EventScriptViewDistance > math.abs(kSubPos:GetX()) then
				if EventScriptNo > 0 then
					GetEventScriptSystem():ActivateEvent(EventScriptNo)
				else
					CreateFuryCamera(actor, tonumber(actor:GetAnimationInfo("FURY_CAMERA_DISTANCE")))
				end
			end
		end
	end
	
	return true;
end

function Act_Mon_Melee_OnTargetListModified(actor,action,bIsBefore)
	if bIsBefore == false then
		--  hit 키 이후라면
		--  바로 대미지 적용시켜준다.
		SkillHelpFunc_DefaultHitOneTime(actor,action);
   end
end
