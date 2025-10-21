local GMCMD_GROUND_PAUSE = 5024

function enchantshift(soul,special,socket)

	local packet = NewPacket(17746)
	packet:PushByte( 1 )
	packet:PushByte( 0 )

	packet:PushByte( 1 )
	packet:PushByte( 1 )

	packet:PushByte( 4 )
	packet:PushByte( 0 )

	packet:PushByte( 4 )
	packet:PushByte( 1 )

	packet:PushByte( 4 )
	packet:PushByte( 2 )

	packet:PushByte( 4 )
	packet:PushByte( 3 )

	packet:PushByte( 4 )
	packet:PushByte( 4 )

	packet:PushByte( 4 )
	packet:PushByte( 5 )

	packet:PushByte( soul )
	packet:PushByte( special )
	packet:PushByte( socket )

	Net_Send(packet)
	DeletePacket(packet)

end


function homechat_roomlist()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local packet = NewPacket(17809)
	local kHome = g_world:GetHome()
	local kNGuid = kHome:GetHomeGuid()
	packet:PushGuid(kNGuid)
	Net_Send(packet)
	DeletePacket(packet)

end

function reset_attach(inv1,pos1,inv2,pos2)

	local packet = NewPacket(17771)
	packet:PushByte( inv1 )
	packet:PushByte( pos1 )
	packet:PushByte( inv2 )
	packet:PushByte( pos2 )
	Net_Send(packet)
	DeletePacket(packet)

end

function homechat_chat_private(chat, guid)

	local kGuid = GUID(guid)
	local packet = NewPacket(50003)
	packet:PushByte( 34 )
	packet:PushWString( WideString(chat) )
	packet:PushInt( 0 )
	packet:PushInt( 0 )
	packet:PushGuid(kGuid)
	
	Net_Send(packet)
	DeletePacket(packet)

end

function homechat_chat_public(chat)

	local packet = NewPacket(50003)
	packet:PushByte( 33 )
	packet:PushWString( WideString(chat) )
	packet:PushInt( 0 )
	packet:PushInt( 0 )
	Net_Send(packet)
	DeletePacket(packet)

end


function homechat_modify_guest(enablemsg, standing)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local packet = NewPacket(17807)
	local kHome = g_world:GetHome()
	local kNGuid = kHome:GetHomeGuid()
	packet:PushGuid(kNGuid)
	packet:PushBool( enablemsg )
	packet:PushBool( standing )
	Net_Send(packet)
	DeletePacket(packet)

end


function homechat_modify(title, noti, password, guestnum)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local kGuid = GUID(guid)
	local packet = NewPacket(17805)
	local kHome = g_world:GetHome()
	local kNGuid = kHome:GetHomeGuid()
	packet:PushGuid(kNGuid)
	packet:PushWString( WideString(title) )
	packet:PushWString( WideString(noti) )
	packet:PushWString( WideString(password) )
	packet:PushInt( guestnum )
	Net_Send(packet)
	DeletePacket(packet)

end


function homechat_create(title, noti, password, guestnum)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local kGuid = GUID(guid)
	local packet = NewPacket(17813)
	local kHome = g_world:GetHome()
	local kNGuid = kHome:GetHomeGuid()
	packet:PushGuid(kNGuid)
	packet:PushWString( WideString(title) )
	packet:PushWString( WideString(noti) )
	packet:PushWString( WideString(password) )
	packet:PushInt( guestnum )
	Net_Send(packet)
	DeletePacket(packet)

end

function homechat_exit()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local packet = NewPacket(17803)
	local kHome = g_world:GetHome()
	local kGuid = kHome:GetHomeGuid()
	packet:PushGuid(kGuid)
	Net_Send(packet)
	DeletePacket(packet)

end

function homechat_enter(guid, password)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local kGuid = GUID(guid)
	local packet = NewPacket(17801)
	local kHome = g_world:GetHome()
	local kNGuid = kHome:GetHomeGuid()
	packet:PushGuid(kNGuid)
	packet:PushGuid(kGuid)
	packet:PushWString( WideString(password) )
	Net_Send(packet)
	DeletePacket(packet)

end


function homesell()

	local packet = NewPacket(17296)
	Net_Send(packet)
	DeletePacket(packet)

end

function extendskill(inv,pos,skill)

	local packet = NewPacket(17769)
	packet:PushByte(inv)
	packet:PushByte(pos)
	packet:PushInt(skill)
	Net_Send(packet)
	DeletePacket(packet)

end

function learnskill(skill)

	local packet = NewPacket(12560)
	packet:PushInt(skill)
	Net_Send(packet)
	DeletePacket(packet)

end


function gmixup()

	local packet = NewPacket(17517)
	packet:PushInt(5)
	packet:PushByte(4)
	packet:PushByte(0)
	packet:PushByte(4)
	packet:PushByte(1)
	packet:PushByte(4)
	packet:PushByte(2)
	packet:PushByte(4)
	packet:PushByte(3)
	packet:PushByte(4)
	packet:PushByte(4)
	Net_Send(packet)
	DeletePacket(packet)

end

function gmixupreload()

	local packet = NewPacket(17519)
	Net_Send(packet)
	DeletePacket(packet)

end

function gmixupresult()

	local packet = NewPacket(17521)
	Net_Send(packet)
	DeletePacket(packet)

end

function optamp(inv1,pos1,inv2,pos2)

	local packet = NewPacket(17767)
	packet:PushByte(inv1)
	packet:PushByte(pos1)
	packet:PushByte(inv2)
	packet:PushByte(pos2)
	Net_Send(packet)
	DeletePacket(packet)

end

function optup(inv1,pos1,inv2,pos2,idx)

	local packet = NewPacket(17765)
	packet:PushByte(inv1)
	packet:PushByte(pos1)
	packet:PushByte(inv2)
	packet:PushByte(pos2)
	packet:PushInt(idx)
	Net_Send(packet)
	DeletePacket(packet)

end

function buyhome(town)

	local packet = NewPacket(17201)
	packet:PushWord(town)
	Net_Send(packet)
	DeletePacket(packet)

end

function log2exp()

	local packet = NewPacket(17293)
	Net_Send(packet)
	DeletePacket(packet)

end

function addlog(guid,string)

	local kGuid = GUID(guid)
	local packet = NewPacket(17235)
	packet:PushGuid(kGuid)
	packet:PushString(string)
	packet:PushInt(0)
	Net_Send(packet)
	DeletePacket(packet)

end

function homeenter(town,house)

	local packet = NewPacket(17203)
	packet:PushWord(town)
	packet:PushInt(house)
	Net_Send(packet)
	DeletePacket(packet)

end

function redice(inv1,pos1,inv2,pos2)

	local packet = NewPacket(17761)
	packet:PushByte(inv1)
	packet:PushByte(pos1)
	packet:PushByte(inv2)
	packet:PushByte(pos2)
	Net_Send(packet)
	DeletePacket(packet)

end

function invsort(inv)

	local packet = NewPacket(17706)
	packet:PushInt(inv)
	Net_Send(packet)
	DeletePacket(packet)

end


function attach(idx)

	local packet = NewPacket(16301)
	packet:PushInt(idx)
	Net_Send(packet)
	DeletePacket(packet)

end

function attachmulti(num)

	for i = 1,num do

		attach(i)
	end

end

function gs(inv,pos)

	local packet = NewPacket(17510)
	packet:PushByte(inv)
	packet:PushByte(pos)
	Net_Send(packet)
	DeletePacket(packet)

end

function gr(inv,pos)

	local packet = NewPacket(17512)
	Net_Send(packet)
	DeletePacket(packet)

end

function ge(inv,pos)

	local packet = NewPacket(17514)
	Net_Send(packet)
	DeletePacket(packet)

end

function reqtimelimit(idx,time,usetime,inv,pos)

	local packet = NewPacket(16234)
	packet:PushInt(idx)
	packet:PushByte(time)
	packet:PushInt(usetime)
	packet:PushByte(inv)
	packet:PushByte(pos)
	packet:PushInt(0)
	packet:PushInt(0)
	Net_Send(packet)
	DeletePacket(packet)

end

function reqenchant(npcguid,inv,pos)

	local packet = NewPacket(13001)
	local kGuid = GUID(npcguid)
	packet:PushGuid(kGuid)
	packet:PushByte(inv)
	packet:PushByte(pos)
	packet:PushByte(0)
	packet:PushByte(0)
	packet:PushByte(0)
	packet:PushByte(0)
	Net_Send(packet)
	DeletePacket(packet)

