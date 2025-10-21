-- skill

function Act_MagicCannon_OnlyCast_OnCheckCanEnter(actor, action)
end

function Act_MagicCannon_OnlyCast_SetState(actor,action,kState)
	local	iNewState = -1
	
	--ODS(GetAccumTime() .. "   " .. "Act_MagicCannon_OnlyCast_SetState kState : " .. kState .."\n", false, 998)

	if kState == "BATTLEIDLE_START" then
		--ODS(GetAccumTime() .. "   " .. "action:GetSlotAnimName(0) : " ..action:GetSlotAnimName(0) .."\n", false, 998)
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			--ODS("애니시간이 없음 \n", false, 998)
			return	false	
		end
				
		action:SetSlot(0)
		iNewState = 0
	elseif kState == "BATTLEIDLE_LOOP" then
		actor:AttachParticle(9998, "char_root", "ef_Magiccannon_01_btlidle_04_char_root")
		actor:AttachParticle(9999, "p_ef_controler", "ef_Magiccannon_hilight_01_p_ef_controler")
		--ODS(GetAccumTime() .. "   " .. "action:GetSlotAnimName(1) : " ..action:GetSlotAnimName(1) .."\n", false, 998)
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
			action:SetSlot(4) --	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1
	elseif kState == "FIRE" then
		--actor:ReleaseAllParticles()
		actor:DetachFrom(9997)
		actor:DetachFrom(9998)
		actor:DetachFrom(9999)
		--ODS(GetAccumTime() .. "   " .. "action:GetSlotAnimName(2) : " ..action:GetSlotAnimName(2) .."\n", false, 998)
		if actor:GetAnimationLength(action:GetSlotAnimName(2)) == 0 then
			return false
		end
		action:SetSlot(2)
		iNewState = 2
	elseif kState == "RETURN" then
		--ODS(GetAccumTime() .. "   " .. "action:GetSlotAnimName(3) : " ..action:GetSlotAnimName(3) .."\n", false, 998)
		if actor:GetAnimationLength(action:GetSlotAnimName(3)) == 0 then
			return false
		end
		action:SetSlot(3)
		iNewState = 3
	end

	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	action:SetParamInt(0,iNewState)
	
	return	true
end

function Act_MagicCannon_OnlyCast_OnEnter(actor, action)

	--ODS(GetAccumTime() .. "   " .. "Act_MagicCannon_OnlyCast_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n", false, 998)
	
	local	prevAction = actor:GetAction()
	if prevAction:GetID() ~= "a_jump" then
		actor:Stop()
	end

	actor:ResetAnimation()

	if action:GetActionParam() == AP_FIRE then
		Act_MagicCannon_OnlyCast_OnCastingCompleted(actor,action)
	else
		if false == Act_MagicCannon_OnlyCast_SetState(actor, action, "BATTLEIDLE_START") then	--애니 시작
			Act_MagicCannon_OnlyCast_SetState(actor,action,"BATTLEIDLE_LOOP")
		end
	end

	return true
end
function Act_MagicCannon_OnlyCast_OnCastingCompleted(actor,action)
	Act_MagicCannon_OnlyCast_Fire(actor,action)
end

function Act_MagicCannon_OnlyCast_Fire(actor,action)
	Act_MagicCannon_OnlyCast_SetState(actor,action,"FIRE")
end

function Act_MagicCannon_OnlyCast_OnUpdate(actor, accumTime, frameTime)

	local	action = actor:GetAction()
	local	iState = action:GetParamInt(0)
	local	bDone = actor:IsAnimationDone()
	local	aParam = action:GetActionParam()

	--ODS( GetAccumTime() .. "  Act_MagicCannon_OnlyCast_OnUpdate\n", false, 998)
	actor:DetachFrom(9996)
	if aParam == AP_FIRE then
		if bDone then
			if iState == 2 then
				local bRet = Act_MagicCannon_OnlyCast_SetState(actor,action,"RETURN")	--fire이후 리턴 애니 하자
				if bRet==false then
					--ODS("bRet==false\n",false,998)
				end
				return bRet
			else
				--ODS("리턴애니까지 다 끝났다\n",false,998)
				return false	--리턴애니까지 다 끝났다
			end
		end
	elseif aParam == AP_CASTING then
		if iState == 0 then
			if bDone then
				Act_MagicCannon_OnlyCast_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	end

	return true
end
function Act_MagicCannon_OnlyCast_OnCleanUp(actor, action)
	--ODS(GetAccumTime() .. "   " .. "Act_MagicCannon_OnlyCast_OnCleanUp\n", false, 998)
end

function Act_MagicCannon_OnlyCast_OnLeave(actor, action)

	local	kCurAction = actor:GetAction()
	local	actionID = action:GetID()
	--ODS(GetAccumTime() .. "   " .. "Act_MagicCannon_OnlyCast_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n")
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

function Act_MagicCannon_OnlyCast_OnEvent(actor,textKey)
	--ODS(GetAccumTime() .. "   " .. "Act_MagicCannon_OnlyCast_OnEvent actor:"..actor:GetID().." textKey:"..textKey.."\n", false, 998)
	if textKey == "hit" or textKey == "fire" then
		local   action = actor:GetAction();
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
	elseif textKey == "load02" then
		actor:AttachParticle(9997, "p_ef_arrow", "ef_Magiccannon_01_cast_p_ef_arrow")
	end
	
	return true
end

function Act_MagicCannon_OnlyCast_OnTargetListModified(actor,action,bIsBefore)
end
