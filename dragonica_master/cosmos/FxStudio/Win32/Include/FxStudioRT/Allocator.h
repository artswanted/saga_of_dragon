/*****************************************************************

    MODULE    : Allocator.h

    PURPOSE   : Inherit from the Allocator and register it

    CREATED   : 5/2/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/
#ifndef FXSTUDIO_ALLOCATOR_H_
#define FXSTUDIO_ALLOCATOR_H_

#include "FxStudioDefines.h"

#include <utility>

namespace FxStudio
{
	//	Class: Allocator
	//
	//		Inherit from this class in order to override the default allocator.
	//		The default allocator uses malloc/free to allocate the requested bytes.
	//		The registered allocator is always used in the same thread as the Runtime,
	//		so it does not need to be thread-safe.
	//
	class FXSTUDIO_API Allocator
	{
	public :

		//	Enumerations: MemoryCategory
		//
		//		Each allocation will be classified by one of these categories.
		//
		//	ManagerMemory - Memory overhead for the Manager class.
		//	PreviewMemory - Memory overhead for the Preview system.
		//	BankMemory - Memory overhead for wrapping the byte-stream of an Fx bank.
		//
		//	FxMemory - Memory overhead for the internal Fx class.
		//	FxComponentMemory - Memory overhead for the <ComponentData>
		//						struct created per component.
		//	FxInputMemory - Memory overhead for each input used by the Fx.
		//	FxInputDataMememory - Memory used to store the input data used by the Fx.
		//
		//	ComponentMemory - This category is never used by the internal Runtime
		//					  library.  It exists so that memory used in the 
		//					  ComponentFactory and in the derived Component classes.
		//
		enum MemoryCategory
		{
			ManagerMemory = 0,
			PreviewMemory,
			BankMemory,

			FxMemory,
			FxComponentMemory,
			FxInputMemory,
			FxInputDataMemory,

			ComponentMemory,

			MaxMemoryCategories
		};

		//	Enumerations: MemoryCategorySize
		//
		//		These are the sizes of the fixed-memory allocations.
		//		A size of zero indicates a non-fixed size allocation will occur.
		//		Some of the sizes are an array element size.  If it is an array
		//		element size, the number of bytes allocated with that category will
		//		always be a multiple of this size.
		//
		//	ManagerMemorySize - The fixed-size memory overhead for each Manager.
		//	PreviewMemorySize - The preview system allocates a variable amount of memory.
		//	BankMemorySize - The fixed-size memory overhead for each Fx bank.
		//
		//	FxMemorySize - The fixed-size overhead for each Fx created.
		//
		//	FxComponentMemorySize - The array element size for the component data of each
		//							created Fx.
		//	FxInputMemorySize - The array element size for each input used by the Fx.
		//
		//	FxInputDataMemorySize - The input data will be a variable amount of memory.
		//
		//	ComponentMemorySize - As explained above, the Component category is not used
		//						  internally.
		//
		enum MemoryCategorySize
		{
			ManagerMemorySize = 8,
			PreviewMemorySize = 0,
			BankMemorySize = 24,

			FxMemorySize = 108,

			// This is allocated as an array of elements.
			FxComponentMemorySize = 8,

			// This is allocated as an array of elements.
			FxInputMemorySize = 8,

			FxInputDataMemorySize = 0,

			ComponentMemorySize = 0
		};

		//	Function: GetFixedSize
		//
		//		Gets the value of the <MemoryCategorySize> which corresponds 
		//		to the <MemoryCategory>.
		//
		//	Parameters:
		//		eCategory - The memory category being requested.
		//
		//	Returns:
		//		The size in bytes for that memory category.
		//
		//	Remarks:
		//		See <MemoryCategorySize> above for an explaination of the size value
		//		returned. Use <IsArrayElementSize> to determine if the size is an
		//		array element size or the full size.
		//
		static std::size_t GetFixedSize(MemoryCategory eCategory);

		//	Function: IsArrayElementSize
		//
		//		Determines if the <MemoryCategory> is an array block allocation.
		//
		//	Parameters:
		//		eCategory - The memory category being requested.
		//
		//	Returns:
		//		True if the category is block allocated.
		//
		static bool IsArrayElementSize(MemoryCategory eCategory);

	public :

		//	Destructor: ~Allocator
		//
		virtual ~Allocator() {}

		//	Function: AllocateBytes
		//
		//		Called by the internal Runtime to allocate any memory.
		//
		//	Parameters:
		//		nNumBytes - The number of bytes needed.  This value will never be zero when
		//					called from within the Runtime.
		//		eCategory - The category for the allocation.
		//
		//	Returns:
		//		Should return a pointer to the allocated memory.
		//
		//	Remarks:
		//		If NULL is returned from this call, the attempted operation will fail and
		//		<ErrorHandler::HandleError> will be called.  The internal Runtime will
		//		not crash due to a NULL return value.
		//
		virtual void* AllocateBytes(std::size_t nNumBytes, MemoryCategory eCategory) = 0;

		//	Function: ReleaseBytes
		//
		//		Called to release memory previously allocated with <AllocateBytes>.
		//
		//	Parameters:
		//		pBytes - Pointer to the memory to be de-allocated. This value will never be
		//				 NULL when called from within the Runtime.
		//		eCategory - The category for the allocation.
		//
		virtual void  ReleaseBytes(void* pBytes, MemoryCategory eCategory) = 0;
	};

	//	Function: RegisterAllocator
	//
	//		Registers a <Allocator> derived class the memory allocator to be used by 
	//		the Runtime.
	//
	//	Parameters:
	//		allocator - The allocator to be used.
	//
	//	Remarks:
	//		Be careful not to register a new allocator when allocations are still
	//		outstanding in the old allocator!  It is best to call this function as early
	//		possible.  It cannot be called after the Manager has been created.
	//
	FXSTUDIO_API void RegisterAllocator(Allocator& allocator);

	//	Function: GetAllocator
	//
	//		Gets the currently registered allocator.
	//
	//	Returns:
	//		The currently registered allocator.
	//
	FXSTUDIO_API Allocator& GetAllocator();
}

FXSTUDIO_API void* operator new(std::size_t numBytes, FxStudio::Allocator& allocator, FxStudio::Allocator::MemoryCategory eCategory);
FXSTUDIO_API void  operator delete(void* pMemory, FxStudio::Allocator& allocator, FxStudio::Allocator::MemoryCategory eCategory);

// new[] and delete[] are provided for completeness, but they are not used within FxStudioRT.
FXSTUDIO_API void* operator new[](std::size_t numBytes, FxStudio::Allocator& allocator, FxStudio::Allocator::MemoryCategory eCategory);
FXSTUDIO_API void  operator delete[](void* pMemory, FxStudio::Allocator& allocator, FxStudio::Allocator::MemoryCategory eCategory);



inline std::size_t FxStudio::Allocator::GetFixedSize(FxStudio::Allocator::MemoryCategory eCategory)
{
	switch(eCategory)
	{
	case ManagerMemory :
		return ManagerMemorySize;

	case PreviewMemory :
		return PreviewMemorySize;

	case BankMemory :
		return BankMemorySize;

	case FxMemory :
		return FxMemorySize;

	case FxComponentMemory :
		return FxComponentMemorySize;

	case FxInputMemory :
		return FxInputMemorySize;

	case FxInputDataMemory :
		return FxInputDataMemorySize;

	case ComponentMemory :
		return ComponentMemorySize;

	default :
		return 0;
	};
}

inline bool FxStudio::Allocator::IsArrayElementSize(FxStudio::Allocator::MemoryCategory eCategory)
{
	switch(eCategory)
	{
	case FxComponentMemory :
	case FxInputMemory :
		return true;

	default :
		return false;
	};
}


#endif // FXSTUDIO_ALLOCATOR_H_