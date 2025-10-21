--[[
dofile("Script/Map/Delhapa.lua")
dofile("Script/Map/BigMaze.lua")
dofile("Script/Map/BigMaze_R.lua")
dofile("Script/Map/Boss.lua")
dofile("Script/Map/Dawiner.lua")
dofile("Script/Map/MyRoom.lua")
dofile("Script/Map/Sneartrail.lua")
dofile("Script/Map/Sneartrail02.lua")
dofile("Script/Map/Store.lua")
dofile("Script/Map/WhiteCliff1.lua")
dofile("Script/Map/WhiteCliff2.lua")
dofile("Script/Map/nabra.lua")
]]--
dofile("Script/Map/WarZone.lua")
dofile("Script/Map/BossSaphire.lua")

-- 01_Ingrid
--[[
dofile("Script/Map/01_Ingrid/018200_SC.lua")
dofile("Script/Map/01_Ingrid/010100_FD.lua")
dofile("Script/Map/01_Ingrid/010200_FD.lua")
dofile("Script/Map/01_Ingrid/010201_FD.lua")
dofile("Script/Map/01_Ingrid/010202_FD.lua")
dofile("Script/Map/01_Ingrid/010203_FD.lua")
dofile("Script/Map/01_Ingrid/010204_FD.lua")
dofile("Script/Map/01_Ingrid/010300_FD.lua")
dofile("Script/Map/01_Ingrid/010400_FD.lua")
dofile("Script/Map/01_Ingrid/010401_FD.lua")
dofile("Script/Map/01_Ingrid/010402_FD.lua")
dofile("Script/Map/01_Ingrid/010403_FD.lua")
dofile("Script/Map/01_Ingrid/010404_FD.lua")
dofile("Script/Map/01_Ingrid/010501_DG.lua")
dofile("Script/Map/01_Ingrid/010502_DG.lua")
dofile("Script/Map/01_Ingrid/010600_DG.lua")
dofile("Script/Map/01_Ingrid/018100_BC.lua")
]]--

-- 02_ChaosArea
--[[
dofile("Script/Map/02_ChaosArea/028100_SC.lua")
dofile("Script/Map/02_ChaosArea/020100_FD.lua")
dofile("Script/Map/02_ChaosArea/020101_FD.lua")
dofile("Script/Map/02_ChaosArea/020102_FD.lua")
dofile("Script/Map/02_ChaosArea/020103_FD.lua")
dofile("Script/Map/02_ChaosArea/020104_FD.lua")
dofile("Script/Map/02_ChaosArea/020300_FD.lua")
dofile("Script/Map/02_ChaosArea/020301_FD.lua")
dofile("Script/Map/02_ChaosArea/020302_FD.lua")
dofile("Script/Map/02_ChaosArea/020303_FD.lua")
dofile("Script/Map/02_ChaosArea/020304_FD.lua")
dofile("Script/Map/02_ChaosArea/020400_FD.lua")
dofile("Script/Map/02_ChaosArea/020401_FD.lua")
dofile("Script/Map/02_ChaosArea/020402_FD.lua")
dofile("Script/Map/02_ChaosArea/020403_FD.lua")
dofile("Script/Map/02_ChaosArea/020404_FD.lua")
dofile("Script/Map/02_ChaosArea/020500_FD.lua")
]]--


-- 99 Boss
dofile("Script/Map/99_Boss/010503_DG.lua")
dofile("Script/Map/99_Boss/020200_DG.lua")

