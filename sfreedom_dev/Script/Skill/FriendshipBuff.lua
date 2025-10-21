-- [ATTACK_TIME] : 공격 틱 시간을 설정
function FriendShipBuff_OnCheckCanEnter(actor,action)
	ODS("FriendShipBuff_OnCheckCanEnter\n", false, 912)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	return true	
end

function FriendShipBuff_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	action:SetDoNotBroadCast(true)
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end
	FriendShipBuff_OnCastingCompleted(actor, action)
	
	local   kFireEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS");
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= "" then
    
		local iSize = 1.0
		if "TRUE" == action:GetScriptParam("CASTING_EFFECT_SIZE_TO_SKILLRANGE") then
			iSize = iSize * (action:GetSkillRange(0,actor) / 100)
			iSize = iSize * 2 
		end
		
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPointS(7215,kDir,kFireEffectID, iSize);
            
        elseif kFireEffectTargetNodeID ~= "" then
            actor:AttachParticleS(7215,kFireEffectTargetNodeID,kFireEffectID,iSize);
			--actor:AttachParticleS(7215,kFireEffectTargetNodeID,"EF_Mon_Agnis_whirlwind_01_char_root",iSize);
			--actor:AttachParticleS(7215,kFireEffectTargetNodeID,"ef_Aram_skill_02_03_char_root",iSize);
			--actor:AttachParticleS(7215,kFireEffectTargetNodeID,"ef_Enchant_clw_02_l",iSize);
			--actor:AttachParticleS(7215,kFireEffectTargetNodeID,"ef_Enchant_clw_03_l",iSize);
			--actor:AttachParticleS(7215,kFireEffectTargetNodeID,"ef_legend_R_01",iSize);
			--actor:AttachParticleS(7215,kFireEffectTargetNodeID,"ef_Magma_Call_01_char_root",iSize);
			--actor:AttachParticleS(7215,kFireEffectTargetNodeID,"EF_Mon_Agnis_whirlwind_01_char_root",iSize);
			--actor:AttachParticleS(7215,kFireEffectTargetNodeID,"EF_Mon_Penryl_attk_03_char_root",iSize);
			--actor:AttachParticleS(7215,kFireEffectTargetNodeID,"ef_Myroom_Buf_01_B",iSize);
			--actor:AttachParticleS(7215,kFireEffectTargetNodeID,"ef_rest_01_char_root",iSize);	

		else
			actor:AttachParticleToPointS(7215,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID,iSize);
        end
		action:SetParam(3,"1");
    end	
	
	local	kSoundID = action:GetScriptParam("CASTING_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID);
	end	
--[[	
	local kAxis1 = Point3(1,0,0)	
	local kQuat = Quaternion(90, kAxis1)
	local kActorRot = actor:GetRotateQuaternion()
	Quat = kActorRot:Multiply(kQuat)
]]
	local   kMarkEffectID = action:GetScriptParam("MARK_EFFECT_ID");    
	if(kMarkEffectID ~= "") then
		local kPos = actor:GetPos()
		--kPos:SetZ(kPos:GetZ()+ 70)
		kPos:SetZ(kPos:GetZ()-20)
		actor:AttachParticleToPointWithRotate(128, kPos, kMarkEffectID, Quaternion(0, Point3(0,0,1)), 5)
	end
	
	local	kSoundID = action:GetScriptParam("ENTER_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end	
	
	return true
end

function FriendShipBuff_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

end

function FriendShipBuff_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local iSlotNum = action:GetCurrentSlot();

	local animDone = actor:IsAnimationDone()
	if(animDone == true) then
		ODS("animDone\n", false, 912)
		--actor:PlayCurrentSlot();		
		action:SetParam(1, "end")
		return false
	end
	--actor:PlayCurrentSlot()
	--ODS("FriendShipBuff_OnUpdate\n", false, 912)	
	
	
	
	return true
end

function FriendShipBuff_Fire(actor, action)
	
	if actor:IsMyActor() == true then		
		action:CreateActionTargetList(actor);	    
		if IsSingleMode() or action:GetTargetCount() == 0 then	    
			FriendShipBuff_OnTargetListModified(actor,action,false);	    
		else	    
			action:BroadCastTargetListModify(actor:GetPilot(), true);
			action:ClearTargetList();
		end
	end

	--actor:AttachParticleWithRotate(10000, "p_ef_heart", "ef_SiegeDragon_skill_03_01_char_root", 2, false)
	--actor:AttachParticleWithRotate(10000, "char_root", "ef_Resurrection_01_01_char_root", 2, false)	
	--	actor:AttachParticleWithRotate(10000, "char_root", "ef_Resurrection_01_02_char_root", 1, false)	
	--actor:AttachParticleWithRotate(10000, "char_root", "ef_Resurrection_02_01_char_root", 1.5, false)	
	--actor:AttachParticleWithRotate(10000, "char_root", "ef_Resurrection_02_02_char_root", 1.5, false)	
	--actor:AttachParticleWithRotate(10000, "p_ef_heart", "ef_Resurrection_03_01_p_ef_heart", 1, false)
	--actor:AttachParticleWithRotate(10000, "char_root", "ef_Resurrection_03_02_char_root", 2, false)
	--actor:AttachParticleWithRotate(10000, "char_root", "ef_Master_machine_skill_02_01_p_ef_foot_04", 4, false)	
	
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	local   kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS");
	local   kFireEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE");

    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= ""  or kFireEffectTargetNodeIDToPosWithRotate ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPoint(7212,kDir,kFireEffectID);            
        elseif kFireEffectTargetNodeID ~= "" then
            actor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID);
		elseif kFireEffectTargetNodeIDToPos ~= "" then
			actor:AttachParticleToPoint(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID);			
		else
		    actor:AttachParticleToPointWithRotate(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPosWithRotate), kFireEffectID, actor:GetRotateQuaternion())
        end    
    end
	
	--번쩍이는 이펙트
	local kPos = actor:GetPos()
	kPos:SetZ(kPos:GetZ()+40)
	kPos:SetX(kPos:GetX()+20)						
	actor:AttachParticleToPointS(7220, kPos ,"ef_Enchant_clw_02_l", 5);
			
