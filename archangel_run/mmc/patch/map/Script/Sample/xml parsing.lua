-- parser sample
function lol(doc)
	local i = 0
	
	while i < doc.n do
		element = doc[i]

		-- tag 
		if type(element) == "table" then
			InfoLog(1, element.name)
		
			-- attr
			if element.attr ~= nil then
				for key, value in pairs(element.attr) do
					InfoLog(1, key .. " = " .. value)
				end
			end
	
			-- children
			lol(element)
		
		-- text
		elseif type(element) == "string" then
			InfoLog(1, element)
		end

		-- sibling
		i = i + 1
	end
end

doc = Xml_ParseFile("test.xml")
if doc ~= nil then
	lol(doc)
end
