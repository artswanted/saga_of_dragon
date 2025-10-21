iPreQuakeTime = 1.3	-- 슬슬 떨리기 시작 할 시간
bPreQuake = false		

function Act_SP_Attk_06_OnEnter(actor, action)
	bPreQuake = false		
	action:SetParam(0,0)
	return true
end

function Act_SP_Attk_06_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end
	Nowaction = actor:GetAction()		-- 지금 액션 객체를 반환
	curSlot = Nowaction:GetCurrentSlot()	-- 액션이 플레이하고 있는 애니 순번

	if Nowaction:GetParamFloat(0) == 0 then
		Nowaction:SetParam(0, accumTime)
	elseif accumTime - Nowaction:GetParamFloat(0) > iPreQuakeTime and bPreQuake == false then
		QuakeCamera(2.5, 1.0)
		bPreQuake = true
	elseif curSlot == 0 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
		return false					-- 애니가 하나밖에 없음
	end
	return true
end

function Act_SP_Attk_06_OnCleanUp(actor, action)
end

function Act_SP_Attk_06_OnLeave(actor, action)
	if action:GetID() == "a_dmg" then
		return false
	end
	
	return true
end
