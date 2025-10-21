-- Melee_Charge

function Act_Melee_Charge_OnEnter(actor, action)

	actor:SetMagicFireActionID("");
	
		
	if actor:IsMyActor() == true then 
	
		if actor:GetAbil(3)<action:GetAbil(2010) then		--	차지 포인트 부족
			AddWarnDataStr(WideString(".............."),0);
			return	false;
		end
	
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddComboButton(1); 
	end
	
	actor:PushActor(actor:IsToLeft(),5);
	actor:SetTargetScale(1.4,100);
	
	iChargeNum = 0;
	
	actor:StartWeaponTrail();
			
	return true
end

function Act_Melee_Charge_OnUpdate(actor, accumTime, frameTime)
	action = actor:GetAction()
	param = action:GetParam(0)
	if actor:IsAnimationDone() == true then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		if actor:IsMyActor() == true then g_world:ClearComboButton(); end
		action:SetParam(1, "done")
		return false
	end

	if actor:IsMeetFloor() == false then
		right = actor:GetActionState("a_run_right")
		left = actor:GetActionState("a_run_left")

		if right > left then
			actor:Walk(false, actor:GetAbil(1001))
		elseif right < left then
			actor:Walk(true, actor:GetAbil(1001))	
		end
	end
	return true
end
function Act_Melee_Charge_ReallyLeave(actor)
	actor:EndWeaponTrail();
	actor:SetMagicFireActionID("");
	actor:SetTargetScale(1,100);
	return true;
end
function Act_Melee_Charge_OnLeave(actor, action)
	if actor:IsMyActor() == false then
		actor:SetHide(false);	--	이 코드는 뭐지???
		return Act_Melee_Charge_ReallyLeave(actor);
	end

	if actor:GetAction():GetParam(1) == "done" then
		actionID = action:GetID()
		if actionID == "a_idle" or
			actionID == "a_die" or 
			actionID == "a_run_right" or
			actionID == "a_run_left" then
			if( CheckNil(g_world == nil) ) then return true end
			if( CheckNil(g_world:IsNil()) ) then return true end
			g_world:ClearComboButton();
			return Act_Melee_Charge_ReallyLeave(actor); 
		end
	end
	return false
end


