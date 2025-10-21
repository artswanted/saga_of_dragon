-- [DIRECTION] : 이동 할 방향  "LEFT" or "RIGHT"
function Act_BackStep_OnCheckCanEnter(actor,action)
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	
	local kCurAction = actor:GetAction()
	if( false == kCurAction:IsNil() ) then
		if actor:IsMyActor() then
			local kCurActionID = kCurAction:GetID();
			
			ODS("Act_BackStep_OnCheckCanEnter kCurActionID:"..kCurActionID.."\n");
			
			if kCurActionID == "a_archer_charge_shot" 
				or kCurActionID == "a_melee_charge_attack" 
			then
					
				-- 차징 공격의 캐스팅때문에 백스탭 액션이 무시되어버리는 것을 막기위함.
			
				actor:CutSkillCasting();
				
			end
		end
    end
    return true;

end

function Act_BackStep_OnEnter(actor, action)

    ODS("Act_BackStep_OnEnter\n");
		
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	if actor:IsMyActor() == false then
		actor:ClearReservedAction()
	end

    local	iBaseActorType = actor:GetPilot():GetBaseClassID()

	
	local   fMoveSpeed = actor:GetAbil(AT_C_MOVESPEED)*1.2;   --  이동속도의 2배
	
	if IsSingleMode() then
		fMoveSpeed= 120
	end
	
	ODS("BackStep fMoveSpeed : "..fMoveSpeed.."\n");

	action:SetParamFloat(0, g_world:GetAccumTime());	-- Start Time
	action:SetParamFloat(1, fMoveSpeed);	-- Start Velocity
	action:SetParam(2,"");
	
	actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0);
	
	local	pt = actor:GetTranslate()
	pt:SetZ(pt:GetZ()-30)
	actor:SetComboCount(0);
	actor:StopJump();
	
	if actor:IsMyActor() == false then
		actor:SetTranslate(action:GetActionStartPos());
		
		GetComboAdvisor():OnNewActionEnter(action:GetID());
	    
		if iBaseActorType == CT_ARCHER then
			GetComboAdvisor():AddNextAction("a_archer_backstep_shot");
		end		
	end
	
	action:SetParamFloat(6,actor:GetLookingDir():GetX());
	action:SetParamFloat(7,actor:GetLookingDir():GetY());
	action:SetParamFloat(8,actor:GetLookingDir():GetZ());

	actor:SetMovingDelta(Point3(0,0,0));
	

	
			
	return true
end

function Act_BackStep_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	local	action = actor:GetAction()
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());
	
	local fAccel = -100 * frameTime
	local fVelocity = action:GetParamFloat(1)
	
	ODS("BackStep fVelocity :"..fVelocity.."\n");

	fVelocity = fVelocity + fAccel
	action:SetParamFloat(1, fVelocity)

	local fElapsedTime = accumTime - action:GetParamFloat(0);
	if fElapsedTime > 0.5 or fVelocity < 0 then
	    action:SetParam(2, "end")
		return false;
	end
	
	if action:GetParam(2) =="end" then
		return false;
	end
	
	local kMovingDir = Point3(action:GetParamFloat(6),
	                            action:GetParamFloat(7),
	                            action:GetParamFloat(8));
	                            
	kMovingDir:Multiply(-fVelocity)
	actor:SetMovingDelta(kMovingDir);
	
	return	true;
end

function Act_BackStep_OnCleanUp(actor, action)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	if action == nil or action:IsNil() or action:GetID() ~= "a_archer_backstep_shot" then
	    actor:EndBodyTrail();
    end

	return true
end

function Act_BackStep_OnLeave(actor, action)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());
	
	
	local	kCurAction = actor:GetAction();
	local newActionID = action:GetID()
	
	ODS("Act_BackStep_OnLeave newActionID:"..newActionID.."\n");
	
	if actor:IsMyActor() == false then
		return  true;
	end

	if action:GetActionType()=="EFFECT"  then
		return true;
	end		
	local weapontype = actor:GetEquippedWeaponType();
	
	if newActionID == "a_archer_shot_01" then
	    
		if action:CheckCanEnter(actor,"a_archer_backstep_shot") then
			kCurAction:SetNextActionName("a_archer_backstep_shot")
			kCurAction:SetParam(2, "end")
		end
		
		return	false
	
	end
	
	if newActionID == "a_archer_backstep_shot" then
	
		if action:CheckCanEnter(actor,"a_archer_backstep_shot") then
		
			action:SetParamFloat(0,kCurAction:GetParamFloat(0));
			action:SetParamFloat(1,kCurAction:GetParamFloat(1));
			action:SetParamFloat(6,kCurAction:GetParamFloat(6));
			action:SetParamFloat(7,kCurAction:GetParamFloat(7));
			action:SetParamFloat(8,kCurAction:GetParamFloat(8));
		    
			return true;
	    end
	    return	false;
	
	end
		
	if kCurAction:GetParam(2) == "end" then
		return	true;
	elseif 	newActionID == "a_lock_move" then
		return true;
	end

	return false
end
