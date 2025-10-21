
function PetMeleeRiding_OnCheckCanEnter(actor,action)
	if nil==actor or actor:IsNil() then
		return false
	end
	if nil==action or action:IsNil() then
		return false
	end
	
	--탑승시에만 사용 가능
	local kCurAction = actor:GetAction()
	if nil == kCurAction or kCurAction:IsNil() then
		return false
	end
	if kCurAction:GetID() == "a_pet_melee_02" and actor:CheckCanRidingAttack() == false then
		return false
	end
	if kCurAction:GetID() ~= "a_riding_ground" and kCurAction:GetID() ~= "a_pet_melee_01" then
		AddWarnDataTT(451005)
		return false
	end

	if actor:IsMeetFloor() == false then
		return false
	end
	
	return true
end

function PetMeleeRiding_OnEnter(actor, action)
	if nil==actor or actor:IsNil() then
		return false
	end
	if nil==action or action:IsNil() then
		return false
	end
	
	if actor:CheckCanRidingAttack() then
		action:CreateActionTargetList(actor);
	end
	
	local actorID = actor:GetID()
	local actionID = action:GetID()

	actor:ResetAnimation();
	action:SetSlot(0)
	actor:PlayCurrentSlot(true)
	
	return true
end

function PetMeleeRiding_OnFindTarget(actor,action,kTargets)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:CheckCanRidingAttack() == false then
		return 0
	end

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 30
	end
	
	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor),iParam2,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
end

function PetMeleeRiding_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local	action = actor:GetAction()
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local animDone = actor:IsAnimationDone()
	
	if animDone == true then
		return false
		--return	MagicianShot_Finished(actor,action);
	end

	if actor:CheckCanRidingAttack() == true then
		local fMovingSpeed = 0;
		if IsSingleMode() == true then
			fMovingSpeed = 120
		else
			fMovingSpeed = actor:GetAbil(AT_C_MOVESPEED)
		end	

		local dir = actor:GetDirection()
		if dir ~= DIR_NONE then
			actor:Walk(dir, fMovingSpeed)
		end
	end

	return true
end

function PetMeleeRiding_OnEvent(actor, textKey)
end

function PetMeleeRiding_OnLeave(actor, action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local nextActionName = action:GetID()
	if nextActionName ~= "a_riding_ground" then
		return false
	end
--	if action:GetID() == "a_pet_melee_01" then
--	end
	
	return true
end

function PetMeleeRiding_OnCleanUp(actor)
	return true
end