end

function Net_AuctionEnd(street,house)

	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4021)
	packet:PushWord(street)
	packet:PushInt(house)
	Net_Send(packet)
	DeletePacket(packet)
	
end

function Net_SetTexTime(street,house,year,mon,day)

	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4020)
	packet:PushWord(street)
	packet:PushInt(house)
	packet:PushInt(year)
	packet:PushInt(mon)
	packet:PushInt(day)
	Net_Send(packet)
	DeletePacket(packet)
	
end

function Net_SetTexTimeMine(year,mon,day)
	local actor = GetPlayer()
	if actor:IsNil() then
		return
	end
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4020)
	packet:PushWord(actor:GetHomeAddrTown())
	packet:PushInt(actor:GetHomeAddrHouse())
	packet:PushInt(year)
	packet:PushInt(mon)
	packet:PushInt(day)
	Net_Send(packet)
	DeletePacket(packet)
	
end

function discharge(inv,pos,tinv,tpos)

	local packet = NewPacket(12997)
	packet:PushByte(inv)
	packet:PushByte(pos)
	packet:PushByte(tinv)
	packet:PushByte(tpos)
	Net_Send(packet)
	DeletePacket(packet)

end

function use(inv,pos,tinv,tpos)

	local packet = NewPacket(13006)
	packet:PushByte(inv)
	packet:PushByte(pos)
	packet:PushByte(tinv)
	packet:PushByte(tpos)
	Net_Send(packet)
	DeletePacket(packet)

end


function useportal(inv,pos,guid)

	local Guid = GUID(guid)
	local packet = NewPacket(17103)
	packet:PushByte(inv)
	packet:PushByte(pos)
	packet:PushInt(0)
	packet:PushGuid(Guid)
	Net_Send(packet)
	DeletePacket(packet)

end

function regportal()

	local packet = NewPacket(17101)
	packet:PushWString( WideString("test") )
	Net_Send(packet)
	DeletePacket(packet)

end


function useitemcount(inv,pos,count)

	for i = 0,count do
	
		useitem(inv,pos)
	
	end

end


function fran()

	for i = 0,100 do
		local Guid = GUID("5E44DE31-28A5-4FBF-A60F-FB2077C41B28")
		local packet = NewPacket(12452)	-- PT_C_M_REMOVEITEM
		packet:PushGuid(Guid)
		Net_Send(packet)
		DeletePacket(packet)
	end

end



function deleteitem(pos,type)

	for i = 0,500 do 
	
		local packet = NewPacket(12403)	-- PT_C_M_REMOVEITEM
		packet:PushByte(1)
		packet:PushByte(pos)
		packet:PushInt(type) -- 0 이번 그냥 버림, 1이면 분해.
		Net_Send(packet)
		DeletePacket(packet)

	end
end


function safeextend(inv)

	local packet = NewPacket(16271)
	packet:PushByte(4)
	packet:PushByte(0)
	packet:PushInt(inv)

	Net_Send(packet)
	DeletePacket(packet)

end

function rarityup(insurance,bonusrate)

	local packet = NewPacket(12999)
	local kGuid = GUID("")
	packet:PushGuid(kGuid)
	packet:PushInt(0)
	packet:PushByte(1)
	packet:PushByte(0)
	packet:PushByte(insurance)
	packet:PushByte(4)
	packet:PushByte(0)
	packet:PushByte(bonusrate)
	packet:PushByte(4)
	packet:PushByte(1)
	Net_Send(packet)
	DeletePacket(packet)

end

function upgrade(pos,insurance,bonusrate)

	local packet = NewPacket(13001)
	local kGuid = GUID("")
	packet:PushGuid(kGuid)
	packet:PushByte(1)
	packet:PushByte(pos)
	packet:PushByte(insurance)
	packet:PushByte(bonusrate)
	Net_Send(packet)
	DeletePacket(packet)

end

function bind(it1,ii1,it2,ii2)

	local packet = NewPacket(16261)
	packet:PushByte(it1)
	packet:PushByte(ii1)
	packet:PushByte(it2)
	packet:PushByte(ii2)
	Net_Send(packet)
	DeletePacket(packet)

end

function unbind(it1,ii1,it2,ii2)

	local packet = NewPacket(16263)
	packet:PushByte(it1)
	packet:PushByte(ii1)
	packet:PushByte(it2)
	packet:PushByte(ii2)
	Net_Send(packet)
	DeletePacket(packet)

end

function attachmedal(idx)

	local packet = NewPacket(16301)
	packet:PushInt(idx)
	Net_Send(packet)
	DeletePacket(packet)

end

function hacking()

	local packet = NewPacket(16551)
	packet:PushInt(100)
	Net_Send(packet)
	DeletePacket(packet)

end

function Net_SetItemTime(bInv,pos,year,mon,day)

	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4010)
	packet:PushByte(bInv)
	packet:PushByte(pos)
	packet:PushInt(year)
	packet:PushInt(mon)
	packet:PushInt(day)
	Net_Send(packet)
	DeletePacket(packet)
	
end

function Net_SetItemLimitTime( byInv, byPos, iUseTime, iTimeType )

	if 1 ~= iTimeType then
		iTimeType = 2
	end
	
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4011)
	packet:PushByte(byInv)
	packet:PushByte(byPos)
	packet:PushInt(iTimeType)
	packet:PushInt(iUseTime)
	Net_Send(packet)
	DeletePacket(packet)
	
end

function oxenter(guid)

	local packet = NewPacket(16402)
	local kGuid = GUID(guid)
	packet:PushGuid(kGuid)
	Net_Send(packet)
	DeletePacket(packet)

end

function oxans(ox)

	local packet = NewPacket(16408)
	packet:PushInt(ox)
	Net_Send(packet)
	DeletePacket(packet)

end

function addtime(idx,inv,pos,timetype,usetime)

	local packet = NewPacket(16234)
	packet:PushInt(idx)
	packet:PushByte(timetype)
	packet:PushInt(usetime)
	packet:PushByte(inv)
	packet:PushByte(pos)
	Net_Send(packet)
	DeletePacket(packet)

end

function summon(inv,pos,guid)

	local packet = NewPacket(16341)
	local kGuid = GUID(guid)
	packet:PushGuid(kGuid)
	packet:PushByte(inv)
	packet:PushByte(pos)
	Net_Send(packet)
	DeletePacket(packet)

end

function Net_PT_M_C_UM_REQ_SUMMONPARTYMEMBER(packet)

	local guid = packet:PopGuid()
	local mapno = packet:PopInt()
	local mapguid = packet:PopGuid()
	local invtype = packet:PopByte()
	local invpos = packet:PopByte()
	
	local packet = NewPacket(16348)
	
	packet:PushGuid(guid)
	packet:PushInt(mapno)
	packet:PushGuid(mapguid)
	packet:PushByte(invtype)
	packet:PushByte(invpos)
	packet:PushInt(0)
	Net_Send(packet)
	DeletePacket(packet)

end

function extend(inv,pos,invtype)

	local packet = NewPacket(16331)
	packet:PushInt(invtype)
	packet:PushByte(inv)
	packet:PushByte(pos)
	packet:PushInt(0)
	Net_Send(packet)
	DeletePacket(packet)

end

function pmm(inv,pos,guid)

	local packet = NewPacket(16321)
	local kGuid = GUID(guid)
	packet:PushGuid(kGuid)
	packet:PushByte(inv)
	packet:PushByte(pos)
	Net_Send(packet)
	DeletePacket(packet)

end

function m2pm(guid)

	local packet = NewPacket(16348)
	local kGuid = GUID(guid)
	packet:PushByte(0)
	packet:PushGuid(kGuid)
	packet:PushInt(0)
	packet:PushGuid(kGuid)
	packet:PushByte(1)
	packet:PushByte(0)
	Net_Send(packet)
	DeletePacket(packet)

end

function umm(inv,pos,groundno)

	local packet = NewPacket(16311)
	packet:PushInt(groundno)
	packet:PushByte(inv)
	packet:PushByte(pos)
	Net_Send(packet)
	DeletePacket(packet)

end

function useitem(inv,pos)

	local packet = NewPacket(12422)
	packet:PushByte(inv)
	packet:PushByte(pos)
	packet:PushInt(0)
	Net_Send(packet)
	DeletePacket(packet)

end

function moveitem(inv1,pos1,inv2,pos2)

	local packet = NewPacket(12347)
	packet:PushByte(inv1)
	packet:PushByte(pos1)
	packet:PushByte(inv2)
	packet:PushByte(pos2)
	packet:PushInt(0)
	Net_Send(packet)
	DeletePacket(packet)

