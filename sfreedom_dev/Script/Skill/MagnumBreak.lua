-- Melee
function Skill_MagnumBreak_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function Skill_MagnumBreak_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_MagnumBreak_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Skill_MagnumBreak_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:StartWeaponTrail();
	actor:AttachSound(105,"BrokenMagnum_Start");
end
function Skill_MagnumBreak_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	return	Skill_MagnumBreak_FindTargetEx(actor,action,action:GetActionNo(),action:GetSkillRange(0,actor),kTargets);
end

function Skill_MagnumBreak_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	fEffectRange = 60;

	local	kMovingDir = actor:GetLookingDir();
	local	kTargetPos = actor:GetNodeWorldPos("char_root");
	kTargetPos:Add(kMovingDir);

	local kParam = FindTargetParam();
	kParam:SetParam_1(kTargetPos,actor:GetLookingDir());
	kParam:SetParam_2(0,30,fEffectRange,0);
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN);
	
	return	action:FindTargetsEx(iActionNo,TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_MagnumBreak_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_MagnumBreak_OnCastingCompleted(actor,action);	
	
	return true
end

function Skill_MagnumBreak_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then
	
	
		local curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			nextActionName = action:GetParam(1);

			action:SetParam(0, "null")
			action:SetParam(1, "end")
			
			action:SetNextActionName(nextActionName)
			return false;
		else
		
			action:SetParam(1, "end")
			return false
		end
	end

	return true
end

function Skill_MagnumBreak_OnCleanUp(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:RestoreItemModel(ITEM_WEAPON);
	
	actor:EndWeaponTrail();
	return true;
end

function Skill_MagnumBreak_OnLeave(actor, action)

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
	
	if action:GetEnable() == false then
		
		if curParam==actionID then
			curAction:SetParam(0,"");
		end
		
	end	
	
	if action:GetEnable() == true then
	

		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);

		return false;

	end

	return false 
end


function Skill_MagnumBreak_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if action:GetParam(2) == "HIT" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            if actor:IsMyActor()  then
				action:GetTargetList():ApplyActionEffects(true,true);
            else
                SkillHelpFunc_DefaultHitOneTime(actor,action, true);
            end
        
        end        
    
    end
    
    
end

function Skill_MagnumBreak_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
		-- 이펙트가 붙는 위치
		local	fDistance = kAction:GetSkillRange(0,actor);		
		
	if textKey == "start" then		
		local	kMovingDir = actor:GetLookingDir();
		kMovingDir:Multiply(fDistance);
		local	kTargetPos = actor:GetNodeWorldPos("char_root");
		kTargetPos:Add(kMovingDir);
		actor:AttachParticleToPointWithRotate(8,kTargetPos,"ef_Magnumbreak_03_char_root", actor:GetRotateQuaternion());

	end
	if textKey == "hit" then
	
	    kAction:SetParam(2,"HIT");
	
		actor:AttachSound(105,"BrokenMagnum");

		local	kMovingDir = actor:GetLookingDir();
		kMovingDir:Multiply(fDistance);
		local	kTargetPos = actor:GetNodeWorldPos("char_root");
		kTargetPos:Add(kMovingDir);
		actor:AttachParticleToPointWithRotate(9,kTargetPos,"ef_Magnumbreak_02_char_root", actor:GetRotateQuaternion());

		actor:DetachFrom(8);

	    if actor:IsMyActor() then
    	    kAction:CreateActionTargetList(actor);   --  타겟 잡기
	    
	        if IsSingleMode() then
	            Skill_MagnumBreak_OnTargetListModified(actor,kAction,false)
	        else
	            kAction:BroadCastTargetListModify(actor:GetPilot());
	        end
	    
	    end
				
		if actor:IsMyActor() then
			QuakeCamera(0.2, 3, 1, 1, 1);
		end
	end

	return	true;
end
