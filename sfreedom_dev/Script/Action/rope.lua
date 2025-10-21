-- Rope!
--

-- actor에 있는 action은 Prviuos Action이다.
-- action 인자는 현재 Rope를 나타낸다.
function Act_Rope_OnEnter(actor, action)
	local kCurAction = actor:GetAction()
	if( false == kCurAction:IsNil() ) then 
		local actionID = kCurAction:GetID()
		if actionID ~= "a_rope" then
			actor:FreeMove(true)
			actor:Concil(false)
			actor:SeeFront(true)
			return true
		end
	else 
		return true;
	end

	return false
end

-- Slot 0 : 점프 -> 매달리기를 시도하는 애니
-- Slot 1 : 매달려서 가만히 있는 Idle 애니
-- Slot 2 : 매달려 있다가 위로 올라가는 애니

function Act_Rope_OnUpdate(actor, accumTime, frameTime)
	action = actor:GetAction()
	curSlot = action:GetCurrentSlot()

	actor:SeeFront(true)

	right = actor:GetActionState("a_run_right")
	left = actor:GetActionState("a_run_left")

	if right > left then
		NextSlot = 1
		actor:Walk(false, actor:GetAbil(AT_C_MOVESPEED)/2.5)
	elseif right < left then
		NextSlot = 1
		actor:Walk(true, actor:GetAbil(AT_C_MOVESPEED)/2.5)	
	else
		NextSlot = 0
	end
	
	if curSlot ~= NextSlot then
		action:SetSlot(NextSlot)
		actor:PlayCurrentSlot()
	end

	if actor:HangOnRope() == false then
		action:SetNextActionName("a_idle")
		action:SetParam(20,"Done")
		return false
	end

	return true
end
function Act_Rope_OnCleanUp(actor, action)
end

-- action 인자는 다음으로 전이될 Next Action이다.
function Act_Rope_OnLeave(actor, action)
	curAction = actor:GetAction()
	actionID = action:GetID()

	if actionID == "a_jump" then
		actor:FreeMove(false)
		actor:Concil(true)
		action:SetSlot(1)
		action:SetParam(1, "jump_again")
		action:SetParam(10, "just_hang")
		return true
	elseif actionID == "a_idle" then
		actor:FreeMove(false)
		actor:Concil(true)
		return true
	elseif curAction:GetParam(20) == "Done" and
		(actionID == "a_run_right" or
		actionID == "a_run_left") then
		actor:Concil(true)
		return true
	end

	return false 
end

