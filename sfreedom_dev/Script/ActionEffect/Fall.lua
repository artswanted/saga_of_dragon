------------------------------------------------
-- Definitions
------------------------------------------------
-- Param Definition
-- 0[int] : Action State (See Action State Definition)
-- 1[float] : Falling Initial Speed
-- 2[float] : Falling Accelation
-- 3[float] : Stay Down Time
-- 4[float] : Stay Down Start Time
-- 5[float] : Total Fall Distance

-- Point Param Definition
-- 0 : Falling Start Pos
-- 1 : Falling Target Pos
-- 2 : Falling Direction

-- Action State Definition
ACT_FALL_ACTIONSTATE_FALLING_DOWN=0
ACT_FALL_ACTIONSTATE_STAY_DOWN=1
ACT_FALL_ACTIONSTATE_DONE=2

-- Script Param Definition
-- "INITIAL_SPEED" : Falling Initial Speed
-- "ACCELATION" : Falling Accelation
-- "STAY_DOWN_TIME" : Stay Down Time

------------------------------------------------
-- Event Handling Functions
------------------------------------------------
function Act_Fall_OnEnter(actor, action)
	if actor:IsRidingPet() then --라이딩펫: 탑승 중 피격당하면 강제 하차→피격 액션 진행
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end


	Act_Fall_InitActorState(actor,action);
	Act_Fall_InitActionParams(actor,action);
	
	local	bIsMeetFloor = actor:IsMeetFloor();
	
	if bIsMeetFloor then
	
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		Act_Fall_StartStayDown(actor,action,g_world:GetAccumTime());
	
	else
	
		Act_Fall_StartFallingDown(actor,action);
	
	end
	
	return true
end

function Act_Fall_OnUpdate(actor, accumTime, frameTime,action)
	
	local	kAction = actor:GetAction();
	local	iActionState = Act_Fall_GetActionState(kAction);
	
	if iActionState == ACT_FALL_ACTIONSTATE_FALLING_DOWN then
	
		Act_Fall_OnActionStateFallingDown(actor,kAction,accumTime);
		
	elseif iActionState == ACT_FALL_ACTIONSTATE_STAY_DOWN then
	
		Act_Fall_OnActionStateStayDown(actor,kAction,accumTime);
	
	elseif iActionState == ACT_FALL_ACTIONSTATE_DONE then
		
		return	false;
	
	end
	
	return true
end

function Act_Fall_OnCleanUp(actor, action)

	Act_Fall_RestoreActorState(actor,action);
	
end


function Act_Fall_OnLeave(actor, action)

	return	true;
end


------------------------------------------------
-- Implementation Functions
------------------------------------------------

function Act_Fall_GetScriptParams(action)

	Act_Fall_GetScriptParam_InitialSpeed(action);
	Act_Fall_GetScriptParam_Accelation(action);
	Act_Fall_GetScriptParam_StayDownTime(action);

end
function Act_Fall_GetScriptParam_InitialSpeed(action)

	local	kInitialSpeed = action:GetScriptParam("INITIAL_SPEED");
	local	fInitialSpeed = tonumber(kInitialSpeed);
	
	if fInitialSpeed ~= nil then
	
		Act_Fall_SetInitialSpeed(action,fInitialSpeed);
		
	else
		
		MessageBox("INITIAL_SPEED Required.","Act_Fall Error.");
	
	end

end
function Act_Fall_GetScriptParam_Accelation(action)
	local	kAccelation = action:GetScriptParam("ACCELATION");
	local	fAccelation = tonumber(kAccelation);
	
	if fAccelation ~= nil then
	
		Act_Fall_SetAccelation(action,fAccelation);
		
	else
		
		MessageBox("ACCELATION Required.","Act_Fall Error.");
	
	end
end
function Act_Fall_GetScriptParam_StayDownTime(action)
	local	kStayDownTime = action:GetScriptParam("STAY_DOWN_TIME");
	local	fStayDownTime = tonumber(kStayDownTime);
	
	if fStayDownTime ~= nil then
	
		Act_Fall_SetStayDownTime(action,fStayDownTime);
		
	else
		
		MessageBox("STAY_DOWN_TIME Required.","Act_Fall Error.");
	
	end
