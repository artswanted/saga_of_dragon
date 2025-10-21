function Act_LAVALON_Dmg_OnEnter(actor, action)
	actor:DetachFrom(12985)	-- 브레스 이펙트 붙어 있으면 브레스 이펙트 떼주자.
	return true
end

function Act_LAVALON_Dmg_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end
	
	actor:SeeFront(true)
	
	local action = actor:GetAction()		-- 지금 엑션 객체를 반환
	local curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if actor:IsAnimationDone() then
		if curSlot >= action:GetSlotCount() - 1 then
			return false
		else
			actor:PlayNext()
		end
	end

	return true
end
function Act_LAVALON_Dmg_OnCleanUp(actor, action)
	return true
end

function Act_LAVALON_Dmg_OnLeave(actor, action)
	return true
end

function Act_LAVALON_Dmg_OnEvent(actor, textKey, seqID)

	if textKey == "hit" then

	end

	return	true;
end
