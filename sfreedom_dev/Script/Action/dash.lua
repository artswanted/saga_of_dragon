-- [DIRECTION] : 이동 할 방향  "LEFT" or "RIGHT"
function Act_Dash_OnCheckCanEnter(actor,action)

	-- 기획팀장님이 공중에서 대쉬 할 수 있게 해달랍니다;;(단 한 번만)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	if actor:IsMeetFloor() == false then
		if actor:GetJumpAccumHeight()<50 then
			return false
		end	
	end
	
	if actor:IsOnlyMoveAction() then
		return false
	end

	if IsSingleMode() == false then
		--	이동 속도가  0이면 안된다.
		if actor:GetAbil(AT_C_MOVESPEED) == 0 then
			return	false
		end
	
	end

	return true
end

function Act_Dash_OnEnter(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	if actor:IsMyActor() then
		if actor:GetPilot():IsCorrectClass(CT_ASSASSIN) then
			CheckNil(nil==action)
			CheckNil(action:IsNil())
			if action:CheckCanEnter(actor,"a_ass_attk_dash") then
			
				if action:GetDirection() ~= DIR_LEFT and
					action:GetDirection() ~= DIR_RIGHT then
			
					action:SetNextActionName("a_ass_attk_dash");
					action:ChangeToNextActionOnNextUpdate(true, true)
					return	true;
				
				end
			end
		end
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	if actor:IsMyActor() == false then
--		ODS("======================== Other actor's dash Begin =====================\n")
		actor:ClearReservedAction()
	end

	action:SetParamFloat(0, g_world:GetAccumTime());	-- Start Time
	action:SetParamFloat(1, g_fDashSpeed);	-- Start Velocity
	action:SetParam(2,"");
	
	actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0);
	
	actor:FindPathNormal()		
	local	pt = actor:GetTranslate()
	pt:SetZ(pt:GetZ()-30)
	--actor:AttachParticleToPoint(201, pt, "e_special_transform")	
	actor:SetComboCount(0);
	
	if actor:IsMyActor() == false then
		actor:SetTranslate(action:GetActionStartPos());
	end
	
	actor:UseSkipUpdateWhenNotVisible(false);
	action:SetParamInt(4,0);
	
	if actor:IsMyActor() then
	
        GetComboAdvisor():OnNewActionEnter(action:GetID());	
        GetComboAdvisor():OnNewActionEnter(action:GetID());	
        
        local kNormalAttackActionID = actor:GetNormalAttackActionID();
        GetComboAdvisor():AddNextAction("a_dash_attack");    
        GetComboAdvisor():AddNextAction("a_dash_jump");    
        GetComboAdvisor():AddNextAction("a_dash_blowup");    
        GetComboAdvisor():AddNextAction("a_clown_sliding_tackle");    
        
    end
	
	actor:SetDirection(action:GetDirection())

	return true
end

function Act_Dash_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local	action = actor:GetAction()
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	local	iState = action:GetParamInt(4)

	local jumping = "TRUE"
	if actor:IsJumping() == false then
		jumping = "FALSE"
	end

	--ODS("__________Jumping : " .. jumping .. "________\n")

	
	if action:GetParam(2) == "" and 
	
		( action:GetNextActionName() == "a_dash_attack" or 
		action:GetNextActionName() == "a_magician_dash_attack" or 
		action:GetNextActionName() == "a_thi_dash_attack" or 
		action:GetNextActionName() == "a_archer_dash_attack" or
		action:GetNextActionName() == "a_twin_dash_attack"
		) then
		
	    action:SetNextActionName("a_idle");
	    
	end
	
	if action:GetParam(2) == "ToDashAttack!" then
	    action:SetParam(2,"end");
		return false
	end		
	
	if iState == 0 then
		actor:FindPathNormal()		
		
		local fAccel = -1000 * frameTime
		local fVelocity = action:GetParamFloat(1)
		
--		local kMovingDir = actor:GetLookingDir()
--		kMovingDir:Multiply(fVelocity)
		
		local dir = action:GetDirection()

		--ODS("______________Direction : " .. dir .. "\n")

		actor:Walk(dir, fVelocity)
		
		fVelocity = fVelocity + fAccel
		action:SetParamFloat(1, fVelocity)

		local fElapsedTime = accumTime - action:GetParamFloat(0);
		if fElapsedTime > 0.3 or fVelocity < 0 then
			-- TODO : ElapsedTime이 0.3에서 얼마나 벗어났는지 확인 후에, 그만큼 뒤로 당겨 주어야 한다.
			action:SetParamInt(4,1)
			action:SetParamFloat(5, accumTime)
		end
		
	else
		local fElapsedTime = accumTime - action:GetParamFloat(5);
		if fElapsedTime>=g_fDashFreezeTime then
			action:SetParam(2,"end")
			if actor:IsMeetFloor() == false then
				action:SetNextActionName("a_jump")
			end
			return false
		end
	end
	
	if action:GetParam(3) == "EndNow" then
		return false
	end
	

		
	return	true;
