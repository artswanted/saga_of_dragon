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

-- [TRANSFORM_EFFECT_ID]				: 트렙이 작아질때 붙는 이펙트 
-- [TRANSFORM_EFFECT_TARGET_NODE_POS]	: 트랩이 작아질때 붙는 이펙트의 위치
-- [TRANSFORM_SOUND_ID]					: 트랩이 작아질때 나는 소리
	   
function Trap_Play_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	Trap_Play_Fire(actor,action)
	action:SetSlot(action:GetCurrentSlot()+1)
	actor:PlayCurrentSlot()

	if actor:IsUnderMyControl() then
		local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0
	        	
		if fLifeTime == 0 then
			fLifeTime = 9.0	
		end
		actor:SetInstallTimerGauge(fLifeTime)
	end

end

function Trap_Play_Fire(actor,action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
    --  시작 시간
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    action:SetParamFloat(2,g_world:GetAccumTime())
    action:SetParamFloat(3,g_world:GetAccumTime())

	if(true == actor:IsUnderMyControl()) then
		SkillHelpFunc_AttachParticleToNodePos(7215,"IF_MY_UNDERCONTROL_THEN_INSTALL_EFFECT_ID", "IF_MY_UNDERCONTROL_THEN_INSTALL_EFFECT_TARGET_NODE_POS", 1.5, actor, action)
	end
	
	SkillHelpFunc_AttachParticleToNode(7212, "INSTALL_EFFECT_ID", "INSTALL_EFFECT_TARGET_NODE", 1, actor, action)
	if(false == SkillHelpFunc_AttachParticleToNode(7213, "INSTALL_EFFECT_ID2", "INSTALL_EFFECT_TARGET_NODE2", 1, actor, action)) then
		SkillHelpFunc_AttachParticleToNodePos(7213, "INSTALL_EFFECT_ID2", "INSTALL_EFFECT_TARGET_NODE_POS2", 1, actor, action)	
	end
end

function Trap_Play_OnOverridePacket(actor, action, packet)
--[[	ODS("Trap_Play_OnOverridePacket\n", false,912)
	if(true == actor:IsUnderMyControl()) then 		
		local	kSkillTree = GetSkillTree()
		local   bLearned = kSkillTree:IsLearnedSkill(INVISIBLE_TRAP_SKILL_NO)
		if(bLearned) then
			packet:PushInt(1)
			action:SetParamInt(99, 1)
			ODS("배움\n", false, 912)
		else
			packet:PushInt(0)
			action:SetParamInt(99, 0)
			ODS("안배움\n", false, 912)
		end			
	end	
]]
end

function Trap_Play_StartTimer(actor, action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
    if actor:IsUnderMyControl() then
        
        if action:GetScriptParam("BOMB_AFTER_LIFETIME") ~= "TRUE" then
        	        
	        local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0
        	
	        if fLifeTime == 0 then
	            fLifeTime = 10.0
            end
                
            local   fAttackDelay = 0
            
            fAttackDelay = action:GetAbil(AT_ATT_DELAY)/1000.0
            --ODS("Action :"..action:GetID().." AttackDelay : "..fAttackDelay.." fLifeTime:"..fLifeTime.."\n")
            if fAttackDelay < 0.05 then
                fAttackDelay = 0.05
            end
            
            --  타이머 시작
            action:StartTimer(fLifeTime,fAttackDelay,0)
        
        end
    
    end
    
end

function Trap_Play_OnEnter(actor, action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local actionID = action:GetID()
	
	--ODS("Trap_Play_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n")
	
	action:SetParam(0,"Idle")  --  State
	action:SetDoNotBroadCast(true)
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true
	end

	Trap_Play_OnCastingCompleted(actor,action)
	
	-- 트랩 작아지게 하는 SKILL_CUSTOM_VALUE가 존재 하는가(각 스킬마다 같은 값이라도 다르게 사용할수 있음)
	if(1 == actor:GetAbil(AT_SKILL_CUSTOM_VALUE_STORAGE)) then
		action:SetParamInt(99, 1)
	else
		action:SetParamInt(99, 0)
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	action:SetParam(100,"hide")			
	action:SetParamInt(101, 0)
	action:SetParamFloat(103,1)	--알파값 0 되고 난후 유지되는 시간
	action:SetParamFloat(102,g_world:GetAccumTime()+action:GetParamFloat(103))
	action:SetParamInt(104, 0)
	action:SetParamFloat(105,g_world:GetAccumTime())
	actor:FreeMove(true)
	return true
end

function Trap_Play_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
    if bIsBefore == false then
    
		--ODS("Trap_Play_FindTarget Trap_Play_OnTargetListModified\n")
    
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
        
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
        if action:GetScriptParam("TYPE") == "BOMB" and action:GetID() ~= "a_Caltrap Set" then 
            --  폭발형일 경우
            --  폭발 이펙트를 붙인다.
            local   kBombEffectID = action:GetScriptParam("BOMB_EFFECT_ID")
            if kBombEffectID == "" then
                kBombEffectID = "ef_Magnumbreak_01_char_root"
            end
            GetMyActor():AttachParticleToPoint(1,actor:GetPos(),kBombEffectID)
            --  자신의 모습은 감춘다.
            actor:HideNode("Scene Root",true)
            actor:SetHideShadow(true)
            --  5초 뒤에 제거한다. 현재 시간을 기록한다.
            action:SetParamFloat(1,g_world:GetAccumTime())
            action:SetParam(0,"WaitForDestroying")

			local	kSoundID = action:GetScriptParam("HIT_SOUND_ID")
			if kSoundID~="" then
				local actionID = action:GetID()
				if(actionID == "a_Splash Trap_Explosion" or actionID == "a_Freezing Trap Active" or actionID == "a_Silence Trap Active" or actionID == "a_MP-Zero Trap Active") then
					GetMyActor():AttachSound(20100426, kSoundID)
				else			
					g_world:AttachSound(kSoundID, actor:GetPos())
				end
			end
		elseif action:GetID() == "a_Caltrap Set" then
			--  자신의 모습은 감춘다.
            actor:HideNode("Scene Root",true)
            actor:SetHideShadow(true)
            --  5초 뒤에 제거한다. 현재 시간을 기록한다.
            action:SetParamFloat(1,g_world:GetAccumTime())
            action:SetParam(0,"WaitForDestroying")
		end
    end

end

function Trap_Play_FindTarget(actor,action,bForceToBomb)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local	bIsBombType = (action:GetScriptParam("TYPE") == "BOMB")

	if action:GetScriptParam("SERACH_TYPE") == "FULLSEARCH" then
		action:SetParam(4,"FullSearch")
	else
		action:SetParam(4,"OnlyCollided")
	end

	if bForceToBomb then
		action:SetParam(4,"FullSearch")
	end
	
    action:CreateActionTargetList(actor,true)
    
    if action:GetTargetCount() > 0 or bForceToBomb then
    
		if bIsBombType and bForceToBomb == false then
			action:SetParam(4,"FullSearch")
			action:CreateActionTargetList(actor,true)
		end
    
        if IsSingleMode() or action:GetTargetCount() == 0 then
			
            Trap_Play_OnTargetListModified(actor,action,false)    
        
        else
        
			--ODS("Trap_Play_FindTarget BroadCastTargetListModify\n")
            action:BroadCastTargetListModify(actor:GetPilot())
            action:ClearTargetList()
        
        end    
        
        return true
        
    end
    
    return false
end

function Trap_Play_OnTimer(actor, accumTime, action,iTimerID)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
    --ODS("Trap_Play_OnTimer accumTime:"..accumTime.."\n")
        
	--  1초에 한번씩 타겟을 찾아 서버로 보낸다.
	if Trap_Play_FindTarget(actor,action) then
		if action:GetScriptParam("TYPE") == "BOMB" then -- 폭발형일 경우 한번만 보내야하므로, 타이머를 제거한다. 
			return false
		end
	end
    
    return true

end

function Trap_Play_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local action = actor:GetAction()
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	if(1 == action:GetParamInt(99)) then	-- 인비지블 트랩 스킬을 배웠어야만 가능
		if(0 == action:GetParamInt(101) and accumTime > action:GetParamFloat(102)) then
			actor:SetTargetAlpha(actor:GetAlpha(),0,1)	
			action:SetParamInt(101, 1)
			SkillHelpFunc_AttachParticleToNodePos(7214,"TRANSFORM_EFFECT_ID", "TRANSFORM_EFFECT_TARGET_NODE_POS", 2, actor, action)
			
			local actionID = action:GetID()
			if(actionID == "a_Splash Trap_Explosion" or actionID == "a_Freezing Trap Active" or actionID == "a_Silence Trap Active" or actionID == "a_MP-Zero Trap Active") then
			local kSoundID = action:GetScriptParam("TRANSFORM_SOUND_ID")
				if kSoundID~="" then
					GetMyActor():AttachSound(20100426, kSoundID);
				end		
			else
				SkillHelpFunc_AttachSoundToActorPos("TRANSFORM_SOUND_ID", actor, action)
			end
			
			--if(false == actor:IsUnderMyControl()) then
				local fTargetScale = tonumber(action:GetScriptParam("TARGET_SCALE"))
				local fTargetScaleTime = tonumber(action:GetScriptParam("TARGET_SCALE_TIME")) 			
				if("" ~= fTargetScale and "" ~= fTargetScaleTime) then
					actor:SetTargetScale(fTargetScale, fTargetScaleTime) -- 이제 점점 작아짐 
				end
			--end		
		end	
		
		-- 알파값 반짝
		if(true == actor:IsAlphaTransitDone()) then
			if("hide" == action:GetParam(100)) then 
				if(1 == action:GetParamInt(104)) then 
					if(action:GetParamFloat(105) < accumTime) then
						actor:SetTargetAlpha(0,1,1)
						action:SetParam(100,"show")
						action:SetParamInt(104, 0)
					end
				else
					action:SetParamInt(104, 1)
					action:SetParamFloat(105, accumTime+2)
				end
			else
				action:SetParam(100,"hide")
				actor:SetTargetAlpha(actor:GetAlpha(),0,1)
			end
		end
	end
	
		
	local   kState = action:GetParam(0)
	
	--ODS("kState : "..kState.."\n")
	
	if action:GetParamFloat(3) >0 and (accumTime - action:GetParamFloat(3)> 1) then
		Trap_Play_StartTimer(actor,action)
		action:SetParamFloat(3,0)
	end
	
	if  kState == "Idle" then
	
        --  10초가 흐르면 스스로 제거된다.
        local   fTotalElapsedTime = accumTime - action:GetParamFloat(2)
        local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0
    	
        if fLifeTime == 0 then
            fLifeTime = 10.0
        end
        
        --ODS("Trap_Play_OnUpdate fTotalElapsedTime : "..fTotalElapsedTime.."\n")
        
        if fTotalElapsedTime > fLifeTime-1 then
	    
            if action:GetScriptParam("BOMB_AFTER_LIFETIME") == "TRUE" then
				if actor:IsUnderMyControl() then      
					Trap_Play_FindTarget(actor,action,true)
				end
                action:SetParam(0,"Destroyed")
	            return true
            end
			
			action:SetParam(0,"end")
			action:SetParam(1,"AutoDestroyed")
        end

	elseif  kState == "bomb_force" then --  강제 폭발(디토네이터 스킬같은거)
	
		if actor:IsUnderMyControl() then
			Trap_Play_FindTarget(actor,action,true)
		end
		action:SetParam(0,"Destroyed")
		--  타이머 제거
		action:DeleteTimer(0)
	
	elseif kState == "WaitForDestroying" then
		action:SetParam(0,"end")		

	elseif kState == "end" then
		if action:GetParam(1) == "AutoDestroyed" then
			local   kBombAfterEffectID = action:GetScriptParam("BOME_AFTER_EFFECT_ID")

			if kBombAfterEffectID ~= nil then
				local kBombAfterEffectSize = tonumber(action:GetScriptParam("BOME_AFTER_EFFECT_SCALE"))
				
				if kBombAfterEffectSize ~= nil then
					GetMyActor():AttachParticleToPointS(30,actor:GetPos(), kBombAfterEffectID, kBombAfterEffectSize)
				end				
				
				local actionID = action:GetID()
				if(actionID == "a_Splash Trap_Explosion" or actionID == "a_Freezing Trap Active" or actionID == "a_Silence Trap Active" or actionID == "a_MP-Zero Trap Active") then
				local kSoundID = action:GetScriptParam("BOME_AFTER_SOUND_ID")
					if kSoundID~="" then
						GetMyActor():AttachSound(20100426, kSoundID);
					end		
				else
					SkillHelpFunc_AttachSoundToActorPos("BOME_AFTER_SOUND_ID", actor, action)
				end				
			end
		end

		return false
	end

	return true
end

function Trap_Play_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local	kFindType = action:GetParam(4)
	
	local	fFindRange = 50
	local	iMaxTargetCount = 0
	
	if kFindType == "OnlyCollided" then
		fFindRange = 10
		iMaxTargetCount = 1
	end

	local	kPos = actor:GetPos()
	local	kDir = actor:GetLookingDir()
	local	fBackDistance = 10

	local iAttackRange = action:GetSkillRange(0,actor)

	local kParam = FindTargetParam()

    local   kParam = FindTargetParam()

	kParam:SetParam_1(kPos,kDir)
	kParam:SetParam_2(0,0,fFindRange,iMaxTargetCount)
	kParam:SetParam_3(true,FTO_NORMAL)

	local strFindTargetType = action:GetScriptParam("FIND_TARGET_TYPE")

	--ODS("strFindTargetType : "..strFindTargetType.."\n", false, 1509)

	local iFindTargetType = TAT_SPHERE
	
	if strFindTargetType == "RAY" then
		kDir:Multiply(-fBackDistance)
		kPos:Add(kDir)
	    iFindTargetType = TAT_RAY_DIR
    	kParam:SetParam_1(kPos,kDir)
    	kParam:SetParam_2(iAttackRange,30,fFindRange,iMaxTargetCount)
	
	elseif strFindTargetType == "BAR" then
		kDir:Multiply(-fBackDistance)
		kPos:Add(kDir)
	    iFindTargetType = TAT_BAR
    	kParam:SetParam_1(kPos,kDir)
    	kParam:SetParam_2(iAttackRange,60,fFindRange,iMaxTargetCount)
	
	elseif strFindTargetType == "SPHERE" then
	    iFindTargetType = TAT_SPHERE
    	kParam:SetParam_1(kPos,kDir)
    	kParam:SetParam_2(0,30,iAttackRange,iMaxTargetCount)
	
	end
	
	return action:FindTargets(iFindTargetType,kParam,kTargets,kTargets)

end

function Trap_Play_OnCleanUp(actor)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())

	if actor:IsUnderMyControl() then
		actor:DestroyInstallTimerGauge()
	end

	actor:DetachFrom(7212)
	actor:DetachFrom(7213)
	actor:DetachFrom(7214)
	actor:DetachFrom(7215)

	return true
end

function Trap_Play_OnLeave(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	--ODS("Trap_Play_OnLeave NextAction : "..action:GetID().."\n")
	
	local	kParam = actor:GetAction():GetParam(0)
	
	return (kParam == "end")
end