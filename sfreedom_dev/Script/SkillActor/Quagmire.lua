function Quagmire_Initialize(actor)
	
	ODS("Quagmire_Initialize\n")
	actor:FreeMove(true)
    actor:HideNode("ef_linker",true);
    local kPos = actor:GetNodeWorldPos("ef_linker");    
    actor:AttachParticleToPoint(1,kPos,"ef_Quagmire_01");
	actor:AttachSound(1500,"Quagmire2");
    
end

function Quagmire_Initialize2(actor)
	
	ODS("Quagmire_Initialize2\n")

    -- actor:HideNode("ef_linker",true);
    -- local kPos = actor:GetNodeWorldPos("ef_linker");    
    -- actor:AttachParticleToPoint(1,kPos,"ef_Quagmire_01");
	-- actor:AttachSound(1500,"Quagmire2");
    
end