end

function Act_Dash_OnCleanUp(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	actor:UseSkipUpdateWhenNotVisible(true);
	actor:EndBodyTrail();

	return true
end

function Act_Dash_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local kCurAction = actor:GetAction();
	CheckNil(nil==kCurAction)
	CheckNil(kCurAction:IsNil())
	local newActionID = action:GetID()
    local	iBaseActorType = actor:GetPilot():GetBaseClassID()
    
    
    ODS("Act_Dash_OnLeave "..newActionID.."\n");
    
    if newActionID == "a_jump" then
		--action:SetDoNotBroadCast(true)
		
		action:SetSlot(2);
	end

	if action:GetActionType()=="EFFECT"  then
		return true
	end		
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if newActionID == "a_dash_attack" or
		newActionID == "a_thi_dash_attack" or
		newActionID == "a_nj_dash_attack" or
		newActionID == "a_archer_dash_attack" or
		newActionID == "a_magician_dash_attack" or
		newActionID == "a_clown_sliding_tackle" or
		newActionID == "a_kni_dashdownblow" or
		newActionID == "a_dash_blowup" or
		newActionID == "a_trap" or
		newActionID == "a_twin_dash_attack" 		
		then

		action:SetParamFloat(1,kCurAction:GetParamFloat(1));
		return true
	end
	
	if (string.sub(newActionID,1,7) == "a_melee" or 
		string.sub(newActionID,1,11) == "a_thi_melee" or 
		string.sub(newActionID,1,10) == "a_nj_melee" or 
		newActionID == "a_archer_shot_01" or 
		newActionID == "a_MagicianShot_01" or
		newActionID == "a_twin_melee_01_m"
		) 
		and weapontype~=0 then
		
		-- Dash Attack!
		
		
		local	kPilot = actor:GetPilot();
		local	kNextActionName = "";
		
		if kPilot:IsCorrectClass(CT_CLOWN) and
			Act_Melee_IsToDownAttack(actor,action) then
			
			kNextActionName = "a_clown_sliding_tackle";
			
		elseif kPilot:IsCorrectClass(CT_KNIGHT) and
			 actor:IsMeetFloor() == false then
			
			kNextActionName = "a_kni_dashdownblow";

		else
			
			if actor:CheckStatusEffectExist("se_transform_to_metamorphosis") then
				kNextActionName = "a_nj_dash_attack";
			elseif actor:CheckStatusEffectExist("se_transform_to_Dancer") then
				kNextActionName = "a_thi_dash_attack";
			elseif iBaseActorType == CT_FIGHTER then
				kNextActionName = "a_dash_attack";
			elseif iBaseActorType == CT_MAGICIAN then
				kNextActionName = "a_magician_dash_attack";
			elseif iBaseActorType == CT_ARCHER then
				kNextActionName = "a_archer_dash_attack";
			elseif iBaseActorType == CT_THIEF then
				kNextActionName = "a_thi_dash_attack";
			elseif iBaseActorType == CT_DOUBLE_FIGHTER then
				kNextActionName = "a_twin_dash_attack";			
			end
			
		end
		
		
		ODS("dash kNextActionName : "..kNextActionName.."\n");
		
		if kNextActionName ~="" and action:CheckCanEnter(actor,kNextActionName,true) then
			kCurAction:SetParam(2,"ToDashAttack!");
			kCurAction:SetNextActionName(kNextActionName);
		end
		return false
	end

	if kCurAction:GetParam(2) == "end" then
		if newActionID == "a_jump" then
			action:SetSlot(2)
		end
		return	true;
	elseif newActionID == "a_jump" and actor:IsJumping() == false then
		if action:GetParam(5) == "HiJump" then -- hiJump면 그냥 jump로 넘어감.
			action:SetSlot(1)
			kCurAction:SetParam(3, "EndNow")
			return true
		end	
		
		ODS("____________To Dash Jump______________\n")
		kCurAction:SetNextActionName("a_dash_jump")
		kCurAction:SetParam(3, "EndNow")
		return false 
	elseif newActionID == "a_dash_jump" or
		newActionID == "a_telejump" or
		newActionID == "a_lock_move" or
		newActionID == "a_trap" or
		newActionID == "a_teleport"
		then
		return true;
	end

	return false
end
