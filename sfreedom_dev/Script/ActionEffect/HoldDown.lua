-- damage

function Act_HoldDown_OnEnter(actor, action)

	action:SetParamInt(0,0);	--	Step
	action:SetParamInt(1,0);	--	Anim Speed Setted
	WriteToConsole("12312424\n")
	ODS("Act_HoldDown_OnEnter\n");
	
	return true
end
function Act_HoldDown_OnUpdate(actor, accumTime, frameTime,action)
	WriteToConsole("Update\n")
	local	fElapsedTime = accumTime - action:GetActionEnterTime();
	
	local	iStep = action:GetParamInt(0);
	
	if iStep == 0 then
	
		local	kEffectDef = GetEffectDef(action:GetActionNo());
		local	fTotalPlayTime = 3;
		if kEffectDef:IsNil() == false then
			fTotalPlayTime = kEffectDef:GetDurationTime()/1000.0;
		end	
	
		if action:GetParamInt(1) == 0 then
		
			action:SetParamInt(1,1)
		
		end
	
		if fElapsedTime >= fTotalPlayTime then
		
			action:SetParamInt(0,1);
			actor:PlayNext();	--	쓰러진다
			actor:SetDownState(true);

		end
	
	elseif iStep == 1 then

		if actor:IsAnimationDone() == true then
			action:SetParamInt(0,2);
			actor:PlayNext();	--	일어난다
			actor:SetDownState(false);
			return true
		end
		
	else
			
		if actor:IsAnimationDone() == true then
			action:SetParam(0,"end");
			return false
		end
			
	end
	
	return true
end
function Act_HoldDown_OnCleanUp(actor, action)

	actor:SetDownState(false);
	ODS("Act_HoldDown_OnCleanUp\n");
end

function Act_HoldDown_OnLeave(actor, action)

	if action:GetActionType() ~= "EFFECT" then
		
		local	kEffectDef = GetEffectDef(actor:GetAction():GetActionNo());
		local	fGodTime = 1;
		if kEffectDef:IsNil() == false then
			fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0;
		end
		actor:StartGodTime(fGodTime);
		
	end
	


	return true
end