function Map_ParseXml( in_map, xmlPath, iDiff)
	--InfoLog(1, "Map_ParseXml1")
	if in_map == nil or xmlPath == nil then
		InfoLog(1, "map.lua : Map_ParseXml(maybe nil, maybe nil) called")
		return false
	end

	--InfoLog(1, "Map_ParseXml2")
	local doc = Xml_ParseFile(xmlPath)
	if doc == nil then
		InfoLog(1, "map.lua : " .. xmlPath .. " is invalid xml file")
		return false
	end

	--InfoLog(1, "Map_ParseXml3")
	local map = GroundResource(in_map)

	node = doc[0]	-- root node
	if node.name ~= "WORLD" then
		InfoLog(1, "map.lua : " .. xmlPath .. " isn't map xml")
		return false
	end

	--InfoLog(9, "Map_ParseXml4")
	local i = 0
	while i < node.n do
		child = node[i]
		if child.name == "NIFPATH" then
			--InfoLog(1, "Map_ParseXml NIF")
			map:LoadNif(string.sub(child[0], 4))
		elseif child.name == "GSAPATH" then
			--InfoLog(1, "Map_ParseXml GSA")
			map:LoadGsa(string.sub(child[0], 4))
		elseif child.name == "DECORATOR" then
			map:SetDecorator(child[0])			
		elseif child.name == "NPC" then
			attr = child.attr
			npc_type = "FIXED"	-- default value
			for key, value in pairs(attr) do
				if key == "NAME" then
					npc_name = value
				elseif key == "ACTOR" then
					npc_actor = value
				elseif key == "SCRIPT" then
					npc_script = value
				elseif key == "LOCATION" then
					npc_location = value
				elseif key == "GUID" then
					npc_guid = value
				elseif key == "KID" then
					npc_id = value
				elseif key == "TYPE" then
					npc_type = value
				end
			end
			--InfoLog(9, "Map_ParseXml..AddNPC")
			map:AddNPC(npc_name, npc_actor, npc_script, npc_location, GUID(npc_guid), npc_id)
			--InfoLog(9, "Map_ParseXml..SetNPC")
			map:SetNPC(GUID(npc_guid), npc_type)
			--InfoLog(9, "Map_ParseXml..SetNPC..END")

		elseif child.name == "SCRIPT" then
			if child.attr ~= nil then
				for key, value in pairs(child.attr) do
					if "INIT" == key then
						map:SetInitScript( value )
					elseif "READY" == key then
						map:SetReadyScript( value )
					elseif "UPDATE" == key then
						map:SetUpdateScript( value )
					elseif "START" == key then
						map:SetStartScript( value )
					end
				end
			end	
		elseif child.name == "WORLD_ACTION_SETTING" then
			attr = child.attr
			local j = 0
			while j<child.n do
				childchild = child[j]
				if childchild.name == "WORLD_ACTION" then
					local dwDuration = 0
					for key, value in pairs(childchild.attr) do
						if key == "ID" then
							iID = value
						elseif key == "SCRIPT" then
							strScriptName = value
						elseif key == "DURATION" then
							dwDuration = tonumber(value)
						end
					end
					map:AddWorldAction(iID, strScriptName, dwDuration)
				end
				j = j+1
			end -- while j<child.n do
		elseif child.name == "STONES" then
			kAttr = child.attr
			local iMin = 0
			local iMax = 0
			if nil ~= kAttr then
				for kKey, kVal in pairs(kAttr) do
					if kKey == "MINIMAM" then
						iMin = kVal
					elseif kKey == "MAXIMAM" then
						iMax = kVal
					end
				end
				map:SetMaxStone(iMin, iMax)
				--<STONES> <STONE> </STONES>
				local j = 0
				while j<child.n do
					childchild = child[j]
					if childchild.name == "STONE" then
						kChildAttr = childchild.attr
						if nil ~= kChildAttr then
							local iID = 0
							local fX, fY, fZ = 0, 0, 0
							local iRate, iItemBagNo = 0, 0
							for kKey, kVal in pairs(kChildAttr) do
								if kKey == "POSX" then
									fX = kVal
								elseif kKey == "POSY" then
									fY = kVal
								elseif kKey == "POSZ" then
									fZ = kVal
								elseif kKey == "RATE" then
									iRate = kVal
								elseif kKey == "ID" then
									iID  = kVal
								elseif kKey == "ITEMBAGNO" then
									iItemBagNo = kVal
								end
							end
							map:AddStone(iID, fX, fY, fZ, iItemBagNo, iRate)
						end
					end
					j = j + 1
				end--while
			end
		elseif child.name == "PVP" then
			attr = child.attr
			local i = 0
			while i < child.n do
				childchild = child[i]
				if childchild.name == "HILL" then
					local iID = 0
					local kName = ''
					for key, value in pairs(childchild.attr) do
						if key == "NAME" then
							kName = value
						elseif key =="NO" then
							iID = value
						end
						
						if 0 ~= iID and kName ~= '' then
							map:LoadHill(iID,kName)
						end
					end
				end
				i = i + 1
			end
				
		
		elseif child.name == "SECTION" then
			attr = child.attr
			local SectionID = ""
			local iThisDiff = 0;
			for key, value in pairs(child.attr) do
				if key == "ID" then
					SectionID = value
				elseif key == "DIFF" then	-- 난이도
					iThisDiff = tonumber(value)
				end
			end
