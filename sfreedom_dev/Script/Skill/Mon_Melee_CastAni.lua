-- skill
-- Add되자마자 바로 화면에 보이는 몬스터에만 사용할 것. 필드에서 쓰면 캐스팅 타이밍이 맞지 않아 애니가 다르게 보임.
--
function Act_Mon_Melee_CastAni_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mon_Melee_CastAni_SetState(actor,action,kState)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iNewState = -1;

	local fLenth = actor:GetAnimationLength(action:GetSlotAnimName(kState))

	--ODS("Act_Mon_Melee_CastAni_SetState "..kState.." "..action:GetSlotAnimName(kState).."\n", false, 1509)

	action:SetSlot(kState)

	if 0==kState%2 then	--idle
		if fLenth == 0 then
			action:SetSlot(8);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
	else				--action
		if fLenth == 0 then
			action:SetSlot(9)
		end
	end

	Act_Mon_Melee_CastAni_AddEffect(actor,action,kState)

	iNewState = kState

	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,iNewState)
	
	return	true;

end

function Act_Mon_Melee_CastAni_AddEffect(actor,action,kState)
	local kAniName = action:GetSlotAnimName(kState)

	for i=1,3 do	--3개 까지만 지원하자
		local lwName = actor:GetAnimationInfoFromAniName("CAST_EFFECT"..i, kAniName)
		local iSlot = math.random(9999, 99999)
		local fScale = 1.0
		local szNode = "char_root"
		if false==lwName:IsNil() then
			local lwSlot = actor:GetAnimationInfoFromAniName("SLOT"..i, kAniName)
			if false==lwSlot:IsNil() then
				iSlot = tonumber(lwSlot:GetStr())
			end
			local lwScale = actor:GetAnimationInfoFromAniName("SCALE"..i, kAniName)
			if false==lwScale:IsNil() then
				fScale = tonumber(lwScale:GetStr())
			end
			local lwNode = actor:GetAnimationInfoFromAniName("EFFECT_NODE"..i, kAniName)
			if false==lwNode:IsNil() then
				szNode = lwNode:GetStr()
			end
			--ODS("AddEffect Node:"..szNode.." Name:"..lwName:GetStr().." Slot:"..iSlot.."\n", false, 1509)
			actor:AttachParticleS(iSlot, szNode, lwName:GetStr(), fScale)
			action:SetParamInt(10+action:GetParamInt(9), iSlot)	--슬롯저장. 나중에 지워야 하니까
			action:SetParamInt(9, action:GetParamInt(9)+1)
		else
			break
		end
	end
end

function Act_Mon_Melee_CastAni_OnEnter(actor, action)
	--ODS("Act_Mon_Melee_CastAni_OnEnter\n", false, 1509)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	local iCastTime = action:GetAbil(AT_CAST_TIME)

	action:SetParamFloat(1, iCastTime*0.001)

	local iNum = actor:GetAnimationInfoFromAniName("CAST_ANI_NUM", "castani_01"):GetStr()
	if nil==iNum then
		iNum=1
	else
		iNum = tonumber(iNum)
	end

	local fInter = (iCastTime*0.001)/iNum


	action:SetParamFloat(2, fInter)		--기준
	action:SetParamFloat(3, fInter)		--여기서 깍아먹자
	action:SetParamInt(9, 0)		--파티클이 몇개나 붙었나?

	actor:ResetAnimation()

	Act_Mon_Melee_CastAni_SetState(actor,action,0)

	return true
end
function Act_Mon_Melee_CastAni_OnCastingCompleted(actor,action)
	Act_Mon_Melee_CastAni_SetState(actor,action,8)
	Act_Mon_Melee_Fire(actor,action)
end

function Act_Mon_Melee_CastAni_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then
			return false
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		local fInter = action:GetParamFloat(3)
		fInter = fInter - frameTime

		if iState%2 == 0 then
			if actor:IsAnimationDone() then
				iState = iState+1
				Act_Mon_Melee_CastAni_SetState(actor,action,iState)
			end
		else
			if fInter <= 0 then
				fInter = fInter + action:GetParamFloat(2)
				iState = iState+1
				Act_Mon_Melee_CastAni_SetState(actor,action,iState)
			end
		end

		action:SetParamFloat(3, fInter)

		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Melee_CastAni_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mon_Melee_CastAni_OnCleanUp(actor, action)
end

function Act_Mon_Melee_CastAni_OnLeave(actor, action)
	--ODS("Act_Mon_Melee_CastAni_OnLeave\n", false, 1509)
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end

	local kCurAction = actor:GetAction()
	if( CheckNil(nil==kCurAction) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end

	local iNo = kCurAction:GetParamInt(9)
	if 0<iNo then
		for i=0,iNo-1 do
			--ODS("Detach : "..kCurAction:GetParamInt(10+i).."\n", false, 1509)
			actor:DetachFrom(kCurAction:GetParamInt(10+i))
		end
	end

	return true
end
function Act_Mon_Melee_CastAni_DoDamage(actor,action)
	Act_Mon_Melee_DoDamage(actor,action)
end

function Act_Mon_Melee_CastAni_OnEvent(actor,textKey)
	return Act_Mon_Melee_OnEvent(actor,textKey)
end

function Act_Mon_Melee_CastAni_OnTargetListModified(actor,action,bIsBefore)
end