end

-- GMCMD_SET_ACHIEVEMENT

function setachiv(idx,value)

	Net_SetAchievement(idx,value)
	
end

function setachivall(num,value)

	Net_SetAchievementAll(num,value)
	
end

function setachivTime(idx,value)

	Net_SetAchievementTime(idx,value)
	
end


function Net_CompleteAchievement(idx)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(204)
	packet:PushInt(idx)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

function Net_SetAchievementTime(idx,value)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(203)
	packet:PushInt(idx)
	packet:PushInt(value)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

function Net_SetAchievementAll(num,value)
	for i = 1, num do
		Net_SetAchievement(i,value)
	end
end

function Net_SetAchievement(idx,value)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(201)
	packet:PushInt(idx)
	packet:PushByte(value)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

-- GMCMD_GIVEITEM
function Net_GiveItem(itemNo, iCount, iRarityControlNo, isCurse, isSeal)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(1)
	packet:PushInt(itemNo)
	packet:PushInt(iCount)
	
	packet:PushInt(iRarityControlNo)
	packet:PushInt(isCurse)
	packet:PushInt(isSeal)
	
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

-- GMCMD_GIVEITEMSET
function Net_GiveItemSet(itemNo)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(2)
	packet:PushInt(itemNo)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

function Net_SetItemCount(iInvType, iInvPos, iCount)
	-- 지정된 위치의 아이템 내구도(개수) 조절
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(3)
	packet:PushShort(iInvType)
	packet:PushShort(iInvPos)
	packet:PushInt(iCount)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_GiveItemMulti(itemNo,iCount,InvNum)
	for i = 1, InvNum do
		Net_GiveItem(itemNo, iCount)
	end
end

function Logout()
	CallYesNoMsgBox( GetTT(60069), GUID(), 4 )
end

-- GMCMD_CLASSCHANGE
function Net_ChangeClass(class, level, bGod)
	if nil == bGod then
		bGod = true
	end
	
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4)
	packet:PushInt(class)
	packet:PushInt(level)
	packet:PushBool(bGod)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

function Net_ClassChange(class, level, bGod)
	return Net_ChangeClass(class, level, bGod)
end

-- GMCMD_SKILLPOINT
function Net_SkillPoint(iPoint)
    local packet = NewPacket(PT_C_M_GODCMD)
    local pilot = g_pilotMan:FindPilot(g_playerInfo.guidPilot)
    if pilot:IsNil() == true then
		return false
    end
	
    --packet:PushGuid(g_playerInfo.guidPilot)
    packet:PushInt(5)
    packet:PushShort(iPoint)

    Net_Send(packet)
    DeletePacket(packet)
	return true
end 

-- GMCMD_GMABIL
function Net_GmAbil(chAbil, bAdd)
	ODS("Net_GmAbil...")
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(6)	-- GmAbil GodCommand
	packet:PushInt(chAbil)
	packet:PushChar(bAdd)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

-- GMCMD_HEALME
function Net_HealMe()
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(7)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

-- GMCMD_QUEST
function Net_Quest(iStartID, iEndID, bComplete, bRebuildLoop)
	local packet = NewPacket(PT_C_M_GODCMD)
	local pilot = g_pilotMan:FindPilot(g_playerInfo.guidPilot)
	if pilot:IsNil() == true then
		return false
	end
	
	packet:PushInt(8)
	packet:PushShort(iStartID)
	packet:PushShort(iEndID)
	packet:PushBool(bComplete)
	packet:PushBool(bRebuildLoop)

    Net_Send(packet)
    DeletePacket(packet)
	return true
end

-- GMCMD_GMABIL64
function Net_GiveExp(iLow, iHigh)
    local kValue = INT64()
    kValue:Set(iLow, iHigh)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(9)
	packet:PushShort(AT_EXPERIENCE)
	packet:PushInt64(kValue)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

function Net_GiveMoney(iLow, iHigh)
	local kValue = INT64()
    kValue:Set(iLow, iHigh)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(9)
	packet:PushShort(AT_MONEY)
	packet:PushInt64(kValue)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

-- GMCMD_DROPITEM
function Net_DropItem( itemNo )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(10)
	packet:PushInt(itemNo)
	packet:PushInt(0)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_DropMoney( iMoney )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(10)
	packet:PushInt(0)
	packet:PushInt(iMoney)
	Net_Send(packet)
	DeletePacket(packet)
end

function mm( num, spawn )
	Net_ReqMapMove( num, spawn )
end

-- GMCMD_NOTICE_ALL
function Net_ReqNotice( strNoticeContent, bSendToAllChannel )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(12)
	packet:PushWString( WideString(strNoticeContent) )
	packet:PushBool(bSendToAllChannel)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

-- GMCMD_KICKUSER
function Net_KickUser(strUserName)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(13)
	packet:PushWString( WideString(strUserName) )

	Net_Send(packet)
	DeletePacket(packet)
end

-- GMCMD_BLOCKUSER
function Net_BlockUser(strUserName, iValue, iEndYear, iEndMonth, iEndDay)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(15)
	packet:PushWString( WideString(strUserName) )
	
	if iValue == 0 then
		iValue = 0
	else iValue = 100	
	end
	
	packet:PushInt(iValue)
	packet:PushInt(iEndYear)
	packet:PushInt(iEndMonth)
	packet:PushInt(iEndDay)

	Net_Send(packet)
	DeletePacket(packet)
end

-- GMCMD_QUESTPARAM
function Net_QuestState(iQuestID, iParamNo, iCount)
	local kPacket = NewPacket(PT_C_M_GODCMD)
	kPacket:PushInt(16)
	kPacket:PushInt(iQuestID)
	kPacket:PushInt(iParamNo)
	kPacket:PushInt(iCount)
	Net_Send(kPacket)
	DeletePacket(kPacket)
	return true
end

-- GMCMD_SPEEDCHANGE
function Net_SpeedChange(iSpeed)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(18)
	packet:PushInt(iSpeed)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

-- GMCMD_COOLTIMEINIT
function Net_SkillCoolTimeInit()
  -- 모든 스킬의 CoolTime 초기화
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(19)
	Net_Send(packet)
	DeletePacket(packet)
end

-- GMCMD_KILLUNIT
function Net_KillTarget( unitguid )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(20)
	local guid = GUID(unitguid)
	packet:PushGuid(guid)
	packet:PushBool(false)
	Net_Send(packet)
	DeletePacket(packet)
end


function Net_KillUnit( kUnitGuid, bNoRegen )

	if nil == bNoRegen then
		bNoRegen = false
	end
	
	-- Unit을 죽인다.
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(20)

	if kUnitGuid == nil then
     		local guid = GUID("")
	     	packet:PushGuid(guid)
	else
	    	packet:PushGuid(kUnitGuid)
	end
	packet:PushBool(bNoRegen)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_KillMe()
	Net_KillUnit( g_pilotMan:GetPlayerPilotGuid() )
end

local GMAMT_Monster		= 0
local GMAMT_Object		= 1

-- GMCMD_ADDMONSTER
function Net_AddMonster(iMonsterID, iMonsterCount, iType, bDropAllItem, iEnchantGradeNo)
	if nil==iMonsterCount or 0==iMonsterCount then
		iMonsterCount = 1
	end
	if nil==iType then
		iType = GMAMT_Monster
	end
	
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(21)
	packet:PushInt(iMonsterID)
	packet:PushInt(iMonsterCount)
	packet:PushInt(iType)
	packet:PushBool(bDropAllItem)
	packet:PushInt(iEnchantGradeNo)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_AddObject(iObjectId, iCount)
	Net_AddMonster(iObjectId, iCount, GMAMT_Object)
end

function Net_AddMonsterGenGroup(iGenGroup, iMonsterCount)
	return Net_AddMonster(iMonsterID, iMonsterCount, 2)
end

-- GMCMD_MUTECHAT
function Net_Mute(strUserName, iMinTime)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(23)
	packet:PushWString( WideString(strUserName) )
	packet:PushInt(iMinTime)
	Net_Send(packet)
	DeletePacket(packet)
	
end
  
 -- GMCMD_QUESTBEGIN
function Net_BeginQuest( iQuestID )
	local packet = NewPacket(PT_C_M_GODCMD)
	local pilot = g_pilotMan:FindPilot(g_playerInfo.guidPilot)
	if pilot:IsNil() == true then
		return false
	end	
	
	packet:PushInt(24)
	packet:PushShort(iQuestID)
    Net_Send(packet)
    DeletePacket(packet)	
