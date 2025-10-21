-- Up 

function Skill_Bruce_OnEnter(actor, action)
	actor:Stop()
	
	loc = actor:GetTranslate()

	actor:SetHide(true)
	guid = GUID("4071530A-C710-4c1d-B23B-DEB02C43189F")
	guid:Generate()
	pilot = g_pilotMan:NewPilot(guid, "Pilot/bruce.xml")
	bruceActor = pilot:GetActor()
	bruceActor:SetOriginalActor(actor);
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:AddActor(guid, bruceActor, loc, 2)
	pilot:GetActor():ReserveTransitAction("a_transform")
	bruceActor:ToLeft(actor:IsToLeft())
	bruceActor:FindPathNormal()
	bruceActor:AttachParticle(100, "Bip01", "e_special_transform")
	bruceActor:AttachParticle(101, "Bip01", "e_special_transform_tex")

	actor:SetMagicFireActionID("");
	
	return true
end

function Skill_Bruce_OnUpdate(actor, accumTime, frameTime)
	action = actor:GetAction()
	param = action:GetParam(0)
	if param == "null" then
		action:SetParam(0, accumTime)
	elseif accumTime - param > 3.2 then
		action:SetParam(1, "done")
		return false
	end

	return true
end

function Skill_Bruce_OnLeave(actor, action)

	if actor:IsMyActor() == false then
		actor:SetHide(false);
		return true;
	end
	actionID = action:GetID()

	if actor:GetAction():GetParam(1) == "done" then
		if actionID == "a_idle" or
			actionID == "a_die" or 
			actionID == "a_run_right" or
			actionID == "a_run_left" then
			actor:SetHide(false)
			return true 
		end
	else
		if actionID == "a_die" then
			actor:SetHide(false)
		return	true;
		end
		
	end
	return false
end
