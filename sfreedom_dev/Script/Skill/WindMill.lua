
-- Melee
function Skill_WindMill_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if(true == actor:IsInCoolTime(action:GetActionNo(), false) ) then
		return false
	end
	
	local curAction = actor:GetAction();
	if( false == curAction:IsNil() ) then
		local kActionName = curAction:GetID();		
		if("a_head_spin" == kActionName or "a_Nineteen Ninety" == kActionName) then
			curAction:SetNextActionName("a_Windmill");
			curAction:ChangeToNextActionOnNextUpdate(true, true);
		end
	end
	
	return		true;
	
end
function Skill_WindMill_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_WindMill_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function Skill_WindMill_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	action:SetParamFloat(2,g_world:GetAccumTime()); --  시작 시간
	action:SetParamFloat(6,0);

	actor:HideParts(EQUIP_POS_WEAPON, true);
	
	actor:AttachParticle(19002,"char_root","ef_Windmill_01_char_root");
	actor:AttachParticleWithRotate(19003,"p_ef_heart","ef_Windmill_01_p_ef_heart", 1.0, true);

	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end
end

function Skill_WindMill_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	
	--ODS("Skill_WindMill_OnFindTarget iAttackRange:"..iAttackRange.."\n");
	if iAttackRange == 0 then
	    iAttackRange = 100
	end

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();

	local kParam = FindTargetParam();
	
    local iFindTargetType = TAT_SPHERE;
	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(0,0,iAttackRange,0);
	
	kParam:SetParam_3(true,FTO_NORMAL);
	local iTargets = action:FindTargets(iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Skill_WindMill_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_WindMill_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end
	
	action:SetParamFloat(30, g_world:GetAccumTime())
	Skill_WindMill_OnCastingCompleted(actor,action);

	return true
end
function Skill_WindMill_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    ODS("Skill_WindMill_OnTargetListModified\n");
    
    if bIsBefore == false then
        --  때린다.
		SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end
end

function Skill_WindMill_UpdateTarget(actor,action,accumTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if actor:IsMyActor() == false then
        return
    end
    
	local iSlot = action:GetCurrentSlot();

	if iSlot == 3 then
		local   fLastUpdateTime = action:GetParamFloat(6);
		if accumTime - fLastUpdateTime <0.3 then
			return
		end
	else
		local   fLastUpdateTime = action:GetParamFloat(6);
		if accumTime - fLastUpdateTime <0.4 then
			return
		end
	end

    action:SetParamFloat(6,accumTime);

    --  새로 타겟을 찾는다.        
    action:CreateActionTargetList(actor);
    
	--  서버로 브로드캐스팅
	if action:GetTargetCount()>0 then
	    action:BroadCastTargetListModify(actor:GetPilot());
		local iSlot = action:GetCurrentSlot();
		if iSlot ~= 3 then
			actor:AttachParticleWithRotate(500,"p_ef_heart","ef_Windmill_02_p_ef_heart", 1.0, true);
		end
	end
	
	action:ClearTargetList();
	
	ODS("Skill_WindMill_UpdateTarget\n");
	    
end

function Skill_WindMill_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actionID = action:GetID();
--	local movingSpeed = 0
	local bAnimDone = actor:IsAnimationDone();
	local iSlot = action:GetCurrentSlot();
	local nextActionName = action:GetNextActionName();

	--ODS("Skill_WindMill_OnUpdate NextActionName : "..nextActionName.."\n");
	if nextActionName == "a_Head Spin" then
		action:SetNextActionName("a_idle");
	end

	local iSkillLevel = action:GetAbil(AT_LEVEL);

	-- 윈드 밀 스킬 레벨이 3 이상이면 이동가능
	if actor:IsMyActor() then
		if iSkillLevel > 2 then
			local	bKeyUp = KeyIsDown(KEY_UP);
			local	bKeyDown = KeyIsDown(KEY_DOWN);
			local	bKeyLeft = KeyIsDown(KEY_LEFT);
			local	bKeyRight = KeyIsDown(KEY_RIGHT);
			
			local	iDir = 0
			if bKeyUp then 
				iDir = iDir + DIR_UP
			end
			if bKeyDown then
				iDir = iDir + DIR_DOWN
			end
			if bKeyLeft then
				iDir = iDir + DIR_LEFT
			end
			if bKeyRight then
				iDir = iDir + DIR_RIGHT
			end

			if iDir ~= 0 then
				if( CheckNil(g_world == nil) ) then return false end
				if( CheckNil(g_world:IsNil()) ) then return false end
				local iMoveSpeed = actor:GetAbil(AT_C_MOVESPEED) * 0.5
				local kDirection = actor:GetDirectionVector(iDir);

				kDirection:Multiply(iMoveSpeed);
				actor:SetMovingDelta(kDirection);
				local fTimeDelta = g_world:GetAccumTime() - action:GetParamFloat(30)
				if(fTimeDelta > 0.02) then -- 서버로 이동 위치 갱신 보내는 시간 간격
					g_pilotMan:BroadcastDirection(actor:GetPilot(), iDir)
					action:SetParamFloat(30, g_world:GetAccumTime())
				end
			end
		end
	end	

    if iSlot == 1 then

	    if bAnimDone then
	        actor:PlayNext();
	        action:SetParamFloat(2,accumTime);
	    end    
    elseif iSlot == 2 then	
	
    	Skill_WindMill_UpdateTarget(actor,action,accumTime);
    	
    	local   fElapsedTime = accumTime - action:GetParamFloat(2);
    	if fElapsedTime > 2.3 then 
    	    actor:PlayNext();
			actor:DetachFrom(19003,true);
			actor:AttachParticle(19003,"char_root","ef_Windmill_02_char_root");
    	end    	
    else
		Skill_WindMill_UpdateTarget(actor,action,accumTime);

        if bAnimDone then        
            action:SetParam(1,"end");
            return false;        
        end    
    end

	return true
end

function Skill_WindMill_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(19002);
	actor:DetachFrom(19003);
	actor:HideParts(EQUIP_POS_WEAPON, false);
	return true;
end

function Skill_WindMill_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
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

	return false 
end
