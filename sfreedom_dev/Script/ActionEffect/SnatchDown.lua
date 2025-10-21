------------------------------------------------
-- Definitions
------------------------------------------------
-- Param Definition
-- 0[int] : Action State (See Action State Definition)
-- 1[float] : total move time
-- 2[float] : move start time
-- 3[float] : total move distance

-- Point Param Definition
-- 0 : Move Start Position
-- 1 : Move Target Position
-- 2 : Moving Direction

-- Action State Definition
ACTIONSTATE_MOVING_TO_THE_TARGET_POSITION=0
ACTIONSTATE_MOVING_COMPLETED=1

------------------------------------------------
-- Event Handling Functions
------------------------------------------------
function Act_SnatchDown_OnEnter(actor, action)
	if actor:IsRidingPet() then --라이딩펫: 탑승 중 피격당하면 강제 하차→피격 액션 진행
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end


	Act_SnatchDown_InitActorState(actor,action);
	Act_SnatchDown_InitActionParams(actor,action);
	
	actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0);
	
	return true
end

function Act_SnatchDown_OnUpdate(actor, accumTime, frameTime,action)
	
	local	kAction = actor:GetAction();
	local	iActionState = Act_SnatchDown_GetActionState(kAction);
	
	if iActionState == ACTIONSTATE_MOVING_TO_THE_TARGET_POSITION then

		Act_SnatchDown_MoveToTheTarget(actor,kAction,accumTime);
		
	elseif iActionState == ACTIONSTATE_MOVING_COMPLETED then
	
		return	false;
		
	end

	return true
end

function Act_SnatchDown_OnCleanUp(actor, action)

	Act_SnatchDown_RestoreActorState(actor,action);
	
end


function Act_SnatchDown_OnLeave(actor, action)

	return	true;
end


------------------------------------------------
-- Implementation Functions
------------------------------------------------

function Act_SnatchDown_CalcMoveTargetPos(actor,action)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	kEffectCasterGuid = action:GetEffectCasterGuid();
	local	kEffectCasterActor = g_world:FindActor(kEffectCasterGuid);
	local	kMoveTargetPos;
	if kEffectCasterActor:IsNil() then
	
		kMoveTargetPos = g_world:ThrowRay(actor:GetPos(),Point3(0,0,-1),10000);
		if kMoveTargetPos:IsEqual(Point3(-1,-1,-1)) then
			kMoveTargetPos = actor:GetPos();
		end
	
	else
	
		local	kCasterToMeDir = actor:GetPos():_Subtract(kEffectCasterActor:GetPos());
		kCasterToMeDir:SetZ(0);
		kCasterToMeDir:Unitize();
		
		local	kRayStartPos = kEffectCasterActor:GetPos():_Add(kCasterToMeDir:_Multiply(40));
		kMoveTargetPos = g_world:ThrowRay(kRayStartPos,Point3(0,0,-1),10000);
		if kMoveTargetPos:IsEqual(Point3(-1,-1,-1)) then
			kMoveTargetPos = kEffectCasterActor:GetPos()
		end
	
	end
	
	kMoveTargetPos = g_world:FindActorFloorPos(kMoveTargetPos);
	
	return	kMoveTargetPos;

end
function Act_SnatchDown_InitActorState(actor,action)
	actor:FreeMove(true);	
	actor:StopJump();
	actor:StartGodTime(0);
	actor:SetMovingDelta(Point3(0,0,0));
	actor:SetNoWalkingTarget(false);
	actor:Concil(false)
	actor:SetCanHit(false);		
	actor:SetBlowUp(true);		

end
function Act_SnatchDown_RestoreActorState(actor,action)
	actor:SetCanHit(true);
	actor:EndBodyTrail();
	actor:Concil(true)
	actor:FreeMove(false);
	actor:SetBlowUp(false);

end
function Act_SnatchDown_CalcTotalMoveTime(actor,action)
	local	kEffectDef = GetEffectDef(action:GetActionNo());
	local	fTotalMoveTime = 0.1;
	if kEffectDef:IsNil() == false then
		fTotalMoveTime = kEffectDef:GetDurationTime()/1000.0;
		
		if fTotalMoveTime == 0 or fTotalMoveTime>0.1 then
			fTotalMoveTime = 0.1
		end
	end

	return	fTotalMoveTime;
end

