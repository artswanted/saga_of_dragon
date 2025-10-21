-- sexygirl 

function Skill_SexyGirl_OnEnter(actor, action)
	actor:Stop()
	if actor:IsMyActor() == true then
		if actor:GetAbil(2)<action:GetAbil(2009) then		-- 마나 포인트 부족
			AddWarnDataStr(GetTextW(35),0);
			return	false;
		end
	end

	actor:SetHide(true)
	loc = actor:GetTranslate()

	guid = GUID("648FA024-586A-473b-8083-A49DBEAB7100")
	guid:Generate()
	pilot = g_pilotMan:NewPilot(guid, "Pilot/sexygirl.xml")
	eroActor = pilot:GetActor()	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:AddActor(guid, eroActor, loc, 2)
	pilot:GetActor():ReserveTransitAction("a_transform")
	eroActor:ToLeft(actor:IsToLeft())
	eroActor:FindPathNormal()
	eroActor:AttachParticle(100, "Bip01", "e_special_transform")
	eroActor:AttachParticle(101, "Bip01", "e_special_transform_tex")
	eroActor:AttachParticle(102, "char_root", "e_heart")
	eroActor:AttachParticle(201, "p_ef_heart", "e_steam")
--	direction = actor:GetDirection()
--	if direction == 2 then
--		eroActor:SeeFront(true)
--	elseif direction == 3 then
--		eroActor:SeeFront(false)
--	end
	return true
end

function Skill_SexyGirl_OnUpdate(actor, accumTime, frameTime)
	action = actor:GetAction()
	param = action:GetParam(0)
	if param == "null" then
		action:SetParam(0, accumTime)
	elseif accumTime - param > 3.1 then
		action:SetParam(1, "done")
		return false
	end
	return true
end

function Skill_SexyGirl_OnLeave(actor, action)

	if actor:IsMyActor() == false then
		actor:SetHide(false)
	end

	if actor:GetAction():GetParam(1) == "done" then
		actionID = action:GetID()
		if actionID == "a_idle" or
			actionID == "a_run_right" or
			actionID == "a_run_left" then
			actor:SetHide(false)
			return true 
		end
	end
	return false
end
