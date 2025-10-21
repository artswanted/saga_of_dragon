function Use_Item20200001(who, item, posGroup, posKey, arg)
	-- 20200001 : 회복포션
	--InfoLog(8, "Use_Item20200001 --")
	local iAddHP = item:GetAbil(AT_HP)
	local iMaxHP = who:GetAbil(AT_C_MAX_HP)
	local iCurrentHP = who:GetAbil(AT_HP)
	-- HP 올리고
	if iMaxHP <= iCurrentHP then
		return false
	else 
		who:SetAbil(AT_HP, math.min(iMaxHP, iCurrentHP+iAddHP), false)
	end
	local iAddMP = item:GetAbil(AT_MP)
	local iMaxMP = who:GetAbil(AT_C_MAX_MP)
	local iCurrentMP = who:GetAbil(AT_MP)
	-- MP 올리고
	if iMaxMP <= iCurrentMP then
		return false
	else 
		who:SetAbil(AT_MP, math.min(iMaxMP, iCurrentMP+iAddMP), false)
	end	

	--InfoLog(8, "Use_Item20200001 20")	
	-- 회복포션 한개 줄이고
	local Inven = who:GetInventory()
	if Inven:IsNil() == true then
		who:SetAbil(AT_HP, iCurrentHP, false)
		who:SetAbil(AT_MP, iCurrentMP, false)
		return false
	end
	
	--InfoLog(8, "Use_Item20200001 30")
	local kIPacket = NewPacket(PT_M_C_ITEMCHANGED)
	if Inven:RemoveItem(posGroup, posKey, 1, kIPacket) == false then
		who:SetAbil(AT_HP, iCurrentHP, false)
		who:SetAbil(AT_MP, iCurrentMP, false)
		return false
	end
	--InfoLog(8, "Use_Item20200001 40")
	-- 상태정보 보내고
	Net_Send(who, kIPacket)
	DeletePacket(kIPacket)
	
	who:SendAbil(AT_HP)
	who:SendAbil(AT_MP)
	return true
end

function Use_Item20200002(who, item, posGroup, posKey, arg)
	-- 20200002 : 빨간포션
	Use_Item20200001(who, item, posGroup, posKey, arg)
end

function Use_Item20200003(who, item, posGroup, posKey, arg)
	Use_Item20200001(who, item, posGroup, posKey, arg)
end

function Use_Item20200004(who, item, posGroup, posKey, arg)
	Use_Item20200001(who, item, posGroup, posKey, arg)
end

function Use_Item20200005(who, item, posGroup, posKey, arg)
	Use_Item20200001(who, item, posGroup, posKey, arg)
end

function Use_Item20200006(who, item, posGroup, posKey, arg)
	Use_Item20200001(who, item, posGroup, posKey, arg)
end

function Use_Item20200007(who, item, posGroup, posKey, arg)
	Use_Item20200001(who, item, posGroup, posKey, arg)
end

function Use_Item20200008(who, item, posGroup, posKey, arg)
	Use_Item20200001(who, item, posGroup, posKey, arg)
end

function Use_Item20200009(who, item, posGroup, posKey, arg)
	Use_Item20200001(who, item, posGroup, posKey, arg)
end

function Use_Item20200010(who, item, posGroup, posKey, arg)
	Use_Item20200001(who, item, posGroup, posKey, arg)
end

function Use_Item20200011(who, item, posGroup, posKey, arg)
	Use_Item20200001(who, item, posGroup, posKey, arg)
end

function Use_Item20200085(who, item, posGroup, posKey, arg)
	Use_Item20200001(who, item, posGroup, posKey, arg)
	return true
end


function Use_Item20700003(who, item, posGroup, posKey, arg)
	-- 20700003 : 펫보관함
	-- 펫이 있나 검사
	if who:HavePet() == true then
		local kErrorPacket = NewPacket(PT_M_C_ANS_ERROR_MESSAGE)
		kErrorPacket:PushInt(EMT_ITEM)
		--kErrorPacket:PushInt(EMC_NOMORE_HAVE)
		kErrorPacket:PushInt(60068)
		Net_Send(who,kErrorPacket)
		DeletePacket(kErrorPacket)
		return false
	end

	local ground = arg:Get(ACTARG_GROUND)
	ground = Ground(ground)
	local byBlood = math.random(0, 3)
	--local kPet = unitmgr:CreatePet(90000010, 1, who:GetGuid(), 1, 30, byBlood);

	--kPet:SetName("MyPet")
	--unitmgr:AddUnit(kPet, who:GetPos())
	
	
	--Inven = who:GetInventory()
	--if Inven:IsNill() == true then
	--	InfoLog(8, "Use_Item20700003...Inven is NILL")
	--	return false
	--end
	--local kPacket = NewPacket(PT_M_C_ITEMCHANGED)
	--if Inven:RemoveItem(posGroup, posKey, 1, kPacket) == false then
	--	DeletePacket(kPacket)
	--	return false
	--end

	--Net_Send(who, kPacket)
	--DeletePacket(kPacket)	
	return true
end
