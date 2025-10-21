-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.

-- Melee
function HammerCrush_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function HammerCrush_OnCastingCompleted(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	HammerCrush_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function HammerCrush_Fire(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iSkillLevel = action:GetAbil(AT_LEVEL);

	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") == "" then
		if iSkillLevel == 5 then
			actor:ChangeItemModel(ITEM_WEAPON, "Item/Common/12_Weapon/09_special/100t_hammer.xml");
		elseif iSkillLevel >= 3 then
			actor:ChangeItemModel(ITEM_WEAPON, "Item/Common/12_Weapon/09_special/50t_hammer.xml");
		else
			actor:ChangeItemModel(ITEM_WEAPON, "Item/Common/12_Weapon/09_special/10t_hammer.xml");
		end
	else
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
	end

	actor:AttachSound(106,"HammerCrush_Start");
	
	actor:StartWeaponTrail();
	actor:ResetAnimation();
end
function HammerCrush_OnFindTarget(actor,action,kTargets)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	action:SetParam(0,"");
	return	HammerCrush_FindTargetEx(actor,action,action:GetActionNo(),action:GetSkillRange(0,actor),kTargets);
end

function HammerCrush_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 60
	end	

	local	kMovingDir = actor:GetLookingDir();
	kMovingDir:Multiply(iAttackRange);
	local	kTargetPos = actor:GetNodeWorldPos("char_root");
	kTargetPos:Add(kMovingDir);

	local kParam = FindTargetParam();
	kParam:SetParam_1(kTargetPos,actor:GetLookingDir());
	kParam:SetParam_2(0,iParam2,iParam2,0);
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN);
	
	return	action:FindTargetsEx(iActionNo,TAT_SPHERE,kParam,kTargets,kTargets);
end

function HammerCrush_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	HammerCrush_OnCastingCompleted(actor,action);
	
	return true
end

function HammerCrush_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
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

function HammerCrush_OnCleanUp(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	actor:RestoreItemModel(ITEM_WEAPON);
	
	actor:EndWeaponTrail();
	return true;
end

function HammerCrush_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local curAction = actor:GetAction();
	if( CheckNil(nil==curAction) ) then return false end
	if( CheckNil(curAction:IsNil()) ) then return false end
	
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


function HammerCrush_OnTargetListModified(actor,action,bIsBefore)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
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

function HammerCrush_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" then
	
	    kAction:SetParam(2,"HIT");
	
		local	fDistance = kAction:GetSkillRange(0,actor);
	    local	kMovingDir = actor:GetLookingDir();
	    kMovingDir:Multiply(fDistance);
	    local	kTargetPos = actor:GetNodeWorldPos("char_root");
	    kTargetPos:Add(kMovingDir);

		if kAction:GetID() == "a_Hammer Crush" then
			actor:AttachParticleToPoint(8,kTargetPos,"ef_Hammer_Crush_01");
			actor:AttachSound(106,"HammerCrush");
		elseif kAction:GetID() == "a_Fire Youth" then
			actor:AttachParticleToPoint(8,kTargetPos,"ef_Fire_hammer_01");
			actor:AttachSound(106,"Fire_Hammer");
		elseif kAction:GetID() == "a_Heartless Life" then
			actor:AttachParticleToPoint(8,kTargetPos,"ef_Ice_hammer_01");
			actor:AttachSound(106,"Ice_Hammer");
		end

	    if actor:IsMyActor() then
    	    kAction:CreateActionTargetList(actor);   --  타겟 잡기
	    
	        if IsSingleMode() then
	            HammerCrush_OnTargetListModified(actor,kAction,false)
	        else
	            kAction:BroadCastTargetListModify(actor:GetPilot());
	        end
	    
	    end
				
		if actor:IsMyActor() then
			QuakeCamera(g_fMeleeDropQuakeValue[1]
			, g_fMeleeDropQuakeValue[2]
			, g_fMeleeDropQuakeValue[3]
			, g_fMeleeDropQuakeValue[4]
			, g_fMeleeDropQuakeValue[5])
		end		
								
	end

	return	true;
end