end

function Act_Fall_InitActorState(actor,action)
	actor:FreeMove(true);	
	actor:StopJump();
	actor:StartGodTime(0);
	actor:SetMovingDelta(Point3(0,0,0));
	actor:SetNoWalkingTarget(false);
	actor:Concil(false)
	actor:SetBlowUp(true);		

end
function Act_Fall_RestoreActorState(actor,action)
	actor:EndBodyTrail();
	actor:Concil(true)
	actor:FreeMove(false);
	actor:SetBlowUp(false);
	actor:SetDownState(false);
end

function Act_Fall_OnActionStateFallingDown(actor,action,accumTime)

	local	kFallingDownPosition = Act_Fall_CalcFallingPosition(actor,action,accumTime);
	
	actor:SetTranslate(kFallingDownPosition);
	
	if action:GetCurrentSlot() == 0 then
		local	bAnimDone = actor:IsAnimationDone();
		actor:PlayNext();
	end
	
	if Act_Fall_IsFallingDownOver(actor,action,accumTime) then
	
		Act_Fall_StartStayDown(actor,action,accumTime);
	
	end

end

function Act_Fall_IsFallingDownOver(actor,action,accumTime)
	local	fFallingDistance = Act_Fall_CalcFallingDistance(actor,action,accumTime);
	local	fTotalMoveDistance = Act_Fall_GetTotalFallDistance(action)

	return fFallingDistance>=fTotalMoveDistance;
end

function Act_Fall_CalcFallingDownElapsedTime(actor,action,accumTime)

	local	fElapsedTime = accumTime - action:GetActionEnterTime();
	return	fElapsedTime;
	
end


function Act_Fall_CalcFallingPosition(actor,action,accumTime)
	local	kStartPos = Act_Fall_GetFallingStartPos(action);
	local	kDirection = Act_Fall_GetFallingDirection(action);
	
	local	fFallingDistance = Act_Fall_CalcFallingDistance(actor,action,accumTime);
	local	kNextFallingPosition = kStartPos:_Add(kDirection:_Multiply(fFallingDistance));
	return	kNextFallingPosition;

end

function Act_Fall_CalcFallingDistance(actor,action,accumTime)
	local	fElapsedTime = Act_Fall_CalcFallingDownElapsedTime(actor,action,accumTime);
	local	fInitalSpeed = Act_Fall_GetInitialSpeed(action);
	local	fAccelation = Act_Fall_GetAccelation(action);
	local	fTotalMoveDistance = Act_Fall_GetTotalFallDistance(action)
	
	local	fMoveDistance = (fAccelation*fElapsedTime + fInitalSpeed)*fElapsedTime;
	
	if fMoveDistance>fTotalMoveDistance then
		fMoveDistance = fTotalMoveDistance
	end
	
	return	fMoveDistance;
end


function Act_Fall_OnActionStateStayDown(actor,action,accumTime)

	local	fElapsedTime = Act_Fall_CalcStayDownElapsedTime(actor,action,accumTime);
	
	if Act_Fall_IsStayDownTimeOver(actor,action,fElapsedTime) then
		Act_Fall_SetActionState(action,ACT_FALL_ACTIONSTATE_DONE);
	end

end


function Act_Fall_IsStayDownTimeOver(actor,action,fElapsedTime)

	local	fTotalStayDownTime = Act_Fall_GetStayDownTime(action);
	
	return	fElapsedTime>=fTotalStayDownTime;

end

function Act_Fall_CalcStayDownElapsedTime(actor,action,accumTime)

	local	fElapsedTime = accumTime - Act_Fall_GetStayDownStartTime(action);
	local	fTotalStayDownTime = Act_Fall_GetStayDownTime(action);
	
	if fElapsedTime > fTotalStayDownTime then
		fElapsedTime = fTotalStayDownTime
	end
	
	return	fElapsedTime;
	
end

function Act_Fall_InitActionParams(actor,action)
	
	Act_Fall_GetScriptParams(action);

end

