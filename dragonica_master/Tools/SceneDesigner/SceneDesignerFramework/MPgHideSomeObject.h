#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgHideSomeObject
	{
	public:
		MPgHideSomeObject(void);
		virtual ~MPgHideSomeObject(void);

	public:
		bool Initialize();
		bool HideNeedlessObject();

	public:
		bool bIsHide;
	};
}}}}

