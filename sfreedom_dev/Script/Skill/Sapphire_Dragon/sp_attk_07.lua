bQuakedByHand = false

function Act_SP_Attk_07_OnEnter(actor, action)
	action:SetParam(1,0)		-- 업데이트 루프로 처음 들어온 시간
	action:SetParam(0, 0)
	bQuakedByHand = false
	return true
end

function Act_SP_Attk_07_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end

	action = actor:GetAction()		-- 지금 엑션 객체를 반환
	curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번
	if action:GetParamFloat(1) == 0 then
		action:SetParam(1, accumTime)
		bQuakedByHand = false
	elseif bQuakedByHand == false and (accumTime - action:GetParamFloat(1)) > 0.4 then
		bQuakedByHand = true
		if iDownPuppetID8%6 ~= 0 then
			puppets[iDownPuppetID8]:AttachParticle(22344, "block_s", "e_b_dragonsapphire_hand_attk") 
		else
			puppets[iDownPuppetID8]:AttachParticle(22344, "block_b", "e_b_dragonsapphire_hand_attk") 
		end
	end

	if curSlot == 0 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
		action:SetParam(0, accumTime)			-- 처음 에니가 끝난 시간을 기록
		actor:PlayNext()
		curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번
	elseif curSlot == 1 and actor:IsAnimationDone() then	-- Idle 애니가 끝나면
		return false
	end

	return true
end

function Act_SP_Attk_07_OnCleanUp(actor, action)
end

function Act_SP_Attk_07_OnLeave(actor, action)
	if action:GetID() == "a_dmg" then
		return false
	end
	
	bQuakedByHand = false
	actor:SetAnimSpeed(1)
	return true
end
