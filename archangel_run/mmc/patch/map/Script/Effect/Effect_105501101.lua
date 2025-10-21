
-- Paladin (오펜시브 오라 : a_Offecsive Aura) : Level 1-5
function Effect_Begin105501101(unit, iEffectNo, actarg)
--	local effect = unit:GetEffect(iEffectNo);
--	local kEffectTargetList = effect:GetTargetList();

	--InfoLog(5, "Effect_Begin105501101 : iEffectNo : "..iEffectNo)

--	kEffectTargetList:Add(unit:GetGuid());

	-- 실제 효과 이펙트	
--	local iNewEffectNo = effect:GetAbil(AT_EFFECTNUM1);
	--InfoLog(5, "Effect_Begin105501101 : iNewEffectNo : "..iNewEffectNo)
--	unit:AddEffect(iNewEffectNo, 0, actarg, unit)
	
	return 1
end
                                                                                                                          
function Effect_End105501101(unit, iEffectNo, actarg)
	local effect = unit:GetEffect(iEffectNo);
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround);

	-- 실제 효과 이펙트	
	local iNewEffectNo = effect:GetAbil(AT_EFFECTNUM1);
	local kEffectTargetList = effect:GetTargetList();
	local nListSize = kEffectTargetList:GetSize();
	
	local nCount = 0;

	while nCount < nListSize do
		local kTargetGuid = kEffectTargetList:Get(nCount);
		if kTargetGuid:IsNil() == false then
			local kTarget = kGround:GetUnit(kTargetGuid);
			kTarget:DeleteEffect(iNewEffectNo);
		end
		nCount = nCount + 1;
	end

	kEffectTargetList:Clear();

	return 1
end

function Effect_Tick105501101(unit, elapsedtime, effect, actarg)
	local iNeed = effect:GetAbil(AT_MP)
	local iMP = unit:GetAbil(AT_MP)

	-- 실제 효과 이펙트	
	local iNewEffectNo = effect:GetAbil(AT_EFFECTNUM1);
		
	--InfoLog(5, "Effect_Tick105501101 : iNewEffectNo : "..iNewEffectNo)
	--NeedMP값이 -로 세팅되어있다.
	if -iNeed > iMP then
		--InfoLog(5, "Effect_Tick105501101 : NeedMP : "..iNeed.. "  MP : "..iMP)

		return ECT_MUSTDELETE
	end

	-- Party Member를 얻어온다.
	local kGround = actarg:Get(ACTARG_GROUND)
	if kGround == nil then
		InfoLog(5, "Effect_Tick105500901 Ground is NILL")
		return ECT_MUSTDELETE
	end
	
	kGround = Ground(kGround)
	if kGround:IsNil() then
		InfoLog(5, "Effect_Tick105501101 Ground is NILL")
		return ECT_MUSTDELETE
	end
	
	local kMyPos = unit:GetPos()
	local iRange = effect:GetAbil(AT_DETECT_RANGE)	
	if unit:GetPartyGuid():IsNil() == false then
		local kPartyMgr = GetLocalPartyMgr()	
		local kGuidVector = NewVecGuid()
		kPartyMgr:GetPartyMember(unit:GetPartyGuid(), kGuidVector)
		local iIndex = 0
		local kEffectTargetList = effect:GetTargetList();
		local kMemberGuid = kGuidVector:Get(iIndex)
		--InfoLog(5, "Effect_Tick105501101 : iRange "..iRange)
		while kMemberGuid:IsNil() == false do
			--InfoLog(5, "Effect_Tick105501101 : Party: index "..iIndex)
			-- 타겟 리스트에 없으면
			if false == kEffectTargetList:IsGuid(kMemberGuid) then
				--InfoLog(5, "Effect_Tick105501101 : kEffectTargetList:IsGuid() == false")
				local kMember = kGround:GetUnit(kMemberGuid)
				if kMember:IsNil() == false then
					-- Effect 사거리에 들어오면 추가
					if IsInRange(kMyPos, kMember:GetPos(), iRange, 30) == true then
						--InfoLog(5, "Effect_Tick105501101 : Not InTargetList : InRange ")
						if kMember:AddEffect(iNewEffectNo, 0, actarg, unit):IsNil() == false then
							-- Effect가 추가 되면 리스트에 추가
							kEffectTargetList:Add(kMemberGuid);
						end
					end
				end
			-- 타겟 리스트에 있으면
			else
				local kMember = kGround:GetUnit(kMemberGuid)
				if kMember:IsNil() == false then
					-- Effect 사거리를 벗어나면 삭제
					if IsInRange(kMyPos, kMember:GetPos(), iRange, 30) == false then
						--InfoLog(5, "Effect_Tick105501101 : InTargetList : Not InRange ")
						kEffectTargetList:Delete(kMemberGuid);
						kMember:DeleteEffect(iNewEffectNo);
					-- Effect 사거리안에 있으면
					else
						--InfoLog(5, "Effect_Tick105501101 : InTargetList : InRange ")
						kMember:AddEffect(iNewEffectNo, 0, actarg, unit);
					end
				end
			end
			iIndex = iIndex + 1
			kMemberGuid = kGuidVector:Get(iIndex)
		end
		DeleteVecGuid(kGuidVector)
	else --파티가 아닐 경우 Player한테만 걸어 준다.
		local kEffectTargetList = effect:GetTargetList();
		if false == kEffectTargetList:IsGuid(unit:GetGuid()) then
			unit:AddEffect(iNewEffectNo, 0, actarg, unit)
			kEffectTargetList:Add(unit:GetGuid());			
		 end
	end

	--InfoLog(9, "Effect_Tick105501101 NeedMP : " .. iNeed.."   C_MP:"..iMP)
		
	unit:SetAbil(AT_MP, iMP+iNeed, true, false)
	return ECT_DOTICK
end



-- Paladin (오펜시브 오라<효과> : a_Offecsive Aura) : Level 1-5
function Effect_Begin1055011001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin1055011001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 공격력 증가
	local iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	-- 마법 공격력 증가
	iAdd = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
		
	return 1
end
                                                                                                                          
function Effect_End1055011001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End1055011001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 공격력 감소
	local iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	-- 마법 공격력 감소
	iAdd = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
			
	return 1
end

function Effect_Tick1055011001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick1055011001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
