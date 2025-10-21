function Act_Rest_OnCheckCanEnter(actor, action)
	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if actor:IsMeetFloor() == false then
		return	false
	end
	
	return	true;
end
function Act_Rest_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local actionID = action:GetID()
	local iSlotCount = action:GetSlotCount()
	
	if iSlotCount ~= 3  then
		return false
	end

	actor:HideParts(6, true)
	action:SetParam(6,"HideParts")
	
	if actionID == "a_sleep" then
		action:SetSlot(1)
	end
	
	action:SetThrowRayStatus(true)
	actor:FreeMove(true);	
	actor:StopJump();
	actor:SetNoWalkingTarget(false);
	actor:ResetAnimation();	
	local ptTargetPos = 0;	
	ptTargetPos = actor:GetPos()

	if ptTargetPos ~= nil or ptTargetPos:IsNil() == false then
		--local kKind = action:GetParam(100)	
		local kKind = actor:GetStartParamID("a_rest")
		if kKind ~= "null" then
			if kKind ~= nil or kKind ~= "" then	
				if( CheckNil(nil == g_world) ) then return false end
				if( CheckNil(g_world:IsNil()) ) then return false end
				
				local TargetPos = g_world:ThrowRay(Point3(ptTargetPos:GetX(),ptTargetPos:GetY(),ptTargetPos:GetZ()+50.0),Point3(0,0,-1),500)
				
				actor:LockBidirection(false)
				actor:SeeFront(true, true)

				local kPPos = actor:GetNodeWorldPos("char_root");
				
				actor:AttachParticleToPointWithRotate(99100, TargetPos, kKind, actor:GetRotateQuaternion(), 1.0)
				
				local kPosition = 0
				if actor ~= nil or actor:IsNil() == false then
					local Gender = actor:GetAbil(AT_GENDER)
					if Gender == 1 then
						kPosition = actor:GetParticleNodeWorldPos(99100, "p_pt_ride_m")
					else
						kPosition = actor:GetParticleNodeWorldPos(99100, "p_pt_ride_f")
					end				
				else
					kPosition = Point3(0,0,15)
				end

				--의자가 생성되는 위치와 실제 Player가 앉는 위치가 다르다.
				--실제 캐릭터가 서있는곳에 Player가 앉아야 하기 때문에 그 차이를 구해서 차이만큼 빼준다.

				local kDiffPos = kPPos:_Subtract(kPosition);
				kDiffPos:SetZ(0);
				ptTargetPos = Point3(kPosition:GetX(),kPosition:GetY(),kPosition:GetZ() + 25)
				ptTargetPos = ptTargetPos:_Add(kDiffPos);

				actor:SetTranslate(ptTargetPos)
				
				actor:AttachSound(9991,"Use_RestChair");
			end
		end
	end		
	actor:Stop()
	SubActorHide(actor, true)
	return true
end

function Act_Rest_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local action = actor:GetAction()
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	
	local iCurrentSlot = action:GetCurrentSlot()
	local bIsAnimDone = actor:IsAnimationDone()
		
	if iCurrentSlot == 0 and
		bIsAnimDone == true then
		actor:PlayNext()
	elseif iCurrentSlot == 1 then
		return true
	elseif iCurrentSlot == 2 and
		bIsAnimDone == true then
		return false
	end

	return true
end

function Act_Rest_OnLeave(actor, action)	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if action:GetEnable() == false then
		return	false
	end
	
	return true
--[[	
	local action = actor:GetAction()
	local iCurrentSlot = action:GetCurrentSlot()
	local bIsAnimDone = actor:IsAnimationDone()
	local kNextActionName = action:GetID()

	if iCurrentSlot == 1 then
		actor:PlayNext()
		return false
	elseif iCurrentSlot == 2 and 
		bIsAnimDone == true then
		return true
	end

	return false
]]	
end

function Act_Rest_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--local kActionID = action:GetID()
	--if kActionID == "a_run" or kActionID == "a_dmg" or kActionID == "a_lock_move" or kActionID == "a_unlock_move" or kActionID == "a_rest" or kActionID == "a_idle" or kActionID == "a_entering_pvp" then
		--local kEffectID = actor:GetAction():GetParam(101)
		local kEffectID = actor:GetStartEffectSave("a_rest")
		if kEffectID ~= 0 and actor:IsMyActor() then
			local	kPacket = NewPacket(12130)
			kPacket:PushGuid(actor:GetPilotGuid())
			kPacket:PushInt(kEffectID)
			Net_Send(kPacket)
			DeletePacket(kPacket)
			--RemoveStatusEffect(kEffectID)
		end
		local paramValue = actor:GetAction():GetParam(6)
		local actionID = actor:GetAction():GetID()
		if paramValue == "HideParts" then				
			actor:HideParts(6, false)
			actor:GetAction():SetParam(6,"")
		end			
		Act_Rest_OnClearUpRun(actor)
	--end	
	SubActorHide(actor, false)
end

function Act_Rest_OnClearUpRun(actor)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	actor:DetachFrom(99100);
	actor:FreeMove(false);
	actor:SeeFront(false,true)
	actor:RestoreLockBidirection()		
	--actor:HideParts(6, false)	
	--actor:ReserveTransitAction("a_idle")
	
	local kCurAction = actor:GetAction();
	if( CheckNil(kCurAction == nil) ) then return false end
	if( CheckNil(kCurAction:IsNil()) ) then return false end
	kCurAction:SetThrowRayStatus(false)	
end
