-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID

-- Melee
function Skill_ChainLightning_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function Skill_ChainLightning_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_ChainLightning_Fire(actor,action);
	--action:SetSlot(action:GetCurrentSlot());
	actor:PlayCurrentSlot();
	actor:CutSkillCasting();

end

function Skill_ChainLightning_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    ODS("Skill_ChainLightning_OnTargetListModified\n");
    
    --  타겟 리스트가 바뀌었다.
    
    if bIsBefore then
    
        --Skill_ChainLightning_AttachLightningEffect(actor,action,false); --  기존의 타겟들에게서 라이트닝 대미지 이펙트를 제거한다.
        action:ClearTargetList();

    else
	    Skill_ChainLightning_CreateLinkedPlane(actor,action);   --  LinkedPlane 재생성
	    --Skill_ChainLightning_AttachLightningEffect(actor,action,true);  --  새로운 타겟들에게 라이트닝 대미지 이펙트를 붙인다.
	    
	    action:GetTargetList():ApplyActionEffects();
	    
    end
    
end


function Skill_ChainLightning_Fire(actor,action)
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("RestoreItemModel Skill_ChainLightning_Fire\n");
	
	action:SetParamInt(2,-1)    --  그룹 번호
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(3,g_world:GetAccumTime()-1)  --  타겟리스트 생성 시간	
	action:SetParamInt(4,0); -- 이전 타겟리스트의 사이즈
