function Act_RP_Walk_OnCheckCanEnter(actor, action)
	local kPetActor = actor:GetMountTargetPet()
	if kPetActor == nil or kPetActor:IsNil() then
		return false
	end
	local kPetAction = kPetActor:GetAction()
	if kPetAction:IsNil() == false and kPetAction:GetID() == "a_pet_melee_02" and kPetActor:CheckCanRidingAttack() == false then
		return false
	end
	
	local curAction = actor:GetAction()
	if( false == curAction:IsNil() ) then
		local curActionID = curAction:GetID()
		if curActionID == "a_rp_walk" then
			ODS("Current Action is \"a_run\" : transit failed!\n")
			return false 
		end
	end
	
	return	true;
end
function Act_RP_Walk_OnEnter(actor, action)
	local actionID = action:GetID()
	local prevAction = actor:GetAction()
	local kPetActor = actor:GetMountTargetPet()
	if kPetActor == nil or kPetActor:IsNil() then
		return false
	end
	
	if prevAction:IsNil() == false then
		local prevActionID = prevAction:GetID()
		if prevActionID == "a_rp_jump" and kPetActor:IsMeetFloor() == false then
			-- 현재 액션이 점프이고 바닥이 아니면, 전이 불가능
			ODS("__________________return false on run action _________________\n")
			return false
		elseif prevActionID == "a_rp_walk" then
			return false
		end
		-- 그 외에는 전이 가능
	end

	actor:UseSkipUpdateWhenNotVisible(false);
	action:SetParamInt(1,0)

	local kTargetPos = action:GetParamAsPoint(0)
	if kTargetPos:IsZero() == false then
	
		action:SetParamInt(3,1);
		
		local	kMoveDirection = kTargetPos:_Subtract(actor:GetPos())
		kMoveDirection:Unitize();
		action:SetParamAsPoint(1,kMoveDirection);
		action:SetParamAsPoint(2,actor:GetPos());
		
		actor:SetMovingDir(kMoveDirection)
		actor:LookAt(kTargetPos,true,true,false);
	end
	if action:GetParamInt(5) == 1 then
		actor:BackMoving(true)
	end

	
	--펫 조종 (이동시 내가 아닌, 펫을 달리게 해줘야 함.액션만)--
	action:SetSlot(0) --나의 액션은 아이들로..
	
	local kActorPet = actor:GetMountTargetPet()
	if kActorPet:IsNil() then
		return true
	end
	local kPetAction = kActorPet:GetAction()
	if kPetAction:IsNil() then
		return true
	end
	
	if kPetAction:GetID() == "a_riding_ground" then
		kPetAction:SetSlot(5)
		kActorPet:PlayCurrentSlot(false)
	end

	return true
end

function Act_RP_Walk_OnUpdate(actor, accumTime, frameTime)
	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	local kActorPet = actor:GetMountTargetPet()
	if kActorPet:IsNil() then
		return false
	end
	local kActionPet = kActorPet:GetAction()
	if kActionPet:IsNil() then
		return false
	end
	
	if kActorPet:CheckCanRidingAttack() == false and kActionPet:GetID() == "a_pet_melee_02" then
		return false
	end
	
	if kActionPet:GetID() == "a_riding_ground" and kActionPet:GetCurrentSlot() ~= 5 then --다른 PC에서 Slot이 0인 경우가 간헐적으로 발생했음(이해불가!!)
		kActionPet:SetSlot(5) --슬롯이 아이들(0)일 경우 무조건 달리기 모션(5)으로 바꿔주자.
		kActorPet:PlayCurrentSlot()
	end
	
	local movingSpeed = kActorPet:GetAbil(AT_C_MOVESPEED)
	local bMoveToPos = (action:GetParamInt(3) == 1);

	if movingSpeed == 0 then
		movingSpeed = 200
	end

	local fOriginalMoveSpeed = kActorPet:GetAbil(AT_MOVESPEED)
	if fOriginalMoveSpeed == 0 then
		fOriginalMoveSpeed = 150
	end
	
	local	fAnimSpeed = 0.0;
	
	--펫 이동속도에 따른 애니 속도 제어
	--현재 펫의 애니속도는 kfm 그대로 가므로 사용하지 않음
