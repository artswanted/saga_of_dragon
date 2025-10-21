
-- Melee
function Skill_WheelWind_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());	
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_WheelWind_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_WheelWind_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	
	action:SetParamAsPoint( 100, actor:GetLookingDir() )  -- 액션이 종ㄹ되고 난후 바라볼 방향을 설정해주기 위해 저장
end


function Skill_WheelWind_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	ODS("RestoreItemModel Skill_WheelWind_Fire\n");
	
	action:SetParamFloat(2,g_world:GetAccumTime()); --  시작 시간
	action:SetParamFloat(3,actor:GetLookingDir():GetX());
	action:SetParamFloat(4,actor:GetLookingDir():GetY());
	action:SetParamFloat(5,actor:GetLookingDir():GetZ());	
	
	action:SetParamFloat(6,0);
	
	actor:AttachParticle(29912,"char_root","ef_Wheelwind_01_char_root");
	actor:AttachSound(2000, "WheelWind");

end
function Skill_WheelWind_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	
	ODS("Skill_WheelWind_OnFindTarget iAttackRange:"..iAttackRange.."\n");
	if iAttackRange == 0 then
	    iAttackRange = 100
	end

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();

	local kParam = FindTargetParam();
	
	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(0,0,iAttackRange,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	local iTargets = action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Skill_WheelWind_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_WheelWind_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_WheelWind_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_WheelWind_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    ODS("Skill_WheelWind_OnTargetListModified\n");
    
    if bIsBefore == false then
        --  때린다.
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end
end

function Skill_WheelWind_UpdateTarget(actor,action,accumTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if actor:IsMyActor() == false then
        return
    end
    
    --  1초에 한번씩 타겟을 새로 찾는다.
    local   fLastUpdateTime = action:GetParamFloat(6);
    if accumTime - fLastUpdateTime <0.5 then
        return
    end
    
    action:SetParamFloat(6,accumTime);

    --  새로 타겟을 찾는다.        
    action:CreateActionTargetList(actor);
    
	--  서버로 브로드캐스팅
	if action:GetTargetCount() > 0 then
	    action:BroadCastTargetListModify(actor:GetPilot());
	end
	
	action:ClearTargetList();
	
	ODS("Skill_WheelWind_UpdateTarget\n");
	    
end

function Skill_WheelWind_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actionID = action:GetID();
	local movingSpeed = 0
	
	if IsSingleMode() == true then
		movingSpeed = 50 
	else
		movingSpeed = actor:GetAbil(AT_C_MOVESPEED)
	end
	
	Skill_WheelWind_UpdateTarget(actor,action,accumTime);

    --  캐릭터 회전 시키기
	local	fRotateTime = accumTime - action:GetParamFloat(2);
	
	if fRotateTime>2 then
	    action:SetParam(1,"end");
	    return  false
	end
	
	actor:IncRotate(frameTime*6*460.0*3.141592/180.0);
	
	local kMovingDir = Point3(action:GetParamFloat(3),action:GetParamFloat(4),action:GetParamFloat(5));
	kMovingDir:Multiply(movingSpeed*0.8);
	
	actor:SetMovingDelta(kMovingDir);
	
	return true
end

function Skill_WheelWind_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
			
	actor:DetachFrom(29912);
	local kCurAction = actor:GetAction()
	if(nil ~= kCurAction and false == kCurAction:IsNil()) then	
		local kLookingDir = kCurAction:GetParamAsPoint(100)
		local kLookAtPos = actor:GetPos()
		kLookAtPos:Add(kLookingDir)
		actor:LookAt(kLookAtPos)
	end

	return true;
end

function Skill_WheelWind_OnLeave(actor, action)

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
