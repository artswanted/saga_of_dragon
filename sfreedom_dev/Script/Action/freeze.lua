-- freeze 

function Act_Freeze_OnEnter(actor, action)
	if actor:IsRidingPet() then --라이딩펫: 탑승 중 피격당하면 강제 하차→피격 액션 진행
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end

	actor:Stop()
	return true
end

function Act_Freeze_OnUpdate(actor, accumTime, frameTime)
	return true
end
function Act_Freeze_OnCleanUp(actor, action)
end

function Act_Freeze_OnLeave(actor, action)
	-- 이러는게 좋은지는 모르겠다. Skill/SwiftShot.lua의 OnLeave부분을 볼 것
	if action:GetID() == "a_run_left" or action:GetID() == "a_run_right" or
		action:GetID() == "a_idle" then
		return false
	end
	
	return true
end
