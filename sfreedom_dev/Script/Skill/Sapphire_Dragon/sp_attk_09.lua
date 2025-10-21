bQuakedByHand = false
iQuakedByHandTime = 1.867
iGrapTime = 15			-- 보스가 손을 내리고 있는 시간

function Act_SP_Attk_09_OnEnter(actor, action)	
	action:SetParam(1,0)		-- 업데이트 루프로 처음 들어온 시간
	action:SetParam(0, 0)
	return true
end

function Act_SP_Attk_09_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end
	action = actor:GetAction()		-- 지금 엑션 객체를 반환
	curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if action:GetParamFloat(1) == 0 then
		action:SetParam(1, accumTime)
		bQuakedByHand = false
	elseif bQuakedByHand == false and (accumTime - action:GetParamFloat(1)) > iQuakedByHandTime then
		bQuakedByHand = true
		if iDownPuppetID11%6 ~= 0 then
			puppets[iDownPuppetID11]:AttachParticle(22344, "block_s", "e_b_dragonsapphire_hand_attk") 
		else
			puppets[iDownPuppetID11]:AttachParticle(22344, "block_b", "e_b_dragonsapphire_hand_attk") 
		end
	end

	if curSlot == 0 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
		action:SetParam(0, accumTime)			-- 처음 에니가 끝난 시간을 기록
		actor:PlayNext()
		curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번
	elseif curSlot == 1 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
		return false
	end

	return true
end

function Act_SP_Attk_09_OnCleanUp(actor, action)
end

function Act_SP_Attk_09_OnLeave(actor, action)
	if action:GetID() == "a_dmg" then
		return false
	end

	bQuakedByHand = false
	return true
end
