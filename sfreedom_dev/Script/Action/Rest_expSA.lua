------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 12[float] : 중간 시간

function Act_Rest_ExpSA_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	action:SetParamInt(10,0);
	action:SetParamFloat(11,g_world:GetAccumTime());
	
	return true
end

function Act_Rest_ExpSA_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	-- local animDone = actor:IsAnimationDone()
	-- local iSlotNum = action:GetCurrentSlot();
	-- local iState = action:GetParamInt(10);
	
	-- if iState == 0 then
		-- if iSlotNum == 0 then
			-- if animDone == true then
				-- action:SetSlot(iSlotNum + 1);
				-- actor:PlayCurrentSlot(true);
				-- action:SetParamFloat(12,accumTime);
			-- end
		-- elseif iSlotNum == 1 then
			-- local fElapsedTime = accumTime - action:GetParamFloat(12);
			-- if fElapsedTime >= 3.0 then
				-- -- action:SetSlot(iSlotNum + 1);
				-- -- actor:PlayCurrentSlot(true);
			-- end
		-- elseif iSlotNum == 2 then
			-- if animDone == true then
				-- action:SetParamInt(10,1);
			-- end
		-- end
	-- else
		-- return false;
	-- end

	return true;
end

function Act_Rest_ExpSA_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Act_Rest_ExpSA_OnLeave(actor, action)
	return true;
end

function Act_Rest_ExpSA_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
end

function Act_Rest_ExpSA_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	return	true;
end

function Act_Rest_ExpSA_OnTargetListModified(actor,action,bIsBefore)
end

function Act_Rest_ExpSA_OnCastingCompleted(actor,action,bIsBefore)
end
