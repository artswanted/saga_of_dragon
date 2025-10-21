-- TeleJump

function Net_PT_C_M_REQ_START_HYPER_MOVE(kTriggerID, iHyperType) -- 1이 텔레점프 2는 텔레무브
	CheckNil(nil==kTriggerID)
	if nil==kTriggerID or ""==kTriggerID then
		return
	end

	if nil==iHyperType or 0>=iHyperType then
		return
	end

	local kMyActor = GetMyActor()
	if nil==kMyActor or kMyActor:IsNil() then
		return
	end
	local ptPos = kMyActor:GetTranslate()

	local packet = NewPacket(19001)
	packet:PushInt(iHyperType)
	packet:PushString(kTriggerID)	--순서 바뀌면 안됨
	packet:PushPoint3(ptPos)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_PT_C_M_NFY_END_HYPER_MOVE(ptPos, bRealyJump)
	if nil==ptPos then
		local kMyActor = GetMyActor()
		if nil==kMyActor or kMyActor:IsNil() then
			return
		end
		ptPos = kMyActor:GetTranslate()
	end
	local packet = NewPacket(19003)
	packet:PushPoint3(ptPos)
	packet:PushGuid(g_pilotMan:GetHyperJumpGuid())
	packet:PushBool(bRealyJump)
	Net_Send(packet)
	DeletePacket(packet)
end

function DoTeleJump(actor, target, extraHeight, iTriggerIndex)
	local curAction = actor:GetAction()
	if actor:IsNil() or curAction:IsNil() then
		return true
	end
	
	if actor:IsMyActor() == false or actor:IsNowFollowing() then
		return	true
	end

	curAction:SetParam(911, target)
	curAction:SetParamFloat(912, extraHeight)
	curAction:SetParamInt(913, iTriggerIndex)

	ODS("ExtraHeight : " .. extraHeight .. "\n", false, 1509)
	ODS("Target : " .. target .. "\n", false, 1509)
	ODS("Trigger Index : " .. iTriggerIndex .. "\n", false, 1509)
	actor:ReserveTransitAction("a_telejump")
end

function Act_TeleJump_OnEnter(actor, action)
	-- 점프 타겟을 못 찾으면 어디로 뛰라고? 
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local curAction = actor:GetAction()
	local jumpTarget = curAction:GetParam(911)
	local fHeight = curAction:GetParamFloat(912)
	
	if actor:IsMyActor() and actor:IsNowFollowing() == false then
		local iTriggerIndex = curAction:GetParamInt(913)
		action:SetParamInt(913, iTriggerIndex)
		if jumpTarget == "null" then
			--curAction:GetID() == "a_telejump" then
			return false
		end
	else
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
		jumpTarget = kTrigger:GetParamAsString():GetStr()
		fHeight = kTrigger:GetParam()
	end

	-- 위와 마찬가지!
	local kTargetLoc = g_world:GetObjectPosByName(jumpTarget)
	if kTargetLoc:IsZero() == true then
		return false
	end

	actor:SetWalkingTargetLoc(kTargetLoc, 1)
	if actor:IsRidingPet() then
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() then
			return false
		end
		kActorPet:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",300,50,0)
	else
		actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",300,50,0)
	end
	
	-- 멋지게 한 번 뛰어보자
	local fVel = actor:StartTeleJump(kTargetLoc, fHeight)
	kTargetLoc:Subtract(actor:GetTranslate())
	kTargetLoc:SetZ(0)
	local fDistance = kTargetLoc:Length()
	action:SetParamFloat(1366, math.abs(fVel))
	actor:SetAdjustValidPos(false)
	actor:SetForcePos(false)
	if actor:IsMyActor() then
		UseCameraHeightAdjust(false)
	end
	actor:SetActiveGrp(PG_PHYSX_GROUP_BASE_WALL, false)
	actor:LockBidirection(false)

	local targetDir = actor:GetWalkingTargetDir()
	action:SetParamFloat(2080, targetDir:GetX())
	action:SetParamFloat(2081, targetDir:GetY())

	-- 텔레점프 하는데 걸릴 시간.
	action:SetParamFloat(3000, fDistance / fVel)

	--action:SetParamFloat(1367, fDistance / fVel * 0.98)

	--ODS("Time : " .. fDistance / fVel .. "\n")
	--ODS("ExtraHeight : " .. fHeight .. "\n")
	--ODS("Horizontal Velocity = " .. fVel .. "\n")
	if actor:IsRidingPet() then --펫탑승중이면
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() then
			return false
		end
		local kActionPet = kActorPet:GetAction()
		if kActionPet:IsNil() then
			return false
		end

		action:SetSlot(4) --펫의 애니메이션도 같이 조종해준다
		actor:PlayCurrentSlot()
		kActionPet:SetSlot(11)
		kActorPet:PlayCurrentSlot()
	end

	Net_PT_C_M_REQ_START_HYPER_MOVE(curAction:GetParam(914), 1)
	return true
end

function Act_TeleJump_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	local currentSlot = action:GetCurrentSlot()
	local fVel = action:GetParamFloat(1366)

	if action:GetParamFloat(6184) == 0 then
		action:SetParamFloat(6184, accumTime)
	end
	
	
		

	local elapsedTime = accumTime - action:GetParamFloat(6184)
	--ODS("Elapsed Time : " .. elapsedTime .. "\n")

	if action:GetParamFloat(3000) < elapsedTime then
		local kTargetLoc = actor:GetWalkingTargetLoc()
		if kTargetLoc:GetX() ~= 0 and kTargetLoc:GetY() ~= 0 then
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			ODS("Falling on Telejump!!, Warp to Target : " .. kTargetLoc:GetX() .. " , " .. kTargetLoc:GetY() .. " , " .. kTargetLoc:GetZ() .. "\n")
			actor:SetForcePos(true)
			actor:SetTranslate(g_world:FindActorFloorPos(kTargetLoc))
		end
	end
	
	
	if actor:IsRidingPet() then
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() then
			return false
		end
		local kActionPet = kActorPet:GetAction()
		if kActionPet:IsNil() then
			return false
		end
		
		if kActionPet:GetCurrentSlot() ~= 10 and kActionPet:GetCurrentSlot() > 7 and actor:IsMeetFloor() then --착지 했어? 그럼 착지 액션해
			kActionPet:SetSlot(10)
			kActorPet:PlayCurrentSlot()
		elseif kActorPet:IsAnimationDone() then
			if kActionPet:GetCurrentSlot() == 10 then
				return false
			else
				kActionPet:SetSlot(11)
				kActorPet:PlayCurrentSlot()
			end
		end
	elseif actor:IsAnimationDone() == true then
		actor:PlayNext()
	else
		if currentSlot ~= 0 and currentSlot ~= 7 and currentSlot ~= 10 and
			actor:IsMeetFloor() then
			if action:GetParam(10) == "Next_Die" then
				action:SetParam(10,"Telejump_Die_OK")
				action:SetNextActionName("a_die");
			end
			return false
		end
	end
	

	-- 방향은 뭘 넣든, 가야되는 방향으로 간다.
	actor:Walk(DIR_NONE, fVel, frameTime)
	

	return true
end
function Act_TeleJump_OnCleanUp(actor, action)
	local curAction = actor:GetAction()
	local targetDir = Point3(curAction:GetParamFloat(2080), curAction:GetParamFloat(2081), 0)
	--ODS("______________MovingDir : " .. targetDir:GetX() .. ", " .. targetDir:GetY() .. ", " .. targetDir:GetZ() .. "____________\n")
	actor:SetLookingDirection(actor:GetDirFromMovingVector(targetDir))

	actor:SetNoWalkingTarget(false)
	if actor:IsRidingPet() then
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:EndBodyTrail()
		end
	else
		actor:EndBodyTrail()
	end
	actor:SetAdjustValidPos(true)
	actor:SetForcePos(true)
	actor:ResetJumpAccumHeight()
	actor:SetActiveGrp(PG_PHYSX_GROUP_BASE_WALL, true)
	actor:SetMeetFloor(true)
	actor:FindPathNormal()
	actor:RestoreLockBidirection()


	if actor:IsMyActor() then		
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		UseCameraHeightAdjust(true)
		local jumpTarget = curAction:GetParam(911)
		local kTargetPos = g_world:GetObjectWorldPosByName(jumpTarget)
		local bRealyJump = false
		if nil~=kTargetPos then
			local fDist = kTargetPos:Distance(actor:GetTranslate())
			bRealyJump = fDist <= 80
		end
		Net_PT_C_M_NFY_END_HYPER_MOVE(actor:GetTranslate(), bRealyJump)
		if actor:IsLockBidirection() then
			--	if lock bidirection is true, we make this actor to see left direction
			local	kPathNormal = actor:GetPathNormal();
			local	kNewDir = kPathNormal:Cross(Point3(0,0,-1));
			actor:SetMovingDir(kNewDir);	
			actor:ToLeft(true,true);
		else
			actor:SeeFront(true, false)
		end
		actor:SeeFront(true, false)
	end
end

function Act_TeleJump_OnLeave(actor, action)
	local actionID = action:GetID()
	local curAction = actor:GetAction()

	if actionID == "a_telejump" then
		return true
	elseif actionID == "a_die" then
		return	true
	elseif actionID == "a_idle" or "a_step_the_beat_idle" == actionID	then
		curAction:SetParam(1256, "Done")	
		return true
	elseif actionID == "a_run" and 
		curAction:GetParam(1256) == "Done" then
		return true
	elseif actionID == "a_act" then
		-- 실행 가능한 트리거가 있는지 체크해보자.
		ODS("while jump, action check\n")
		if curAction:GetParam(1256) == "Done" then
			return true
		end
		
		local kTrigger = actor:GetCurrentTrigger();
		if kTrigger:IsNil() == false then
			local	kConditionAction = kTrigger:GetConditionAction();
			if kConditionAction == actionID and 
				kTrigger:GetConditionType() == CT_ACTION then
				ODS("can transit action.\n")
				return true
			end
		end
	end
	return false
end

function Act_TeleJump_OnOverridePacket(actor, action, packet)
	local iTriggerIndex = action:GetParamInt(913)
	packet:PushInt(iTriggerIndex)
end

function CheckCanSpecialRidingAction(kActorPet, fJumpTime)
	if kActorPet:IsNil() then
		return false
	end

	local fAnimationTime = kActorPet:GetAnimationTime("jump_01") + kActorPet:GetAnimationTime("jump_02") + 
	kActorPet:GetAnimationTime("jump_03") + kActorPet:GetAnimationTime("jump_idle_02")
	
	return (fAnimationTime >= fJumpTime)
	
end
