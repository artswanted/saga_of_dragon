#ifndef NX_PHYSICS_NX_IMPLICITMESH
#define NX_PHYSICS_NX_IMPLICITMESH
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
#include "Nxp.h"
#include "NxPhysicsSDK.h"

class NxImplicitMeshDesc;
class NxParticleData;
class NxMeshData;

/**
\brief The implicit mesh class is used to generate a fluid surface mesh or a
implict mesh defined by a set of user points.

The mesh generation works as follows:

Each point or particle is sampled onto a uniform 3D grid using a radially-symmetric density 
function (kernel).  From the resulting data grid an implicit surface is extracted and a 
triangle surface mesh is generated to represent that surface. 
The user mesh buffers (indices and vertices) must be specified using the 
NxMeshData wrapper in order to generate a mesh.
If particles are specified using setParticles(), an implicit mesh
for these particles is generated.
If the Fluid data mesh is owned by an NxFluid instance, a mesh is 
generated only for the fluid, and the particles specified with setParticles()
are ignored.

\warning <b>Preliminary API, subject to change</b>
*/
class NxImplicitMesh
	{
	protected:
	NX_INLINE						NxImplicitMesh() : userData(NULL)	{}
	virtual							~NxImplicitMesh()	{}

	public:

	/**
	\brief Sets particles which are used for custom implicit mesh generation.
	
	This is done by passing an NxParticleData user buffer wrapper. The specified buffers are only 
	read until the function returns. Calling this method when the NxImplicitMesh 
	instance is owned by an NxFluid instance has no effect. 

	\warning <b>Preliminary API, subject to change</b>

	\param[in] particleData Structure descibing the particles to use for mesh generation.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxParticleData
	*/
	virtual		void 				setParticles(NxParticleData& particleData) const = 0;

	/**
	\brief Sets the user buffer wrapper for the implict mesh.

	\warning <b>Preliminary API, subject to change</b>

	\param[in] meshData User buffer wrapper.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxMeshData
	*/
	virtual		void 				setMeshData(NxMeshData& meshData) = 0;

	/**
	\brief Returns a copy of the user buffer wrapper for the implict mesh.

	\warning <b>Preliminary API, subject to change</b>

	\return User buffer wrapper.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxMeshData@see NxMeshData
	*/
	virtual		NxMeshData			getMeshData() = 0;

	/**
	\brief Returns the maximal distance of the mesh to the particles.

	\warning <b>Preliminary API, subject to change</b>

	\return Surface radius.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxImplicitMeshDesc
	*/
	virtual		NxReal				getSurfaceRadius() const = 0;

	/**
	\brief Sets the maximal distance of the mesh to the particles.

	\warning <b>Preliminary API, subject to change</b>

	\param[in] radius The new surface radius.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxImplicitMeshDesc
	*/
	virtual		void 				setSurfaceRadius(NxReal radius) = 0;

	/**
	\brief Sets the minimal distance of the mesh to the particles (distance of the mesh 
	to one separated particle).

	\warning <b>Preliminary API, subject to change</b>

	\return The minimal distance of the mesh to the particles.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxImplicitMeshDesc
	*/
	virtual		NxReal				getSingleRadius() const = 0;

	/**
	\brief Sets the minimal distance of the mesh to the particles (distance of the mesh 
	to one separated particle).

	\warning <b>Preliminary API, subject to change</b>

	\param[in] radius The minimal distance of the mesh to the particles.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxImplicitMeshDesc
	*/
	virtual		void 				setSingleRadius(NxReal radius) = 0;

	/**
	\brief Returns the the blend parameter of the implicit mesh.

	\warning <b>Preliminary API, subject to change</b>

	\return The blend parameter of the implicit mesh.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxImplicitMeshDesc
	*/
	virtual		NxReal				getBlend() const = 0;

	/**
	\brief Sets the the blend parameter of the implicit mesh.

	\warning <b>Preliminary API, subject to change</b>

	\param[in] blend The blend parameter of the implicit mesh.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxImplicitMeshDesc
	*/
	virtual		void 				setBlend(NxReal blend) = 0;

	/**
	\brief Returns the spatial resolution of the mesh.

	\warning <b>Preliminary API, subject to change</b>

	\return The spatial resolution of the mesh.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxImplicitMeshDesc
	*/
	virtual		NxReal				getTriangleSize() const = 0;

	/**
	\brief Sets the spatial resolution of the mesh.

	\warning <b>Preliminary API, subject to change</b>

	\param[in] size The spatial resolution of the mesh.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxImplicitMeshDesc
	*/
	virtual		void 				setTriangleSize(NxReal size) = 0;

	/**
	\brief Returns the number of smoothing iterations performed.

	\warning <b>Preliminary API, subject to change</b>

	\return The number of smoothing iterations.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxImplicitMeshDesc
	*/
	virtual		NxU32				getSmoothingIterations() const = 0;

	/**
	\brief Sets the number of smoothing iterations performed.

	\warning <b>Preliminary API, subject to change</b>

	\param[in] iters The number of smoothing iterations.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No

	@see NxImplicitMeshDesc
	*/
	virtual		void 				setSmoothingIterations(NxU32 iters) = 0;

	/**
	\brief Sets a name string for the object that can be retrieved with getName().  
	
	This is for debugging and is not used by the SDK.  The string is not copied by the SDK; 
	only the pointer is stored. 

	\warning <b>Preliminary API, subject to change</b>

	\param[in] name The new name.

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No
	*/
	virtual	void			setName(const char* name)		= 0;

	/**
	\brief Retrieves the name string set with setName().

	\warning <b>Preliminary API, subject to change</b>

	<b>Platform:</b>
	\li PC SW: No
	\li PPU  : No
	\li PS3  : No
	\li XB360: No
	*/
	virtual	const char*		getName()			const	= 0;

	//public variables:
	void*					userData;	//!< user can assign this to whatever, usually to create a 1:1 relationship with a user object.
	};
/** @} */
#endif


//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright © 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND

