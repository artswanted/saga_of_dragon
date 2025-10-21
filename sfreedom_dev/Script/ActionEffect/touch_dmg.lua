-- damage

function Act_TouchDamage_OnEnter(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())

	actor:Stop()
	actor:BackMoving(true)

	CheckNil(nil==action)
	CheckNil(action:IsNil())
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local kDir = nil
	local ptCaster = action:GetParamAsPoint(20110630)
	if false==ptCaster:IsZero() then
		local kMovingDir = actor:GetPos()
		kMovingDir:Subtract( ptCaster )
		kMovingDir:Unitize()
		action:SetParamAsPoint(20110631, kMovingDir)
		
		if kMovingDir:GetX()>0 then
			kDir = DIR_RIGHT
		else
			kDir = DIR_LEFT
		end
	else
		if actor:IsToLeft() then
			kDir = DIR_RIGHT
		else
			kDir = DIR_LEFT
		end
	end

	action:SetSlot(math.random(0, 1));
	action:SetParamFloat(0,g_world:GetAccumTime());
	actor:SetCanHit(false);
	action:SetParam(1209, kDir)
	if actor:IsMyActor() then
		actor:SetSendBlowStatus(false, true, true);
	end
	return true
end

function Act_TouchDamage_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	
	if accumTime -  action:GetParamFloat(0) > g_fTouchDmgTotalTime then
		action:SetParam(1, "Done")
		return false
	end

	actor:Walk(action:GetParam(1209), g_fTouchDmgSpeed,frameTime)

	return true
end

function Act_TouchDamage_OnCleanUp(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	actor:SetCanHit(true);
	if actor:IsMyActor() then
		actor:SetSendBlowStatus(false, false, true);
	end
end


function Act_TouchDamage_OnLeave(actor, action)

	local actionID = action:GetID()

	if actor:IsMyActor() == false or 
		actionID == "a_die" or 
		actionID == "a_dmg" or
		actionID == "a_touch_dmg" or
		actor:GetAction():GetParam(1) == "Done" then

		actor:SetMovingDir(Point3(0,0,0))
		actor:BackMoving(false)
		if nil==g_fTouchDmgGodTime then
			g_fTouchDmgGodTime = 0.5
		end
		actor:StartGodTime(g_fTouchDmgGodTime);

		return true
	end
		
	return false
end
