---------------------------------------------------------------------------------------
--Calc Party Bonus Exp by Member count
function Calc_PartyMember_Bonus(iMemberCount)
	local BonusRate = 0.2
	
	if 2 == iMemberCount then
		BonusRate = 0.2
	elseif 3 == iMemberCount then
		BonusRate = 0.4
	elseif 4 == iMemberCount then
		BonusRate = 0.6
	end
	
	return BonusRate
end

---------------------------------------------------------------------------------------
--Calc Level per Exp
--[[
function Calc_PartyMember_SharePerExp(iAddExp, iMemberCount, iTotalLevel)
	if 0 >= iTotalLevel then iTotalLevel = 100 end
	local fLevelPerExp = 0
	
	local BonusRate = Calc_PartyMember_Bonus(iMemberCount)
	
	fLevelPerExp = iAddExp * BonusRate
	fLevelPerExp = fLevelPerExp / iTotalLevel
	return fLevelPerExp
end
]]

---------------------------------------------------------------------------------------
--Calc My Result Exp
--[[
function Calc_PartyMember_ShareExp(fLevelPerExp, iCurLevel)
	local fMyResultExp = (fLevelPerExp*iCurLevel) 
	fMyResultExp = math.max(1, fMyResultExp)--Minimum 1
	return math.floor( fMyResultExp )--Return int calcurated value
end
]]

--[[
--Test
local iRet = Calc_PartyMember_SharePerExp(40, 3, 22)
--local iRet = Calc_PartyMember_SharePerExp(12, 3, 22)
print(iRet)
print("MyExp: ".. Calc_PartyMember_ShareExp(iRet, 5) )
print("MyExp: ".. Calc_PartyMember_ShareExp(iRet, 7) )
print("MyExp: ".. Calc_PartyMember_ShareExp(iRet, 10) )
]]


---------------------------------------------------------------------------------------
function Calc_PartyMember_ShareGold(iAddGold, iMemberCount)
	local iMyResultGold = math.floor(iAddGold / iMemberCount)
	iMyResultGold = math.max(1, iMyResultGold)
	return iMyResultGold --Return calcurated value
end
