function Skill_CarpetBombing_Play_OnCastingCompleted(actor, action)

	Skill_CarpetBombing_Play_Fire(actor,action);

end

function Skill_CarpetBombing_Play_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

	--ODS("Skill_CarpetBombing_Play_Fire\n");
    --  시작 시간
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    action:SetParamFloat(2,g_world:GetAccumTime());
    action:SetParamFloat(3,g_world:GetAccumTime());

	local   kFireEffectID = action:GetScriptParam("INSTALL_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("INSTALL_EFFECT_TARGET_NODE");
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPoint(7212,kDir,kFireEffectID);
            
        else
            actor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID);
        end
    
    end	
end

function Skill_CarpetBombing_Play_StartTimer(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("Skill_CarpetBombing_Play_StartTimer\n");
    
    if actor:IsUnderMyControl() then
        
        if action:GetScriptParam("BOMB_AFTER_LIFETIME") ~= "TRUE" then
        	        
	        local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
        	
	        if fLifeTime == 0 then
	            fLifeTime = 10.0;
            end
                
            local   fAttackDelay = 0;
            
            fAttackDelay = action:GetAbil(AT_ATT_DELAY)/1000.0;
            --ODS("Action :"..action:GetID().." AttackDelay : "..fAttackDelay.." fLifeTime:"..fLifeTime.."\n");
            if fAttackDelay < 0.05 then
                fAttackDelay = 0.05
            end
            
            --  타이머 시작
            action:StartTimer(fLifeTime,0.3,0)
        
        end
    
    end
    
end


function Skill_CarpetBombing_Play_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("Skill_CarpetBombing_Play_OnEnter\n");
	local actorID = actor:GetID()
	local actionID = action:GetID()
	action:SetDoNotBroadCast(true)
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(0,g_world:GetAccumTime());
	
	--	State
	action:SetParamInt(4,0)
	
	--	Hit Count
	action:SetParamInt(5,0)
	
	--	Last Hit Time
	action:SetParamFloat(6,0);
	action:SetParamFloat(7,g_world:GetAccumTime() + 0.3);
	action:SetParamInt(8,0);
	
	-- Looking Dir
	local kLookDir = actor:GetLookingDir();
	action:SetParamFloat( 11, kLookDir:GetX() );
	action:SetParamFloat( 12, kLookDir:GetY() );
	action:SetParamFloat( 13, kLookDir:GetZ() );
	
	actor:SetMovingDelta(Point3(0,0,0));

	local kPos = actor:GetPos();
	kPos:SetZ(kPos:GetZ());
	actor:SetTranslate(kPos);

	actor:StopJump();
	actor:FreeMove(true);
	actor:SetTargetScale(0.0001, 0);
	actor:SetHideShadow(true);
	action:SetParamInt(10,action:GetAbil(AT_SKILL_CUSTOM_DATA_01));

	Skill_CarpetBombing_Play_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_CarpetBombing_Play_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	

	--ODS("Skill_CarpetBombing_Play_OnTargetListModified\n");
    if bIsBefore == false then
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end
end

function Skill_CarpetBombing_Play_FindTarget(actor,action,bForceToBomb)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	

	--ODS("Skill_CarpetBombing_Play_FindTarget\n");
    action:CreateActionTargetList(actor);
    
    if action:GetTargetCount() > 0 then
    
        if IsSingleMode() or action:GetTargetCount() == 0 then
			
            Skill_CarpetBombing_Play_OnTargetListModified(actor,action,false);    
        
        else
        
            action:BroadCastTargetListModify(actor:GetPilot());
            action:ClearTargetList();
        
        end    
        
        return true;
        
    end
    
    return false
end

function Skill_CarpetBombing_Play_OnTimer(actor, accumTime, action,iTimerID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	--ODS("Skill_CarpetBombing_Play_OnTimer\n");
	Skill_CarpetBombing_Play_FindTarget(actor,action);
    
    return true;

end



function Skill_CarpetBombing_Play_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local animDone = actor:IsAnimationDone()
	local actionID = action:GetID();
	local iSlotNum = action:GetCurrentSlot();
	local fMoveSpeed  = 300;
	local fMaxMoveDistance = action:GetParamInt(10)+10;
	local	iState = action:GetParamInt(4);
	
	if iSlotNum == 0 then		
	
		local	fElapsedTime = accumTime - action:GetParamFloat(0);
		if fElapsedTime>=0.3 then
			action:SetParamFloat(0,accumTime);	--	이동 시작 시간 기록
			
			local	kPos = actor:GetPos();
			-- 현재 위치 기록
			action:SetParamFloat(1,kPos:GetX());
			action:SetParamFloat(2,kPos:GetY());
			action:SetParamFloat(3,kPos:GetZ());
			actor:PlayNext();
			Skill_CarpetBombing_Play_StartTimer(actor,action);
			
		end	
	end
	
	if iSlotNum == 1 then
	
		local	fElapsedTime = accumTime - action:GetParamFloat(0);
		local	fMoveDistance = fElapsedTime * fMoveSpeed;
		
		if fMoveDistance>=fMaxMoveDistance then
			actor:PlayNext();
		else
		
			local kMovingDir = Point3(action:GetParamFloat(11),action:GetParamFloat(12),action:GetParamFloat(13));
			
			kMovingDir:Multiply(fMoveDistance);
			
			local kNextPos = Point3(action:GetParamFloat(1),action:GetParamFloat(2),action:GetParamFloat(3));
			kNextPos:Add(kMovingDir);
			
			actor:SetTranslate(kNextPos);
		end
	
	end
	
	-- 바닥 펑펑 터지는 이펙트
	local fBoomElapsedTime = accumTime - action:GetParamFloat(7);
	if fBoomElapsedTime >= 0.2 and action:GetParamInt(8) < 6 then		

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local kPos = actor:GetPos();
		kPos:SetZ(kPos:GetZ() + 200);
		local ptcl = g_world:ThrowRay(kPos, Point3(0,0,-1),500);
		if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
			--위로도 쏴 본다.
			ptcl = g_world:ThrowRay(kPos, Point3(0,0,1),500);
			if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
				return false;
			end			
		end
		actor:AttachSound(200 + action:GetParamInt(8),"CarpetBombing_Bomb");
		ptcl:SetY(ptcl:GetY() + math.random(-30,30));
		actor:AttachParticleToPoint(100 + action:GetParamInt(8), ptcl, "ef_boom_01");
		action:SetParamFloat(7,g_world:GetAccumTime());
		action:SetParamInt(8,action:GetParamInt(8) + 1);
	end

	return true
end

function Skill_CarpetBombing_Play_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("Skill_CarpetBombing_Play_OnFindTarget\n");
    local   kParam = FindTargetParam();

	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(0,0,80,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_CarpetBombing_Play_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	ODS("Skill_CarpetBombing_Play_OnCleanUp\n");
--	if actor:IsUnderMyControl() then
--		Net_C_M_REQ_DELETE_ENTITY(actor:GetPilotGuid());
--	end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());

end

function Skill_CarpetBombing_Play_OnLeave(actor, action)
	ODS("Skill_CarpetBombing_Play_OnLeave\n");
	return true;
end

-- 로딩이 끝난 후에 초기화 처리가 필요한 것들을 세팅하는 함수
function CarpetBombingPlayInit(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	actor:SetHideShadow(true);	
end
