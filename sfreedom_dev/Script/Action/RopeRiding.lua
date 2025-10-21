-- RopeRiding

function DoRopeRiding(trigger, actor)
	if actor:GetAbil(AT_HP) <= 0 then
		return
	end

	local ropename = trigger:GetParamAsString():GetStr();
	local movespeed = trigger:GetParam();
	local iTriggerIndex = trigger:GetIndex();

	local curAction = actor:GetAction()
	if actor:IsNil() or curAction:IsNil() then
		return;
	end
	
	if actor:IsMyActor() == false or actor:IsNowFollowing() then
		return;
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	if movespeed<=0 then
		return
	end
	
	local	kRope = g_world:GetRope(ropename)
	if kRope:IsNil() then
		return;
	end

	local kRidingAction = actor:ReserveTransitAction("a_rope_riding")
	
	kRidingAction:SetParam(911, ropename)
	kRidingAction:SetParamFloat(912, movespeed)
	kRidingAction:SetParamInt(913, iTriggerIndex)	
end

function Act_RopeRiding_OnEnter(actor, action)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	ODS("Act_RopeRiding_OnEnter\n");

	-- 점프 타겟을 못 찾으면 어디로 뛰라고? 
	local kRopeName = action:GetParam(911)
	local fMoveSpeed = action:GetParamFloat(912)

	if actor:IsMyActor() == false or actor:IsNowFollowing() then

		local kPacket = action:GetParamAsPacket()
		if kPacket:IsNil() then
			return false
		end

		local iTriggerNo = kPacket:PopInt()
		local kTrigger = g_world:GetTriggerByIndex(iTriggerNo)
		if kTrigger:IsNil() then
			return false
		end

		-- Jump Target과 뛸 높이를 설정
		kRopeName = kTrigger:GetParamAsString():GetStr()
		fMoveSpeed = kTrigger:GetParam()
	end

	ODS("Act_RopeRiding_OnEnter Find Rope\n");
	local	kRope = g_world:GetRope(kRopeName)
	if kRope:IsNil() then
		ODS("Act_RopeRiding_OnEnter Find Failed\n");
		return	false
	end
	
	local targetDir = actor:GetWalkingTargetDir()
	action:SetParamFloat(2080, targetDir:GetX())
	action:SetParamFloat(2081, targetDir:GetY())
		
	local	fDistance = kRope:GetStartPos():Distance(kRope:GetEndPos());
	action:SetParamFloat(0,fDistance);	--	총거리
	action:SetParamFloat(1,g_world:GetAccumTime());	--	시작시간
	
	action:SetParamAsPoint(0,kRope:GetStartPos());	--	출발위치
	action:SetParamAsPoint(1,kRope:GetEndPos());	--	종료위치
	
	local	kMoveDir = kRope:GetEndPos():_Subtract(kRope:GetStartPos());
	kMoveDir:Unitize();
	action:SetParamAsPoint(2,kMoveDir);	--	이동 방향 벡터

	actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",300,50,0)
	
	if actor:IsMyActor() then
		UseCameraHeightAdjust(false)
	end

	actor:LockBidirection(false)
	actor:FreeMove(true);
	actor:SetAdjustValidPos(false);

	--	시작위치로 이동
	actor:SetTranslate(kRope:GetStartPos());
	
	--	목표 지점을 바라보자!
	actor:LookAt(kRope:GetEndPos(),true,false,true);
	
	ODS("Act_RopeRiding_OnEnter Success\n");

	return true
end

function Act_RopeRiding_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	local fMoveSpeed = action:GetParamFloat(912)
	local	fTotalDistance = action:GetParamFloat(0)
	local	fStartTime = action:GetParamFloat(1)
	local	fElapsedTime = accumTime - fStartTime;
	local	fMoveDistance = fElapsedTime*fMoveSpeed;
	
	if fMoveDistance>fTotalDistance then
		fMoveDistance = fTotalDistance 
	end

	local	kStartPos = action:GetParamAsPoint(0)
	local	kEndPos = action:GetParamAsPoint(1)
	local	kMoveDir = action:GetParamAsPoint(2)
	
	actor:LookAt(kEndPos,true,false,true);
	
	local	kNextPos = kMoveDir:_Multiply(fMoveDistance);
	kNextPos:Add(kStartPos);

	actor:SetTranslate(kNextPos);	
	
	if fTotalDistance == fMoveDistance then	--	다 왔다, 종료하자
		return	false	
	end

	return true
end
function Act_RopeRiding_OnCleanUp(actor, action)
	local curAction = actor:GetAction()
	local targetDir = Point3(curAction:GetParamFloat(2080), curAction:GetParamFloat(2081), 0)
	--ODS("______________MovingDir : " .. targetDir:GetX() .. ", " .. targetDir:GetY() .. ", " .. targetDir:GetZ() .. "____________\n")
	actor:SetLookingDirection(actor:GetDirFromMovingVector(targetDir))

	actor:EndBodyTrail()
	actor:SetAdjustValidPos(true)
	actor:SetForcePos(true)
	actor:ResetJumpAccumHeight()
	actor:FindPathNormal()
	actor:RestoreLockBidirection()
	actor:FreeMove(false);

	if actor:IsMyActor() then
		UseCameraHeightAdjust(true)
	end
end

function Act_RopeRiding_OnLeave(actor, action)

	ODS("Act_RopeRiding_OnLeave\n");
	
	local actionID = action:GetID()
	local curAction = actor:GetAction()

	if actionID == "a_die" then
		return	true
	elseif actionID == "a_idle" then
		curAction:SetParam(1256, "Done")	
		return true
	elseif actionID == "a_run" and 
		curAction:GetParam(1256) == "Done" then
		return true
	end
	return false
end

function Act_RopeRiding_OnOverridePacket(actor, action, packet)
	local iTriggerIndex = action:GetParamInt(913)
	packet:PushInt(iTriggerIndex)
end
