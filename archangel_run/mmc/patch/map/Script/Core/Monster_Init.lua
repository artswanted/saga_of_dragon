dofile("Script/Monster/Monster.lua")
dofile("Script/Monster/MonsterAI.lua")
dofile("Script/Monster/saphire_boss.lua")
dofile("Script/Monster/Boss/b_default.lua")
dofile("Script/Monster/Boss/b_lavalon.lua")

--dofile("Script/Monster/Monster_200002.lua")
--dofile("Script/Monster/Monster_200016.lua")
--dofile("Script/Monster/Monster_200020.lua")


function MonsterAI_Pattern( kMng, xmlPath)
	local doc = Xml_ParseFile(xmlPath)
	if doc == nil then
		InfoLog(5, "MonsterAI_Pattern : (" .. xmlPath .. ") is invalid xml file")
		return false
	end

	node = doc[0]	-- root node
	if node.name ~= "AI" then
		InfoLog(5, "MonsterAI_Pattern : " .. xmlPath .. " isn't AI xml")
		return false
	end
	
	--InfoLog(9, "MonsterAI_Pattern 20")
	local i = 0
	while i < node.n do
		child = node[i]
		--InfoLog(9, "MonsterAI_Pattern 21")
		-- PATTERN	
		if child.name == "PATTERN" then
			local kPat = {}
			local attr = child.attr
			--InfoLog(9, "MonsterAI_Pattern 22")
			for key, value in pairs(attr) do
				if key == "ID" then
					kPat.iID = value
				elseif key == "NAME" then
					kPat.Name = value
				end
			end
			--InfoLog(9, "MonsterAI_Pattern 23")
			if kMng:Add(kPat.iID, kPat.Name) == false then
				return false
			end
			--InfoLog(9, "MonsterAI_Pattern 24")
			
			--InfoLog(9, "MonsterAI_Pattern 30")
			local j = 0
			while j < child.n do
				childchild = child[j]
				
				if childchild.name == "ACT_CHANGE" then
					local attr = childchild.attr
					for key, value in pairs(attr) do
						if key == "FROM" then
							kPat.From = value
						elseif key == "TO" then
							kPat.To = value
						elseif key == "WEIGHT" then
							kPat.weight = value
						end
					end
					kMng:AddActTransit(kPat.iID, kPat.From, kPat.To, kPat.weight)
				end

				j = j + 1
			end -- while
		end
		i = i + 1
	end
	return true
end
