------------------------------------------------
-- Definitions
------------------------------------------------
-- Param Definition
-- 0[int] : Action State (See Action State Definition)
-- 1[int] : Is the weapon hidden currently? ( 0 : Not Hidden 1 : Hidden)
-- 2[int] : Target Actor Count
-- 3[string] : Target Actor GUID
-- 4[float] : Fly Angle
-- 5[int] : Hand Class No
-- 6[int] : Hand Count
-- 7[float] : Hand Gap Angle
-- 8[int] : Hide Hand During Creation Animation ( 0 : Not Hide 1 : Hide)
-- 9[int] : use weapon hide or not
-- 10[int] : Num of Hand Skill Actor Guid 
-- 11 ~ 11+HandCount [int] : Hand Skill Actor Guid

-- Point Param Definition

-- GUID Container : Target Actor's GUID

-- Action State
DEATHSNATCH_ACTIONSTATE_WAIT_FIRE_TEXTKEY = 0;
DEATHSNATCH_ACTIONSTATE_WAIT_HAND_REACH_TARGET = 1;
DEATHSNATCH_ACTIONSTATE_WAIT_TARGETLISTMODIFY_PACKET = 2;
DEATHSNATCH_ACTIONSTATE_WAIT_DRAW_ANIM_DONE = 3;
DEATHSNATCH_ACTIONSTATE_FINISHED = 4;

-- Script Param
-- "FLY_ANGLE" : Angle which the hand fly. In Degree (Default : 45)
-- "HAND_CLASS_NO" : class no of the hand skill actor
-- "HAND_COUNT" : How many hands are going to be fired?(Default : 1)
-- "HAND_GAP_ANGLE" : Angle of gap between each hands(Default : 30 degree)
-- "HIDE_HAND_CREATION_ANIMATION" : "TRUE" or "FALSE", do not show hand during creation animation is on going(Default : FALSE)
-- "HIDE_WEAPON" : "TRUE" or "FALSE", hide weapon or not

