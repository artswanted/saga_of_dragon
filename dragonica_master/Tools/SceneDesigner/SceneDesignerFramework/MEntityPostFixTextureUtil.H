#pragma once

#include "MFramework.H"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{

	public __gc class MEntityPostFixTextureUtil
	{
	public:

		static	String* ApplyPostFixTextureToEntity(MEntity* pmEntity, String* pkNewPostFix);

	private:

		static	bool	ApplyPostFixTextureToTexture(NiTexturingProperty::Map *pkMap,String* pkNewPostFix,String* pkOldPostFix);
		static	bool	ApplyPostFixTextureToSceneRoot(MEntity* pmEntity,NiNode* pkRoot,String* pkNewPostFix,String* pkOldPostFix);
		static	bool	ApplyPostFixTextureToGeometry(NiGeometry* pkGeom,String* pkNewPostFix,String* pkOldPostFix);
		static	bool	CheckCanApplyPostFixTextureToGeometry(NiGeometry* pkGeom,String* pkNewPostFix,String* pkOldPostFix);
		static	bool	CheckCanApplyPostFixTextureToTexture(NiTexturingProperty::Map *pkMap,String* pkNewPostFix,String* pkOldPostFix);
		static	bool	ReplacePostFixTextureOldToNew(NiTexturingProperty::Map *pkMap,String* pkNewTexFileName);
		static	void	GetAllGeometries(const NiNode *pkNode, NiObjectList &kGeometries);

		static	String*	GetNewTexFileName(String *pkTexFileName,String *pkNewPostFix,String *pkOldPostFix);
	};
	}}}};