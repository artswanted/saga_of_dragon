GCR_Success = 1
GCR_Level = 14
GCR_Money = 15

--/////////////////////////////// How much creat Guild ///////////////////////////////
function HowMuchCreateGuild()--Lua
	--Set How much need money at create guild
	--this function
	return 1, 0, 0 --Gold, Silver, Bronze
end

--/////////////////////////////// Check limit create Guild ///////////////////////////////
function Check_CreateGuild(kUnit)
	if nil == kUnit then
		return 0
	end
	if kUnit:IsNil() then
		return 0
	end
	
	if 20 > kUnit:GetAbil(AT_LEVEL) then
		return GCR_Level
	end
	
	local iGold, iSilver, iBronze = HowMuchCreateGuild()
	if kUnit:CompareMoney("<", iGold, iSilver, iBronze) then--minimum 1 gold
		return GCR_Money
	end
	
	--if bAble then return GCR_Level end--level
	--if bAble then return GCR_Money end--Money
	return GCR_Success --able create user
end

--/////////////////////////////// Get create guild price ///////////////////////////////
function Get_HowMuchCreateGuild()--C++
	local iGoldPerCopper = 10000
	local iSilverPerCopper = 100
	local iGold, iSilver, iBronze = HowMuchCreateGuild()
	return (iGold*iGoldPerCopper)+(iSilver*iSilverPerCopper)+iBronze
end