function Act_Fall_StartStayDown(actor,action,accumTime)

	Act_Fall_SetActionState(action,ACT_FALL_ACTIONSTATE_STAY_DOWN);
	Act_Fall_SetStayDownStartTime(action,accumTime);

	actor:SetDownState(true);
	action:SetSlot(2);
	actor:PlayCurrentSlot();
end

function Act_Fall_StartFallingDown(actor,action)

	Act_Fall_SetActionState(action,ACT_FALL_ACTIONSTATE_FALLING_DOWN);
	Act_Fall_SetParamsForFallingDown(actor,action);

end
function Act_Fall_SetParamsForFallingDown(actor,action)

	Act_Fall_SetFallingStartPos(action,actor:GetPos());
	Act_Fall_SetFallingTargetPos(action,Act_Fall_FindFallDownPos(actor,action));
	Act_Fall_SetFallingDirection(action,Act_Fall_Calc_FallDownDirection(actor,action));
	Act_Fall_SetTotalFallDistance(action,Act_Fall_Calc_FallTotalDistance(actor,action));

end
function Act_Fall_Calc_FallTotalDistance(actor,action)

	local	fFallDownDistance = Act_Fall_GetFallingTargetPos(action):Distance(Act_Fall_GetFallingStartPos(action));
	return	fFallDownDistance;

end

function Act_Fall_Calc_FallDownDirection(actor,action)

	local	kFallDownDirection  = Act_Fall_GetFallingTargetPos(action):_Subtract(Act_Fall_GetFallingStartPos(action));
	kFallDownDirection:Unitize();
	
	return	kFallDownDirection;

end
function Act_Fall_FindFallDownPos(actor,action)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local kFloorPos = g_world:ThrowRay(actor:GetPos(),Point3(0,0,-1),10000);
	if kFloorPos:IsEqual(Point3(-1,-1,-1)) then
		kFloorPos = actor:GetPos()
	end

	
	kFloorPos = g_world:FindActorFloorPos(kFloorPos);
	
	return	kFloorPos;

end

------------------------------------------------
-- Parameter Set/Get Functions
------------------------------------------------

function Act_Fall_SetActionState(action,iState)

	action:SetParamInt(0,iState);

end
function Act_Fall_GetActionState(action)

	return	action:GetParamInt(0);

end
function Act_Fall_SetInitialSpeed(action,fInitialSpeed)

	action:SetParamFloat(1,fInitialSpeed);

end
function Act_Fall_GetInitialSpeed(action)

	return	action:GetParamFloat(1);

end
function Act_Fall_SetAccelation(action,fAccelation)

	action:SetParamFloat(2,fAccelation);

end
function Act_Fall_GetAccelation(action)

	return	action:GetParamFloat(2);

end
function Act_Fall_SetStayDownTime(action,fStayDownTime)

	action:SetParamFloat(3,fStayDownTime);

end
function Act_Fall_GetStayDownTime(action)

	return	action:GetParamFloat(3);

end
function Act_Fall_SetStayDownStartTime(action,fStayDownStartTime)

	action:SetParamFloat(4,fStayDownStartTime);

end
function Act_Fall_GetStayDownStartTime(action)

	return	action:GetParamFloat(4);

end
function Act_Fall_SetTotalFallDistance(action,fTotalFallDistance)

	action:SetParamFloat(5,fTotalFallDistance);

end
function Act_Fall_GetTotalFallDistance(action)

	return	action:GetParamFloat(5);

end
function Act_Fall_SetFallingStartPos(action,kFallingStartPos)

	action:SetParamAsPoint(0,kFallingStartPos);

end
function Act_Fall_GetFallingStartPos(action)

	return	action:GetParamAsPoint(0);

end
function Act_Fall_SetFallingTargetPos(action,kFallingTargetPos)

	action:SetParamAsPoint(1,kFallingTargetPos);

end
function Act_Fall_GetFallingTargetPos(action)

	return	action:GetParamAsPoint(1);

end
function Act_Fall_SetFallingDirection(action,kFallingDirection)

	action:SetParamAsPoint(2,kFallingDirection);

end
function Act_Fall_GetFallingDirection(action)

	return	action:GetParamAsPoint(2);

end
