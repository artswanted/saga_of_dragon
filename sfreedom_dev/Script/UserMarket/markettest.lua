function UserMarketAdd(invtype,invpos)
	
	local kValue = INT64()
    	kValue:Set(10, 0)
    
	local packet = NewPacket(16102)

	packet:PushByte(invtype)
	packet:PushByte(invpos)
	packet:PushByte(0)
	packet:PushInt64(kValue)
	packet:PushByte(0)

	Net_Send(packet)
	DeletePacket(packet)
	
end

function UserMarketDereg(kArticleGuid)

	local packet = NewPacket(16106)
	local guid = GUID(kArticleGuid)
	
	packet:PushGuid(guid)
	
	Net_Send(packet)
	DeletePacket(packet)
	
end

function UserMarketBuy(kArticleGuid)

	local packet = NewPacket(16114)
	local guid = GUID(kArticleGuid)
	
	packet:PushGuid(guid)
	
	Net_Send(packet)
	DeletePacket(packet)
	
end


function UserMarketRead(kDealingGuid)

	local packet = NewPacket(16126)
	local guid = GUID(kDealingGuid)
	
	packet:PushGuid(guid)
	
	Net_Send(packet)
	DeletePacket(packet)
	
end

function UserMarketDelete(kDealingGuid)

	local packet = NewPacket(16122)
	local guid = GUID(kDealingGuid)
	
	packet:PushGuid(guid)
	
	Net_Send(packet)
	DeletePacket(packet)
	
end

function SearchArticle(Name,NameType,Mode,ModeType,Min,Max,Grade,CostType)

	g_SearchName = Name
	g_SearchNameType = NameType
	g_SearchMode = Mode
	g_SearchModetype = ModeType
	g_SearchMinLevel = Min
	g_SearchMaxLevel = Max
	g_SearchGrade = Grade
	g_CostType = CostType

	local packet = NewPacket(16110)
	local kWName = WideString(Name)
	local kValue = INT64()
	kValue:Set(0, 0)
    
	packet:PushWString(kWName)
	packet:PushByte(NameType)
	packet:PushInt(Mode)
	packet:PushByte(ModeType)
	packet:PushInt(Min)
	packet:PushInt(Max)
	packet:PushInt(Grade)
	packet:PushInt64(kValue)
	packet:PushByte(CostType)
	packet:PushInt64(kValue)
	
	Net_Send(packet)
	DeletePacket(packet)
end

function SearchMyArticle()

	local packet = NewPacket(16134)
	Net_Send(packet)
	DeletePacket(packet)

end

function UserMarketRegAll()

	for invtype = 1,4 do
	
		for invpos = 0,35 do
		
			UserMarketAdd(invtype,invpos)
		
		end
		
	end

end

function UserMarketDealing()

	local packet = NewPacket(16118)
	Net_Send(packet)
	DeletePacket(packet)

end

function Net_PT_M_C_UM_ANS_ARTICLE_REG(packet)
	
	local result = packet:PopByte()
	ODS("[PT_M_C_UM_ANS_ARTICLE_REG] Result:" ..result .."\n")
	
end

function Net_PT_M_C_UM_ANS_ARTICLE_DEREG(packet)

	local result = packet:PopByte()
	ODS("[PT_M_C_UM_ANS_ARTICLE_DEREG] Result:" ..result .."\n")

end

function Net_PT_M_C_UM_ANS_ARTICLE_QUERY(packet)

	local result = packet:PopByte()
	local articlecount = packet:PopSize_t()
	
	ODS("[PT_M_C_UM_ANS_ARTICLE_QUERY] Result:" ..result .."Count:" ..articlecount .."\n")

	for counter = 1,articlecount do
	
		local guid = packet:PopGuid()
		local index = packet:PopInt64()
		local sellername = packet:PopWString()
		local cost packet:PopInt64()
		local costtype = packet:PopByte()
		local itemno = packet:PopInt()
		local itemcount = packet:PopWord()
		local enchant_01 = packet:PopInt64()
		local enchant_02 = packet:PopInt64()
		local articlestate = packet:PopByte()
		local itemstate = packet:PopByte()
		local timetype = packet:PopByte()
	
		ODS("Article:" ..guid:GetString() .." Seller:" ..sellername .." ArticleState:" ..articlestate " ItemState:" ..itemstate .."\n")
	
	end

end
	
function Net_PT_M_C_UM_ANS_ARTICLE_BUY(packet)

	local result = packet:PopByte()
	ODS("[PT_M_C_UM_ANS_ARTICLE_BUY] Result:" ..result .."\n")

end
	
function Net_PT_M_C_UM_ANS_DEALINGS_QUERY(packet)

	local result = packet:PopByte()
	ODS("[PT_M_C_UM_ANS_DEALINGS_QUERY] Result:" ..result .."\n")

end

function Net_PT_M_C_UM_ANS_DEALINGS_DELETE(packet)

end

function Net_PT_M_C_UM_ANS_DEALINGS_READ(packet)

end

function Net_PT_M_C_UM_ANS_MINIMUM_COST_QUERY(packet)

end

function Net_PT_M_C_UM_ANS_MY_ARTICLE_QUERY(packet)


	local result = packet:PopByte()
	local articlecount = packet:PopSize_t()
	
	ODS("[PT_M_C_UM_ANS_MY_ARTICLE_QUERY] Result:" ..result .."Count:" ..articlecount .."\n")

	for counter = 1,articlecount do
	
		local guid = packet:PopGuid()
		local index = packet:PopInt64()
		local sellername = packet:PopWString()
		local cost packet:PopInt64()
		local costtype = packet:PopByte()
		local itemno = packet:PopInt()
		local itemcount = packet:PopWord()
		local enchant_01 = packet:PopInt64()
		local enchant_02 = packet:PopInt64()
		local articlestate = packet:PopByte()
		local itemstate = packet:PopByte()
		local timetype = packet:PopByte()
	
		ODS("Article:" ..guid:GetString() .." Seller:" ..sellername .." ArticleState:" ..articlestate " ItemState:" ..itemstate .."\n")
	
	end

end