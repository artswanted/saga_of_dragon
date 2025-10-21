-- skill

function Act_Mon_Melee_Loop_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mon_Melee_Loop_SetState(actor,action,kState)
	
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
		
			action:SetSlot(5);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1;
		
	elseif kState == "FIRE" then
	
		action:SetSlot(2)
		iNewState = 2;
	elseif kState == "LOOP" then
		
		action:SetSlot(3)
		iNewState = 3;
	elseif kState == "RETURN" then
		
		action:SetSlot(4)
		iNewState = 4;

		local	bNotDetachEffect = actor:GetAnimationInfo("IS_NOT_DETACH_FIRE_EFFECT")
		if nil==bNotDetachEffect or "TRUE"~=bNotDetachEffect then
			actor:DetachFrom(129)
		end
	end
	
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,iNewState)

	local	bDetachCastEffect = actor:GetAnimationInfo("USE_DETACH_CAST_EFFECT")
	if nil~=bDetachCastEffect and "TRUE"==bDetachCastEffect then
		actor:DetachFrom(122)
	end
	
	return	true;

end

function Act_Mon_Melee_Loop_OnEnter(actor, action)
	-- if actor:IsEqualObjectName("m_elga02") then
		-- CallElga02_DownGroggy()
	-- end
	action:SetParamInt(55,0)	--이펙트 사용유무
	--이벤트 스크립트인경우, 스킬의 CastTime얻어와서, 스타트 애니를 대신하도록 함
	local iEventNo = actor:GetAnimationInfoFromAniName("EVENT_SCRIPT",action:GetCurrentSlotAnimName())
	if nil~=iEventNo and ""~=iEventNo:GetStr() then
		iEventNo = tonumber(iEventNo:GetStr())
		GetEventScriptSystem():ActivateEvent(iEventNo) --실행
	end

	return Act_Mon_Melee_OnEnter(actor, action)
end
function Act_Mon_Melee_Loop_OnCastingCompleted(actor,action)	

	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(nil==g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local iDelay = action:GetAbil(AT_MAINTENANCE_TIME2) + action:GetAbil(AT_MAINTENANCE_TIME)	--어빌 두개를 사용한다
	action:SetParamFloat(5,g_world:GetAccumTime() + iDelay*0.001)	--공격시작시간 저장
	Act_Mon_Melee_SetState(actor,action,"FIRE")
	
	Act_Mon_Melee_Fire(actor,action);
	
end

function Act_Mon_Melee_Loop_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if 0 == action:GetParamInt(55) then	--이펙트 사용유무
		local	effect = actor:GetAnimationInfo("FIRE_EFFECT")
		if nil ~= effect and "" ~= effect then
			local	node = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
			if nil==node or ""==node then
				node = "char_root"
			end
			local	scale = actor:GetAnimationInfo("FIRE_EFFECT_SCALE")
			if nil==scale or ""==scale then
				scale = 1
			else
				scale = tonumber(scale)
			end

			if "NONE"~=effect then
				actor:AttachParticleS(129,node,effect,scale)
				action:SetParamInt(55,1)
			end
		end
	end

	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then
	--루프로 넘어가 주자
			if 4==iState then
				return false;	--피니쉬 동작까지 끝났음
			end
			Act_Mon_Melee_Loop_SetState(actor,action,"LOOP")	
		end

		if action:GetParamFloat(5)<accumTime then
			if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then	--피니쉬 동작이 없으면
				return false
			elseif 4~=iState then
				Act_Mon_Melee_Loop_SetState(actor,action,"RETURN")	
			end
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
function Act_Mon_Melee_Loop_OnCleanUp(actor, action)
	Act_Mon_Melee_OnCleanUp(actor, action)
	actor:DetachFrom(129)
end

function Act_Mon_Melee_Loop_OnLeave(actor, action)
	return Act_Mon_Melee_OnLeave(actor, action)
end
function Act_Mon_Melee_Loop_DoDamage(actor,action)
	Act_Mon_Melee_DoDamage(actor,action)
end

function Act_Mon_Melee_Loop_OnEvent(actor,textKey)
	Act_Mon_Melee_OnEvent(actor,textKey)
	return true;
end

function Act_Mon_Melee_Loop_OnTargetListModified(actor,action,bIsBefore)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if bIsBefore == false then
        --  때린다.
		Actor_Mob_Base_DoDefaultMeleeAttack(actor,action);   --  타격 효과
		action:GetTargetList():ApplyActionEffects();    
	end
end
