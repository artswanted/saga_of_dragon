
-- Melee
function Skill_NineteenNinety_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
		
	if(true == actor:IsInCoolTime(action:GetActionNo(), false) ) then
		return false
	end
	
	local curAction = actor:GetAction();
	if( false == curAction:IsNil() ) then
		local kActionName = curAction:GetID();
		if("a_head_spin" == kActionName or "a_Windmill" == kActionName) then
			curAction:SetNextActionName("a_Nineteen Ninety");
			curAction:ChangeToNextActionOnNextUpdate(true, true);
		end
	end
	return		true;
	
end
function Skill_NineteenNinety_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

	Skill_NineteenNinety_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end

function Skill_NineteenNinety_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	action:SetParamFloat(3,g_world:GetAccumTime()); --  시작 시간
	action:SetParamFloat(6,0);
	action:SetParamInt(7,0)	--	Hit Count
	action:SetParamInt(8,2)	--	Max Count

	actor:HideParts(EQUIP_POS_WEAPON, true);
	
	if false == actor:CheckStatusEffectExist("se_transform_to_Dancer") then
		actor:AttachParticle(19002,"char_root","ef_NineteenNine_01_char_root");
	end
	
	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end
end

function Skill_NineteenNinety_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
		
	local  iAttackRange = action:GetSkillRange(0,actor);
	
	--ODS("Skill_NineteenNinety_OnFindTarget iAttackRange:"..iAttackRange.."\n");
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

function Skill_NineteenNinety_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	--ODS("Skill_NineteenNinety_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_NineteenNinety_OnCastingCompleted(actor,action);

	return true
end
function Skill_NineteenNinety_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end

		local	iTotalHit = action:GetParamInt(8);
		if iTotalHit == nil then
			iTotalHit = 1
		end
		
        if iTotalHit>1 or action:GetParamInt(7) > 0  then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
			SkillHelpFunc_DefaultHitOneTime(actor,action, true);
        end        
    
    end
end

function Skill_NineteenNinety_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actionID = action:GetID();
--	local movingSpeed = 0
	local bAnimDone = actor:IsAnimationDone();
	local iSlot = action:GetCurrentSlot();
	local nextActionName = action:GetNextActionName();

	--ODS("Skill_NineteenNinety_OnUpdate NextActionName : "..nextActionName.." slot :" ..iSlot .. "\n");

	if iSlot == 1 then
	    if bAnimDone then
			action:SetSlot(action:GetCurrentSlot()+1);
	        actor:PlayCurrentSlot();
	        action:SetParamFloat(3,accumTime);
	    end    
    elseif iSlot == 2 then	
		local   fElapsedTime = accumTime - action:GetParamFloat(3);
		--ODS("fElapsedTime = "..fElapsedTime.."\n");
    	if bAnimDone or fElapsedTime > 0.533 then 
    	
    	    action:SetSlot(action:GetCurrentSlot()+1);
	        actor:PlayCurrentSlot();
			if false == actor:CheckStatusEffectExist("se_transform_to_Dancer") then
				actor:DetachFrom(19002,true);
			end
    	end
    else
        if bAnimDone then        
            action:SetParam(1,"end");
            return false;        
        end    
    end

	return true
end

function Skill_NineteenNinety_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if false == actor:CheckStatusEffectExist("se_transform_to_Dancer") then
		actor:DetachFrom(19002);
	end
	--actor:DetachFrom(19003);
	actor:HideParts(EQUIP_POS_WEAPON, false);
	return true;
end

function Skill_NineteenNinety_OnLeave(actor, action)

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

function Skill_NineteenNinety_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	-- 최대 힛트 수를 넘게 되면 더 이상 때리지 못하도록 한다.
	if kAction:GetParamInt(7) >= kAction:GetParamInt(8) then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
	
		if actor:IsUnderMyControl() then
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
				kAction:CreateActionTargetList(actor);
				kAction:BroadCastTargetListModify(actor:GetPilot());
			end	
		end
	
		kAction:SetParamInt(7, kAction:GetParamInt(7) + 1)	--	Hit Count
	end

	return	true;
end
