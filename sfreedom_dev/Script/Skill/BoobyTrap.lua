-- Melee
function Skill_BoobyTrap_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_BoobyTrap_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

	Skill_BoobyTrap_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Skill_BoobyTrap_CreateActionTargetList(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if actor:IsMyActor() == false then
        return
    end
	
	local	kTargets = CreateActionTargetList(actor:GetPilotGuid(),action:GetActionInstanceID(),action:GetActionNo());
	local	iFound = Skill_BoobyTrap_OnFindTarget(actor,action,kTargetPos,kTargets);
	
	if iFound > 0 then
		-- 타겟 잡고, 액션 결과 산출해내기
		local i =0 ;
		
		while i<iFound do
		
			local	kTargetInfo = kTargets:GetTargetInfo(i);
			local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());		
			if kTargetPilot:IsNil() == false then
			
				local	kTargetActor = kTargetPilot:GetActor();
			
				local kUnitArray = NewUNIT_PTR_ARRAY()
				kUnitArray:AddUnit(kTargetPilot:GetUnit())
				local kActionResult = NewActionResultVector()
				
				action:Call_Skill_Result_Func(actor:GetPilot(), kUnitArray, kActionResult);
				local kAResult = kActionResult:GetResult(kTargetInfo:GetTargetGUID(), false)
				if kAResult:IsNil() == false and kAResult:GetInvalid() == false then
					action:AddTarget(kTargetActor:GetPilotGuid(),kTargetActor:GetHitObjectABVIndex(), kAResult)
				end
				DeleteActionResultVector(kActionResult)
				DeleteUNIT_PTR_ARRAY(kUnitArray)
			end
			i=i+1;
		end
	end
	DeleteActionTargetList(kTargets);
end

function Skill_BoobyTrap_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	

    --  통 세개가 날아가 떨어질 목표 좌표를 구하고, 통을 생성한다.
    
    local   fDistance = { 5,35,65 }
    local   kTargetPoint;

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    local   i=0;
    while i<3 do
            
        local   kStartPos = actor:GetPos();
        local   kLookDir = actor:GetLookingDir();
        
        kLookDir:Multiply(fDistance[i+1]);
        kStartPos:Add(kLookDir);
        
        kTargetPoint = g_world:FindActorFloorPos(kStartPos);
        
        --  통 생성하자.
        local   kGenPoint = actor:GetPos();
        
	    local guid = GUID("123")
	    guid:Generate()

	    action:AddNewGUID(guid);
    	
	    local pilot = g_pilotMan:NewPilot(guid, 900012, 0)
	    if pilot:IsNil() == false then
		    g_world:AddActor(guid, pilot:GetActor(), kGenPoint, 6)
			pilot:GetActor():FreeMove(true)
		    pilot:GetActor():ReserveTransitAction("a_Booby Trap_Play")
		    
		    local kAction = pilot:GetActor():GetReservedTransitAction();
		    if kAction:IsNil() == false then
				kAction:SetDoNotBroadCast(true)
    		
		        kAction:SetParamFloat(0,kTargetPoint:GetX());
		        kAction:SetParamFloat(1,kTargetPoint:GetY());
		        kAction:SetParamFloat(2,kTargetPoint:GetZ());
		        
		    end
	    end	        
        i=i+1;
    end
    
    
    action:SetParamFloat(6,-actor:GetLookingDir():GetX());
    action:SetParamFloat(7,-actor:GetLookingDir():GetY());
    action:SetParamFloat(8,-actor:GetLookingDir():GetZ());

end

function Skill_BoobyTrap_OnFindTarget(actor,action,kTargetPos,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end

	local kParam = FindTargetParam();
	
    local iFindTargetType = TAT_SPHERE;
	kParam:SetParam_1(kTargetPos,actor:GetLookingDir());
	kParam:SetParam_2(0,0,50,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	local iTargets = action:FindTargets(iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Skill_BoobyTrap_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_BoobyTrap_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_BoobyTrap_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_BoobyTrap_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
    
        local   iBarrelIndex = action:GetParamInt(5);
        
	    local   kGuid = action:GetGUIDInContainer(iBarrelIndex);
	    local   kPilot = g_pilotMan:FindPilot(kGuid);
	    local   kActor = kPilot:GetActor();
	    
	    local   kAction = kActor:GetAction();    

	    action:GetTargetList():ApplyActionEffects();
	    
	    kAction:SetParam(4,"BLOW");
	    ODS("Barrel "..iBarrelIndex.." To BLOW\n");
	    
	    action:SetParamInt(5,iBarrelIndex+1);
    
    end

end

function Skill_BoobyTrap_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	
	local   iTotalBarrel = action:GetTotalGUIDInContainer();
	local   i=0;
	
	
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

	if iTotalBarrel == 0 and animDone then
	    action:SetParam(1,"end");
	    return  false;
    end
    
    if animDone and action:GetParamInt(5) == 3 then
    
        action:SetParam(1,"end");
        return  false;
    
    end
	
	while i<iTotalBarrel do
	
	    local   kGuid = action:GetGUIDInContainer(i);
	    local   kPilot = g_pilotMan:FindPilot(kGuid);
	    local   kActor = kPilot:GetActor();
	    
	    if kActor:IsNil() == false then
	    
	        local   kAction = kActor:GetAction();
	        if kAction:IsNil() == false  then

                if kAction:GetParam(4) == "MOVING" then 
                    break
                end
                
                if kAction:GetParam(4) == "ARRIVED" then
                    
                    kAction:SetParam(4,"WAIT"); 
                    ODS("Barrel "..i.." To WAIT\n");
                    
                    --  통이 목표좌표까지 다 날아갔다.
                    --  타겟을 잡아서 서버로 보내자.
                    Skill_BoobyTrap_CreateActionTargetList(actor,action,kActor:GetPos());
                    if IsSingleMode() or action:GetTargetCount()==0 then
                        Skill_BoobyTrap_OnTargetListModified(actor,action,false);
                    else
                        action:BroadCastTargetListModify(actor,action,false);
                        action:ClearTargetList();
                    end
                
                end
                
	        end
	    
	    end
	
	    i=i+1;
	
	end
	
	return true
end

function Skill_BoobyTrap_OnCleanUp(actor)

	return true;
end

function Skill_BoobyTrap_OnLeave(actor, action)

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
