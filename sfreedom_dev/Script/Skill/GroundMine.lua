
-- Melee
function Skill_GroundMine_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_GroundMine_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_GroundMine_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function Skill_GroundMine_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    action:SetParamFloat(6,-actor:GetLookingDir():GetX());
    action:SetParamFloat(7,-actor:GetLookingDir():GetY());
    action:SetParamFloat(8,-actor:GetLookingDir():GetZ());

end

function Skill_GroundMine_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end

	local	kPos = Point3(action:GetParamFloat(2),action:GetParamFloat(3),action:GetParamFloat(4));

	local kParam = FindTargetParam();
	
    local iFindTargetType = TAT_SPHERE;
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(0,0,50,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	local iTargets = action:FindTargets(iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Skill_GroundMine_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_GroundMine_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_GroundMine_OnCastingCompleted(actor,action);
	
	return true
end
function Skill_GroundMine_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
    
        --  폭탄을 터뜨리고 타겟들에게 대미지 이펙트 뿌린다.
        actor:DetachFrom(378001);        
    
    	local	kPos = Point3(action:GetParamFloat(2),action:GetParamFloat(3),action:GetParamFloat(4));
    	
    	actor:AttachParticleToPoint(378002,kPos,"ef_Magnumbreak_01_char_root");
    	action:GetTargetList():ApplyActionEffects();
    	
    	action:SetParam(1,"fire");
    
    end

end

function Skill_GroundMine_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	local iSlotNum = action:GetCurrentSlot();

    if 	iSlotNum == 1 then
    
        if animDone then
        
            if actor:IsMyActor() then
            
                --  타겟 잡아 서버로 보낸다.
                action:CreateActionTargetList(actor);
                
                if IsSingleMode() or action:GetTargetCount() == 0 then
                   Skill_GroundMine_OnTargetListModified(actor,action,false);
                else
                    action:BroadCastTargetListModify(actor:GetPilot());
                    action:ClearTargetList();
                end
                
            
            end
            
            --  뒤로 점프한다.
            actor:PlayNext();
        
        end
        
    elseif iSlotNum == 2 then
    
        if animDone and action:GetParam(1) == "fire" then
        
            action:SetParam(1,"end");
            return  false;
            
        end
    
        
        if animDone == false then
            
	        local movingSpeed = 0

	        if IsSingleMode() == true then
		        movingSpeed = 120 
	        else
		        movingSpeed = actor:GetAbil(AT_C_MOVESPEED)
	        end        
    	    
	        local   kMovingDir = Point3(action:GetParamFloat(6),action:GetParamFloat(7),action:GetParamFloat(8));
	        kMovingDir:Multiply(movingSpeed*0.6);
    	    
	        actor:SetMovingDelta(kMovingDir);
	    
	    end
    
    end

	return true
end

function Skill_GroundMine_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    actor:DetachFrom(378001);
    
	return true;
end

function Skill_GroundMine_OnLeave(actor, action)

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

function Skill_GroundMine_CreateMine(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

    local   iAttackRange = action:GetSkillRange(0,actor);
    if iAttackRange == 0 then
        iAttackRange = 50
    end
    
    local   kPos = actor:GetPos();
    local   kLookDir =actor:GetLookingDir();
    kLookDir:Multiply(iAttackRange);
    kPos:Add(kLookDir);
    
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    kPos = g_world:FindActorFloorPos(kPos);
    
    kPos:SetZ(kPos:GetZ()-25);
    
    action:SetParamFloat(2,kPos:GetX());
    action:SetParamFloat(3,kPos:GetY());
    action:SetParamFloat(4,kPos:GetZ());
    
    actor:AttachParticleToPoint(378001,kPos,"ef_Ground_mine");

end


function Skill_GroundMine_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
	
	    --  Mine 생성
	    Skill_GroundMine_CreateMine(actor,kAction);
	
	end

	return	true;
end
