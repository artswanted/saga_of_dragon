
-- 기사 (제네식기어-수호 : Genesic Gear - Protect ) : Level 1-10
function Effect_Begin105000201(unit, iEffectNo, actarg)
	return 1
end
                                                                                                                          
function Effect_End105000201(unit, iEffectNo, actarg)
	return 1
end

function Effect_Tick105000201(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick105000201 Effect=" .. effect:GetEffectNo())
	--InfoLog(9, "Effect_Tick105000201 --")
	local iNewEffect = effect:GetAbil(AT_EFFECTNUM1)
	-- 파티에 가입했는가?
	local kPartyGuid = unit:GetPartyGuid()
	if kPartyGuid:IsNil() == true then
		unit:AddEffect(iNewEffect, 0, actarg, unit)
		return ECT_TICKBREAK
	end
	
	-- Party Member를 얻어온다.
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	if kGround:IsNil() then
		InfoLog(5, "Effect_Tick105000201 Ground is NILL")
		return ECT_MUSTDELETE
	end
	local kMyPos = unit:GetPos()
	local iRange = effect:GetAbil(AT_DETECT_RANGE)
	local kPartyMgr = GetLocalPartyMgr()
	local kGuidVector = NewVecGuid()
	kPartyMgr:GetPartyMember(unit:GetPartyGuid(), kGuidVector)
	local iIndex = 0
	local kMemberGuid = kGuidVector:Get(iIndex)
	while kMemberGuid:IsNil() == false do
		local kMember = kGround:GetUnit(kMemberGuid)
		if kMember:IsNil() == false then
			if IsInRange(kMyPos, kMember:GetPos(), iRange, 0) == true then
				kMember:AddEffect(iNewEffect, 0, actarg, unit)
			end
		end
		iIndex = iIndex + 1
		kMemberGuid = kGuidVector:Get(iIndex)
	end
	DeleteVecGuid(kGuidVector)
	return ECT_TICKBREAK
end
