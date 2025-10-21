-- [TYPE] : "BOMB" : 폭발형(밟으면 터지면서 스스로도 제거되는 형태이다)
-- [BOMB_EFFECT_ID] : 폭발 이펙트 ID
-- [BOMB_AFTER_LIFETIME] : LIFE타임이 끝났을 때 자동 폭발한다.
-- [BOME_AFTER_EFFECT_ID] : 자동 폭발할때 폭발 이펙트 ID
-- [BOME_AFTER_EFFECT_SCALE] : 자동 폭발할때 폭발 이펙트 사이즈
-- [HIT_SOUND_ID] : 폭파될때 나오는 사운드
-- [BOME_AFTER_SOUND_ID] : 자동 폭파될때 나오는 사운드
-- [INSTALL_EFFECT_TARGET_NODE] : 실제로 설치시 이펙트를 붙일때, 붙일 노드
-- [INSTALL_EFFECT_ID] : 실제로 설치시 붙일 이펙트 ID
-- [INSTALL_EFFECT_TARGET_NODE2] : 실제로 설치시 이펙트를 붙일때, 붙일 노드
-- [INSTALL_EFFECT_ID2] : 실제로 설치시 붙일 이펙트 ID
-- [FIND_TARGET_TYPE] : 타겟 찾는 방법 : RAY(DEFAULT),BAR,SPHERE

function Skill_InstallDefault_Play_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_InstallDefault_Play_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

	if actor:IsUnderMyControl() then
		local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
	        	
		if fLifeTime == 0 then
			fLifeTime = 9.0;	
		end
		actor:SetInstallTimerGauge(fLifeTime);
	end

end

function Skill_InstallDefault_Play_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
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

	local   kFireEffectID2 = action:GetScriptParam("INSTALL_EFFECT_ID2");
    local   kFireEffectTargetNodeID2 = action:GetScriptParam("INSTALL_EFFECT_TARGET_NODE2");
    
    if kFireEffectID2 ~= "" and kFireEffectTargetNodeID2 ~= "" then
    
        if kFireEffectTargetNodeID2 == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPoint(7213,kDir,kFireEffectID2);
            
        else
            actor:AttachParticle(7213,kFireEffectTargetNodeID2,kFireEffectID2);
        end
    
    end	


end
function Skill_InstallDefault_StartTimer(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
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
            action:StartTimer(fLifeTime,fAttackDelay,0)
        
        end
    
    end
    
end

function Skill_InstallDefault_Play_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actionID = action:GetID()
	
	--ODS("Skill_InstallDefault_Play_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	action:SetParam(0,"Idle");  --  State
	action:SetDoNotBroadCast(true)
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_InstallDefault_Play_OnCastingCompleted(actor,action);
	actor:FreeMove(true)
	return true
end

function Skill_InstallDefault_Play_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
    
		--ODS("Skill_InstallDefault_Play_FindTarget Skill_InstallDefault_Play_OnTargetListModified\n");
    
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
        
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
        if action:GetScriptParam("TYPE") == "BOMB" and action:GetID() ~= "a_Caltrap Set" then 
            --  폭발형일 경우
            --  폭발 이펙트를 붙인다.
            local   kBombEffectID = action:GetScriptParam("BOMB_EFFECT_ID");
            if kBombEffectID == "" then
                kBombEffectID = "ef_Magnumbreak_01_char_root"
            end
            GetMyActor():AttachParticleToPoint(1,actor:GetPos(),kBombEffectID);
            --  자신의 모습은 감춘다.
            actor:HideNode("Scene Root",true)
            actor:SetHideShadow(true);
            --  5초 뒤에 제거한다. 현재 시간을 기록한다.
            action:SetParamFloat(1,g_world:GetAccumTime());
            action:SetParam(0,"WaitForDestroying");

			local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
			if kSoundID~="" then
				local actionID = action:GetID()
				if(actionID == "a_Splash Trap_Explosion" or actionID == "a_Freezing Trap Active" or actionID == "a_Silence Trap Active" or actionID == "a_MP-Zero Trap Active") then
					GetMyActor():AttachSound(20104026, kSoundID);
				else			
					g_world:AttachSound(kSoundID, actor:GetPos());
				end
			end
		elseif action:GetID() == "a_Caltrap Set" then
			--  자신의 모습은 감춘다.
            actor:HideNode("Scene Root",true)
            actor:SetHideShadow(true);
            --  5초 뒤에 제거한다. 현재 시간을 기록한다.
            action:SetParamFloat(1,g_world:GetAccumTime());
            action:SetParam(0,"WaitForDestroying");
		end
    end

end

function Skill_InstallDefault_Play_FindTarget(actor,action,bForceToBomb)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	bIsBombType = (action:GetScriptParam("TYPE") == "BOMB");

	if action:GetScriptParam("SERACH_TYPE") == "FULLSEARCH" then
		action:SetParam(4,"FullSearch");
	else
		action:SetParam(4,"OnlyCollided");
	end

	if bForceToBomb then
		action:SetParam(4,"FullSearch");
	end
	
    action:CreateActionTargetList(actor,true);
    
    if action:GetTargetCount() > 0 or bForceToBomb then
    
		if bIsBombType and bForceToBomb == false then
			action:SetParam(4,"FullSearch");
			action:CreateActionTargetList(actor,true);
		end
    
        if IsSingleMode() or action:GetTargetCount() == 0 then
			
            Skill_InstallDefault_Play_OnTargetListModified(actor,action,false);    
        
        else
        
			--ODS("Skill_InstallDefault_Play_FindTarget BroadCastTargetListModify\n");
            action:BroadCastTargetListModify(actor:GetPilot());
            action:ClearTargetList();
        
        end    
        
        return true;
        
    end
    
    return false
end


function Skill_InstallDefault_Play_OnTimer(actor, accumTime, action,iTimerID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    --ODS("Skill_InstallDefault_Play_OnTimer accumTime:"..accumTime.."\n");
        
	--  1초에 한번씩 타겟을 찾아 서버로 보낸다.
	if Skill_InstallDefault_Play_FindTarget(actor,action) then
		if action:GetScriptParam("TYPE") == "BOMB" then -- 폭발형일 경우 한번만 보내야하므로, 타이머를 제거한다. 
			return false;
		end
	end
    
    return true;

end

function Skill_InstallDefault_Play_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local   kState = action:GetParam(0);
	
	--ODS("kState : "..kState.."\n");
	
	if action:GetParamFloat(3) >0 and (accumTime - action:GetParamFloat(3)> 1) then
		Skill_InstallDefault_StartTimer(actor,action)
		action:SetParamFloat(3,0);
	end
	
	if  kState == "Idle" then
	
        --  10초가 흐르면 스스로 제거된다.
        local   fTotalElapsedTime = accumTime - action:GetParamFloat(2)
        local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
    	
        if fLifeTime == 0 then
            fLifeTime = 10.0;
        end
        
        --ODS("Skill_InstallDefault_Play_OnUpdate fTotalElapsedTime : "..fTotalElapsedTime.."\n");
        
        if fTotalElapsedTime > fLifeTime-1 then
	    
            if action:GetScriptParam("BOMB_AFTER_LIFETIME") == "TRUE" then
				if actor:IsUnderMyControl() then      
					Skill_InstallDefault_Play_FindTarget(actor,action,true)
				end
                action:SetParam(0,"Destroyed");
	            return true;
            end
			
			action:SetParam(0,"end");
			action:SetParam(1,"AutoDestroyed");
        end

	elseif  kState == "bomb_force" then --  강제 폭발(디토네이터 스킬같은거)
	
		if actor:IsUnderMyControl() then
			Skill_InstallDefault_Play_FindTarget(actor,action,true)
		end
		action:SetParam(0,"Destroyed");
		--  타이머 제거
		action:DeleteTimer(0);
	
	elseif kState == "WaitForDestroying" then
		action:SetParam(0,"end");		

	elseif kState == "end" then
		if action:GetParam(1) == "AutoDestroyed" then
			local   kBombAfterEffectID = action:GetScriptParam("BOME_AFTER_EFFECT_ID");

			if kBombAfterEffectID ~= nil then
				local kBombAfterEffectSize = tonumber(action:GetScriptParam("BOME_AFTER_EFFECT_SCALE"));
				
				if kBombAfterEffectSize ~= nil then
					GetMyActor():AttachParticleToPointS(30,actor:GetPos(), kBombAfterEffectID, kBombAfterEffectSize);
				end

				local	kSoundID = action:GetScriptParam("BOME_AFTER_SOUND_ID");
				if kSoundID ~= "" then
					if( CheckNil(g_world == nil) ) then return false end
					if( CheckNil(g_world:IsNil()) ) then return false end
					g_world:AttachSound(kSoundID, actor:GetPos());
				end
			end
		end

		return false;
	end

	return true
end

function Skill_InstallDefault_Play_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kFindType = action:GetParam(4)
	
	local	fFindRange = 50;
	local	iMaxTargetCount = 0;
	
	if kFindType == "OnlyCollided" then
		fFindRange = 10
		iMaxTargetCount = 1
	end

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;

	local iAttackRange = action:GetSkillRange(0,actor);

	local kParam = FindTargetParam();

    local   kParam = FindTargetParam();

	kParam:SetParam_1(kPos,kDir);
	kParam:SetParam_2(0,0,fFindRange,iMaxTargetCount);
	kParam:SetParam_3(true,FTO_NORMAL);

	local strFindTargetType = action:GetScriptParam("FIND_TARGET_TYPE");

	--ODS("strFindTargetType : "..strFindTargetType.."\n", false, 1509)

	local iFindTargetType = TAT_SPHERE;
	
	if strFindTargetType == "RAY" then
		kDir:Multiply(-fBackDistance);
		kPos:Add(kDir);
	    iFindTargetType = TAT_RAY_DIR;
    	kParam:SetParam_1(kPos,kDir);
    	kParam:SetParam_2(iAttackRange,30,fFindRange,iMaxTargetCount);
	
	elseif strFindTargetType == "BAR" then
		kDir:Multiply(-fBackDistance);
		kPos:Add(kDir);
	    iFindTargetType = TAT_BAR;
    	kParam:SetParam_1(kPos,kDir);
    	kParam:SetParam_2(iAttackRange,60,fFindRange,iMaxTargetCount);
	
	elseif strFindTargetType == "SPHERE" then
	    iFindTargetType = TAT_SPHERE;
    	kParam:SetParam_1(kPos,kDir);
    	kParam:SetParam_2(0,30,iAttackRange,iMaxTargetCount);
	
	end
	
	return action:FindTargets(iFindTargetType,kParam,kTargets,kTargets);

end

function Skill_InstallDefault_Play_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());

	if actor:IsUnderMyControl() then
		actor:DestroyInstallTimerGauge();
	end

	actor:DetachFrom(7212);
	actor:DetachFrom(7213);
    
	return true;
end

function Skill_InstallDefault_Play_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("Skill_InstallDefault_Play_OnLeave NextAction : "..action:GetID().."\n");
	
	local	kParam = actor:GetAction():GetParam(0)
	
	return (kParam == "end");
end