end

function FriendShipBuff_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	  local   kCurAction = actor:GetAction();
	
	ODS("RestoreItemModel Act_Melee_Default_ReallyLeave\n");
	--	무기 원래대로
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
	
	local   kMarkEffectID = kCurAction:GetScriptParam("MARK_EFFECT_ID");    
	if(kMarkEffectID ~= "") then
		actor:DetachFrom(128,true)
	end
	actor:DetachFrom(7220)
	
	-- 모습 원래대로(이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
    local kTransformXMLID = kCurAction:GetScriptParam("TRANSFORM_XML");
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(kCurAction:GetScriptParam("TRANSFORM_ID"));
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan();
            kMan:RemoveStatusEffectFromActor(actor:GetPilotGuid(),iTransformEffectID);
        
        end    
    end
end

function FriendShipBuff_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	ODS("actionID:"..actionID.."\n",false, 912)
	 
	if actor:IsMyActor() == false then
		return true;
	end
	
	 if "a_hi" ~= actionID then 
		return true		
	end	
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	if action:GetEnable() == false then
		if curParam == actionID then
			curAction:SetParam(0,"");
		end
	end
	
	if action:GetEnable() == true then
		curAction:SetParam(0,"GoNext");
		return false;
	end

	return true
end

function FriendShipBuff_OnCastingCompleted(actor, action)	
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end

function FriendShipBuff_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction();
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	if textKey == "hit" or textKey == "fire" then	
		FriendShipBuff_Fire(actor, action)
				
	end
	
	return	true;
end