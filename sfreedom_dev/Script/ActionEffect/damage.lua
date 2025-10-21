-- damage

function Act_Damage_OnEnter(actor, action)
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

	
	actor:Stop()
	actor:ResetAnimation();
	action:SetParam(0,"");
	local dmgAni = 0
	-- 왼쪽을 바라보고 있을 때는 0, 오른쪽을 바라보고 있을 때는 1
	if actor:IsToLeft() == true then
		dmgAni = 0
	else
		dmgAni = 1
	end
	
	if dmgAni >= action:GetSlotCount() then
		dmgAni = action:GetSlotCount()-1
	end

	action:SetSlot(dmgAni);	
	
	local	animname = action:GetSlotAnimName(dmgAni);
	
	if actor:GetAnimationLength(animname) == 0 then
		action:SetSlot(0);	
	end
	
	
	return true
end
function Act_Damage_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	fElapsedTime = accumTime - action:GetActionEnterTime();
	
	if actor:IsAnimationDone() == true then
		action:SetParam(0,"end");
		return false
	end
	
	return true
end
function Act_Damage_OnCleanUp(actor, action)
--	ODS("Act_Damage_OnCleanUp : "..action:GetID().." Cur : "..actor:GetAction():GetID().."\n", false, 1509)
	return true
end

function Act_Damage_OnLeave(actor, action)

	return true
end

function Act_Damage_OnTimer(actor,fAccumTime,action,iTimerID)

	return true
end