--	if fOriginalMoveSpeed>0 then
--		fAnimSpeed = movingSpeed/fOriginalMoveSpeed
--	end
--	kActorPet:SetAnimSpeed(fAnimSpeed) --탑승시에는 PC 대신 펫의 애니 속도를 조절해준다

	if actor:IsMyActor() and action:GetParamInt(1) == 0 then
	
		if accumTime - action:GetActionEnterTime() > 0.1 then

			actor:SetComboCount(0);
			action:SetParamInt(1,1);
		end
	
	end
	
	
	if bMoveToPos then
		
		local	kMoveTargetPos = action:GetParamAsPoint(0)
		local	kMoveDirection = action:GetParamAsPoint(1)	
		local	kMoveStartPos = action:GetParamAsPoint(2)
		
		local	kDir1 = actor:GetPos():_Subtract(kMoveTargetPos);
		kDir1:Unitize();
		local	kDir2 = kMoveStartPos:_Subtract(kMoveTargetPos);
		kDir2:Unitize();
		
		if kDir1:Dot(kDir2) < 0 then
		--if 0 > kDir1:Dot(kDir2) or 5 > actor:GetPos():Distance(kMoveTargetPos) then
			actor:SetTranslate(kMoveTargetPos);
			return	false;		
		end
		
		kMoveDirection:Multiply(movingSpeed);		
		actor:SetMovingDelta(kMoveDirection);
		return	true;
	end
	
	local dir = actor:GetDirection()

	if dir == DIR_NONE then
		if actor:GetWalkingToTarget() == false then
			ODS("[Act_Run_OnUpdate] Direction is None : transit Next Action\n")
			return false 
		end
	end
	
	if movingSpeed == 0 then
	    return  false
    end

	actor:Walk(dir, movingSpeed)

	local vel = actor:GetVelocity()
	local z = vel:GetZ()
	-- 뛰어가다가 발이 땅에서 떨어졌을 경우
	-- 올라가는 점프를 해야 할지, 내려오는 점프를 해야 할지 결정
	if actor:IsMeetFloor() == false then
		if z < -2 then
			action:SetNextActionName("a_rp_jump")
			action:SetParam(2, "fall_down")
			return false
		elseif z > 2 then
			action:SetNextActionName("a_rp_jump")
			action:SetParam(2, "fall_up")
			return false
		end
	end
	
	return true
end

function Act_RP_Walk_OnLeave(actor, action)	
	
	if action:GetID() == "a_rp_jump" then
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() then
			return false
		end
		local kActionPet = kActorPet:GetAction()
		if kActionPet:IsNil() then
			return false
		end
		
		if action:GetParam(2) == "fall_down" then
			kActionPet:SetSlot(9)
		elseif action:GetParam(2) == "fall_up" then
			kActionPet:SetSlot(8)
		elseif action:GetParam(2) == "null" then --달리기->이동 이면 준비 동작없이 바로 점프모션 시작
			if actor:IsMeetFloor() then
				kActionPet:SetSlot(8)
			else
				kActionPet:SetSlot(9)
			end
		end
	elseif action:GetID() ~= "a_idle" and
		action:GetID() ~= "a_walk" and
		action:GetID() ~= "a_walk_left" and
		action:GetID() ~= "a_walk_right" and
		action:GetID() ~= "a_walk_up" and
		action:GetID() ~= "a_walk_down" and
		action:GetID() ~= "a_run" and
		action:GetID() ~= "a_run_left" and
		action:GetID() ~= "a_run_right" and
		action:GetID() ~= "a_run_up" and
		action:GetID() ~= "a_run_down" and
		action:GetID() ~= "a_dmg" and
		action:GetID() ~= "a_jump" and
		action:GetID() ~= "a_trap" and
		action:GetID() ~= "a_rp_idle" and
		action:GetID() ~= "a_rp_walk" and
		action:GetID() ~= "a_telejump"
		and action:GetID() ~= "a_class_promotion_dragonian"
		and action:GetID() ~= "a_class_promotion_human"
	then
		return false
	end
	
	if nextActionName == "a_rp_jump" then
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() then
			return true
		end
		local kPetAction = kActorPet:GetAction()
		if kPetAction:IsNil() then
			return true
		end
		
		if kPetAction:GetID() ~= "a_riding_ground" then
			return false
		end
	end
	
	return true
end

function Act_RP_Walk_OnCleanUp(actor, action)
end

function Act_RP_Walk_OnClearUpRun(actor)
end
