function Skill_MegaDrill_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end

function Skill_MegaDrill_OnCastingCompleted(actor, action)
	
end  

function Skill_MegaDrill_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actorID = actor:GetID()
	local actionID = action:GetID()
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(5,g_world:GetAccumTime());
	action:SetParamFloat(6,g_world:GetAccumTime());	
	action:SetParamInt(8,0);	
		
	actor:AttachParticle(7210,"char_root","ef_skill_Headspin_01_char_root"); -- 뒤에 붙는 연기
	actor:HideParts(EQUIP_POS_WEAPON, true);

	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_MegaDrill_OnCastingCompleted(actor,action);
	
	return true
end


function Skill_MegaDrill_OnTimer(actor,fAccumTime,action,iTimerID)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	action:CreateActionTargetList(actor)
		    
	if IsSingleMode() or action:GetTargetCount() == 0 then
	
		Skill_MegaDrill_OnTargetListModified(actor,action,false);
	else
		if actor:IsMyActor() then
			action:BroadCastTargetListModify(actor:GetPilot());
			action:ClearTargetList();
		end
	end
			
	return	true
end

function Skill_MegaDrill_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
    local   kParam = FindTargetParam();

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 40
	end

	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor),iParam2,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
end

function Skill_MegaDrill_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end

end

function Skill_MegaDrill_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local actorID = actor:GetID()
	local action = actor:GetAction()

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local iCurrentSlot = action:GetCurrentSlot();
	local iState = action:GetParamInt(3);
	
	local   kMoveDir = actor:GetLookingDir();

	if  iCurrentSlot == 0 then
		if animDone == true then
			action:SetSlot(1);
			actor:PlayCurrentSlot();
			actor:DetachFrom(7210,true)
			actor:AttachParticle(7210,"char_root","ef_skill_Headspin_02_char_root");
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			action:SetParamFloat(5,g_world:GetAccumTime());
			action:SetParamFloat(6,g_world:GetAccumTime());
		end
	elseif  iCurrentSlot == 1 then	
		kMoveDir:Multiply(600);
		actor:SetMovingDelta(kMoveDir);
		local   fElapsedTIme = accumTime - action:GetParamFloat(5);
		if fElapsedTIme > 0.05 and action:GetParamInt(8) == 0 then
			
			action:SetParamFloat(5,accumTime)
			-- 전진 시작과 동시에 타겟을 때릴 수 있게 하고
			action:CreateActionTargetList(actor)		    
			if IsSingleMode() or action:GetTargetCount() == 0 then
				Skill_MegaDrill_OnTargetListModified(actor,action,false);
		    else
				if actor:IsMyActor() then
		    		action:BroadCastTargetListModify(actor:GetPilot());
					action:ClearTargetList();
				end
		    end	
			action:SetParamInt(8,1)
			
			-- 질주 하는 중에 타겟을 잡아 때릴수 있게 타이머를 세팅한다(타이머에서 타겟을 잡아 때림)
			action:StartTimer(1.0,0.1,0)
		end

		local   fTotalElapsedTime = accumTime - action:GetParamFloat(6);
		if  fTotalElapsedTime > 0.2 then
			action:SetSlot(2);
			actor:PlayCurrentSlot();			
			action:SetParamInt(7, 500)
		end
	elseif   iCurrentSlot == 2 then
		local iCurSpeed = action:GetParamInt(7);
		kMoveDir:Multiply(iCurSpeed);
		actor:SetMovingDelta(kMoveDir);

		iCurSpeed = iCurSpeed - iCurSpeed * 0.13;
		if iCurSpeed < 0 then
			iCurSpeed = 0;
		end		
		action:SetParamInt(7, iCurSpeed);
		if animDone == true then
			local curActionParam = action:GetParam(0)
			if curActionParam == "GoNext" then
				actor:DetachFrom(7210,true)
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
	end

	return true
end

function Skill_MegaDrill_OnCleanUp(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:DetachFrom(7210,true)
	actor:HideParts(EQUIP_POS_WEAPON, false);
	return true;
end

function Skill_MegaDrill_OnLeave(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	actor:EndBodyTrail();

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

	return 
end
