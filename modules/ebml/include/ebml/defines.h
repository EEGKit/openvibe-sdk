/**
 * \file defines.h
 * \author Yann Renard (INRIA/IRISA)
 * \date 2006-08-07
 * \brief Contains basic type definitions and EBML standard identifiers
 */

#pragma once

#include <ov_common_defines.h>
#include <cstdlib>	// For Unix Compatibility
#include <cstdint>

//___________________________________________________________________//
//                                                                   //
// EBML Standard identifiers definitions                             //
//___________________________________________________________________//
//                                                                   //

/**
 * \name EBML standard identifiers definitions
 *
 * Be sure to look at http://ebml.sourceforge.net/specs/ to
 * have more information on EBML and its standard identifiers.
 */

//@{

/**
 * EBML Header.
 * This is a master node.
 * It contains several information about the ebml stream.
 */
#define EBML_Identifier_Header				EBML::CIdentifier(0x0A45DFA3)
/**
 * EBML Version.
 * This is a child node of type \c integer.
 * It contains the EBML version.
 */
#define EBML_Identifier_EBMLVersion			EBML::CIdentifier(0x0286)
/**
 * EBML Read Version.
 * This is a child node of type \c integer.
 * Indicates the EBML version a parser has to support to read this file
 */
#define EBML_Identifier_EBMLReadVersion		EBML::CIdentifier(0x02F7)
/**
 * EBML Identifier Length.
 * This is a child node of type \c integer.
 * Indicates the parser the maximum number of bytes an identifier
 * can have.
 */
#define EBML_Identifier_EBMLIdLength		EBML::CIdentifier(0x02F2)
/**
 * EBML Size Length.
 * This is a child node of type \c integer.
 * Indicates the paser the maximum number of bytes a node
 * can have.
 */
#define EBML_Identifier_EBMLSizeLength		EBML::CIdentifier(0x02F3)
/**
 * EBML Document Type.
 * This is a child node of type \c string.
 * Indicates the parser the kind of document being parsed.
 */
#define EBML_Identifier_DocType				EBML::CIdentifier(0x0282)
/**
 * EBML Document Type Version.
 * This is a child node of type \c integer.
 * Indicates the parser the specification version of the document
 * being parsed.
 */
#define EBML_Identifier_DocTypeVersion		EBML::CIdentifier(0x0287)
/**
 * EBML Document Type Read Version.
 * This is a child node of type \c integer.
 * Indicates the parser the version of the reader to use in order
 * to understand the document content.
 */
#define EBML_Identifier_DocTypeReadVersion	EBML::CIdentifier(0x0285)
/**
 * EBML Void.
 * This is a child node of type \c binary.
 * Indicates an ununderstandable chunk of data to skip.
 */
#define EBML_Identifier_Void				EBML::CIdentifier(0x6C)
/**
 * EBML CRC32.
 * This is a child node of type \c binary.
 * This is a CRC32 checksum of the content between this CRC32
 * and the last one.
 */
#define EBML_Identifier_CRC32				EBML::CIdentifier(0x02FE)

//@}

#if defined EBML_Shared
#	if defined TARGET_OS_Windows
#		define EBML_API_Export __declspec(dllexport)
#		define EBML_API_Import __declspec(dllimport)
#	elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
#		define EBML_API_Export __attribute__((visibility("default")))
#		define EBML_API_Import __attribute__((visibility("default")))
#	else
#		define EBML_API_Export
#		define EBML_API_Import
#	endif
#else
#	define EBML_API_Export
#	define EBML_API_Import
#endif

#if defined EBML_Exports
#	define EBML_API EBML_API_Export
#else
#	define EBML_API EBML_API_Import
#endif
