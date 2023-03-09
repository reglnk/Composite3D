#ifndef CM3D_GLSL_PREPROCESSOR_HPP
#define CM3D_GLSL_PREPROCESSOR_HPP

/* ===============================================================
 * A preprocessor for GLSL, currently targeting these features:
 * 1. Adding definitions without modifying shader files (@todo);
 * 2. Allowing to include GLSL files in shaders like in C/C++
 *    without GLSL extensions.
 *
 * @todo: #pragma once
**/

#ifndef CM3D_GLSL_INCLUDE_PRAGMA
#  define CM3D_GLSL_INCLUDE_PRAGMA "cm3d_include"
#endif

#include <Types/DynArray.hpp>
#include <Core/FileSystem.hpp>

#include <cstring>
#include <utility>
#include <unordered_map>
#include <fstream>

namespace cm3d
{
	namespace GLSLPreprocessor
	{
		typedef const char * (*pNavFunc)(const char *);

		enum class Directive
		{
			None = 0,
			// Empty, // #
			// Define,
			// Undef,
			// If,
			// Ifdef,
			// Ifndef,
			// Else,
			// Elif,
			// Endif,
			// Error,
			Pragma,
			// Extension,
			// Version,
			// Line
		};

		namespace Status
		{
			constexpr int Success = 0x00;
			// Indicates that a directive is already processed
			constexpr int DirectivePass = 0x01;
			// Indicates that a directive should be copied to output code
			constexpr int DirectiveSkip = 0x02;

			constexpr int ErrorMask = 0x20;
			constexpr int ParseError = 0x20;
			constexpr int FSError = 0x21;
			constexpr int RecursionDepthExceeded = 0x22;
		};

		struct parseData {
			const char *srcPos;
			// Pointer to the beginning of next line
			const char *nlPos;
			size_t currentLine = 0;
		};

		inline bool isWhitespace(const char Ch)
		{
			switch (Ch)
			{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
			case '\v':
			case '\f':
				return true;
			default:
				return false;
			}
		}

		// Skip spaces and tabs
		inline const char *skipSpTabs(const char *pos)
		{
			while (*pos && (*pos == ' ' || *pos == '\t'))
				++pos;
			return pos;
		}
		// Find such symbols that can follow tokens in directives
		inline const char *findSpTabsCrLf(const char *pos)
		{
			while (*pos && (*pos != ' ' && *pos != '\t' && *pos != '\r' && *pos != '\n'))
				++pos;
			return pos;
		}
		inline const char *findCrLf(const char *pos)
		{
			while (*pos && (*pos != '\r' && *pos != '\n'))
				++pos;
			return pos;
		}
		// Go to next line (right after line continuation symbols)
		inline const char *nextLine(const char *pos)
		{
			// Single line termination in GLSL can consist of
			// carriage-return, line-feed or both together.

			while (true) {
				char ch = *pos;
				// treat '\0' as next line symbol to simplify parsing
				if (!ch)
					return pos;
				++pos;
				if (ch == '\r') {
					if (*pos == '\n')
						++pos;
					return pos;
				}
				if (ch == '\n') {
					if (*pos == '\r')
						++pos;
					return pos;
				}
			}
		}

		// Get next token within directive line.
		// Success case: begin iterator returned, *end is valid and different
		// Failed: *end isn't changed, nullptr returned.
		inline const char *nextTokenInDir(const char *pos, const char **end)
		{
			const char *tokenBeg = skipSpTabs(pos);
			if (!*tokenBeg)
				return nullptr;

			const char *tokenEnd = findSpTabsCrLf(tokenBeg);
			if (tokenEnd == tokenBeg)
				return nullptr;
			
			*end = tokenEnd;
			return tokenBeg;
		}
		
		inline const char *findComment(const char *srcbeg, const char *srcend, const char *from) {
			for (auto iter = from; iter != srcend; ++iter)
				if (*iter == '/' && (iter == srcbeg || isWhitespace(iter[-1])))
					if (iter[1] == '/' || iter[1] == '*')
						return iter;
			return srcend;
		}

		// Doesn't set '\0' at the end of output
		int deleteComments(const char *srcbeg, const char *srcend, DynArray<char> *output);

		int processDirective(
			const char *beg, // the pointer to a space/tab after #directive
			const Directive type,
			DynArray<char> *dest, // output
			const FileSystem::sPath &srcPath = ".", // the name of CWD of file containing the code currently being processed
			const size_t recursionDepth = 0
			// parseData *, // for future use
		);

		int processSourceBlock(
			const char *src, // current source code position
			DynArray<char> *dest, // the preprocessed text is appended here
			const FileSystem::sPath &srcPath = ".",
			const size_t recursionDepth = 0
		);

		inline int processSource(
			const char *srcbeg,
			const char *srcend,
			DynArray<char> *dest, // the preprocessed text is appended here
			const FileSystem::sPath srcPath = ".",
			const size_t recursionDepth = 0
		) {
			DynArray<char> source;
			if (deleteComments(srcbeg, srcend, &source))
				return Status::ParseError;
			source.resize(source.length() + 1);
			
			return processSourceBlock(source.begin(), dest, srcPath, recursionDepth);
		}
	};
}

#endif
