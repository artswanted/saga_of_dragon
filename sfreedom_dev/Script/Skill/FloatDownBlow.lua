function Skill_FloatDownBlow_OnCastingCompleted(actor,action)
end

function Skill_FloatDownBlow_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	ODS("Skill_FloatDownBlow_OnCheckCanEnter\n");
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	
	if actor:IsMeetFloor() then
		return	false
	end
	
	if actor:GetFloatHeight(30)<40 then
		ODS("Skill_FloatDownBlow_OnCheckCanEnter actor:GetFloatHeight(30):"..actor:GetFloatHeight(30).."\n");
		return	false;	
	end
	
	return		true;
	
end

function Skill_FloatDownBlow_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();

	ODS("Skill_FloatDownBlow_OnEnter action : .."..action:GetID().."\n");

	if actor:IsMyActor() == true then
		action:SetParam(4, "");
		action:SetParam(0,"");
	end
	
	actor:FreeMove(true);	
	actor:StopJump();
	
	actor:ChangeItemModel(ITEM_WEAPON, "Item/Common/12_Weapon/09_special/bonghammer.xml");	
	
	return true
end

function Skill_FloatDownBlow_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = actor:GetPos();
--	local	kDir = actor:GetLookingDir();
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 30
	end
	

	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor),iParam2,0,0);
	kParam:SetParam_3(true,FTO_BLOWUP);
	
	local iTargetCount = action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	ODS("Skill_FloatDownBlow_OnFindTarget iTargetCount:"..iTargetCount.."\n");
	
	return	iTargetCount;
	
end

function Skill_FloatDownBlow_Finished(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local	actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();
	local nextActionName = action:GetNextActionName()
	
	actor:SetNormalAttackEndTime();
			
	local curActionParam = action:GetParam(0)
	if curActionParam == "GoNext" then
		nextActionName = action:GetParam(1);
		action:SetNextActionName(nextActionName)
	end

	actor:FreeMove(false);
	
	action:SetParam(0, "null")
	action:SetParam(1, "end")

	actor:RestoreItemModel(ITEM_WEAPON);

	return false
end

function Skill_FloatDownBlow_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local actionID = action:GetID();
	
	if animDone == true then
		return Skill_FloatDownBlow_Finished(actor,action);
	end

	return true
end

function Skill_FloatDownBlow_OnCleanUp(actor, action)
	actor:FreeMove(false);
	actor:EndWeaponTrail();

	if action:GetID() == "a_jump" then
		action:SetSlot(2)
		action:SetDoNotBroadCast(true)
	end
	
	actor:RestoreItemModel(ITEM_WEAPON);
	

	return true;
end

function Skill_FloatDownBlow_OnLeave(actor, action)

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
	
	if curParam == "end" and actionID == "a_jump" then 
		action:SetSlot(2)
		action:SetDoNotBroadCast(true)
		return  true;
	end
	
	if action:GetActionType()=="EFFECT" then
		
		actor:SetComboCount(0);	
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	if action:GetEnable() == false then
		
		if kNextActionName==actionID then
			curAction:SetParam(0,"");
		end
		
	end
	
	if action:GetEnable() == true then
	
		local	bCorrect = false;
		
		if 	actionID == "a_run" then
			
			curAction:SetParam(0,"GoNext");
			curAction:SetParam(1,actionID);
			
		end

		return false;
	
	elseif curParam == "end" and actionID == "a_run" then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Skill_FloatDownBlow_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return
		end

        if action:GetParam(2) == "ATTACKED" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
			SkillHelpFunc_DefaultHitOneTime(actor,action, true);
        end        
    
    end

end

function Skill_FloatDownBlow_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	

	if  textKey == "hit" and kAction:GetParamInt(20) == 0 then

		if actor:GetPilot():GetBaseClassID() == CT_FIGHTER then
			
			actor:AttachParticle(31,"char_root","ef_fig_jump_attk_01_char_root");
		
		end
	
		if actor:IsUnderMyControl() then
		
			kAction:CreateActionTargetList(actor);
			if IsSingleMode() then
				Skill_FloatDownBlow_OnTargetListModified(actor,kAction,false)
			else
				kAction:BroadCastTargetListModify(actor:GetPilot());
			end			
		end
	
	
		kAction:SetParam(2,"ATTACKED");		
		
		kAction:SetParamInt(20,1);	--	hit 키가 두개 있네!! 이런, 그래도 한번만 때리게 하자.
			
	end
	return	true;
end
