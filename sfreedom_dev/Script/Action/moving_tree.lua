-- Moving Tree
function Act_Moving_Tree_OnEnter(actor, action)
	return true
end
function Act_Moving_Tree_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	local curAnimSlot = action:GetCurrentSlot() -- Current Animation Slot
	local IsAnimDone = actor:IsAnimationDone()
	
	if IsAnimDone == true then
		actor:PlayNext()
	end
	--if actor:IsAnimationDone() == true then
		--return false
	--end
	return true
end
function Act_Moving_Tree_OnCleanUp(actor, action)
	return true
end
function Act_Moving_Tree_OnLeave(actor, action)
	return true
end
