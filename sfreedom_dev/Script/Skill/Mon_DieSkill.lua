-- skill

function Act_Mon_DieSkill_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mon_DieSkill_SetState(actor,action,kState)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	local bRemoveStatusEffectFromActor = false
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
		
			ODS("Act_Mon_DieSkill_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
		
			action:SetSlot(3);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		action:SetParamFloat(2, g_world:GetAccumTime())
		local kDef = GetSkillDef(action:GetActionNo())
		action:SetParamFloat(3,0)
		if false==kDef:IsNil() then
			local time = kDef:GetAbil(AT_CAST_TIME)*0.001
			ODS("스킬번호 : " .. action:GetActionNo() .. " 캐스팅시간 : " .. time .. "\n", false, 1509)
			action:SetParamFloat(3, time)
			action:SetParamFloat(10, time)	--기준
		end
		action:SetParamFloat(5,0.6)	-- 다음번 깜빡이는 시간 간격
		action:SetParamFloat(6,0)	-- 깜빡이는 시간 저장
		action:SetParamFloat(11,action:GetParamFloat(5))	-- 깜빡이는 시간 간격 기준
		
	elseif kState == "FIRE" then
		action:SetParamInt(4,0)
		if actor:GetAnimationLength(action:GetSlotAnimName(2)) == 0 then	-- 죽는 애니가 없으면
			action:SetParamInt(4,1)
		end
		action:SetSlot(2)
		iNewState = 2
		bRemoveStatusEffectFromActor = true
	end
	
	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	action:SetParamInt(0,iNewState)
	
	if(bRemoveStatusEffectFromActor) then
		-- (이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
		local EffMan = GetStatusEffectMan()
		local guid = actor:GetPilotGuid()
		EffMan:RemoveStatusEffectFromActor(guid, 200000010)	--띄었다가
	end
	return	true

end

function Act_Mon_DieSkill_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Mon_DieSkill_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_DieSkill_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_DieSkill_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_DieSkill_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
	    ODS("Act_Mon_DieSkill_OnEnter Start Casting\n");
		
	end
	
	return true
end
function Act_Mon_DieSkill_OnCastingCompleted(actor,action)

	Act_Mon_DieSkill_SetState(actor,action,"FIRE")

end

function Act_Mon_DieSkill_Hide(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	actor:HideNode("Scene Root",true);	--	모습을 감추자
	actor:ReleaseAllParticles();
	actor:SetHideShadow(true)

	local	die_particle_id = actor:GetDieParticleID();
	if nil~=die_particle_id and ""~=die_particle_id then
		local heartPos = actor:GetNodeTranslate("p_ef_heart")
		local DieParticleNode = actor:GetDieParticleNode()
		if nil~=DieParticleNode and 0<string.len(DieParticleNode) then
			heartPos = actor:GetNodeTranslate(DieParticleNode)
		end
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AttachParticleSWithRotate(die_particle_id, heartPos, actor:GetDieParticleScale(), Point3(0,0,0))
	end			
	
end

function Act_Mon_DieSkill_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if 0~=action:GetParamInt(4) then--죽는 애니가 없으면 바로 끝
			Act_Mon_DieSkill_DoDamage(actor, action)
			Act_Mon_DieSkill_Hide(actor, action)
			return false
		elseif actor:IsAnimationDone() == true then
			Act_Mon_DieSkill_Hide(actor, action)
			return false
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_DieSkill_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
		local EffMan = GetStatusEffectMan()
		local guid = actor:GetPilotGuid()
		local fAlarmDelta = accumTime - action:GetParamFloat(6)
		local fInter = action:GetParamFloat(5)
		--ODS("fAlarmDelta : " .. fAlarmDelta .. " fInter : " .. fInter .. "\n", false, 1509)
		if fAlarmDelta > fInter or action:GetParamFloat(6)==0 then	--시간간격보다 크면
			action:SetParamFloat(6, accumTime)			--시간 저장

			local iInstanceID = action:GetParamInt(7);
			EffMan:RemoveStatusEffectFromActor2(guid, iInstanceID) -- 기존에 붙어있는거 띄어버리고(아래쪽에 띄는 코드가 있지만, 만약을 위해 확실히 띄자)
			
			local kDieBlink = actor:GetAnimationInfo("DIEBLINK")
			if nil==kDieBlink or ""==kDieBlink then
				kDieBlink = "se_damage_blink_red"
			end

			iInstanceId = EffMan:AddStatusEffectToActor(guid, kDieBlink, 0, 0)	--다시 붙이기
			action:SetParamInt(7,iInstanceID)

			--다음번 깜빡임 준비
			local fDelta = (accumTime - action:GetParamFloat(2))
			local fRate = 1 - (fDelta*1000)/(action:GetParamFloat(10)*1000)	--너무 작은숫자는 곤란
			--ODS("Param10 : " .. action:GetParamFloat(10) .. " Param2 : " .. action:GetParamFloat(2) .. " fDelta : " .. fDelta .. " fRate : " .. fRate .. "\n", false, 1509)
			action:SetParamFloat(5, action:GetParamFloat(11)*fRate)
		else
			local	iInstanceID = action:GetParamInt(7);
			EffMan:RemoveStatusEffectFromActor2(guid, iInstanceID)	--띄었다가
		end

		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_DieSkill_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mon_DieSkill_OnCleanUp(actor, action)
end

function Act_Mon_DieSkill_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Act_Mon_DieSkill_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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
function Act_Mon_DieSkill_DoDamage(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    local	kTargetList = action:GetTargetList();
    local	iTargetCount = kTargetList:size();
	
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
                    
						ODS("TARGET_EFFECT:"..targetEffect.." TARGET_EFFECT_NODE : "..targetEffectNode.."\n", false, 1509);

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
	action:ClearTargetList();	    
end

function Act_Mon_DieSkill_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
       ODS("Act_Mon_DieSkill_OnEvent actor:"..actor:GetID().." textKey:"..textKey.."\n");
       
    if textKey == "hit" or textKey == "fire" then
        local   action = actor:GetAction();
		Act_Mon_DieSkill_DoDamage(actor,action);
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
	
	return true;
end

function Act_Mon_DieSkill_OnTargetListModified(actor,action,bIsBefore)
end
