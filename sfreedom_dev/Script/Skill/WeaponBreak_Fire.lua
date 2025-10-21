function Skill_Weapon_Break_Fire_OnCastingCompleted(actor, action)

end

function Skill_Weapon_Break_Fire_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
	
	action:SetDoNotBroadCast(true)
	
	--	시작 시간 기록
	action:SetParamFloat(0,g_world:GetAccumTime());
	action:SetParamFloat(1,g_world:GetAccumTime());
	
	-- Looking Dir
	local kLookDir = actor:GetLookingDir();
	action:SetParamFloat( 4, kLookDir:GetX() );
	action:SetParamFloat( 5, kLookDir:GetY() );
	action:SetParamFloat( 6, kLookDir:GetZ() );

	--action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	action:SetParamInt(9,0);
	
	return true
end

function Skill_Weapon_Break_Fire_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kParam = FindTargetParam();

	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(0,0,action:GetSkillRange(0,actor),0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_Weapon_Break_Fire_OnTargetListModified(actor,action,bIsBefore)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end

end

function Skill_Weapon_Break_Fire_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	

	local iActionState = action:GetParamInt(9);

	if 0 == iActionState then
		local   fNextAniPlayTIme = accumTime - action:GetParamFloat(0);
		if fNextAniPlayTIme > 0.5 then
			action:SetSlot(action:GetCurrentSlot()+1);
			actor:PlayCurrentSlot();
			action:SetParamInt(9,1);
			action:SetParamFloat(0,accumTime);
		end
	elseif 1 == iActionState then	
		local   fNextAniPlayTIme = accumTime - action:GetParamFloat(0);
		if fNextAniPlayTIme > 4 then
			action:SetSlot(action:GetCurrentSlot()+1);
			actor:PlayCurrentSlot();
			action:SetParamInt(9,2);
			action:SetParamFloat(0,accumTime);
			actor:AttachParticle(100,"p_ef_star","ef_stun_p_ef_star");
		end

		local   kMoveDir = Point3(action:GetParamFloat(4),action:GetParamFloat(5),action:GetParamFloat(6));
		local   fMoveSpeed = actor:GetAbil(AT_C_MOVESPEED);
		if fMoveSpeed == 0 then
			fMoveSpeed = 50
		end
		
		kMoveDir:Multiply(fMoveSpeed);
		
		actor:SetMovingDelta(kMoveDir);
		
		local   fElapsedTIme = accumTime - action:GetParamFloat(1);
		if fElapsedTIme>0.3 then
		
			action:SetParamFloat(1,accumTime);
			action:CreateActionTargetList(actor);
		    
			if IsSingleMode() or action:GetTargetCount() == 0 then	    
				Skill_Weapon_Break_Fire_OnTargetListModified(actor,action,false);	    
			else	    
				action:BroadCastTargetListModify(actor:GetPilot());
				action:ClearTargetList();	    
			end	    
		end		
	elseif 2 == iActionState then
		local   fElapsedTIme = accumTime - action:GetParamFloat(0);
		if fElapsedTIme > 1.5 then
			actor:SetTargetAlpha(actor:GetAlpha(),0, 0.3);
			action:SetParamInt(9,3);
		end
	end

	return true
end

function Skill_Weapon_Break_Fire_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

--	if actor:IsUnderMyControl() then
--		Net_C_M_REQ_DELETE_ENTITY(actor:GetPilotGuid());
--	end

	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
end

function Skill_Weapon_Break_Fire_OnLeave(actor, action)
	return true;
end