------------------------------------------------
-- Event Handling Functions
------------------------------------------------
function Skill_DeathSnatch_OnCheckCanEnter(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_DeathSnatch_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_DeathSnatch_HideWeapon(actor,action,true);
	Skill_DeathSnatch_CreateAllSkillActor(actor,action);
	Skill_DeathSnatch_InitAllSkillActorAction(actor,action);
	Skill_DeathSnatch_Fire(actor,action);
	

end

function Skill_DeathSnatch_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iNumSkillActor = Skill_DeathSnatch_GetNumSkillActorGuid(action);
	local	i =0;
	
	while i<iNumSkillActor do
	
		Skill_DeathSnatch_FindTargetAtSkillActor(actor,action,kTargets,i);
		
		i=i+1;
	
	end

	
	return	kTargets:size();

end

function Skill_DeathSnatch_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local actionID = action:GetID()
	
	Skill_DeathSnatch_LoadScriptParams(actor,action);
	Skill_DeathSnatch_SetActionState(action,DEATHSNATCH_ACTIONSTATE_WAIT_FIRE_TEXTKEY);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_DeathSnatch_OnCastingCompleted(actor,action);

	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if nil~=kSoundID and kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end
	
	return true
end

function Skill_DeathSnatch_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local iActionState = Skill_DeathSnatch_GetActionState(action);
	
	if iActionState == DEATHSNATCH_ACTIONSTATE_WAIT_FIRE_TEXTKEY then
		
		--	Nothing to do. Just wait the Fire Text Key event to occur.
	
	elseif iActionState == DEATHSNATCH_ACTIONSTATE_WAIT_HAND_REACH_TARGET then
	
		Skill_DeathSnatch_Update_WaitHandReachTargetActionState(actor,action,accumTime,frameTime);
		
	elseif iActionState == DEATHSNATCH_ACTIONSTATE_WAIT_TARGETLISTMODIFY_PACKET then
		--	Nothing to do. Just wait for the TargetListModify Packet.
		local kWnd = GetUIWnd("FRM_CASH_SHOP")	-- 단 캐시샵, 오픈마켓일때 액션이 사용되고 있으면 안된다.
		local bEndAction = not kWnd:IsNil()
		if(not bEndAction) then
			kWnd = GetUIWnd("FRM_AUCTION_MAIN")
			bEndAction = not kWnd:IsNil()
		end
		if(bEndAction) then
			actor:PlayNext();
			Skill_DeathSnatch_SetActionState(action,DEATHSNATCH_ACTIONSTATE_WAIT_DRAW_ANIM_DONE);
		end
		
	elseif iActionState == DEATHSNATCH_ACTIONSTATE_WAIT_DRAW_ANIM_DONE then
	
		if false == Skill_DeathSnatch_Update_WaitDrawAnimDoneActionState(actor,action,accumTime,frameTime) then
			return	false
		end

	elseif iActionState == DEATHSNATCH_ACTIONSTATE_FINISHED then
		return	false;
	end
	

	return true
end

function Skill_DeathSnatch_OnCleanUp(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_DeathSnatch_DeleteAllSkillActor(actor,actor:GetAction());
	Skill_DeathSnatch_HideWeapon(actor,actor:GetAction(),false);

	local kTexture = action:GetScriptParam("TRAIL_TEXTURE");
	if nil ~= kTexture and "" ~= kTexture then
		actor:EndBodyTrail();
	end
	
	local   kCurAction = actor:GetAction();
	
	if kCurAction:GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end

	if kCurAction:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, false);
	end

	if kCurAction:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:EndWeaponTrail();
	end
	
	if kCurAction:GetScriptParam("DETACH_FIRE_EFFECT_AT_FINISH") == "TRUE" then
	
		local DefaultDetaChID = false;
		local DefaultDetaChID2 = false;
		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true;
		end

		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID2 = true;
		end		

	    actor:DetachFrom(7212,DefaultDetaChID)
	    actor:DetachFrom(7213,DefaultDetaChID2)
		actor:DetachFrom(7214)
	end
	
	if kCurAction:GetScriptParam("DETACH_CASTING_EFFECT_AT_FINISH") == "TRUE" then
		local DefaultDetaChID = false;
		if kCurAction:GetScriptParam("CASTING_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true;
		end
		actor:DetachFrom(7215,DefaultDetaChID)
	end
		
	local   kCleanUpEffectID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_ID");
    local   kCleanUpEffectTargetNodeID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_TARGET_NODE");
	local	fCleanUpScale = tonumber(kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_SCALE"));

	if fCleanUpScale == 0 or fCleanUpScale == nil then
		fCleanUpScale = 1.0;
	end
    
    if kCleanUpEffectID ~= "" and kCleanUpEffectTargetNodeID ~= "" then
		 actor:AttachParticleS(7216,kCleanUpEffectTargetNodeID,kCleanUpEffectID, fCleanUpScale);
	end
	
	--  모습 원래대로(이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
    local kTransformXMLID = kCurAction:GetScriptParam("TRANSFORM_XML");
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(kCurAction:GetScriptParam("TRANSFORM_ID"));
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan();
            kMan:RemoveStatusEffectFromActor(actor:GetPilotGuid(),iTransformEffectID);
        end
    end
	return true;
end

function Skill_DeathSnatch_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	local	iActionState = Skill_DeathSnatch_GetActionState(curAction);
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end
	
	if iActionState == DEATHSNATCH_ACTIONSTATE_FINISHED then 
		return true;
	end
	

	return false 
end

function Skill_DeathSnatch_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		
		Skill_DeathSnatch_SetAllSkillActorFlyingToStart(actor,action);
		
		actor:PlayNext();
		Skill_DeathSnatch_SetActionState(action,DEATHSNATCH_ACTIONSTATE_WAIT_DRAW_ANIM_DONE);

		action:GetTargetList():ApplyActionEffects();
    
    end

end

function Skill_DeathSnatch_OnEvent(actor, textKey)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());	

	if textKey == "fire" then
		
		Skill_DeathSnatch_ProcessTextKeyEvent_Fire(actor,actor:GetAction());
	
	elseif textKey == "hit" then
	
		Skill_DeathSnatch_ProcessTextKeyEvent_Hit(actor,actor:GetAction());
		
	elseif textKey == "end" then
	
		Skill_DeathSnatch_ProcessTextKeyEvent_End(actor,actor:GetAction());
	
	end

end