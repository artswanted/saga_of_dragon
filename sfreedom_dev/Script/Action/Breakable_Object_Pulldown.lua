function Act_Breakable_Object_Pulldown_OnEnter(actor, action)
		
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());
	
	ODS("Act_Breakable_Object_Pulldown_OnEnter\n");
	action:SetParamFloat(1,action:GetParamFloat(2));
	action:SetParamFloat(0,g_world:GetAccumTime());
	action:SetParamAsPoint(1,actor:GetPos());
	
	return true
end

function Act_Breakable_Object_Pulldown_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	local	action = actor:GetAction();
	local	kTargetPos = action:GetParamAsPoint(0);
	local	kStartPos = action:GetParamAsPoint(1);
	local	fTotalElapsedTime = accumTime - action:GetParamFloat(0);
	local	fTotalZDistance = kTargetPos:GetZ() - kStartPos:GetZ();
	
	
	local	fZ_Velocity_Init = action:GetParamFloat(1);
	local	fZ_Velocity = g_fGravity*(0.5*frameTime+fTotalElapsedTime)+fZ_Velocity_Init;
	action:SetParamFloat(2,fZ_Velocity);
	
	local	fZDistance = fTotalElapsedTime*fZ_Velocity;
	if fZDistance<fTotalZDistance then
		fZDistance = fTotalZDistance;
	end
	ODS("fTotalZDistance : "..fTotalZDistance.." fZDistance:"..fZDistance.."\n");
	
	local	kNewPos = action:GetParamAsPoint(1);
	kNewPos:SetZ(kNewPos:GetZ()+fZDistance);

	actor:SetTranslate(kNewPos);
	
	if fZDistance == fTotalZDistance then
		return	false
	end

	return true;
end

function Act_Breakable_Object_Pulldown_OnCleanUp(actor, action)
	return true
end

function Act_Breakable_Object_Pulldown_OnLeave(actor, action)
	ODS("Act_Breakable_Object_Pulldown_OnLeave\n");
	return true
end
