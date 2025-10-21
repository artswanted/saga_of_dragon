-- [ACTOR_CLASS_NO] : 생성할 스킬 액터 클래스번호
-- Melee
function Skill_Iceberg_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_Iceberg_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Iceberg_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Skill_Iceberg_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("RestoreItemModel Skill_Iceberg_Fire\n");
    action:SetParamInt(2,0) --  State
	
end
function Skill_Iceberg_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end

    local   kTargetPos = actor:GetPos();
    local   kLookDir = actor:GetLookingDir();
    kLookDir:Multiply(iAttackRange);
    kTargetPos:Add(kLookDir);
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    kTargetPos = g_world:FindActorFloorPos(kTargetPos);

	local kParam = FindTargetParam();
    local iFindTargetType = TAT_SPHERE;
	kParam:SetParam_1(kTargetPos,actor:GetLookingDir());
	kParam:SetParam_2(0,0,47,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	local iTargets = action:FindTargets(iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Skill_Iceberg_CreateSkillActor(actor,action)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    ODS("Skill_Iceberg_CreateSkillActor\n");
    
    --  얼음 생성
    local   iAttackRange = action:GetSkillRange(0,actor);
    if iAttackRange == 0 then   
        iAttackRange = 100
    end
    
    local   kTargetPos = actor:GetPos();
    local   kLookDir = actor:GetLookingDir();
    kLookDir:Multiply(iAttackRange);
    kTargetPos:Add(kLookDir);
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    kTargetPos = g_world:FindActorFloorPos(kTargetPos);
    
	local guid = GUID("123")
	guid:Generate()
	
	action:AddNewGUID(guid);
	
	local   iClassNo = tonumber(action:GetScriptParam("ACTOR_CLASS_NO"));
	
	local pilot = g_pilotMan:NewPilot(guid, iClassNo, 0)
	if pilot:IsNil() == false then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, pilot:GetActor(), kTargetPos, 6)
		local kAction = pilot:GetActor():ReserveTransitAction("a_Iceberg_Play")
		kAction:AddNewGUID(actor:GetPilotGuid());
	end	 
	
end

function Skill_Iceberg_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_Iceberg_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Iceberg_OnCastingCompleted(actor,action);
	
	return true
end
function Skill_Iceberg_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

    if bIsBefore == false then
    
        local   kIceBerg = Skill_Iceberg_GetSkillActor(actor,action)
        if kIceBerg == nil then
            actor:PlayNext();        
            action:SetParamInt(2,2);
            return false;
        end
        
        --  얼음을 깨지게 한다.
        local   kIceBergAction = kIceBerg:GetAction()    
        if kIceBergAction:IsNil() == false then
        
        
            kIceBergAction:SetTargetList(action:GetTargetList());
            action:ClearTargetList();
            
            kIceBergAction:SetParam(2,"CRASH");
        
        end
        
        -- 스킬 마무리 동작 시작
        actor:PlayNext();
        action:SetParamInt(2,2);
        
    end    

end

function Skill_Iceberg_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
    local   iState = action:GetParamInt(2);
    
    ODS("Skill_Iceberg_OnUpdate iState : "..iState.."\n");

	
	if animDone == true then
	
	    if iState == 0 then
	    
	        --  IceBerg 생성
	        Skill_Iceberg_CreateSkillActor(actor,action)
	        action:SetParamInt(2,1);
	        
	    elseif iState == 1 then
	    
	        if actor:IsMyActor() then
	            --  생성이 완료되었다면, 타겟을 잡아 서버로 보낸다.
	            local   kIceBerg = Skill_Iceberg_GetSkillActor(actor,action)
	            if kIceBerg == nil then
	                return true;
                end
                
                
                local   kIceBergAction = kIceBerg:GetAction()
                if kIceBergAction:IsNil() == false and kIceBergAction:GetParam(2) == "CREATED" then
                
                    action:CreateActionTargetList(actor);
                    
                    if IsSingleMode() or action:GetTargetCount() == 0 then
                        Skill_Iceberg_OnTargetListModified(actor,action,false)            
                    else    
                    
                        action:BroadCastTargetListModify(actor:GetPilot());
                        action:ClearTargetList();
                    end
                
                end
	            
	        
	        end
	        
	    elseif iState == 2 then
	        
	        action:SetParam(1,"end");
	        return false;
	    
	    end

	end

	return true
end

function Skill_Iceberg_GetSkillActor(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    ODS("Skill_Iceberg_GetSkillActor ["..action:GetTotalGUIDInContainer().."]\n");
    if action:GetTotalGUIDInContainer() > 0 then
    
        local   kGUID = action:GetGUIDInContainer(0);
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
        local   kActor = g_world:FindActor(kGUID);
        if kActor:IsNil() == false then
            ODS("Skill_Iceberg_GetSkillActor kActor:IsNil() Passed\n");
        
            return  kActor;
        
        end
    end
    
    return  nil;

end

function Skill_Iceberg_OnCleanUp(actor)
    	
    --  생성해놓은 IceBerg 가 있다면 제거한다.
  
	return true;
end

function Skill_Iceberg_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
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
	
	return false 
end
    