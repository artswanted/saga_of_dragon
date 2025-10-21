function Act_SP_Attk_13_OnEnter(actor, action)
	return true
end

function Act_SP_Attk_13_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end
	action = actor:GetAction()		-- 지금 엑션 객체를 반환
	curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if curSlot == 0 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
		return false
	end

	return true
end

function Act_SP_Attk_13_OnCleanUp(actor, action)
end

function Act_SP_Attk_13_OnLeave(actor, action)
	return true
end

