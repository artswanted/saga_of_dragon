------------------------------------------------
-- Definitions
------------------------------------------------
-- Param Definition
-- 0[int] : Action State (See Action State Definition)
-- 1[float] : total freeze time

-- Action State Definition
ACTIONSTATE_FREEZED=0
ACTIONSTATE_FINISHED=1

------------------------------------------------
-- Event Handling Functions
------------------------------------------------
function Act_TimeFreezeEffect_OnEnter(actor, action)
	ODS("Act_TimeFreezeEffect_OnEnter\n", false, 1509)
	Act_TimeFreezeEffect_InitActorState(actor,action);
	Act_TimeFreezeEffect_InitActionParams(actor,action);
	Act_TimeFreezeEffect_AttachEffects(actor,action);
	
	return true
end

function Act_TimeFreezeEffect_OnUpdate(actor, accumTime, frameTime,action)
	
	local	kAction = actor:GetAction();
	local	iActionState = Act_TimeFreezeEffect_GetActionState(kAction);
	
	if iActionState == ACTIONSTATE_FREEZED then
		
		Act_TimeFreezeEffect_UpdateFreezeState(actor,kAction,accumTime);
		
	elseif iActionState == ACTIONSTATE_MOVING_COMPLETED then
	
		return	false;
		
	end

	return true
end

function Act_TimeFreezeEffect_OnCleanUp(actor, action)

	Act_TimeFreezeEffect_RestoreActorState(actor,action);
	Act_TimeFreezeEffect_DetachEffects(actor,action);
	
end


function Act_TimeFreezeEffect_OnLeave(actor, action)

	return	true;
end


------------------------------------------------
-- Implementation Functions
------------------------------------------------

function Act_TimeFreezeEffect_InitActorState(actor,action)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	actor:SetBlowUp(actor:IsMeetFloor() == false);	
	local kPos = actor:GetPos();
	local ptcl = g_world:ThrowRay(kPos, Point3(0,0,-1), 46,1) --아래 레이를 쏴서 (공중에 떠있는 몬스터들 때문에 추가됨)
	if ptcl:GetX() ~= -1 and ptcl:GetY() ~= -1 and ptcl:GetZ() ~= -1 then	-- 바닥이 있으면
		--ODS("짧게 떠서 공중에 붙어있는것으로 치지 않음\n", false, 912)
		actor:SetBlowUp(false);
	end
	
	actor:FreeMove(true);
	actor:StopJump();
	actor:SetMovingDelta(Point3(0,0,0));
	actor:SetNoWalkingTarget(false);
	actor:Concil(false)
end

function Act_TimeFreezeEffect_RestoreActorState(actor,action)
	actor:EndBodyTrail();
	actor:Concil(true)
	actor:FreeMove(false);
	actor:SetBlowUp(false);

end

function Act_TimeFreezeEffect_AttachEffects(actor,action)

	actor:AttachParticleWithRotate(876112,"p_ef_star","ef_skill_timefreeze_03_p_ef_star",1,true);

end
function Act_TimeFreezeEffect_DetachEffects(actor,action)

	actor:DetachFrom(876112);

end


function Act_TimeFreezeEffect_CalcTotalFreezeTime(actor,action)
	local	kEffectDef = GetEffectDef(action:GetActionNo());
	local	fTotalFreezeTime = 5;
	if kEffectDef:IsNil() == false then
		fTotalFreezeTime = kEffectDef:GetDurationTime()/1000.0;
		
		if fTotalFreezeTime == 0  then
			fTotalFreezeTime = 5
		end
	end
	
	if actor:IsMeetFloor() == false then
		fTotalFreezeTime = fTotalFreezeTime - 2
		if fTotalFreezeTime<1 then
			fTotalFreezeTime = 1
		end
	end

	return	fTotalFreezeTime;
end

function Act_TimeFreezeEffect_InitActionParams(actor,action)
	
	Act_TimeFreezeEffect_SetActionState(action,ACTIONSTATE_FREEZED);

	Act_TimeFreezeEffect_SetTotalFreezeTime(action,Act_TimeFreezeEffect_CalcTotalFreezeTime(actor,action));

end
function Act_TimeFreezeEffect_GetCurrentTimeRatio(actor,action,accumTime)
	local	fElapsedTime = accumTime - action:GetActionEnterTime();
	local	fTotalTime = Act_TimeFreezeEffect_GetTotalFreezeTime(action);
	
	local	fTimeRatio = fElapsedTime / fTotalTime;
	
	if fTimeRatio > 1 then
		fTimeRatio = 1;
	end

	return	fTimeRatio;
end
function Act_TimeFreezeEffect_FreezeFinished(actor,action)
		
	actor:SetSendBlowStatus(false,false,true);
	Act_TimeFreezeEffect_SetActionState(action,ACTIONSTATE_FINISHED);
	
end

function Act_TimeFreezeEffect_UpdateFreezeState(actor,action,accumTime)

	local	fTimeRatio = Act_TimeFreezeEffect_GetCurrentTimeRatio(actor,action,accumTime);
	if fTimeRatio == 1 then
		
		Act_TimeFreezeEffect_FreezeFinished(actor,action);
	
	end
end
------------------------------------------------
-- Parameter Set/Get Functions
------------------------------------------------

function Act_TimeFreezeEffect_SetActionState(action,iState)

	action:SetParamInt(0,iState);

end
function Act_TimeFreezeEffect_GetActionState(action)

	return	action:GetParamInt(0);

end
function Act_TimeFreezeEffect_SetTotalFreezeTime(action,fTotalFreezeTime)

	action:SetParamFloat(1,fTotalFreezeTime);

end
function Act_TimeFreezeEffect_GetTotalFreezeTime(action)

	return	action:GetParamFloat(1);

end
