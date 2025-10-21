function Effect_BossBattleDress_Begin(actor, iEffectID, iEffectKey)
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	if( actor:IsUnitType(UT_PLAYER) ) then
		local iEffectNo = 60301; -- 아군이 아닐경우
		if( actor:IsMyActor() 
			or IsInPartyMemberGuid( actor:GetPilotGuid() )	-- 내 파티원
			or IsInExpeditionMember( actor:GetPilotGuid() )	-- 내 원정대 이거나			
		)then 
			iEffectNo = 60201;	-- 아군일경우
		end
		actor:AddEffect(iEffectNo, 0, GUID(), 0, 0, true);
	end
	
end

function Effect_BossBattleDress_OnTick(actor, iEffectID, fAccumTime)
end

function Effect_BossBattleDress_End(actor, iEffectID, iEffectKey)	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	if( actor:IsUnitType(UT_PLAYER) ) then 
		GetStatusEffectMan():RemoveStatusEffectFromActor( actor:GetPilotGuid(), 60201);
		GetStatusEffectMan():RemoveStatusEffectFromActor( actor:GetPilotGuid(), 60301);
	end
end