bRelease = false

function Act_SP_Attk_03_OnEnter(actor, action)
	action = actor:GetAction()
	action:SetParam(0, 0)
	action:SetParam(1, 0)
	bBite = false
	bRelease = false	-- 놓아주기
	return true
end

function Act_SP_Attk_03_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end
	action = actor:GetAction()		-- 지금 엑션 객체를 반환
	curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if action:GetParamFloat(1) == 0 then
		action:SetParam(1, accumTime)
	end

	if curSlot == 0 and actor:IsAnimationDone() then
		action:SetParam(1,accumTime)		-- 업데이트 루프로 처음 들어온 시간
		actor:PlayNext()		
	elseif curSlot == 2 and actor:IsAnimationDone() then	
		return false
	end

	if action:GetParamFloat(1) == 0 then
		action:SetParam(1, accumTime)
	elseif (accumTime - action:GetParamFloat(1)) > iBiteLongTime/1000 and bRelease == false then	-- 물고 있는 시간 이상이면
		actor:PlayNext()	
		bRelease = true		
		if iDownPuppetID4 ~= -1 then
			puppets[iDownPuppetID4]:TransitAction("up")	-- 시간 다 되면 up
			iDownPuppetID4 = -1
		end			
	end
	
	return true
end

function Act_SP_Attk_03_OnCleanUp(actor, action)
end

function Act_SP_Attk_03_OnLeave(actor, action)
	if action:GetID() == "a_dmg" then
		return false
	end
	
	return true
end
