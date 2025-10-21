-- Melee
function Skill_Meteor_OnCheckCanEnter(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_Meteor_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Meteor_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end

function	Skill_Meteor_LoadArrowToWeapon(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());

	local	iAttackRange = action:GetSkillRange(0,actor);
	local	kPos = actor:GetPos();
	local	kMovingDir = actor:GetLookingDir();
	local	iRandom = math.random(-20,20);
	if iRandom == 0 then
		iRandom = 1;
	end

	kMovingDir:Multiply(iAttackRange/3+iRandom);
	kPos:Add(kMovingDir);
	kPos:SetZ(kPos:GetZ()+150);
	
	kNewArrow:SetWorldPos(kPos);
	
	return  kNewArrow;
end

function Skill_Meteor_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
    
        --  타겟들을 향해 메테오를 떨어뜨리자.

	    local	iAttackRange = action:GetSkillRange(0,actor)
	    local	kArrow = Skill_Meteor_LoadArrowToWeapon(actor,action);
    	
	    local	targetobject = nil;

	    if kArrow:IsNil() == false then
    	
		    local	kPos = actor:GetPos();
		    local	kMovingDir = actor:GetLookingDir();
			-- 랜덤으로 떨어질수 있게 수정			
			local iStartDistance = -action:GetAbil(AT_SKILL_BACK_DISTANCE) + iAttackRange/2
			local iRandDist = iStartDistance + math.random(-iAttackRange/3, iAttackRange/3)
		    kMovingDir:Multiply(iRandDist);
			
		    kPos:Add(kMovingDir);
		    kPos:SetZ(kPos:GetZ()-20);
			 
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
		    local	kPos2 = g_world:ThrowRay(kPos,Point3(0,0,-1),500);	--	바닥으로 레이를 쏘자
		    local	iTargetCount = action:GetTargetCount();
    		
		    actor:LookAt(kPos);
    	
		    kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
    		
		    if iTargetCount>0 then
			    kArrow:SetTargetObjectList(action:GetTargetList());	
		    end

		    if kPos2:GetX() == -1 and 
			    kPos2:GetY() == -1 and
			    kPos2:GetZ() == -1 then
			    --kPos2 = g_world:ThrowRay(actor:GetPos(), Point3(0,0,-1), 500)
				kPos2 = kPos
		    end

		    kArrow:SetTargetLoc(kPos2);
		    kArrow:Fire();
			local ptcl = kArrow:GetTargetLoc();
			ptcl:SetZ(ptcl:GetZ()+5)
			kArrow:SetParamValue("Scale", "".. 4);
			kArrow:SetScale(tonumber(kArrow:GetParamValue("Scale")));
			--actor:AttachParticleToPointS(1509, ptcl, "ef_Meteorstrike_01_char_root" , tonumber(kArrow:GetParamValue("Scale")));
    	
	    end
	    action:ClearTargetList();        
	    action:SetParam(1,"fire");
    
    end

end


function Skill_Meteor_Fire(actor,action)

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(2,0);    --  State   	
	local iMaxFireCnt = action:GetAbil(AT_COUNT)
	if(0 == iMaxFireCnt) then
		iMaxFireCnt = 1
	end
	action:SetParamInt(3, iMaxFireCnt);
	action:SetParamInt(4,0);	
end
--[[
function Skill_Meteor_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iAttackRange = action:GetSkillRange(0,actor)
	
	if iAttackRange == 0 then
	    iAttackRange = 100
    end

	local	kPos = actor:GetPos();
	local	kMovingDir = actor:GetLookingDir();

	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(0,30,iAttackRange,0);	
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN+FTO_BLOWUP);
	
	return	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets)
end
]]
function Skill_Meteor_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	--ODS("Skill_Meteor_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");

	local	kPos = actor:GetPos();
    local	kMovingDir = actor:GetLookingDir();
	local	iAttackRange = action:GetSkillRange(0,actor);
	local iStartDistance = -action:GetAbil(AT_SKILL_BACK_DISTANCE) + iAttackRange/2 -- 이곳이 중점이므로 여기에 마법진 표시
    kMovingDir:Multiply(iStartDistance);
	kPos:Add(kMovingDir);
	
	ODS("iStartDistance:"..iStartDistance.."\n", false, 912)
	
	local fScale = iAttackRange/19.2
	actor:AttachParticleToPointS(1509, kPos, "ef_Meteorstrike_01_char_root" , fScale);
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Meteor_OnCastingCompleted(actor,action);	

	return true
end

function Skill_Meteor_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local   bAnimDone = actor:IsAnimationDone();
	local   iState =action:GetParamInt(2);
	local iMaxFireCnt = action:GetParamInt(3);
	--ODS("iMaxFireCnt"..iMaxFireCnt.."\n", false, 912)
	local iCurFireCnt = action:GetParamInt(4);
	if iState == 0 then
	
	    if bAnimDone then
	    
            action:CreateActionTargetList(actor);   --  타겟 리스트를 만들고
            
            if IsSingleMode() == false and action:GetTargetCount()>0 then
                action:BroadCastTargetListModify(actor:GetPilot());    -- 서버로 브로드캐스팅
                action:ClearTargetList();
            else
                Skill_Meteor_OnTargetListModified(actor,action,false);
                
            end
			iCurFireCnt = iCurFireCnt+1
			if(iCurFireCnt == iMaxFireCnt) then
				actor:PlayNext();
				action:SetParamInt(2,1);
			else
				actor:PlayCurrentSlot();
				action:SetParamInt(4, iCurFireCnt)
			end

			--남성용 사운드
			if 1 == actor:GetAbil(AT_GENDER) then
				actor:AttachSound(2785,"Male_Strike");
			--여성용 사운드
			else
				actor:AttachSound(2785,"Female_Strike");
			end
			
	    
	    end
	
	elseif iState == 1 then
	
        if action:GetParam(1) == "fire" and bAnimDone then
            action:SetParam(1,"end");
            return  false
        end	
	
	end

	return true
end

function Skill_Meteor_OnCleanUp(actor)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local   kCurAction = actor:GetAction();
	CheckNil(nil==kCurAction);
	CheckNil(kCurAction:IsNil());
	
	actor:DetachFrom(1509);	
	return true;
end

function Skill_Meteor_OnLeave(actor, action)

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
	
	if actionID == "a_jump" or "a_run" then 
		if( false == actor:PlayNext()) then 
			actor:GetAction():SetNextActionName("a_battle_idle");
		end
		actor:GetAction():ChangeToNextActionOnNextUpdate(true,true);
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

function Skill_Meteor_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
        
        if actor:IsMyActor() then 

        end
        
	end

	return	true;
end