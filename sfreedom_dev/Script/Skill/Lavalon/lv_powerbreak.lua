function Act_LAVALON_Powerbreak_OnEnter(actor, action)
	local kPacket = action:GetParamAsPacket()
	if kPacket:IsNil() then
		return true
	end


	--     	---------------------
	--     	|					|
	--	0	|					|    1
	--		|					|
	--		|					|
	--		|					|
	--     	---------------------
	--				2
	--

	-- 부술 땅 번호.`
	local iIndex = kPacket:PopInt()
	action:SetParamInt(5, iIndex)
	action:DeleteParamPacket()
	return true
end

function Act_LAVALON_Powerbreak_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end
	
	actor:SeeFront(true)
	
	local action = actor:GetAction()		-- 지금 엑션 객체를 반환
	local curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if actor:IsAnimationDone() then
		if curSlot == 0 then
			actor:PlayNext()
		elseif curSlot == 2 then
			return false
		end
	end

	return true
end
function Act_LAVALON_Powerbreak_OnCleanUp(actor, action)
	return true
end

function Act_LAVALON_Powerbreak_OnLeave(actor, action)
	return true
end

function Act_LAVALON_Powerbreak_OnCastingCompleted(actor, action)
	ODS("__________________Powerbreak Casting completed____________________\n")
	action:SetSlot(2)
	actor:PlayCurrentSlot()
	return true
end

function Act_LAVALON_Powerbreak_OnEvent(actor, textKey, seqID)

	if textKey == "hit" then
		local action = actor:GetAction()
		local iIndex = action:GetParamInt(5)

		--if LavPuppets[iIndex] ~= nil then
		--	LavPuppets[iIndex]:TransitAction("dmg")
		--	g_world:SetActivePhysXGroup(1, iIndex + 6, false)
		--	-- 데미지는 서버에서 계산해서 준다..
			local kTargetList = action:GetTargetList()
			kTargetList:ApplyActionEffects()
		--end
		QuakeCamera(1,1,1,1,1)
	end

	return	true;
end
