#ifndef NX_PHYSICS_NX_IMPLICITMESHDESC
#define NX_PHYSICS_NX_IMPLICITMESHDESC
/** \addtogroup fluids
  @{
*/
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NovodeX Technology
|
|							     www.novodex.com
|
\*----------------------------------------------------------------------------*/

#include "fluids/NxImplicitMesh.h"
#include "fluids/NxMeshData.h"
/**
\brief Specifies the parameters for implict mesh generation.

\warning <b>Preliminary API, subject to change</b>

@see NxImplicitMesh
*/
class NxImplicitMeshDesc
	{
	public:

	/**
	\brief The maximal distance at which points are considered for surface generation.
	
	The larger this radius is set, the smoother the surface gets and the less influence an 
	individual point has.
	
	This parameter has a large impact on the performance of surface generation. 

	\warning <b>Preliminary API, subject to change</b>
	*/
	NxReal		surfaceRadius;

	/**
	\brief The distance between the fluid surface and one separated particle.
	
	Must be smaller than the surfaceRadius. Giving a small radius relative to the surfaceRadius produces small 
	blobs for individual particles.

	\warning <b>Preliminary API, subject to change</b>
	*/
	NxReal		singleRadius;

	/**
	\brief 	Specifies how the surface around particles bulges.
	
	Smaller values produce less bumpy surfaces which have more temporal discontinuities (popping).

	\warning <b>Preliminary API, subject to change</b>
	*/
	NxReal		blend;

	/**
	\brief Sets the spatial resolution of the surface mesh generated.
	
	The size of a triangle edge in the resulting mesh is limited with this value.

	\warning <b>Preliminary API, subject to change</b>
  	*/
	NxReal		triangleSize;

	/**
	\brief Sets the number of smoothing iterations performed on the mesh.
	
	More smoothing is less efficient but gives a smoother mesh.

	\warning <b>Preliminary API, subject to change</b>
	*/
	NxU32		smoothingIterations;

	/**
	\brief Defines how the generated mesh is written to user mesh buffers.

	\warning <b>Preliminary API, subject to change</b>

	@see NxMeshData
	*/
	NxMeshData	meshData;

	void*		userData;		//!< Will be copied to NxImplicitMesh::userData.
	const char*	name;			//!< Possible debug name. The string is not copied by the SDK, only the pointer is stored.

	NX_INLINE ~NxImplicitMeshDesc();
	/**
	\brief (Re)sets the structure to the default.	

	\warning <b>Preliminary API, subject to change</b>
	*/
	NX_INLINE void setToDefault();

	/**
	\brief Returns true if the current settings are valid

	\warning <b>Preliminary API, subject to change</b>
	*/
	NX_INLINE bool isValid() const;

	/**
	\brief Constructor sets to default.

	\warning <b>Preliminary API, subject to change</b>
	*/
	NX_INLINE	NxImplicitMeshDesc();
	};


NX_INLINE NxImplicitMeshDesc::NxImplicitMeshDesc()
	{
	setToDefault();
	}

NX_INLINE NxImplicitMeshDesc::~NxImplicitMeshDesc()
	{
	}

NX_INLINE void NxImplicitMeshDesc::setToDefault()
	{
	surfaceRadius					= 0.02f;
	singleRadius					= 0.001f;
	blend							= 0.01f;
	triangleSize					= 0.01f;
	smoothingIterations				= 4;

	meshData						.setToDefault();

	userData						= NULL;
	name							= NULL;
	}

NX_INLINE bool NxImplicitMeshDesc::isValid() const
	{
	if (blend < 0.0f) return false;
	if (singleRadius <= 0.0f) return false;
	if (surfaceRadius < singleRadius) return false;
	if (triangleSize <= 0.0f) return false;
	if (triangleSize > surfaceRadius) return false;

	if (!meshData.isValid()) return false;
	
	return true;
	}

/** @} */
#endif


//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright © 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND

