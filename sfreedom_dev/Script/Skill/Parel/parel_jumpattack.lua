
function Act_PAREL_JumpAttack_OnEnter(actor,action)
--	actor:StopJump()
--	actor:ResetAnimation()
	
	local ptTargetPos = actor:GetPos()
	ptTargetPos:SetZ(ptTargetPos:GetZ())
	local kPacket = action:GetParamAsPacket()
	if kPacket ~= nil and false == kPacket:IsNil() then
		ptTargetPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat()+25)
	end
	
	if action:GetActionParam() == AP_CASTING then
		-- Look at targetpos
		
		actor:LookAt(ptTargetPos,true,false)
	
		actor:SetWalkingTargetLoc(ptTargetPos,true)
		actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",300,50,0)
		
		action:SetParamInt(0,0)	--	State
		action:SetParamAsPoint(0,actor:GetWalkingTargetLoc())
		action:SetParamFloat(1,-100)
  		actor:AttachParticleToPointS(299,ptTargetPos,"e_ef_colorshadow_monster",2.5)
	end
	return true
end

function Act_PAREL_JumpAttack_OnEvent(actor,textkey)

	local	action = actor:GetAction()
	
	if action:GetParamInt(0) == 0 then
		if textkey == "fly" or textkey == "end" then
--		    WriteToConsole("Act_PAREL_JumpAttack_OnEvent:"..textkey.."\n")
			action:SetParamInt(0,1)
		end
	end	
	return	true	
end

function Act_PAREL_JumpAttack_OnCastingCompleted(actor, action)
	    Act_PAREL_Clear(actor)
	    
	    action:SetSlot(1)
		actor:ResetAnimation()
		actor:PlayCurrentSlot()
		
		action:SetParamInt(0,4)
		local kPos = g_world:FindActorFloorPos(action:GetParamAsPoint(0))
  		actor:SetTranslate(kPos)
  		actor:AttachParticleToPoint(300,kPos,"e_ef_parel_skill_04")
		QuakeCamera(0.5,5,0,0.1,5)
		Act_Mon_Melee_DoDamage(actor,action)
		actor:AttachSound(1158,"Samb_start")
end

function Act_PAREL_JumpAttack_OnUpdate(actor, accumTime, frameTime)

	local action = actor:GetAction()
	local ptTargetPos = action:GetParamAsPoint(0) --TargetPos
	
--	WriteToConsole("Update TargetPos: " .. ptTargetPos:GetX() .. ", " .. ptTargetPos:GetY() .. ", " .. ptTargetPos:GetZ() .."\n")
  	local ActionState = action:GetParamInt(0)
  	local oldPosZ = action:GetParamFloat(1)
	local fVal = action:GetParamFloat(2)

	if 0 == ActionState then
	
	elseif 1 == ActionState then--올라가기 시작	 
		-- 점프를 시작해보자
			
		action:SetParamFloat(1,actor:GetPos():GetZ())	--	Now Height
		
		local fVal = actor:StartTeleJump(ptTargetPos,400)
		ptTargetPos:Subtract(actor:GetTranslate())
		ptTargetPos:SetZ(0)
		
		local fDistance = ptTargetPos:Length()
		action:SetParamFloat(2, math.abs(fVal))
--		actor:SetAdjustValidPos(false)
--		actor:SetForcePos(false)

--		actor:SetActiveGrp(PG_PHYSX_GROUP_BASE_WALL, false)
--		actor:LockBidirection(false)

		action:SetParamInt(0,2)	--	State
		
	elseif 2 == ActionState then

	    local NowPos = actor:GetTranslate()
		actor:Walk(DIR_NONE, fVal, frameTime)
		
		local kNowPosZ = NowPos:GetZ()
		if oldPosZ > kNowPosZ then
			-- 내려가는 중으로 바꼈다.
			actor:PlayNext()
			action:SetParamInt(0,3)
		end
		action:SetParamFloat(1,kNowPosZ)
		
	elseif 3 == ActionState then -- 내려가는 중이다

        local kWaitPos = ptTargetPos
		kWaitPos:SetZ(oldPosZ)
	    local NowPos = actor:GetTranslate()
		actor:Walk(DIR_NONE, fVal, frameTime)
		NowPos:SetZ(oldPosZ)
--		actor:SetTranslate(kWaitPos)
		
--		local kPos = g_world:FindActorFloorPos(kWaitPos)

--		return false
	elseif 4 == ActionState then
 	-- FireSkill
		-- 빠르게 내려온다
		
		if actor:IsAnimationDone() then
			actor:PlayNext()
			action:SetParamInt(0,5)
		end
--		return false
--	else
--		if actor:IsAnimationDone() then
--			return false
--		end
	elseif 5 == ActionState then
		if actor:IsAnimationDone() then
			return false
		end
	end	
	return true
end

function Act_PAREL_Clear(actor)
	WriteToConsole("Clear : " .. actor:GetPos():GetX() ..  "/" .. actor:GetPos():GetY() .. "/" .. actor:GetPos():GetZ() .. "\n")
--	local targetDir = Point3(curAction:GetParamFloat(5), curAction:GetParamFloat(6), 0)
--	actor:SetLookingDirection(actor:GetDirFromMovingVector(targetDir))

	actor:SetNoWalkingTarget(false)
--	actor:SetAdjustValidPos(true)
--	actor:SetForcePos(true)
	actor:EndBodyTrail()
	actor:StopJump()
	actor:ResetJumpAccumHeight()
--	actor:SetActiveGrp(PG_PHYSX_GROUP_BASE_WALL, true)
	actor:SetMeetFloor(true)
	actor:FindPathNormal()
--	actor:RestoreLockBidirection()
end

function Act_PAREL_JumpAttack_OnCleanUp(actor, action)
end

function Act_PAREL_JumpAttack_OnLeave(actor, action)
	if action:GetParamInt(0) > 3 then
    	actor:DetachFrom(299)
		actor:DetachFrom(300)
	else
	end
	return true
end


