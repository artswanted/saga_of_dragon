g_mmst = -1;	--	Monster Move Speed For Test 테스트용 이동속도 -1일 경우 이 값이 적용되지 않는다. -1 이외의 값일 경우 적용된다.

function Act_Fly_OnEnter(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local prevAction = actor:GetAction()
	if prevAction:IsNil() == false then
		prevActionID = prevAction:GetID()
		if prevActionID == "a_fly" then
			action:SetDoNotBroadCast(true)
			return false
		end
		-- 그 외에는 전이 가능
	end

	actor:FindPathNormal()
	--ODS("a_fly start"..g_world:GetAccumTime().."\n");
	return true
end

function Act_Fly_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local actorID = actor:GetID()
	local movingSpeed = actor:GetAbil(AT_C_MOVESPEED)
	
	if string.sub(actorID, 1, 2) == "c_" then
		movingSpeed = movingSpeed * 0.6
	else
		if g_mmst ~= -1 then
			movingSpeed = g_mmst;
		end
	end
	
	if IsSingleMode() == true then
		movingSpeed = 120 * 0.6
	end

	local dir = actor:GetDirection() 
	actor:Walk(dir, movingSpeed)

	local vel = actor:GetVelocity()
	local z = vel:GetZ()

	return true
end
function Act_Fly_OnCleanUp(actor, action)
end

function Act_Fly_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local curAction = actor:GetAction()

	CheckNil(nil==action)
	CheckNil(action:IsNil())
	ODS("Fly's NextAction : " .. action:GetID() .. "\n")
	if curAction:IsNil() == false and 
		action:GetID() == "a_jump" then
		local param = curAction:GetParam(2)
		if param == "fall_down" then
			action:SetSlot(2)
		elseif param == "fall_up" or
			param == "null" then
			action:SetSlot(1)
		end
	end

	return true
end

function Act_Fly_OnEvent(actor, textKey)
	--if textKey == "start" then
	--	actor:AttachParticle(math.random(-100, -200), "char_root", "e_run")
	--end	
end
