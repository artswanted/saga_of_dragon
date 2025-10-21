-- Up 

function Skill_Bumpercar_OnEnter(actor, action)
	actor:Stop()
	
	if actor:IsMyActor() == true then
		if actor:GetAbil(2)<action:GetAbil(2009) then		-- 마나 포인트 부족
			AddWarnDataStr(GetTextW(35),0);
			return	false;
		end
	end
	
	loc = actor:GetTranslate()

	actor:SetHide(true)
	guid = GUID("4071530A-C710-4c1d-B23B-DEB02C43189F")
	guid:Generate()
	pilot = g_pilotMan:NewPilot(guid, "Pilot/Bumpercar.xml", 1)
	bruceActor = pilot:GetActor()
	bruceActor:SetOriginalActor(actor);
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:AddActor(guid, bruceActor, loc, 2)
	pilot:GetActor():ReserveTransitAction("a_transform")
	bruceActor:ToLeft(actor:IsToLeft())
	bruceActor:FindPathNormal()
	bruceActor:AttachParticle(100, "char_root", "e_bumpercar_fog")


	if actor:IsMyActor() == true then 
		bruceActor:SetCameraFocus();
	end
		

	actor:SetMagicFireActionID("");
	
	return true
end

function Skill_Bumpercar_OnUpdate(actor, accumTime, frameTime)
	action = actor:GetAction()
	param = action:GetParam(0)
	if param == "null" then
		action:SetParam(0, accumTime)
	elseif accumTime - param > 1.2 then
		action:SetParam(1, "done")
		return false
	end

	return true
end

function Skill_Bumpercar_OnLeave(actor, action)

	if actor:IsMyActor() == false then
		actor:SetHide(false);
		return true;
	end

	if actor:GetAction():GetParam(1) == "done" then
		actionID = action:GetID()
		if actionID == "a_idle" or
			actionID == "a_die" or 
			actionID == "a_run_right" or
			actionID == "a_run_left" then
			actor:SetHide(false)
			actor:SetCameraFocus();
			return true 
		end
	end
	return false
end
