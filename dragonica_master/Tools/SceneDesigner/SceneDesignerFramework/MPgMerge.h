#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgMerge
	{
	public:
		MPgMerge()
		{
		}
		~MPgMerge()
		{
		}

		MScene* ParsingGSA(String* pkPathStr);
		String* ParsingPGPropertyType(MEntity* pkTarget);

	private:
	};
}}}}