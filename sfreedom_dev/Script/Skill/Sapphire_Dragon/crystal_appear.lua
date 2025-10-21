function Act_SP_Crystal_OnEnter(actor, action)
   ODS("Act_SP_Crystal_OnEnter\n")
	return true
end

function Act_SP_Crystal_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end
	action = actor:GetAction()		-- 지금 엑션 객체를 반환
	curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if curSlot == 0 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
      ODS("크리스탈 return false\n")
		return false
	end

	return true
end

function Act_SP_Crystal_OnCleanUp(actor, action)
end

function Act_SP_Crystal_OnLeave(actor, action)
   ODS("Act_SP_Crystal_OnLeave\n")
	return true
end