end


-- GMCMD_SETABIL
function Net_GiveCP(iCP)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(25)
	packet:PushShort(AT_CP)
	packet:PushInt(iCP)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

function Net_SetAbil(iType, iValue)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(25)
	packet:PushShort(iType)
	packet:PushInt(iValue)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

function Net_UnitSetAbil(kGuid, iType, iValue)
	if nil == kGuid or kGuid:IsNil() then
		return true
	end

	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(60)
	packet:PushGuid(kGuid)
	packet:PushShort(iType)
	packet:PushInt(iValue)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

-- GMCMD_SETENCHANT
function Net_SetEnchant(inv,pos,ModifyType,v1,v2)
	local packet = NewPacket(PT_C_M_GODCMD)	
	packet:PushInt(26)
	packet:PushByte(inv)
	packet:PushByte(pos)
	packet:PushInt(ModifyType)
	packet:PushInt(v1)
	packet:PushInt(v2)

	Net_Send(packet)
	DeletePacket(packet)
end

-- GMCMD_MISSIONMOVE
function Net_MissionMove( iMissionNum, iLevel, iEvent )

	-- 미션으로 입장
	local iMissionKey = GetMissionKey(iMissionNum)
	
	if 0 <= iMissionKey then
		local packet = NewPacket(PT_C_M_GODCMD)
		packet:PushInt(27)
		packet:PushInt(iMissionKey)
		packet:PushInt(iLevel)
		packet:PushInt(iEvent)
		Net_Send(packet)
		DeletePacket(packet)
	end
end

-- GMCMD_MISSIONSTATGE
function Net_MissionStage(iStageNo)
    -- 미션의 Stage를 변경
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(28)
	packet:PushInt(iStageNo)
	Net_Send(packet)
	DeletePacket(packet)
end


function EquipItem(itemNo)
	local actor = GetPlayer()	
	actor:EquipItem(itemNo, false)
	return true
end

function SetHair(iItemNo)
	local actor = GetPlayer()
	actor:SetItemColor(2, iItemNo)
	return true
end

function SetHairColor(red, green, blue)
	local actor = GetPlayer()
	actor:SetHairColor(red, green, blue)
	return true
end

function SetG(fGravity)
	local actor = GetPlayer()
	if actor:IsNil() then
		return false
	end
	actor:SetGravity(fGravity)
	return true
end

function GetPlayer()
	local pilot = g_pilotMan:FindPilot(g_playerInfo.guidPilot)
	if pilot:IsNil() then
		return nil
	end
	return pilot:GetActor()
end

function ResetPlayerPos()
	local actor = GetPlayer()
	if g_world == nil or
		actor:IsNil() == true then
		return false
	end

	spawnLoc = g_world:FindSpawnLoc("char_spawn_1")
	actor:SetTranslate(spawnLoc)
	g_world:RefreshCamera()
	return true
end


function Net_DeleteItem(iInvenIndex, iDeleteType)
	local packet = NewPacket(12403)	-- PT_C_M_REMOVEITEM
	packet:PushShort(iInvenIndex) -- Pos x. y 를 int 화 시켜서 넣음.
	packet:PushInt(iDeleteType) -- 0 이번 그냥 버림, 1이면 분해.
	Net_Send(packet)
	DeletePacket(packet)

	if 0 == iDeleteType then
		PlaySoundByID( "Item_Chuck" )
	else
		PlaySoundByID( "Item_Break" )
	end
	return true
end

function Net_ClearItem(inv,pos)

	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(202)
	packet:PushByte(inv) -- Pos x. y 를 int 화 시켜서 넣음.
	packet:PushByte(pos) -- 0 이번 그냥 버림, 1이면 분해.
	Net_Send(packet)
	DeletePacket(packet)

end

function Net_ClearInv()

	for i = 1,4 do
	
		for j = 0,120 do
		
			Net_ClearItem(i,j)
		
		end
	
	end

end

function Net_OXOpen(EventID)

	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4001)
	packet:PushInt(EventID)
	Net_Send(packet)
	DeletePacket(packet)

end

function Net_OXStep(step)

	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4002)
	packet:PushInt(step)
	Net_Send(packet)
	DeletePacket(packet)

end

function Net_OX30()

	Net_OXStep(1)

end

function Net_OX10()

	Net_OXStep(2)

end

function Net_OX5()

	Net_OXStep(3)

end

function Net_OXStart()

	Net_OXStep(4)

end

function Net_OXClose()

	Net_OXStep(9)

end

function Net_SetBuilder()
end

function Net_Add_Item()
end

function Net_Recall()
end

function Net_Level()
end

function Net_ResetQuest()
end

function Net_ResetSkill()
end

function ShowRenderState()
end

function SetQuiet()
end



function Net_PvPRoomNameChange(iRoomNo,kName)
	if iRoomNo == nil or iRoomNo <= 0 then
		return
	end
	
	iRoomNo = iRoomNo - 1
	
	local kRoomName = WideString(kName)
	if kRoomName:IsNil() then
		return
	end
	
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(1001)
	packet:PushByte(ePvP["Modify"].MPR_ROOMNAME)
	packet:PushInt(iRoomNo)
	packet:PushWString(kRoomName)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_SetArticleState(state)

	local packet = NewPacket(12806)
	packet:PushInt(2001)
--	local packet = NewPacket(16138)
	packet:PushByte(state)
	Net_Send(packet)
	DeletePacket(packet)

end

function Net_EmporiaOpen( kEmporiaID, wYear, wMonth, wDay, wHour, wMin, iOpenLevel, iBattleWeek, iBattleTime )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(3001)
	packet:PushGuidForString(kEmporiaID)
	
	-- SYSTEMTIME과 똑같이 Pop이 되도록 날짜를 넣어야 한다.
	packet:PushWord(wYear)
	packet:PushWord(wMonth)
	packet:PushWord(0) --wDayOfWeek
	packet:PushWord(wDay)
	packet:PushWord(wHour)
	packet:PushWord(wMin)
	packet:PushWord(0) --wSecond
	packet:PushWord(0) --wMilliseconds
	
	packet:PushInt(iBattleWeek)--0이면 기존값
	packet:PushInt(iBattleTime)--0이면 기존값
	packet:PushInt(iOpenLevel)--0이면 기존값
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_EmporiaClose( kEmporiaID )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(3002)
	packet:PushGuidForString(kEmporiaID)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_EmporiaReady( kEmporiaID )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(3004)
	packet:PushGuidForString(kEmporiaID)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_GameStart()
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(101)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_GameEnd()
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(102)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_GameAddPoint( iPoint )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(110)
	packet:PushInt(iPoint)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_CheckTime()
	local packet = NewPacket(102)--PT_C_M_REQ_SYNCTIME_CHECK
	Net_Send(packet)
	DeletePacket(packet)
end


function Net_SetRarity(Inv,Pos,value)

	Net_SetEnchant(Inv,Pos,0,value,0)

end

function Net_SetPlusMulti(Inv,count,Value,Lv)
	for i = 1,count do
		Net_SetEnchant(Inv,i,1,Value,Lv)
	end
end

function Net_SetOption1Multi(Inv,count,Value,Lv)

	for i = 1,count do
		Net_SetEnchant(Inv,i,2,Value,Lv)
	end

end

function Net_SetOption2Multi(Inv,count,Value,Lv)

	for i = 1,count do
		Net_SetEnchant(Inv,i,3,Value,Lv)
	end

end

function Net_SetOption3Multi(Inv,count,Value,Lv)

	for i = 1,count do
		Net_SetEnchant(Inv,i,3,Value,Lv)
	end

end

function Net_SetOption4Multi(Inv,count,Value,Lv)

	for i = 1,count do
		Net_SetEnchant(Inv,i,4,Value,Lv)
	end

end

function Net_SetPlus(Inv,Pos,Value,Lv)

	Net_SetEnchant(Inv,Pos,1,Value,Lv)

end

function Net_SetOption1(Inv,Pos,Value,Lv)

	Net_SetEnchant(Inv,Pos,2,Value,Lv)

end

function Net_SetOption2(Inv,Pos,Value,Lv)

	Net_SetEnchant(Inv,Pos,3,Value,Lv)

end

function Net_SetOption3(Inv,Pos,Value,Lv)

	Net_SetEnchant(Inv,Pos,4,Value,Lv)

end

function Net_SetOption4(Inv,Pos,Value,Lv)

	Net_SetEnchant(Inv,Pos,5,Value,Lv)

