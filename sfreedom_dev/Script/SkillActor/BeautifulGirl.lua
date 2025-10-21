function BeautifulGirl_OnInit(actor)
	local bOk = actor:UntransitAction("a_idle");

	if true == bOk then
		local action = actor:GetAction();
		if action ~= nil then
			--시작 시간 기록
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			action:SetParamFloat(0, g_world:GetAccumTime());
			action:SetParamInt(1, 0);
		end
	end
	actor:FreeMove(true)
end

function BeautifulGirl_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction();
	local fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;

	local   fTotalElapsedTime = accumTime - action:GetParamFloat(0);

	if fLifeTime - fTotalElapsedTime < 1  and 0 == action:GetParamInt(1) then
		actor:SetTargetAlpha(actor:GetAlpha(),0, 0.3);
		action:SetParamInt(1, 1);
	end
end