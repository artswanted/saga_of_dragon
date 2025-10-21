-- Hang!
--

-- actor에 있는 action은 Prviuos Action이다.
-- action 인자는 현재 Hang를 나타낸다.
function Act_Hang_OnEnter(actor, action)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	local kPrevAction = actor:GetAction()
	if( false == kPrevAction:IsNil() ) then
		actionID = kPrevAction:GetID()
		if actionID ~= "a_hang" and 
			actor:IsMeetFloor() == false then
			actor:FreeMove(true)
			actor:StopJump()
			return true
		end
	end
	
	return false
end

-- Slot 0 : 점프 -> 매달리기를 시도하는 애니
-- Slot 1 : 매달려서 가만히 있는 Idle 애니
-- Slot 2 : 매달려 있다가 위로 올라가는 애니

function Act_Hang_OnUpdate(actor, accumTime, frameTime)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	action = actor:GetAction()
	CheckNil(action==nil)
	CheckNil(action:IsNil())
	curSlot = action:GetCurrentSlot()

	--hang = actor:GetActionState("a_hang")
	right = actor:GetActionState("a_run_right")
	left = actor:GetActionState("a_run_left")

	if curSlot == 0 then	
		--if hang == 0 then		-- 쉬프트 키가 떼어졌으면 메달리기 중단
		--	action:SetNextActionName("a_jump")
		--	return false
		if actor:IsAnimationDone() == true then
			actor:PlayNext()
			return true
		end
	elseif curSlot == 1 then
		if actor:GetActionState("a_run_down") ~= 0 then 	-- 밑으로를 누르면
			action:SetNextActionName("a_jump")			-- 그냥 떨어진다.
			return false
		elseif actor:GetActionState("a_run_up") ~= 0 then	-- 위(↑)로 눌렀으면
			actor:PlayNext()							-- 올라가는 애니 플레이 
			action:SetParam(100, tostring(accumTime))
		end
	elseif curSlot == 2 then
		if actor:IsAnimationDone() == true or
			actor:IsMeetFloor() == true then
			ODS("next Action = " .. action:GetNextActionName() .. "\n")
			action:SetNextActionName("a_jump")
			return false
		else
			lastAccumTime = action:GetParam(100)
			if lastAccumTime ~= "null" then
				lastAccumTime = tonumber(lastAccumTime)
				timeDiff = accumTime - lastAccumTime
				ODS("time diff = " .. timeDiff .. "\n")
				if timeDiff > 0.2 then
					if action:GetParam(101) ~= "Jumping" then
						actor:FreeMove(false)
						actor:StartJump(150)
						action:SetParam(101, "Jumping")
					end
					if right ~= left then
						actor:Walk(right < left, actor:GetAbil(AT_C_MOVESPEED))
					end
				end
				return true
			end
		end
	end

	ODS("OTL :" .. curSlot .. "\n")
	return true
end
function Act_Hang_OnCleanUp(actor, action)
end

-- action 인자는 다음으로 전이될 Next Action이다.
function Act_Hang_OnLeave(actor, action)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	curAction = actor:GetAction()
	CheckNil(curAction==nil)
	CheckNil(curAction:IsNil())
	actionID = action:GetID()

	if actionID == "a_jump" then
		actor:FreeMove(false)
		action:SetSlot(2)
		action:SetParam(1, "jump_again")
		action:SetParam(10, "just_hang")
		return true
	elseif actionID == "a_idle" and
		(actor:IsAnimationDone() == true or 
		actor:IsMeetFloor() == true) then
		--actor:FreeMove(false)
		return true
	elseif (actionID == "a_run_right" or
		actionID == "a_run_left") and
		actor:IsMeetFloor() == true then
		return true
	end

	return false 
end

