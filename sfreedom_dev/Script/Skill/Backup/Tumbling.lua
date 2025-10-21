function Skill_Tumbling_OnEnter(actor, action)
	actor:StartJump(170)
   return true
end

function Skill_Tumbling_OnUpdate(actor, accumTime, frameTime)
   local action = actor:GetAction()
   local slot = action:GetCurrentSlot()

   if slot == 0 or slot == 1 then
      if actor:IsToLeft() then
         dir = 1
      else
         dir = 2
      end
      actor:Walk(dir, 100)
   end

	if slot == 0 then
      if actor:IsAnimationDone() == true then
         actor:PlayNext()
      end

	else
		if actor:IsAnimationDone() == true then
			actor:Stop()
			actor:ToLeft(not actor:IsToLeft())
			actor:FindPathNormal()
			return false
		end
	end

	return true
end

function Skill_Tumbling_OnCleanUp(actor, action)
end

function Skill_Tumbling_OnLeave(actor, action)
   return true
end