function Act_SnatchDown_InitActionParams(actor,action)
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	kMoveStartPos = actor:GetPos();
	local	kMoveTargetPos = Act_SnatchDown_CalcMoveTargetPos(actor,action);
	local	kMoveDir = kMoveTargetPos:_Subtract(kMoveStartPos);
	kMoveDir:Unitize();
	local	fTotalMoveDistance = kMoveTargetPos:Distance(kMoveStartPos);
	
	Act_SnatchDown_SetActionState(action,ACTIONSTATE_MOVING_TO_THE_TARGET_POSITION);

	Act_SnatchDown_SetTotalMoveTime(action,Act_SnatchDown_CalcTotalMoveTime(actor,action));
	Act_SnatchDown_SetMoveStartTime(action,g_world:GetAccumTime());
	Act_SnatchDown_SetTotalMoveDistance(action,fTotalMoveDistance);
	
	Act_SnatchDown_SetMoveStartPosition(action,kMoveStartPos);
	Act_SnatchDown_SetMoveTargetPosition(action,kMoveTargetPos);
	Act_SnatchDown_SetMovingDirection(action,kMoveDir);

end
function Act_SnatchDown_GetCurrentTimeRatio(actor,action,accumTime)
	local	fElapsedTime = accumTime - Act_SnatchDown_GetMoveStartTime(action);
	local	fTotalTime = Act_SnatchDown_GetTotalMoveTime(action);
	
	local	fTimeRatio = fElapsedTime / fTotalTime;
	
	if fTimeRatio > 1 then
		fTimeRatio = 1;
	end

	return	fTimeRatio;
end
function Act_SnatchDown_GetPositionByTimeRatio(actor,action,fTimeRatio)

	local	kMoveStartPos = Act_SnatchDown_GetMoveStartPosition(action);
	local	kMoveDir = Act_SnatchDown_GetMovingDirection(action);
	local	fTotalMoveDistance = Act_SnatchDown_GetTotalMoveDistance(action);
	local	fCurrentMoveDistance = fTotalMoveDistance * fTimeRatio;

	kChangedPosition = kMoveStartPos:_Add(kMoveDir:_Multiply(fCurrentMoveDistance));
	
	return	kChangedPosition;

end
function Act_SnatchDown_MoveToTheTargetFinished(actor,action)
		
	actor:SetSendBlowStatus(false,false,true);
	Act_SnatchDown_SetActionState(action,ACTIONSTATE_MOVING_COMPLETED);

	actor:AttachParticleToPoint(2451,actor:GetPos(),"ef_skill_death_snatch_01");
end

function Act_SnatchDown_MoveToTheTarget(actor,action,accumTime)

	local	fTimeRatio = Act_SnatchDown_GetCurrentTimeRatio(actor,action,accumTime);
	local	kNextPosition = Act_SnatchDown_GetPositionByTimeRatio(actor,action,fTimeRatio);
	
	actor:SetTranslate(kNextPosition);
	
	if fTimeRatio == 1 then
		
		Act_SnatchDown_MoveToTheTargetFinished(actor,action);
	
	end
end
------------------------------------------------
-- Parameter Set/Get Functions
------------------------------------------------

function Act_SnatchDown_SetActionState(action,iState)

	action:SetParamInt(0,iState);

end
function Act_SnatchDown_GetActionState(action)

	return	action:GetParamInt(0);

end
function Act_SnatchDown_SetTotalMoveTime(action,fTotalMoveTime)

	action:SetParamFloat(1,fTotalMoveTime);

end
function Act_SnatchDown_GetTotalMoveTime(action)

	return	action:GetParamFloat(1);

end
function Act_SnatchDown_SetMoveStartTime(action,fMoveStartTime)

	action:SetParamFloat(2,fMoveStartTime);

end
function Act_SnatchDown_GetMoveStartTime(action)

	return	action:GetParamFloat(2);

end
function Act_SnatchDown_SetTotalMoveDistance(action,fTotalMoveDistance)

	action:SetParamFloat(3,fTotalMoveDistance);

end
function Act_SnatchDown_GetTotalMoveDistance(action)

	return	action:GetParamFloat(3);

end
function Act_SnatchDown_SetMoveStartPosition(action,kMoveStartPosition)

	action:SetParamAsPoint(0,kMoveStartPosition);

end
function Act_SnatchDown_GetMoveStartPosition(action)

	return	action:GetParamAsPoint(0);

end
function Act_SnatchDown_SetMoveTargetPosition(action,kMoveTargetPosition)

	action:SetParamAsPoint(1,kMoveTargetPosition);

end
function Act_SnatchDown_GetMoveTargetPosition(action)

	return	action:GetParamAsPoint(1);

end
function Act_SnatchDown_SetMovingDirection(action,kMovingDirection)

	action:SetParamAsPoint(2,kMovingDirection);

end
function Act_SnatchDown_GetMovingDirection(action)

	return	action:GetParamAsPoint(2);

end