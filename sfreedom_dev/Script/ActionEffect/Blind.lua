
function Act_Blind_OnEnter(actor, action)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if actor:IsMyActor() then
		g_world:SetShowWorldFocusFilterColorAlpha(0xFFFFFF, 0.0, 1.0, 1,true,false);
		action:SetParamFloat(10,g_world:GetAccumTime());
		return true
	end
end

function Act_Blind_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()

	ODS("Act_Blind_OnUpdate\n")
	local	fElapsedTime = accumTime - action:GetParamFloat(10);
	if fElapsedTime > 3.0 then
		ODS("Act_Blind_OnUpdate End\n")
		return false
	end

	return true;	
end

function Act_Blind_OnCleanUp(actor, action)
ODS("Act_Blind_OnCleanUp\n")
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	if actor:IsMyActor() then
		g_world:SetShowWorldFocusFilterColorAlpha(0xFFFFFF, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end
	return true
end

function Act_Blind_OnLeave(actor, action)
	return true
end
