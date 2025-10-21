
-- Paladin (타임 리버스 : a_Time Reverse) : Level 1-10
function Effect_Begin105501001(unit, iEffectNo, actarg)
--	local effect = unit:GetEffect(iEffectNo);
--	local kEffectTargetList = effect:GetTargetList();

	--InfoLog(5, "Effect_Begin105501001 : iEffectNo : "..iEffectNo)

--	kEffectTargetList:Add(unit:GetGuid());

	-- 실제 효과 이펙트	
--	local iNewEffectNo = effect:GetAbil(AT_EFFECTNUM1);
	--InfoLog(5, "Effect_Begin105501001 : iNewEffectNo : "..iNewEffectNo)
--	unit:AddEffect(iNewEffectNo, 0, actarg, unit)
	
	return 1
end
                                                                                                                          
function Effect_End105501001(unit, iEffectNo, actarg)
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

function Effect_Tick105501001(unit, elapsedtime, effect, actarg)
	local iNeed = effect:GetAbil(AT_MP)
	local iMP = unit:GetAbil(AT_MP)

	-- 실제 효과 이펙트	
	local iNewEffectNo = effect:GetAbil(AT_EFFECTNUM1);
		
	--InfoLog(5, "Effect_Tick105501001 : iNewEffectNo : "..iNewEffectNo)
	--NeedMP값이 -로 세팅되어있다.
	if -iNeed > iMP then
		--InfoLog(5, "Effect_Tick105501001 : NeedMP : "..iNeed.. "  MP : "..iMP)

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
		InfoLog(5, "Effect_Tick105501001 Ground is NILL")
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
		--InfoLog(5, "Effect_Tick105501001 : iRange "..iRange)
		while kMemberGuid:IsNil() == false do
			--InfoLog(5, "Effect_Tick105501001 : Party: index "..iIndex)
			-- 타겟 리스트에 없으면
			if false == kEffectTargetList:IsGuid(kMemberGuid) then
				--InfoLog(5, "Effect_Tick105501001 : kEffectTargetList:IsGuid() == false")
				local kMember = kGround:GetUnit(kMemberGuid)
				if kMember:IsNil() == false then
					-- Effect 사거리에 들어오면 추가
					if IsInRange(kMyPos, kMember:GetPos(), iRange, 30) == true then
						--InfoLog(5, "Effect_Tick105501001 : Not InTargetList : InRange ")
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
						--InfoLog(5, "Effect_Tick105501001 : InTargetList : Not InRange ")
						kEffectTargetList:Delete(kMemberGuid);
						kMember:DeleteEffect(iNewEffectNo);
					-- Effect 사거리안에 있으면
					else
						--InfoLog(5, "Effect_Tick105501001 : InTargetList : InRange ")
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

	--InfoLog(9, "Effect_Tick105501001 NeedMP : " .. iNeed.."   C_MP:"..iMP)
		
	unit:SetAbil(AT_MP, iMP+iNeed, true, false)
	return ECT_DOTICK
end



-- Paladin (타임 리버스<효과> : a_Time Reverse) : Level 1-10
function Effect_Begin1055010001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin1055010001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 쿨 타임 감소
	local iAdd = kEffect:GetAbil(AT_ADD_COOL_TIME)
	kEffectMgr:AddAbil(AT_ADD_COOL_TIME, iAdd)
	unit:NftChangedAbil(AT_ADD_COOL_TIME, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin1055010001...CoolTime" .. iAdd ..")");

	-- 캐스팅 타임 감소
	iAdd = kEffect:GetAbil(AT_ADD_CAST_TIME)
	kEffectMgr:AddAbil(AT_ADD_CAST_TIME, iAdd)
	unit:NftChangedAbil(AT_ADD_CAST_TIME, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin1055010001...CastTime" .. iAdd ..")");
		
	return 1
end
                                                                                                                          
function Effect_End1055010001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End1055010001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 쿨 타임 증가
	local iAdd = kEffect:GetAbil(AT_ADD_COOL_TIME)
	kEffectMgr:AddAbil(AT_ADD_COOL_TIME, 0-iAdd)
	unit:NftChangedAbil(AT_ADD_COOL_TIME, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin1055010001...CoolTime" .. iAdd ..")");

	-- 캐스팅 타임 증가
	iAdd = kEffect:GetAbil(AT_ADD_CAST_TIME)
	kEffectMgr:AddAbil(AT_ADD_CAST_TIME, 0-iAdd)
	unit:NftChangedAbil(AT_ADD_CAST_TIME, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin1055010001...CastTime" .. iAdd ..")");
	
			
	return 1
end

function Effect_Tick1055010001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick1055010001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