end

function Net_SetCurseAndSeal(Inv,Pos,Curse,Seal)

	Net_SetEnchant(Inv,Pos,6,Curse,Seal)

end

function Net_SetProperty(Inv,Pos,Attr,Lv)

	Net_SetEnchant(Inv,Pos,7,Attr,Lv)

end

function Net_SetRarityAll(inv,value)

	for i = 1,36 do
		Net_SetRarity(inv,i-1,value)
	end

end

function Net_SetAttached(Inv,Pos,value)

	Net_SetEnchant(Inv,Pos,8,value,0)

end

function Net_SetAttachedAll(inv,value)

	for i = 1,36 do
		Net_SetAttached(inv,i-1,value)
	end

end

function Net_AddGuildExp(value)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(29)
	packet:PushInt(value)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_FailDailyQuest()
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(30)
	Net_Send(packet)
	DeletePacket(packet)
end

-- MapMove 관련
function Net_ReqMapMove( num, spawn )
	if nil == spawn or 1 > spawn then
		spawn = 1
	end
	
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(80)
	packet:PushInt( num )
	packet:PushShort( spawn )
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_ReqMapMove_ReCall( strUserName, bParty )
	local kMyActor = g_pilotMan:GetPlayerActor()
	if true == kMyActor:IsNil() then
		ODS("Net_ReqMapMove_ReCall : Not Found MyActor\n")
		return
	end
	
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(81)
	packet:PushPoint3( kMyActor:GetPos() )
	packet:PushWString( WideString(strUserName) )
	packet:PushBool(bParty)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_ReqMapMove_Target( strUserName )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(82)
	packet:PushWString( WideString(strUserName) )
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_ReqMapMove_TargetToMap( strUserName, bParty, iTargetGndNo, nSpawnNo )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(83)
	packet:PushWString( WideString(strUserName) )
	packet:PushBool(bParty)
	packet:PushInt( iTargetGndNo )
	packet:PushShort( nSpawnNo )
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_TakeCoupon(coupon)
	local packet = NewPacket(17001)
	packet:PushString(coupon)
	Net_Send(packet)
	DeletePacket(packet)
end

-- GMCMD_MISSIONSCORE
function Net_MissionScore(iScore)
    -- 미션의 Stage를 변경
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(31)
	packet:PushInt(iScore)
	Net_Send(packet)
	DeletePacket(packet)
end


-- GMCMD_ADD_EFFECT
function Net_ReqAddEffect(iEffectNo)
	if 0 ~= iEffectNo then
		local packet = NewPacket(PT_C_M_GODCMD)
		packet:PushInt(41);
		packet:PushInt(iEffectNo);
		Net_Send(packet)
		DeletePacket(packet)
	end
end

-- GMCMD_DELETE_EFFECT
function Net_ReqDelEffect(iEffectNo)
	if 0 ~= iEffectNo then
		local packet = NewPacket(PT_C_M_GODCMD)
		packet:PushInt(42);
		packet:PushInt(iEffectNo);
		Net_Send(packet)
		DeletePacket(packet)
	end
end

-- GMCMD_DELETE_EFFECT
function Net_ReqDeleteEffect(index)
	local iEffectNo = GetBuffEffectNo(index);

	if 0 ~= iEffectNo then
		local packet = NewPacket(PT_C_M_GODCMD)
		packet:PushInt(42);
		packet:PushInt(iEffectNo);
		Net_Send(packet)
		DeletePacket(packet)
	end
end

function Net_AddFran(iLow, iHigh)
    local kValue = INT64()
    kValue:Set(iLow, iHigh)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(9)
	packet:PushShort(AT_FRAN)
	packet:PushInt64(kValue)
	Net_Send(packet)
	DeletePacket(packet)
	return true
end

function Net_GroundMute(bSet)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(32)
	packet:PushBool(bSet)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_SetDGAttr(iVal, bSet)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(33)
	packet:PushInt(iVal)
	packet:PushBool(bSet)
	Net_Send(packet)
	DeletePacket(packet)
end
function Net_AddBSPoint(iPoint, iTeamPoint)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(34)
	packet:PushInt(iPoint)
	packet:PushInt(iTeamPoint)
	Net_Send(packet)
	DeletePacket(packet)
end
function Net_SetBSState(iGameIDX, iState)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(35)
	packet:PushInt(iGameIDX)
	packet:PushInt(iState)
	Net_Send(packet)
	DeletePacket(packet)
end
function nbs_state(iStart, iEnd, iState)
	for i=iStart,iEnd do
		Net_SetBSState(i, iState)
	end
end

function Net_PetChangeClass( iGrade, sLevel )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(3104)
	packet:PushInt(iGrade)
	packet:PushShort(sLevel)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_PetSetAbil( wType, iValue )
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(3125)
	packet:PushWord(wType)
	packet:PushInt(iValue)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_KillPet()
	local kMyUnit = g_pilotMan:GetPlayerUnit()
	if false == kMyUnit:IsNil() then
		local kPetGuid = kMyUnit:GetSelectedPet()
		if false == kPetGuid:IsNil() then
			Net_KillUnit( kPetGuid )
		end
	end
end

function Net_CoupleInit()
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(90)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_LuckyStarOpen(iEventID)
	if nil == iEventID then return end
	
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4022)
	packet:PushInt(iEventID)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_LuckyStarStep(iStep)
	if nil == iStep then return end
	
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4023)
	packet:PushInt(iStep)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_LuckyStarClose()
	Net_LuckyStarStep(0)
end

function Net_LuckyStarResultOn()
	Net_LuckyStarStep(3)
end

function Net_ServerTime(iTimeType)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4024)
	packet:PushInt(iTimeType)
	Net_Send(packet)
	DeletePacket(packet)
end


function GetCashItemOption(MinLv,MaxLv)
	local Option = 0
	if 0<MinLv and 0<MaxLv then
		Option = Option + 0x02
		Option = Option + (MinLv * 0x10000)
		Option = Option + (MaxLv * 0x100)
	end
	return Option
end

function Net_MemberCashItemGiftAdd(MinLv,MaxLv,CashItemNo,TimeType,UseTime,StartDate,EndDate,Sender,Memo,MailTitle,MailBody)
	local Option = 0x0
	Option = Option + GetCashItemOption(MinLv,MaxLv)

	Net_CashItemGiftAdd(CashItemNo,TimeType,UseTime,StartDate,EndDate,Sender,Memo,MailTitle,MailBody,Option)
end

function Net_CharCashItemGiftAdd(MinLv,MaxLv,CashItemNo,TimeType,UseTime,StartDate,EndDate,Sender,Memo,MailTitle,MailBody)
	local Option = 0x1
	Option = Option + GetCashItemOption(MinLv,MaxLv)
	
	Net_CashItemGiftAdd(CashItemNo,TimeType,UseTime,StartDate,EndDate,Sender,Memo,MailTitle,MailBody,Option)
end

function Net_CashItemGiftAdd(CashItemNo,TimeType,UseTime,StartDate,EndDate,Sender,Memo,MailTitle,MailBody,Option)
	if nil==CashItemNo or nil==TimeType or nil==UseTime or nil==StartDate or nil==EndDate or nil==Sender or nil==Memo or nil==MailTitle or nil==MailBody then
		return
	end
	
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4025)
	packet:PushInt(CashItemNo)
	packet:PushByte(TimeType)
	packet:PushInt(UseTime)
	packet:PushWString( WideString(StartDate) )
	packet:PushWString( WideString(EndDate) )
	packet:PushWString( WideString(Sender) )
	packet:PushWString( WideString(Memo) )
	packet:PushWString( WideString(MailTitle) )
	packet:PushWString( WideString(MailBody) )
	packet:PushInt(Option)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_CashItemGiftDel(EventNo)
	if nil == EventNo then return end

	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(4026)
	packet:PushInt(EventNo)
	Net_Send(packet)
	DeletePacket(packet)
end


function Net_GodHand( bSet )
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt( 4027 )
	packet:PushBool( bSet )
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_PlayerPlayTimeStep(Step)
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(4028)
	packet:PushInt(Step)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_ResetPlayerPlayTime()
	Net_PlayerPlayTimeStep(0);
end

function Net_DefPlayerPlayTimeReload()
	Net_PlayerPlayTimeStep(1);
end

