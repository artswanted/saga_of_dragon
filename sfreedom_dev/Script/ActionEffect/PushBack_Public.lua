-- damage
--
function Act_PushBack_public_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil())
	local CurAction = actor:GetAction()
	CheckNil(nil==CurAction);
	CheckNil(CurAction:IsNil())

	-- action:SetParamFloat(6, 0)
	if action:GetScriptParam("IS_OVERLAP")~="TRUE" and CurAction:GetID()==action:GetID() then
		return false
	end

	return	true
end

function Act_PushBack_public_OnEnter(actor, action)
	if actor:IsRidingPet() then --라이딩펫: 탑승 중 피격당하면 강제 하차→피격 액션 진행
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end
	actor:StopJump();
	actor:StartGodTime(0);
	actor:SetMovingDelta(Point3(0,0,0));
	actor:Stop()
	actor:SetNoWalkingTarget(false);

	MovingActor(actor, action)
	
	action:SetSlot(0);
	return true
end

function Act_PushBack_public_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	if actor:IsAnimationDone() and actor:GetNowPush() == false then
		
		actor:SetSendBlowStatus(false, true, true);

		local iNextSlot = action:GetCurrentSlot()+1
		if false==action:NextSlot() or actor:GetAnimationLength(action:GetSlotAnimName( iNextSlot )) == 0 then
			return	false
		end
		actor:PlayCurrentSlot()

	end
	return true
end

function Act_PushBack_public_OnCleanUp(actor, action)

	actor:SetDownState(false)
	actor:SetCanHit(true)		
	
	actor:SetSendBlowStatus(false)

	actor:FreeMove(false)
	actor:SetMeetFloor(true)
end

function Act_PushBack_public_OnLeave(actor, action)
	if action:GetID() ~= "a_die" then
		local kEffectDef = GetEffectDef(actor:GetAction():GetActionNo());
		local fGodTime = 1;
		if kEffectDef:IsNil() == false then
			fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0;
		end
		actor:StartGodTime(fGodTime);
	end
	
	if actor:IsMeetFloor() then
		
	end
	
	actor:SetMovingDir(Point3(0, 0, 0))
	actor:SetSendBlowStatus(false, false, true);
	return true
end
