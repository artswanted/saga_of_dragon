-- Melee
function Skill_Detonator_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function Skill_Detonator_OnCastingCompleted(actor, action)

	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function Skill_Detonator_OnEnter(actor, action)
	local actorID = actor:GetID()
	local actionID = action:GetID()
	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_Detonator_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");

	action:SetParamInt(2,0);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Detonator_OnCastingCompleted(actor,action);
	

	return true
end

function Skill_Detonator_OnUpdate(actor, accumTime, frameTime)

	local actorID = actor:GetID()
	local action = actor:GetAction()
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
	local actionID = action:GetID();

	if animDone == true then
	
	
		local curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			nextActionName = action:GetParam(1);

			action:SetParam(0, "null")
			action:SetParam(1, "end")
			
			action:SetNextActionName(nextActionName)
			return false;
		else
		
			action:SetParam(1, "end")
			return false
		end
	end

	return true
end

function Skill_Detonator_OnCleanUp(actor)

	return true;
end

function Skill_Detonator_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		return false;
	end
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	if action:GetEnable() == false then
		if curParam == actionID then
			curAction:SetParam(0,"");
		end
	end
	
	if action:GetEnable() == true then
	

		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);

		return false;
	
	elseif curParam == "end" and (actionID == "a_run_right" or actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Skill_Detonator_OnEvent(actor,textKey)

	local action = actor:GetAction();
	if action:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then

		actor:AttachSound(2000,"Claymore3");

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
        
		for i=1,8 do
			local   kClaymoreGUID = actor:GetParam("Claymore_GUID_"..i);
			actor:SetParam("Claymore_GUID_"..i, "");
			if kClaymoreGUID ~= nil and kClaymoreGUID ~= "" then
				--WriteToConsole("Claymore_GUID_"..i.."   bome_force\n");
				actor:SetParam("Claymore_GUID_"..i, "");
				kClaymoreGUID = GUID(kClaymoreGUID)
				local   kClaymore = g_world:FindActor(kClaymoreGUID)
				if kClaymore:IsNil() == false then
                
					local   kAction = kClaymore:GetAction();
					if kAction:IsNil() then 
						kAction = kClaymore:GetReservedTransitAction();
					end
                    
					if kAction:IsNil() then
						return
					end
                    
					if kAction:GetParam(0) == "Idle" then
						kAction:SetParam(0,"bomb_force")
					end                
				end            
			end				
		end
    end

	return	true;
end