function Net_SetPlayerPlayTime(strID,iAccConSec,iAccDisSec,bMember)
	if nil == strID then
		return
	end
	if nil == bMember then
		bMember = false
	end
	
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(4029)
	packet:PushWString( WideString(strID) )
	packet:PushInt(iAccConSec)
	packet:PushInt(iAccDisSec)
	packet:PushBool(bMember)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_SetRealmQuest(kRealmQuestID, iCount)
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(43)
	packet:PushWString( WideString(kRealmQuestID) )
	packet:PushInt( iCount )
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_SGMove(iSuperGroundNo, iMode)
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(44)
	packet:PushInt(iSuperGroundNo)
	packet:PushInt(iMode)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_SGFloor(iFloor, iSpawn, bForce)
	if 0 >= iFloor then
		return
	end
	if nil == iSpawn then
		iSpawn = 1
	end
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(45)
	packet:PushInt(iFloor-1)
	packet:PushInt(iSpawn)
	packet:PushBool(bForce)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_NewAlramMission( iMissionNo )
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(50)
	packet:PushInt(iMissionNo)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_TeleportToMon( iMonNo, iCount )
	if 0 >= iCount then
		iCount = 1
	end
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(91)
	packet:PushInt(iMonNo)
	packet:PushInt(iCount)
	Net_Send( packet )
	DeletePacket( packet )
end
function telmon(iMonNo, iCount) Net_TeleportToMon(iMonNo, iCount) end

function Net_EliteMonGroggy()
	local kGuid = GetEliteMonActor():GetPilotGuid()
	Net_UnitSetAbil(kGuid, 6335, 3) --AT_ELITEPATTEN_NEXTSTATE, EPS_GROGGY
end

function Net_EliteMonFury() -- 정상동작 보장 안함
	local kGuid = GetEliteMonActor():GetPilotGuid()
	Net_UnitSetAbil(kGuid, 6335, 2) --AT_ELITEPATTEN_NEXTSTATE, EPS_GROGGY
end

function Net_ClearIngQuest()
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(51)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_GuildBoardRefresh()
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(52)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_CopyThat(CharName)
	if nil == CharName then return end
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(92)
	packet:PushWString( WideString(CharName) )
	Net_Send( packet )
	DeletePacket( packet )
end
function nam(iNo, iCnt)
	Net_AddMonster(iNo, iCnt)
end

function stm()
	SetTeleMove(1)
end

function ngi(no, count)
	if nil==count or 0==count then
		count = 1
	end
	Net_GiveItem(no, count)
end

function nhm()
	Net_HealMe()
end

function nsi()
	Net_SkillCoolTimeInit()
end

function nku()
	Net_KillUnit()
end

function nmm(missionno, diffno)
	if nil~=missionno and 0<missionno then
		if nil==diffno or 0==diffno then
			diffno = 1
		end
		Net_MissionMove(missionno, diffno)
	end
end

function nms(stageno)
	if nil==stageno or 0==stageno then
		stageno = 1
	end
	Net_MissionStage(stageno)
end

function Net_PvPLeague_SetEvent(iEventType)
	if 0 == iEventType then return end
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(4100)
	packet:PushInt(iEventType)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_LimitHP(iHP)
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(94)
	packet:PushInt(iHP)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_JSAddExp(iSkillNo, iValue)
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(5001)
	packet:PushInt(iSkillNo)
	packet:PushInt(iValue)
	Net_Send( packet )
	DeletePacket( packet )
end
function Net_JSAddExh(iValue)
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(5002)
	packet:PushInt(iValue)
	Net_Send( packet )
	DeletePacket( packet )
end
function Net_JSResetExh(iMinute)
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(5003)
	packet:PushInt(iMinute)
	Net_Send( packet )
	DeletePacket( packet )
end
function Net_JSAlwaysUseSubTool(bUse)
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(5004)
	packet:PushInt(bUse)
	Net_Send( packet )
	DeletePacket( packet )
end
function Net_JSAddBP(iValue)
	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(5010)
	packet:PushInt(iValue)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_JSWorkBenchTurnOver()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local packet = NewPacket( PT_C_M_GODCMD )
	
	local kHome = g_world:GetHome()
	if(kHome:IsNil()) then return end
	
	local kNGuid = kHome:GetHomeGuid()
	if(kNGuid:IsNil()) then return end
	
	packet:PushInt(5005)	
	packet:PushGuid(kNGuid)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_JSWorkBenchDestroyRemainTime(iSlot, iHour, iMin, iSec)
	if(1 > iSlot) then return end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local packet = NewPacket( PT_C_M_GODCMD )
	
	local kHome = g_world:GetHome()
	if(kHome:IsNil()) then return end
	
	local kNGuid = kHome:GetHomeGuid()
	if(kNGuid:IsNil()) then return end
	
	local kWorkBenchGuid = JobSkill_GetCurrentWorkBenchGUID()
	if(kNGuid:IsNil()) then return end
	
	local kPlayerGuid = g_pilotMan:GetPlayerPilotGuid()
	if(kPlayerGuid :IsNil()) then return end	
	
	packet:PushInt(5006)
	packet:PushGuid(kPlayerGuid)
	packet:PushGuid(kNGuid)
	packet:PushGuid(kWorkBenchGuid)
	packet:PushInt(iSlot)
	packet:PushInt(iHour)
	packet:PushInt(iMin)
	packet:PushInt(iSec)
	
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_JSWorkBenchRemainTime(iHour, iMin, iSec)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local packet = NewPacket( PT_C_M_GODCMD )
	
	local kHome = g_world:GetHome()
	if(kHome:IsNil()) then return end
	
	local kNGuid = kHome:GetHomeGuid()
	if(kNGuid:IsNil()) then return end
	
	local kWorkBenchGuid = JobSkill_GetCurrentWorkBenchGUID()
	if(kNGuid:IsNil()) then return end
	
	local kPlayerGuid = g_pilotMan:GetPlayerPilotGuid()
	if(kPlayerGuid :IsNil()) then return end	
	
	packet:PushInt(5007)
	packet:PushGuid(kPlayerGuid)
	packet:PushGuid(kNGuid)
	packet:PushGuid(kWorkBenchGuid)
	
	packet:PushInt(iHour)
	packet:PushInt(iMin)
	packet:PushInt(iSec)
	
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_JSWorkBenchRemainTimeForBless(iHour, iMin, iSec)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local packet = NewPacket( PT_C_M_GODCMD )
	
	local kHome = g_world:GetHome()
	if(kHome:IsNil()) then return end
	
	local kNGuid = kHome:GetHomeGuid()
	if(kNGuid:IsNil()) then return end
	
	local kPlayerGuid = g_pilotMan:GetPlayerPilotGuid()
	if(kPlayerGuid :IsNil()) then return end	
	
	packet:PushInt(5008)
	packet:PushGuid(kPlayerGuid)
	packet:PushGuid(kNGuid)	
	
	packet:PushInt(iHour)
	packet:PushInt(iMin)
	packet:PushInt(iSec)
	
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_JSWorkBenchTrouble()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local packet = NewPacket( PT_C_M_GODCMD )
	
	local kHome = g_world:GetHome()
	if(kHome:IsNil()) then return end
	
	local kNGuid = kHome:GetHomeGuid()
	if(kNGuid:IsNil()) then return end
	
	packet:PushInt(5009)
	packet:PushGuid(kNGuid)
	Net_Send( packet )
	DeletePacket( packet )
end

function abp(iValue)
	Net_JSAddBP(iValue)
end

function wto()
	Net_JSWorkBenchTurnOver()
end

function wdrt(iSlot, iHour, iMin, iSec)
	Net_JSWorkBenchDestroyRemainTime(iSlot, iHour, iMin, iSec)
end

function wrt(iHour, iMin, iSec)
	Net_JSWorkBenchRemainTime(iHour, iMin, iSec)
end

function wrtfb(iHour, iMin, iSec)
	Net_JSWorkBenchRemainTimeForBless(iHour, iMin, iSec)
end

function wt()
	Net_JSWorkBenchTrouble()
end

function RevisionNo()
	CharacterRevisionNo()
end

function GetPickupActor()
	if nil==g_kPickupActorGuid then
		return nil
	end

	local kTargetPilot = g_pilotMan:FindPilot(g_kPickupActorGuid)
	if kTargetPilot:IsNil() then
		return nil
	end
	return kTargetPilot:GetActor()
end