--			if iThisDiff == iDiff then	-- 난이도에 맞으면 파싱
			local NewSc = Section(SectionID)
			local j = 0
			while j<child.n do
				childchild = child[j]
				if childchild.name == "GOAL" then
					local NewGoal = Goal();
					local iID = 0
					local k = 0
					local iRandom = 100
					local iOr = 0
					local szName = ""
					while k<childchild.n do
						local childchild3 = childchild[k]
						szName = childchild3.name
						if szName == "SETTING" then
							for key, value in pairs(childchild3.attr) do
								if key == "ID" then
									iID = tonumber(value)
								elseif key == "CLEAR_ALL" then
									iOr = tonumber(value)
								elseif key == "RANDOM" then
									iRandom = tonumber(value)
								end
							end
							NewGoal:Setting(iID, iOr, iRandom)
						elseif szName == "REQ_ITEM" or szName == "REQ_MONSTER" or szName == "REQ_OBJECT" then
							local iMonID = 0
							local iCount = 0
							for key, value in pairs(childchild3.attr) do
								if key == "ID" then
									iMonID = tonumber(value)
								elseif key == "COUNT" then
									iCount = tonumber(value)
								end
							end
							NewGoal:AddObject(szName, iMonID, iCount)
						end
						k = k + 1
					end
					NewSc:AddGoal(NewGoal)
				elseif childchild.name == "SCRIPT" then
					local FuncName = ""
					local TypeName = ""
					for key, value in pairs(childchild.attr) do
						if key == "ID" then
							FuncName = value
						elseif key == "TYPE" then
							TypeName = value
						end
					end

					NewSc:AddScript(TypeName, FuncName)
				end
				j = j + 1
			end
			map:AddSection(NewSc) -- 여기서 섹션 하나 추가
--			end
		-- MonArea 임시로 막았다.
		--elseif child.name == "MON_AREA" then
		--	map:LoadMonArea(string.sub(child[0], 4))
		end

		i = i + 1
	end

	return true
end

--[[
function MonsterArea_ParseXml( kMng, xmlPath)
	--InfoLog(9, "MonsterArea_ParseXml--")
	local doc = Xml_ParseFile(xmlPath)
	if doc == nil then
		InfoLog(5, "map.lua : " .. xmlPath .. " is invalid xml file")
		return false
	end

	--InfoLog(9, "MonsterArea_ParseXml 10")
	node = doc[0]	-- root node
	if node.name ~= "MONAREA" then
		InfoLog(2, "map.lua : " .. xmlPath .. " isn't MonArea xml")
		return false
	end
	
	--InfoLog(9, "MonsterArea_ParseXml 20")
	local i = 0
	while i < node.n do
		child = node[i]
		-- AREA	
		if child.name == "AREA" then
			local kArea = {}

			attr = child.attr
			for key, value in pairs(attr) do
				if key == "ID" then
					kArea.iID = value
				elseif key == "AREA_TYPE" then
					kArea.Type = value
				end
			end
			local kMonArea = kMng:Add(kArea.iID, kArea.Type)
			if kMonArea:IsNil() == true then
				InfoLog(2, "AddMonArea failed ID:" ..kArea.iID .. ", Type:" ..kArea.Type)
				return false
			end

			--InfoLog(9, "MonsterArea_ParseXml Type:" .. kArea.Type)
			if kArea.Type == "1" then	-- Plane
				--InfoLog(9, "MonsterArea_ParseXml 51")	
				
				local j = 0
				while j < child.n do
					childchild = child[j]
				
					if childchild.name == "POSITION1" then
						kArea.pos1 = childchild[0]
					elseif childchild.name == "POSITION2" then
						kArea.pos2 = childchild[0]
					elseif childchild.name == "POSITION3" then
						kArea.pos3 = childchild[0]
					elseif childchild.name == "POSITION4" then
						kArea.pos4 = childchild[0]
					elseif childchild.name == "LINK_AREA" then
						kMonArea:AddLink(childchild[0])
					end
									
					j = j + 1
				end -- while
				kMonArea:SetPos(Point3(kArea.pos1), Point3(kArea.pos2), Point3(kArea.pos3), Point3(kArea.pos4))
			elseif kArea.Type == "2" then	-- Circle
				--InfoLog(9, "MonsterArea_ParseXml 61")	
				local j = 0
				while j < child.n do
					childchild = child[j]
		
					if childchild.name == "CENTER" then
						kArea.center = childchild[0]
					elseif childchild.name == "RADIUS" then
						kArea.radius = childchild[0]
					elseif childchild.name == "LINK_AREA" then
						kMonArea:AddLink(childchild[0])
					end
									
					j = j + 1
				end -- while
				kMonArea:SetCircle(Point3(kArea.center), kArea.radius)
			end
		end
		i = i + 1
	end
	return true
end
]]

