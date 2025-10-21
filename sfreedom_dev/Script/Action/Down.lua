-- Down

function Act_Down_OnEnter(actor, action)
	if actor:IsRidingPet() then --라이딩펫: 탑승 중 피격당하면 강제 하차→피격 액션 진행
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end

	local prevAction = actor:GetAction()
	if prevAction:IsNil() == false then
		prevActionID = prevAction:GetID()
		if prevActionID == "a_jump" then
			return false
		elseif prevActionID == "a_ladder_up" or
			prevActionID == "a_ladder_idle" then
			actor:ReserveTransitAction("a_ladder_down")
			return false
		elseif prevActionID == "a_run_left" or
			prevActionID == "a_run_right" then
			return false
		end
	end

	return false
--	actor:SeeFront(true)
--	return true
end

function Act_Down_OnUpdate(actor, accumTime, frameTime)
	return false 
end
function Act_Down_OnCleanUp(actor, action)
end

function Act_Down_OnLeave(actor, action)
	return true
end
