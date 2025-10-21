function Act_LAVALON_Die_OnEnter(actor, action)
	CallUI("FRM_MOVIE_IN")
	CloseUI("FRM_BAR_BOSS")
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraModeMovie(2, 1000002, "nop()")
	LockPlayerInputMove(6) -- for lavalon die lock
	LockPlayerInput(6)
	return true
end

function Act_LAVALON_Die_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		SetBreak()
		return false
	end
	
	actor:SeeFront(true)
	
	local action = actor:GetAction()		-- 지금 엑션 객체를 반환
	local kEnd = action:GetParam(219)
	local curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if curSlot == 0 and actor:IsAnimationDone() then
		if kEnd == "null" then	-- 처음 에니이고 끝났다면
			--actor:SetTargetAlpha(actor:GetAlpha(),0.0, 2.0)
			action:SetParam(219, 1)
			return true 
		elseif actor:IsAlphaTransitDone() and
			kEnd == "1" then
			CallUI("FRM_MOVIE_OUT")
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:SetCameraMode(8, GetPlayer())
			action:SetParam(219, 2)
		end
	end

	return true
end
function Act_LAVALON_Die_OnCleanUp(actor, action)

	UnLockPlayerInputMove(6) -- for lavalon die lock
	UnLockPlayerInput(6)
	
	return true
end
function Act_LAVALON_Die_OnLeave(actor, action)

	return true
end

function Act_LAVALON_Die_OnEvent(actor, textKey, seqID)

	if textKey == "quake" then
		QuakeCamera(2,1,1,1,1)
	end

	return	true;
end