end
function Skill_ChainLightning_CreateActionTargetList(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local   kTargetList = action:GetTargetList();
    local   iTargetCount = kTargetList:size();

	action:SetParamInt(4, iTargetCount); -- 이전 타겟리스트의 사이즈를 저장
    action:CreateActionTargetList(actor);

end

function Skill_ChainLightning_UpdateTarget(actor,action,accumTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if actor:IsMyActor() == false then
        return
    end

    --  1초에 한번씩 새로 타겟을 찾아 서버로 보낸다.
    local   fLastUpdateTime = action:GetParamFloat(3);
    if accumTime - fLastUpdateTime <= 0.5 then
        return
    end
    
    --Skill_ChainLightning_AttachLightningEffect(actor,action,false); --  기존의 타겟들에게서 라이트닝 대미지 이펙트를 제거한다.
    
    action:SetParamFloat(3,accumTime);

	--  서버로 브로드캐스팅
	if actor:IsMyActor() == true then
		action:CreateActionTargetList(actor);
		action:BroadCastTargetListModify(actor:GetPilot(), true);
	end
	
	Skill_ChainLightning_CreateLinkedPlane(actor,action);   --  LinkedPlane 재생성
	--Skill_ChainLightning_AttachLightningEffect(actor,action,true);  --  새로운 타겟들에게 라이트닝 대미지 이펙트를 붙인다.

end

function Skill_ChainLightning_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end

    local   kMyHandPos = actor:GetNodeWorldPos("p_wp_l_hand");
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);

	local kParam = FindTargetParam();
	
	--  먼저 전방의 한놈을 찾는다.
	kParam:SetParam_1(kMyHandPos,actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,action:GetAbil(AT_1ST_AREA_PARAM2),0,1);
	kParam:SetParam_3(true,FTO_NORMAL);
	local iTargets = action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	-- 찾지 못했다면 그냥 리턴~
	if iTargets == 0 then
	    return  iTargets;
    end
    
    --  찾은 놈 주변으로 스킬 타겟 갯수만큼 찾는다.
    local   kTargetInfo = kTargets:GetTargetInfo(0);
    if kTargetInfo:IsNil() then
        return  0
    end
    
    local   kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
    if kTargetPilot:IsNil() then
        return  0
    end
    
    local   kTargetActor = kTargetPilot:GetActor();
    if kTargetActor:IsNil() then
        return  0
    end    
    
    local   kTargetPos = kTargetActor:GetPos();

	local kMaxTarget = action:GetAbil(AT_MAX_TARGETNUM);

	if 0 == kMaxTarget then
		kMaxTarget = action:GetAbil(AT_LEVEL) * 2;
	end

	kMaxTarget = kMaxTarget - 1;

	kParam:SetParam_1(kTargetPos,actor:GetLookingDir());
	kParam:SetParam_2(0,0,action:GetAbil(AT_2ND_AREA_PARAM1),kMaxTarget);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	iTargets = action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
	
	ODS("Skill_ChainLightning_OnFindTarget iTargets:"..iTargets.."\n");
	
	return iTargets;        
    
end
function Skill_ChainLightning_AttachLightningEffect(actor,action,bAttach)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kTargetList = action:GetTargetList();
    local   iTargetCount = kTargetList:size();
    
    if iTargetCount == 0 then
        return
    end    
end

function Skill_ChainLightning_CreateLinkedPlane(actor,action,path)

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kMan = GetLinkedPlaneGroupMan();
       
    local   iGroupNum = action:GetParamInt(2);
    local   kGroup;
    if iGroupNum == -1 then
        --  그룹을 새로 생성한다.
        kGroup = kMan:CreateNewLinkedPlaneGroup();
		if nil==path or ""==path then
			path = "../Data/5_Effect/9_Tex/ef_chainlight_ani_001.dds"
		end

        kGroup:SetTexture(path);
        action:SetParamInt(2,kGroup:GetInstanceID());
        
    else
        --  이미 그룹이 있다면 그것을 사용하자.
        kGroup = kMan:GetLinkedPlaneGroup(iGroupNum);
        kGroup:ReleaseAllPlane();
    end

    local kPlane = kGroup:AddNewPlane(127);    --  기본으로 1개는 생성한다.
    kPlane:SetWidth(20);
    
    local   kTargetList = action:GetTargetList();
    local   iTargetCount = kTargetList:size();
    
    local   i=0;

    while i<iTargetCount-1 do
        local kPlane = kGroup:AddNewPlane(127);
        kPlane:SetWidth(20);
        i=i+1;
    end

end
function Skill_ChainLightning_UpdatePlanePos(actor,action,node,end_node)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kMan = GetLinkedPlaneGroupMan();
    
    local   iGroupNum = action:GetParamInt(2);

    local kGroup = kMan:GetLinkedPlaneGroup(iGroupNum);
    
    local   kTargetList = action:GetTargetList();
    local   iTargetCount = kTargetList:size();
    
    local   i=0;
    
    local   kPlane = kGroup:GetPlane(i);

    --  첫번째 플레인의 StartPos 는 나의 좌표이다.
	if nil==node or ""==node then node = "p_wp_l_hand" end
    local   kMyHandPos = actor:GetNodeWorldPos(node);
    kPlane:SetStartPos(kMyHandPos);
	
    if iTargetCount == 0 then
    
        --  타겟이 하나도 없다면 AttackRange 만큼 앞으로 한줄기의 라이트닝 플레인을 만든다.
        local kLookDir = actor:GetLookingDir();
	    local  iAttackRange = action:GetSkillRange(0,actor);
	    if iAttackRange == 0 then
	        iAttackRange = 100
	    end
        kLookDir:Multiply(iAttackRange);
        kLookDir:Add(kMyHandPos);
        
        kPlane:SetEndPos(kLookDir);
    
        return
    end    
    
    local   kFirstPos;
    
    while i<iTargetCount do
    
        local   kTargetInfo = kTargetList:GetTargetInfo(i);
        local   kTargetGUID = kTargetInfo:GetTargetGUID();
        local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
        local   bActorInvalid = true;
        
        if kTargetPilot:IsNil() == false then
        
            local   kTargetActor = kTargetPilot:GetActor();
            if kTargetActor:IsNil() == false then
				local kTargetPos = nil
				if nil==end_node then
					kTargetPos = kTargetActor:GetPos();
				else
					kTargetPos = kTargetActor:GetNodeWorldPos(end_node);
				end
                kPlane:SetEndPos(kTargetPos);
                kPlane:SetEnable(true);
                bActorInvalid = false;
                
                if kFirstPos == nil then
                    kFirstPos = kTargetPos
                end
                
            end
        
        end
        
        kPlane:SetEnable(bActorInvalid == false);   --  유효하지 않은 액터라면 플레인을 비활성화시킨다.
                
        i=i+1;
        
        kPlane = kGroup:GetPlane(i);
   
        if kPlane:IsNil() then
            break;
        end

        if kFirstPos ~= nil then
            kPlane:SetStartPos(kFirstPos);
        else
            kPlane:SetStartPos(kMyHandPos);
        end
        
    end

end

function Skill_ChainLightning_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actorID = actor:GetID()
	local actionID = action:GetID()
	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_ChainLightning_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_ChainLightning_OnCastingCompleted(actor,action);
	actor:SetParam("shot", "false");
	
	return true
end

function Skill_ChainLightning_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	local iCurrentSlot = action:GetCurrentSlot();

	if animDone == true then
		action:SetSlot(action:GetCurrentSlot()+1);
		actor:PlayCurrentSlot();
		actor:SetParam("shot", "true");
		actor:AttachParticle(9221,"char_root","ef_Chainlightning_01_char_root");
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		Skill_ChainLightning_UpdateTarget(actor,action,g_world:GetAccumTime());

		Skill_ChainLightning_CreateLinkedPlane(actor,action);
		--Skill_ChainLightning_AttachLightningEffect(actor,action,true);
		local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end	
	end
	
	if iCurrentSlot == 1 then
		if action:GetActionParam() == AP_FIRE and actor:GetParam("shot") == "true" then
		
			Skill_ChainLightning_UpdateTarget(actor,action,accumTime);
			Skill_ChainLightning_UpdatePlanePos(actor,action);

			--if action:GetTargetCount() == 0 then
			--	Skill_ChainLightning_AttachLightningEffect(actor,action,false);
			--end

			if Skill_ChainIsEffect(actor,action) == false then
				return false;
			end
		end
	end

	return true
end

function Skill_ChainLightning_OnCleanUp(actor)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    --  그룹을 제거한다.
    local   iGroupNum = actor:GetAction():GetParamInt(2);
    if iGroupNum ~= -1 then
        local   kMan = GetLinkedPlaneGroupMan();
        kMan:ReleaseLinkedPlaneGroup(iGroupNum);
        actor:GetAction():SetParamInt(2,-1);
    end
    
    Skill_ChainLightning_AttachLightningEffect(actor,actor:GetAction(),false);  --  라이트닝 대미지 이펙트 제거
    
    actor:DetachFrom(9221);
    
	return true;
end

function Skill_ChainLightning_OnLeave(actor, action)

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
	
	if action:GetEnable() then
	
	    if actionID == "a_run" then
	        return  true;
	    end
	
	end
	
	return false 
end

function Skill_ChainIsEffect(actor, action) --체인 이펙트가 걸려 있는가?

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == false then
        return true;
    end

	local kPilot = actor:GetPilot();
	if kPilot:IsNil() == false then
		local kUnit = kPilot:GetUnit();
		if kUnit:IsNil() == false then
			local kSkillDef = GetSkillDef(action:GetActionNo());
			if kSkillDef:IsNil() == false then
				local kEffect = kUnit:GetEffect(kSkillDef:GetEffectNo(), true)
		 		if kEffect:IsNil() == false then
					return true;
				end
			end
		end
	end

	action:SetParam(1,"end")
	return false;
end
