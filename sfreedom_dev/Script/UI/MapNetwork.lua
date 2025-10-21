--[[g_MapNetwork = {}
--임시
g_MapNetwork[9002000] = { ["PREV"]=0, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true} --카오스
g_MapNetwork[9010190] = { ["PREV"]=9018100, ["NEXT"]=9010200,  ["T"]="DEFAULT", ["CANDI0"]=9010100, ["CANDI1"]=9010130} --해안가

--CB 대륙 [ 엘그레고 1~40]
g_MapNetwork[9018200] = { ["PREV"]=0, ["NEXT"]=9010300,  ["T"]="VILLAGE", ["MIN"]=true, ["CANDI0"]=9018210, ["CANDI1"]=9018220, ["CANDI2"]=9018230, ["CANDI3"]=9018240} 	--종소리마을
g_MapNetwork[9018210] = { ["PREV"]=0, ["NEXT"]=9010300,  ["T"]="VILLAGE",  ["MIN"]=true} 	--용자의 거리.
g_MapNetwork[9018220] = { ["PREV"]=0, ["NEXT"]=9010300,  ["T"]="VILLAGE",  ["MIN"]=true}	--호그니와트.
g_MapNetwork[9018230] = { ["PREV"]=0, ["NEXT"]=9010300,  ["T"]="VILLAGE",  ["MIN"]=true}	--사냥꾼 야영지.
g_MapNetwork[9018240] = { ["PREV"]=0, ["NEXT"]=9010300,  ["T"]="VILLAGE",  ["MIN"]=true}	--어두운 뒷골목.
g_MapNetwork[9010300] = { ["PREV"]=9018200, ["NEXT"]=9010330,  ["T"]="DEFAULT"}	--바람평원.
g_MapNetwork[9010330] = { ["PREV"]=9010300, ["NEXT"]=9010400,  ["T"]="DEFAULT"}	--숙련된 용자의 길.
g_MapNetwork[9010400] = { ["PREV"]=9010330, ["NEXT"]=9010430,  ["T"]="DEFAULT"}	--바람협곡.
g_MapNetwork[9010430] = { ["PREV"]=9010400, ["NEXT"]=9018100,  ["T"]="DEFAULT"}	--망각의협곡.
g_MapNetwork[9018100] = { ["PREV"]=9010430, ["NEXT"]=9010190,  ["T"]="VILLAGE"}	--바람항구.
g_MapNetwork[9010100] = { ["PREV"]=9018100, ["NEXT"]=9010200,  ["T"]="DEFAULT", ["DEF"]=9010190}	--수염고래.
g_MapNetwork[9010130] = { ["PREV"]=9018100, ["NEXT"]=9010200,  ["T"]="DEFAULT", ["DEF"]=9010190}	--앵무조개.
g_MapNetwork[9010200] = { ["PREV"]=9010190, ["NEXT"]=9010600,  ["T"]="DEFAULT"}	--용사의 계곡.
g_MapNetwork[9010600] = { ["PREV"]=9010200, ["NEXT"]=9020100,  ["T"]="DEFAULT"}	--위험한 동굴.
g_MapNetwork[9020100] = { ["PREV"]=9010600, ["NEXT"]=9018300,  ["T"]="DEFAULT"}	--악마의 숨결.
g_MapNetwork[9018300] = { ["PREV"]=9020100, ["NEXT"]=0,  ["T"]="VILLAGE", ["MAX"]=true}	--상인의 마을.
-- 엘그레고 히든맵
g_MapNetwork[9013300] = { ["PREV"]=9010330, ["NEXT"]=9002000,  ["T"]="DEFAULT"}	--바람평원위기.
g_MapNetwork[9013400] = { ["PREV"]=9010430, ["NEXT"]=9002000,  ["T"]="DEFAULT"}	--도적단은신처.
g_MapNetwork[9013100] = { ["PREV"]=9010100, ["NEXT"]=9002000,  ["T"]="DEFAULT"}	--노틸러스의분노.
g_MapNetwork[9013200] = { ["PREV"]=9010200, ["NEXT"]=9002000,  ["T"]="DEFAULT"}	--위험한성지.
g_MapNetwork[9013600] = { ["PREV"]=9010600, ["NEXT"]=9002000,  ["T"]="DEFAULT"}	--발록의둥지.
-- 엘그레고 카오스
g_MapNetwork[9012300] = { ["PREV"]=9013300, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--용자의길(카오스).
g_MapNetwork[9012330] = { ["PREV"]=9013300, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--숙련된용자의길(카오스)
g_MapNetwork[9012400] = { ["PREV"]=9013400, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--배신자의고개(카오스)
g_MapNetwork[9012430] = { ["PREV"]=9013400, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--망각의협곡(카오스)
g_MapNetwork[9012100] = { ["PREV"]=9013100, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--수염고래해안가(카오스)
g_MapNetwork[9012130] = { ["PREV"]=9013100, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--앵무조개선착작(카오스)
g_MapNetwork[9012200] = { ["PREV"]=9013200, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--용사의계곡(카오스)
g_MapNetwork[9012600] = { ["PREV"]=9013600, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--위험한동굴(카오스)
g_MapNetwork[9022100] = { ["PREV"]=9013600, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--악마의숨결(카오스)
-- 엘그레고 보스
g_MapNetwork[9010502] = { ["PREV"]=9018100, ["NEXT"]=9010503,  ["T"]="DEFAULT"}	--파렐대저택.
g_MapNetwork[9010503] = { ["PREV"]=9010502, ["NEXT"]=0,  ["T"]="BOSS", ["MAX"]=true}			--파렐.
g_MapNetwork[9020201] = { ["PREV"]=9020100, ["NEXT"]=9020200,  ["T"]="DEFAULT"}	--마그마던전.
g_MapNetwork[9020200] = { ["PREV"]=9020201, ["NEXT"]=0,  ["T"]="BOSS", ["MAX"]=true}			--라발론.

--OB 대륙 [파미노르 41~80]
g_MapNetwork[9020300] = { ["PREV"]=0, ["NEXT"]=9020400,  ["T"]="DEFAULT", ["MIN"]=true}	--이글거리는 늪지대.
g_MapNetwork[9020400] = { ["PREV"]=9020300, ["NEXT"]=9020500,  ["T"]="DEFAULT"}	--큰나무 지대.
g_MapNetwork[9020500] = { ["PREV"]=9020400, ["NEXT"]=9028100,  ["T"]="DEFAULT"}	--신들의 안식처.
g_MapNetwork[9028100] = { ["PREV"]=9020500, ["NEXT"]=9030100,  ["T"]="VILLAGE"}	--버섯 마을.
g_MapNetwork[9030100] = { ["PREV"]=9028100, ["NEXT"]=9030130,  ["T"]="DEFAULT"}	--잊혀진 마을.
g_MapNetwork[9030130] = { ["PREV"]=9030100, ["NEXT"]=9030200,  ["T"]="DEFAULT"}	--잠자는 마을.
g_MapNetwork[9030200] = { ["PREV"]=9030130, ["NEXT"]=9030230,  ["T"]="DEFAULT"}	--유혹의 땅.
g_MapNetwork[9030230] = { ["PREV"]=9030200, ["NEXT"]=9038200,  ["T"]="DEFAULT"}	--이슬의 숲.
g_MapNetwork[9038200] = { ["PREV"]=9030230, ["NEXT"]=9030300,  ["T"]="VILLAGE"}	--엘로라주둔지.
g_MapNetwork[9030300] = { ["PREV"]=9038200, ["NEXT"]=9030330,  ["T"]="DEFAULT"}	--대강당.
g_MapNetwork[9030330] = { ["PREV"]=9030300, ["NEXT"]=9030360,  ["T"]="DEFAULT"}	--중앙 기관실.
g_MapNetwork[9030360] = { ["PREV"]=9030330, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}		--달빛 테라스.
-- 파미노르 히든맵
g_MapNetwork[9023300] = { ["PREV"]=9020300, ["NEXT"]=9002000,  ["T"]="DEFAULT"}	--반디서식지.
g_MapNetwork[9033100] = { ["PREV"]=9030130, ["NEXT"]=9002000,  ["T"]="DEFAULT"}	--환영의숲.
g_MapNetwork[9033200] = { ["PREV"]=9030230, ["NEXT"]=9002000,  ["T"]="DEFAULT"}	--영혼의 수정호수.
g_MapNetwork[9033300] = { ["PREV"]=9038200, ["NEXT"]=9002000,  ["T"]="DEFAULT"}	--비밀지하창고.
-- 파미노르 카오스
g_MapNetwork[9022300] = { ["PREV"]=9023300, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--반디서식지.
g_MapNetwork[9022400] = { ["PREV"]=9023300, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--반디서식지.
g_MapNetwork[9022500] = { ["PREV"]=9033100, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--반디서식지.
g_MapNetwork[9032100] = { ["PREV"]=9033100, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--반디서식지.
g_MapNetwork[9032130] = { ["PREV"]=9033100, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--반디서식지.
g_MapNetwork[9032200] = { ["PREV"]=9033200, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--반디서식지.
g_MapNetwork[9032230] = { ["PREV"]=9033200, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--반디서식지.
g_MapNetwork[9032300] = { ["PREV"]=9033300, ["NEXT"]=9032330,  ["T"]="DEFAULT"}	--반디서식지.
g_MapNetwork[9032330] = { ["PREV"]=9032300, ["NEXT"]=9032360,  ["T"]="DEFAULT"}	--반디서식지.
g_MapNetwork[9032360] = { ["PREV"]=9032330, ["NEXT"]=0,  ["T"]="DEFAULT", ["MAX"]=true}	--반디서식지.
-- 파미노르 보스
g_MapNetwork[9020601] = { ["PREV"]=9020500, ["NEXT"]=9020600,  ["T"]="DEFAULT"}	--사파이어던전.
g_MapNetwork[9020600] = { ["PREV"]=9020601, ["NEXT"]=0,  ["T"]="BOSS", ["MAX"]=true}			--그렘그리스.
g_MapNetwork[9030401] = { ["PREV"]=9030330, ["NEXT"]=9030400,  ["T"]="DEFAULT"}	--자가람토스.
g_MapNetwork[9030400] = { ["PREV"]=9030401, ["NEXT"]=0,  ["T"]="BOSS", ["MAX"]=true}	--자가람토스.

]]
g_MapNetUVSelect = {["NONE"]= 0, ["VILLAGE"]=3, ["DEFAULT"]= 1, ["BOSS"]= 1, ["HIDDEN"] = 1}
function OnMapNetworkTypeIcon(wnd, mapID)
	if true == wnd:IsNil() then return end

	local NowInfoTbl = g_GroundPos[mapID]
	if NowInfoTbl == nil then return end
	
	local wndNow = wnd:GetControl("IMG_MAP_NOW")
	if true == wndNow:IsNil() then return end
	if string.len(NowInfoTbl["TYPE"]) > 0 then
		if g_MapNetUVSelect[NowInfoTbl["TYPE"]] ~= nil then
			wndNow:SetUVIndex(g_MapNetUVSelect[NowInfoTbl["TYPE"]])
		end
	end

	local wndPrev = wnd:GetControl("FRM_MAP_PREV")
	if true == wndPrev:IsNil() then return end
	if NowInfoTbl["PREV"] ~= 0 then
		wndPrev:Visible(true)
		local FrevInfoTbl = g_GroundPos[NowInfoTbl["PREV"]]
		if string.len(FrevInfoTbl["TYPE"]) > 0 then
			if g_MapNetUVSelect[FrevInfoTbl["TYPE"]] ~= nil then
				local wndPrev_child = wndPrev:GetControl("IMG_ICON")
				if false == wndPrev_child:IsNil() then
					wndPrev:SetStaticTextW(GetMapNameW(NowInfoTbl["PREV"]))
					wndPrev_child:SetUVIndex(g_MapNetUVSelect[FrevInfoTbl["TYPE"]] + 1)
				end
			end
		end
	else
		wndPrev:Visible(false)
	end
	
	local wndNext = wnd:GetControl("FRM_MAP_NEXT")
	if true == wndNext:IsNil() then return end
	if NowInfoTbl["NEXT"] ~= 0 then
		wndNext:Visible(true)
		local NextInfoTbl = g_GroundPos[NowInfoTbl["NEXT"]]
		if string.len(NextInfoTbl["TYPE"]) > 0 then
			if g_MapNetUVSelect[NextInfoTbl["TYPE"]] ~= nil then
				local wndNext_child = wndNext:GetControl("IMG_ICON")
				if false == wndNext_child:IsNil() then
					wndNext:SetStaticTextW(GetMapNameW(NowInfoTbl["NEXT"]))
					wndNext_child:SetUVIndex(g_MapNetUVSelect[NextInfoTbl["TYPE"]] + 1)
				end
			end
		end
	else
		wndNext:Visible(false)	
	end
end

g_MapCenterNum = 2
g_bMaxLimitCatch = false;
g_bMinLimitCatch = false;
function OnMapNetworkTypeIcon2(wnd, mapID, Move)
	if g_bMaxLimitCatch == true and Move < 0 then
		return
	elseif g_bMinLimitCatch == true and Move > 0 then
		return
	else
		g_bMaxLimitCatch = false
		g_bMinLimitCatch = false
	end

	if true == wnd:IsNil() then 
		return 
	end

	if Move == 0 then
		g_MapCenterNum = 2
	else
		g_MapCenterNum = g_MapCenterNum + Move
	end
		
	local NextMapNo = 0
	local InfoTbl = g_GroundPos[mapID]
	if nil ~= InfoTbl then
		NextMapNo = InfoTbl["PREV"]
	end
	
	if g_MapCenterNum > 0 then
		IconSetting(wnd, NextMapNo, g_MapCenterNum-1, 0, -1, false)
	end
	if g_MapCenterNum <= 4 then
		IconSetting(wnd, mapID, g_MapCenterNum, 4, 1, true)
	end
end

function IconSetting(UIParent, MapNo, idx, End, Calc, UserPos)
	local InfoTbl = g_GroundPos[MapNo]
	local NextMapNo = 0
	
	if nil ~= InfoTbl then
		if Calc < 0 then
			NextMapNo = InfoTbl["PREV"]
		else
			NextMapNo = InfoTbl["NEXT"]
		end
		
		if  idx == 2 then
			if nil ~= InfoTbl["MAX"] and InfoTbl["MAX"] == true then
				g_bMaxLimitCatch = true;
			elseif nil ~= InfoTbl["MIN"] and InfoTbl["MIN"] == true then
				g_bMinLimitCatch = true;
			end
		end	
	end
		
	if idx >= 0 and idx <= 4 then
		local IconWnd = UIParent:GetControl("FRM_MAPICON"..idx)
		if false == IconWnd:IsNil() then
			if nil == InfoTbl then
				IconWnd:Visible(false)
			else
				if false == IconWnd:IsVisible() then
					IconWnd:Visible(true)
				end
				IconWnd:SetStaticText(GetMapNameW(MapNo):GetStr())
				if UserPos == false then
					IconWnd:SetFontColorRGBA(255, 255, 255, 255)
--					ODS("IconSetting ".. MapNo .. " TYPE : " .. InfoTbl["TYPE"] .. "\n", false, 1509)
					IconWnd:SetUVIndex(g_MapNetUVSelect[InfoTbl["TYPE"]] + 1)
				else
					IconWnd:SetFontColorRGBA(230, 200, 40, 255)
					IconWnd:SetUVIndex(g_MapNetUVSelect[InfoTbl["TYPE"]])
				end
			end
		end
	end
	if idx ~= End then
		IconSetting(UIParent, NextMapNo, idx + Calc, End, Calc, false)
	else
	 return
	end
end
