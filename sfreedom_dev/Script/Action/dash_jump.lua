
function Act_Dash_Jump_OnEnter(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local prevAction = actor:GetAction()
	
	if( false == prevAction:IsNil() ) then
		local actionName = prevAction:GetID()
		if actor:IsMyActor() == false then
			action:SetSlot(0)
		elseif actionName ~= "a_dash" and 
			actionName ~= "a_ass_attk_dash" 
			then
			return false
		end
		action:SetParamInt(911, prevAction:GetDirection())
	end

	actor:StartJump(jumpForce)
	actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0)
	
--	if actor:IsMyActor() == false then
--		actor:SetTranslate(action:GetActionStartPos());
--	end

	
	actor:UseSkipUpdateWhenNotVisible(false);

	if actor:IsMyActor() then
		
        GetComboAdvisor():OnNewActionEnter(action:GetID());	
    
    end
    	
	return true
end

function Act_Dash_Jump_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local action = actor:GetAction()
	local curAnimSlot = action:GetCurrentSlot() -- Current Animation Slot
	local movingSpeed = g_fDashJumpSpeed

	local IsAnimDone = actor:IsAnimationDone()
	local dir = action:GetParamInt(911)
	
	if actor:GetPilot():IsNil() == false then
	
		ODS("Act_Dash_Jump_OnUpdate actor:"..actor:GetPilot():GetName():GetStr().." dir : "..dir.."\n");

	end
	if curAnimSlot ~= 2 then
		actor:Walk(dir, movingSpeed)
	end

	if curAnimSlot == 0 then
		if IsAnimDone == true then
			actor:PlayNext()
		end
	elseif curAnimSlot == 1 then
		if actor:IsMeetFloor() then
			dir = actor:GetDirection()
			if dir == DIR_NONE then
				actor:PlayNext()
			else
				action:SetNextActionName("a_run")
				action:SetParam(4808, "end")
				return false
			end
		end
	elseif curAnimSlot == 2 then
		if IsAnimDone == true then
			action:SetNextActionName("a_idle")
			action:SetParam(4808, "end")
			return false
		elseif actor:GetDirection() ~= DIR_NONE then
			action:SetNextActionName("a_run")
			action:SetParam(4808, "end")
			return false
		end
	end

	return true
end
function Act_Dash_Jump_OnCleanUp(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	actor:UseSkipUpdateWhenNotVisible(true);
	actor:EndBodyTrail();
end

function Act_Dash_Jump_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local kCurAction = actor:GetAction()
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	local newActionID = action:GetID()
	local iCurrnetSlot = kCurAction:GetCurrentSlot()

	ODS("__________Dash_Jump's NextAction : " .. newActionID .. "\n")
	
	if actor:IsMyActor() == false or 
		action:GetActionType() == "EFFECT" then
		return true
	end
	
	if kCurAction:GetParam(4808) == "end" and
		(iCurrnetSlot == 1 or
		iCurrnetSlot == 2) then
		if newActionID == "a_run" or
			newActionID == "a_idle" then
			return true
		end
	elseif newActionID == "a_telejump" or
		newActionID == "a_lock_move" or
		newActionID == "a_trap" or 
		newActionID == "a_teleport"
		then
		return true
	elseif newActionID == "a_jump" then
		if action:GetParam(5) == "HiJump" then -- hiJump면 그냥 jump로 넘어감.
			kCurAction:SetParam(4808, "EndNow")
			return true
		end	

	end
	
	return false
end
