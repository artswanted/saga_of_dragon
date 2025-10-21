
function Act_knockback_down_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:IsRidingPet() then --라이딩펫: 탑승 중 피격당하면 강제 하차→피격 액션 진행
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end

	actor:FreeMove(true);	
	actor:StopJump();
	actor:StartGodTime(0);
	actor:SetMovingDelta(Point3(0,0,0));
	actor:SetNoWalkingTarget(false);
	MovingActor(actor, action)
	action:SetSlot(1);
	actor:SetCanHit(false);
	return true
end

function Act_knockback_down_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:IsAnimationDone() and actor:GetNowPush() == false then		
		local	iCurrentSlot = action:GetCurrentSlot()
		if 2 == iCurrentSlot then
			actor:ResetAnimation();
			actor:SetTargetAnimation(action:GetCurrentSlotAnimName());
			actor:SetSendBlowStatus(false, true);
			actor:FreeMove(false);	
			actor:SetDownState(false);
			actor:SetCanHit(true);	
		end
		
		local iNextSlot = action:GetCurrentSlot()+1
		if false==action:NextSlot() or actor:GetAnimationLength(action:GetSlotAnimName( iNextSlot )) == 0 then
		
			return false
		end
	end
	actor:PlayNext();
	return true
end

function Act_knockback_down_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	actor:SetDownState(false)
	actor:SetCanHit(true)		
	actor:SetSendBlowStatus(false)
	actor:FreeMove(false)
	actor:SetMeetFloor(true)
end

function Act_knockback_down_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if action:GetID() ~= "a_die" then
		local	kEffectDef = GetEffectDef(actor:GetAction():GetActionNo())
		local	fGodTime = 1
		if kEffectDef:IsNil() == false then
			fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0
		end
		actor:StartGodTime(fGodTime)
	end
	return	true;
end
