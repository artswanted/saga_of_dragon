-- spawn
function Block_OnCheckCanEnter(actor, action)
	return SkillFunc_OnCheckCanEnter(actor, action)
end

function Block_OnEnter(actor, action)
	if( actor:IsNil() ) then return false end
	if( action:IsNil() ) then return false end
	
	if SkillFunc_OnEnter(actor, action)  then
		if action:GetActionParam() ~= AP_CASTING then
			Block_OnCastingCompleted(actor, action);
		end
		
		actor:SetMovingDelta(Point3(0,0,0));
		actor:Stop()
		actor:SetNoWalkingTarget(false);
		
		local fDistance = actor:GetAbil(AT_PUSH_DIST_WHEN_BLOCK)
		--if 50 < fDistance then fDistance = 50 end	-- 최대값 50 이보다 크면, 동기가 안맞음	
		local fAccel = actor:GetAbil(AT_PUSH_ACCEL_WHEN_BLOCK)
		local fSpeed = actor:GetAbil(AT_PUSH_SPEED_WHEN_BLOCK)
		
		local kAttackActor = action:GetEffectCaster()
		if true==kAttackActor:IsNil() then
			actor:PushActor(not actor:IsToLeft(),fDistance,fSpeed,fAccel);
		else
			local kMovingDir = actor:GetPos()
			kMovingDir:Subtract( kAttackActor:GetPos() )
			kMovingDir:Unitize()
			-- 방향 설정
			local iDir = actor:GetDirFromMovingVector8Way(kMovingDir)
			actor:SetDirection(iDir)
			actor:BackMoving(true)	-- 바라볼 방향은 이동방향 반대쪽
			actor:ConcilDirection(kMovingDir, true);				
			actor:SetMovingDir(kMovingDir);
			-- 밀기 
			actor:PushActorDir(kMovingDir, fDistance, fSpeed, fAccel);
			--actor:SetSendBlowStatus(false, true);
		end
		
		return true
	end
	return false
end

function Block_OnCastingCompleted(actor, action)
	SkillFunc_OnCastingCompleted(actor, action)
	return true
end

function Block_OnUpdate(actor, accumTime, frameTime)
	if( actor:IsNil() ) then return false end
	
	if actor:IsAnimationDone() and actor:GetNowPush() == false then		
		actor:SetSendBlowStatus(false, true, false);
	end	
	return SkillFunc_OnUpdate(actor, actor:GetAction(), accumTime, frameTime)	
end
function Block_OnCleanUp(actor, action)	
	if( actor:IsNil() ) then return false end
	if( action:IsNil() ) then return false end
	
	actor:BackMoving(false);
	actor:SetMovingDelta(Point3(0,0,0));
	actor:SetSendBlowStatus(false, false);
	
	actor:SetDirection(0);
	local kDir = actor:GetLookingDir();
	actor:ConcilDirection(kDir, true);
	actor:SetMovingDir(kDir);
	
	return SkillFunc_OnCleanUp(actor, action)
end

function Block_OnTargetListModified(actor, action, bIsBefore)
	return SkillFunc_OnTargetListModified(actor, action, bIsBefore);
end

function Block_OnLeave(actor, action)
	return SkillFunc_OnLeave(actor, action);
end

function Block_OnEvent(actor,textKey)
	return SkillFunc_OnEvent(actor,textKey);
end

function Block_OnFindTarget(actor,action,kTargets)	
	return 0;
end