function Net_PickupUnitOnlySkill(skillnum)
	local actor = GetPickupActor()
	if nil==actor or actor:IsNil() then
		return
	end

	if 0~=skillnum then
		for i = 0, 9 do
			local no = actor:GetAbil(AT_MON_SKILL_01+i)
			if 0~=no then
				local kSkillDef = GetSkillDef(no)
				if nil~=kSkillDef and false==kSkillDef:IsNil() then
					if skillnum==no then
						Net_UnitSetAbil(actor:GetPilotGuid(), AT_LOCK_SKILLNO_01+i, 0 )
					else
						Net_UnitSetAbil(actor:GetPilotGuid(), AT_LOCK_SKILLNO_01+i, no )
					end
				end
			end
		end
		Net_UnitSetAbil(actor:GetPilotGuid(), AT_USE_LOCK_SKILL, 1)--락스킬사용
	else
		Net_UnitSetAbil(actor:GetPilotGuid(), AT_USE_LOCK_SKILL, 0)--락스킬미사용
	end
end

function Net_SetFatigue(Value)
	local MAX = GetMaxDefenceFatigue()
	if Value > MAX or Value < 0 then return end
	Net_SetAbil(65101, Value) -- AT_STRATEGY_FATIGABILITY 전략피로도
end

function nsf(Value)
	local MAX = GetMaxDefenceFatigue()
	if Value > MAX then
		Value = MAX
	elseif Value < 0 then
		Value = 0
	end
	Net_SetAbil(65101, Value) -- AT_STRATEGY_FATIGABILITY 전략피로도
end

function TestExpedition()
	local kMyActor = GetMyActor()
	if(kMyActor:IsNil()) then return end
	local kPilot = kMyActor:GetPilot()
	if(kPilot:IsNil()) then return end	
	local	iBaseActorType = kPilot:GetBaseClassID()
	--ODS("iBaseActorType:"..iBaseActorType.."\n", false, 912)
	if iBaseActorType == CT_FIGHTER then
		--용기사
		Net_GiveItem(	119200355			, 1)
		Net_GiveItem(	119200365			, 1)
		Net_GiveItem(	119200375			, 1)
		Net_GiveItem(	119200385			, 1)
		Net_GiveItem(	119200395			, 1)
		Net_GiveItem(	119200405			, 1)
		Net_GiveItem(111100415, 1)
		--파괴자
		Net_GiveItem(	119200415			, 1)
		Net_GiveItem(	119200425			, 1)
		Net_GiveItem(	119200435			, 1)
		Net_GiveItem(	119200445			, 1)
		Net_GiveItem(	119200455			, 1)
		Net_GiveItem(	119200465			, 1)
		Net_GiveItem(112100355, 1)
	elseif iBaseActorType == CT_MAGICIAN then		
		--아크메이지
		Net_GiveItem(	119200475			, 1)
		Net_GiveItem(	119200485			, 1)
		Net_GiveItem(	119200495			, 1)
		Net_GiveItem(	119200505			, 1)
		Net_GiveItem(	119200515			, 1)
		Net_GiveItem(	119200525			, 1)
		Net_GiveItem(113100415, 1)
		--워로드
		Net_GiveItem(	119200535			, 1)
		Net_GiveItem(	119200545			, 1)
		Net_GiveItem(	119200555			, 1)
		Net_GiveItem(	119200565			, 1)
		Net_GiveItem(	119200575			, 1)
		Net_GiveItem(	119200585			, 1)
		Net_GiveItem(114100355, 1)
	elseif iBaseActorType == CT_ARCHER then
		--수호자
		Net_GiveItem(	119200595			, 1)
		Net_GiveItem(	119200605			, 1)
		Net_GiveItem(	119200615			, 1)
		Net_GiveItem(	119200635			, 1)
		Net_GiveItem(	119200645			, 1)
		Net_GiveItem(115100415, 1)
		--스트라이커
		Net_GiveItem(	119200655			, 1)
		Net_GiveItem(	119200665			, 1)
		Net_GiveItem(	119200675			, 1)
		Net_GiveItem(	119200685			, 1)
		Net_GiveItem(	119200695			, 1)
		Net_GiveItem(	119200705			, 1)
		Net_GiveItem(116100355, 1)
	elseif iBaseActorType == CT_THIEF then	
		--매니악 
		Net_GiveItem(	119200715, 1)
		Net_GiveItem(	119200725			, 1)
		Net_GiveItem(	119200735			, 1)
		Net_GiveItem(	119200745			, 1)
		Net_GiveItem(	119200755			, 1)
		Net_GiveItem(	119200765			, 1)		
		Net_GiveItem(117100415, 1)
		--쉐도우 
		Net_GiveItem(	119200775			, 1)
		Net_GiveItem(	119200785			, 1)
		Net_GiveItem(	119200795			, 1)
		Net_GiveItem(	119200805			, 1)
		Net_GiveItem(	119200815			, 1)
		Net_GiveItem(	119200825			, 1)
		Net_GiveItem(118100355, 1)
	elseif iBaseActorType == CT_DOUBLE_FIGHTER then	
		--격투가
		Net_GiveItem(330110230, 1)
		Net_GiveItem(330210230, 1)
		Net_GiveItem(330310230, 1)
		Net_GiveItem(330410230, 1)
		Net_GiveItem(330510230, 1)
		Net_GiveItem(330610230, 1)
		
		Net_GiveItem(333110070, 1)
		Net_GiveItem(333110075, 1)
	elseif iBaseActorType == CT_SHAMAN then	
		--소환사
		Net_GiveItem(340110230, 1)
		Net_GiveItem(340210230, 1)
		Net_GiveItem(340310230, 1)
		Net_GiveItem(340410230, 1)
		Net_GiveItem(340510230, 1)
		Net_GiveItem(340610230, 1)
		
		Net_GiveItem(343110070, 1)
		Net_GiveItem(343110075, 1)
	end
	Net_GiveItem(98003780,999)
	Net_GiveItem(98000020,10)
	Net_GiveItem(98000030,10)
	Net_GiveItem(72500190,30)
	Net_GiveItem(98004050,30)
	Net_GiveItem(231001920,99)
end

function Net_SetPointCopy(value)
	if 15 > value and value > 0 then
		local packet = NewPacket(PT_C_M_GODCMD)
		packet:PushInt(4200)
		packet:PushInt(value-1)
		Net_Send(packet)
		DeletePacket(packet)
	end
end

function nsp(value)
	Net_SetPointCopy(value)
end

function Net_PremiumInsert(Name,iServiceNo)
	if nil == Name then
		Name = ''
	end

	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(5020)
	packet:PushWString( WideString(Name) )
	packet:PushInt(iServiceNo)
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_PremiumModify(Name,AddMin)
	if nil == Name then
		Name = ''
	end

	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(5021)
	packet:PushWString( WideString(Name) )
	packet:PushInt( AddMin )
	Net_Send( packet )
	DeletePacket( packet )
end

function Net_PremiumRemove(Name)
	if nil == Name then
		Name = ''
	end

	local packet = NewPacket( PT_C_M_GODCMD )
	packet:PushInt(5022)
	packet:PushWString( WideString(Name) )
	Net_Send( packet )
	DeletePacket( packet )
end

function npi(Name,iServiceNo)
	return Net_PremiumInsert(Name,iServiceNo)
end
function npm(Name,AddMin)
	return Net_PremiumModify(Name,AddMin)
end
function npr(Name)
	return Net_PremiumRemove(Name)
end
function tsi()
	Net_GiveItem(98001440, 999) -- 스킬 초기화 
	Net_GiveItem(50120090, 999)	-- 각성물약
	Net_GiveItem(98000020,999) 	-- HP
	Net_GiveItem(98000030,999) 	-- MP
	Net_GiveItem(98003780, 999) -- 쿨타임 5분
end
function soul(n) -- 소울 얻기
	if( 0 == n  or nil == n) then n = 1 end	
	for i = 1,n do
		ngi(79000030, 999)
	end	
end

function  ie() -- 인벤토리 확장 아이템
	Net_GiveItem(98000395, 6)
	Net_GiveItem(98000405, 6)
	Net_GiveItem(98000415, 6)
	Net_GiveItem(98000425, 6)
end

function Net_SetPlayerPos(x,y,z)
	local actor = GetPlayer()
	if g_world == nil or
		actor:IsNil() == true then
		return false
	end

	local loc = Point3(x,y,z)
	if z==nil or z==0 then 
		local actorz = actor:GetPos():GetZ()
		loc = g_world:ThrowRay(Point3(x,y,actorz+100), Point3(0,0,-1),500)
		if -1==loc:GetX() and -1==loc:GetY() then
			AddWarnDataStr(WideString("Invalid Pos",0))	
			return
		end
	end

	loc:SetZ(loc:GetZ()+50)

	actor:SetTranslate(loc)
	g_world:RefreshCamera()
	return true
