//
// Basic numeric types commonly used by OpenViBE kernel and modules
//

// #include <stdint.h>		// For C99
#include <cstdint>
// The following construction allows defining the same variables under different namespaces.
// OV_APPEND_TO_NAMESPACE is a preprocessor define specified outside this file that names the desired namespace.
// Caveat: include guards can not be used on this file as the same project may include it specifying
// different namespaces.
namespace
OV_APPEND_TO_NAMESPACE
{

	//___________________________________________________________________//
	//                                                                   //
	// Boolean basic type                                                //
	//___________________________________________________________________//
	//                                                                   //

	//typedef bool boolean;                   ///< Boolean

	//___________________________________________________________________//
	//                                                                   //
	// Integer basic types                                               //
	//___________________________________________________________________//
	//                                                                   //

	//typedef std::uint64_t uint64;          ///< 64 bits unsigned integer
	//typedef std::uint32_t uint32;          ///< 32 bits unsigned integer
	//typedef std::uint16_t uint16;          ///< 16 bits unsigned integer
	//typedef std::uint8_t uint8;            ///< 8 bits unsigned integer

	//typedef std::int64_t int64;            ///< 64 bits signed integer
	//typedef std::int32_t int32;            ///< 32 bits signed integer
	//typedef std::int16_t int16;            ///< 16 bits signed integer
	//typedef std::int8_t int8;              ///< 8 bits signed integer

	//___________________________________________________________________//
	//                                                                   //
	// Addressing and indexing types                                      //
	//___________________________________________________________________//
	//                                                                   //

	typedef void* pointer;                  ///< untyped pointer
	typedef unsigned long int pointer_size; ///< addressing range

	//___________________________________________________________________//
	//                                                                   //
	// Floating point basic types                                        //
	//___________________________________________________________________//
	//                                                                   //

	//typedef float float;                  ///< 32 bits floating point
	//typedef double double;                 ///< 64 bits floating point
	//typedef long double float80;            ///< 80 bits floating point

	//___________________________________________________________________//
	//                                                                   //
	// Time basic type                                                   //
	//___________________________________________________________________//
	//                                                                   //

	typedef struct _time64
	{
		_time64(uint64_t ui64TimeValue) : m_ui64TimeValue(ui64TimeValue) {}
		uint64_t m_ui64TimeValue = 0;
	} time64;                                ///< Time on uint64
}  // namespace OpenViBETypeTest
