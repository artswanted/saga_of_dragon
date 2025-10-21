-- skill

function Skill_Mon_Blocking_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Skill_Mon_Blocking_SetState(actor,action,kState)

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
		
			ODS("Skill_Mon_Blocking_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
		
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

function Skill_Mon_Blocking_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Skill_Mon_Blocking_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Skill_Mon_Blocking_OnCastingCompleted(actor,action)
	else
	
		if( Skill_Mon_Blocking_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Skill_Mon_Blocking_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
	    ODS("Skill_Mon_Blocking_OnEnter Start Casting\n");
		
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
function Skill_Mon_Blocking_OnCastingCompleted(actor,action)	

	Skill_Mon_Blocking_SetState(actor,action,"FIRE")
	
	Skill_Mon_Blocking_Fire(actor,action);
	
end

function Skill_Mon_Blocking_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--GetActionResultCS(action, actor)

	-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
	local	kTargetList = action:GetTargetList();
	local	iTargetCount = kTargetList:size();
	
	ODS("Skill_Mon_Blocking_Fire iTargetCount:"..iTargetCount.."\n");
	
	local fireStartNode	= actor:GetAnimationInfo("FIRE_EFFECT_NODE")	
	if fireStartNode == nil then
		fireStartNode = "char_root"
	end

	local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")
	if fireEffect == nil or fireEffect=="" then
		fireEffect = "ef_deathmaster_skill_03_02_char_root"
		local lwFireEffect = actor:GetAnimationInfoFromAniName("FIRE_EFFECT", "blocking_01"):GetStr()
		if "NONE"==lwFireEffect then
			fireEffect = "NONE"
		end
	end

	if fireEffect ~= "NONE" then
		actor:AttachParticle(89110,fireStartNode,fireEffect);
	end
	
	if iTargetCount>0 then
		local	kTargetInfo = kTargetList:GetTargetInfo(0);
		if kTargetInfo:IsNil() == false then
		
			local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then
			
				local	kTargetActor = kTargetPilot:GetActor();
				if kTargetActor:IsNil() == false then
				
					ODS("Skill_Mon_Blocking_Fire LookAtBidirection Attacker:"..actor:GetID().." Target:"..kTargetActor:GetID().."\n");
					--actor:LookAtBidirection(kTargetActor:GetPos());
					actor:LookAt(kTargetActor:GetPos(),true,true);
				end
			end
		end
	end
	
end

function Skill_Mon_Blocking_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
		
	if action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Skill_Mon_Blocking_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Skill_Mon_Blocking_OnCastingCompleted(actor,action)
			
			end
		end
		
	elseif iState == 2 then
	
		if actor:IsAnimationDone() then
			actor:PlayNext();
			action:SetParamInt(0,3);
		end	
	
	end
	return true
end
function Skill_Mon_Blocking_OnCleanUp(actor, action)
	actor:DetachFrom(89110)end

function Skill_Mon_Blocking_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Skill_Mon_Blocking_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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

function Skill_Mon_Blocking_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	if actor:GetAction():GetActionParam() == AP_FIRE then
		if textKey == "end" then
			local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
			if nil==fireEffect or "NONE"~=fireEffect then
				actor:AttachParticle(89110,"char_root","ef_deathmaster_skill_03_01_char_root");	
			end
			
		end
	end

	return true;
end