end

function nspp(x,y,z)
	Net_SetPlayerPos(x,y,z)
end

function tsi()
	Net_GiveItem(98001440, 999) -- 스킬 초기화 
	Net_GiveItem(50120090, 999)	-- 각성물약
	Net_GiveItem(98000020,999) 	-- HP
	Net_GiveItem(98000030,999) 	-- MP
	Net_GiveItem(98003780, 999) -- 쿨타임 5분
end

function Net_KOH_TriggerUnitPoint(kTriggerID,iPoint)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(111)
	packet:PushString(kTriggerID)
	packet:PushInt(iPoint)
	Net_Send(packet)
	DeletePacket(packet)
end

function soul(n) -- 소울 얻기	
	if( 0 == n  or nil == n) then n = 1 end	
	for i = 1,n do
		ngi(79000030, 999)
	end	
end

function  ie() -- 인벤토리 확장 아이템
	Net_GiveItem(98000395, 6)
	Net_GiveItem(98000405, 6)
	Net_GiveItem(98000415, 6)
	Net_GiveItem(98000425, 6)
end

function Net_LoveRoundWin(WinTeam)	-- 현재 라운드 승리.
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(120)
	packet:PushInt(WinTeam)
	Net_Send(packet)
	DeletePacket(packet)
end

function nrw(WinTeam)
	Net_LoveRoundWin(WinTeam)
end

function Net_MoveToEventGround(EventNo)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(152)
	packet:PushInt(EventNo)
	Net_Send(packet)
	DeletePacket(packet)
end

function mteg(EventNo)
	Net_MoveToEventGround(EventNo)
end
									
function hit(iCasterLevel, iTargetLevel, iValue, iFinalRate )
	if(nil == iFinalRate) then iFinalRate = 0; end
	local fRate =  NewCalcValueToRate(AT_HIT_SUCCESS_VALUE, iValue,  
									AT_FINAL_HIT_SUCCESS_RATE,  iFinalRate , 
									iCasterLevel, iTargetLevel);	
	local str1 = "{C=0xFFFFFFFF/} 공격자LV:"..iCasterLevel;
	local str2 = "/피격자LV:"..iTargetLevel;
	local str3 = "/명중수치:"..iValue;
	local str4 = "/추가명중률:"..iFinalRate;	
	local str5 = "/{C=0xFFFFD934/}결과= "..fRate;
	
	local CompleteStr = str1..str2..str3..str4..str5;
	g_ChatMgrClient:AddLogMessage( WideString(CompleteStr), true, 0, 12 );
	ODS(CompleteStr)
end

function dod(iCasterLevel, iTargetLevel, iValue, iFinalRate)
	if(nil == iFinalRate) then iFinalRate = 0; end
	
	local fRate = NewCalcValueToRate(AT_DODGE_SUCCESS_VALUE, iValue,  
									AT_FINAL_DODGE_SUCCESS_RATE,  iFinalRate , 
									iCasterLevel, iTargetLevel);	
	local str1 = "{C=0xFFFFFFFF/} 공격자LV:"..iCasterLevel;
	local str2 = "/피격자LV:"..iTargetLevel;
	local str3 = "/회피수치:"..iValue;
	local str4 = "/추가회피율:"..iFinalRate;	
	local str5 = "/{C=0xFFA0FFFF/}결과= "..fRate;
	
	local CompleteStr = str1..str2..str3..str4..str5;
	g_ChatMgrClient:AddLogMessage( WideString(CompleteStr), true, 0, 12 );
	ODS(CompleteStr)
end

function blk(iCasterLevel, iTargetLevel, iValue, iFinalRate)
if(nil == iFinalRate) then iFinalRate = 0; end
	local fRate = NewCalcValueToRate(AT_BLOCK_SUCCESS_VALUE, iValue,  
									AT_FINAL_BLOCK_SUCCESS_RATE,  iFinalRate , 
									iCasterLevel, iTargetLevel);	
	local str1 = "{C=0xFFFFFFFF/} 공격자LV:"..iCasterLevel;
	local str2 = "/피격자LV:"..iTargetLevel;
	local str3 = "/블록수치:"..iValue;
	local str4 = "/추가블록률:"..iFinalRate;	
	local str5 = "/{C=0xFF00FF90/}결과= "..fRate;
	
	local CompleteStr = str1..str2..str3..str4..str5;
	g_ChatMgrClient:AddLogMessage( WideString(CompleteStr), true, 0, 12 );
	ODS(CompleteStr)
end

function cri(iCasterLevel, iTargetLevel, iValue, iFinalRate)
if(nil == iFinalRate) then iFinalRate = 0; end
	local fRate = NewCalcValueToRate(AT_CRITICAL_SUCCESS_VALUE, iValue,  
									AT_FINAL_CRITICAL_SUCCESS_RATE,  iFinalRate , 
									iCasterLevel, iTargetLevel);	
	local str1 = "{C=0xFFFFFFFF/} 공격자LV:"..iCasterLevel;
	local str2 = "/피격자LV:"..iTargetLevel;
	local str3 = "/크리수치:"..iValue;
	local str4 = "/추가크리율:"..iFinalRate;	
	local str5 = "/{C=0xFFDF9090/}결과= "..fRate;
	
	local CompleteStr = str1..str2..str3..str4..str5;
	g_ChatMgrClient:AddLogMessage( WideString(CompleteStr), true, 0, 12 );
	ODS(CompleteStr)
end

function block()
	Net_SetAbil(9021, 10000)
	Net_SetAbil(7121, 10000)
end

function hit()
	Net_SetAbil(9001, 10000)
end

function cri()
	Net_SetAbil(7141, 90000)
	Net_SetAbil(9031, 10000)
end
function dod()
	Net_SetAbil(9011, 10000)
end


function Net_MoveToEventGround(EventNo)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(152)
	packet:PushInt(EventNo)
	Net_Send(packet)
	DeletePacket(packet)
end

function mteg(EventNo)
	Net_MoveToEventGround(EventNo)
end

function Net_StartCommunityEvent(EventNo)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(150)
	packet:PushInt(EventNo)
	Net_Send(packet)
	DeletePacket(packet)
end

function sce(EventNo)
	Net_StartCommunityEvent(EventNo)
end

function Net_EndComuunityEvent()
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(151)
	Net_Send(packet)
	DeletePacket(packet)
end

function ece()
	Net_EndComuunityEvent()
end

g_FastBoxOpen = false
function FastBoxOpen(check)
	g_FastBoxOpen = check
	if check == 0 then
		g_FastBoxOpen = false
	elseif check == 1 then
		g_FastBoxOpen = true
	end
end

function FBO(check)
	FastBoxOpen(check)
end

function fbo(check)
	FastBoxOpen(check)
end

function GetDailyReward()
	local packet = NewPacket(PT_C_M_REQ_GET_DAILY)
	Net_Send(packet)
	DeletePacket(packet)
end

local g_kPaused = false

function Net_TogglePause()
	if g_kPaused then
		Net_Pause(false)
	else
		Net_Pause(true)
	end
end

function Net_Pause(bPaused)
	local packet = NewPacket(PT_C_M_GODCMD)
	packet:PushInt(GMCMD_GROUND_PAUSE)
	packet:PushBool(bPaused)
	Net_Send(packet)
	DeletePacket(packet)
	g_kPaused = bPaused
	if bPaused then
		g_bUpdateProjectile = false
		g_bUpdateTrail = false
		g_bDoSimulate = false
		g_bUpdateWorldObject = false
		g_bUpdateSceneRoot = false
		g_bUpdateDynamicRoot = false
		g_bUpdateStaticRoot = false
		g_bUpdateSelectiveRoot = false
	else
		g_bUpdateProjectile = true
		g_bUpdateTrail = true
		g_bDoSimulate = true
		g_bUpdateWorldObject = true
		g_bUpdateSceneRoot = true
		g_bUpdateDynamicRoot = true
		g_bUpdateStaticRoot = true
		g_bUpdateSelectiveRoot = true
	end
end

local CMODE_FOLLOW = 1
local CMODE_FREE = 5
local g_kPrevCamMode = CMODE_FOLLOW
function ToggleFreeCam()
	local kCamMode = g_world:GetCameraMode()
	if kCamMode == CMODE_FREE then
		g_world:SetCameraMode(g_kPrevCamMode, GetMyActor())
	else
		g_kPrevCamMode = kCamMode
		g_world:SetCameraMode(CMODE_FREE, GetMyActor())
	end
end
