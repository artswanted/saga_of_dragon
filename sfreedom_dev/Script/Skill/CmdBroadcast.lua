-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [NO_WEAPON_TRAIL] : 웨폰 트레일 사용하지 않을것인가

-- Melee
function Cmd_BroadCast_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Cmd_BroadCast_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
end

function Cmd_BroadCast_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("RestoreItemModel Cmd_BroadCast_Fire\n");	
end

function Cmd_BroadCast_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());   
end

function Cmd_BroadCast_OnTimer(actor,fAccumTime,action,iTimerID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("Cmd_BroadCast_OnTimer fAccumTime:"..fAccumTime.."\n");
	return	true	

end

function Cmd_BroadCast_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	return	0;
end

function Cmd_BroadCast_OnOverridePacket(actor, action, packet)
	local iType = action:GetParamInt(100)
	packet:PushInt(iType)
	ODS("Cmd_BroadCast_OnOverridePacket\n", false, 912)
	--	local kEndPos = action:GetParamAsPoint(500)
	--nil ~= string.find(action:GetID(), "a_bite_off")s
end


function Cmd_BroadCast_OnEnter(actor, action)
	ODS("Cmd_BroadCast_OnEnter\n", false, 912)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();

	Cmd_BroadCast_OnCastingCompleted(actor,action);
	
-- 자신이 아닐때는 패킷을 받은거임	
--[[
	local iType = 0
	if actor:IsUnderMyControl() then
		iType = action:GetParamInt(100)
	else
		local kPacket = action:GetParamAsPacket()
		if nil==kPacket or kPacket:IsNil() then
			return false
		end
		iType = kPacket:PopInt()		
	end
	
	if(0 < iType) then
		if(1 == iType) then 
			action:SetNextActionName("a_Multiple Shot")
		elseif (2== iType) then
			action:SetNextActionName("a_Arrow Shower")
		elseif (3== iType) then
			action:SetNextActionName("a_Padong_Succeed_0")
		end
	else
		action:SetNextActionName("a_battle_idle")
	end
	]]
	
	local kNextActionName = action:GetParam(100000)	
	if(nil ~= kNextActionName and "" ~= kNextActionName) then
		--ODS("kNextActionName:"..kNextActionName.."\n", false, 912)
		action:SetNextActionName(kNextActionName)
	end
			
	action:ChangeToNextActionOnNextUpdate(true,true);
	
	action:SetParam(1, "end")
	return true
end

function Cmd_BroadCast_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	
	return false
end

function Cmd_BroadCast_OnCleanUp(actor)
	return true;
end

function Cmd_BroadCast_OnLeave(actor, action)

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
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end

	return false 
end

function Cmd_BroadCast_HitOneTime(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

end

function Cmd_BroadCast_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kAction = actor:GetAction();
			
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	return	true;
end
