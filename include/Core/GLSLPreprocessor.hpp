#ifndef CM3D_GLSL_PRE_PREPROCESSOR_HPP
#define CM3D_GLSL_PRE_PREPROCESSOR_HPP

/* ===============================================================
 * A preprocessor for GLSL, currently targeting these features:
 * 1. Adding definitions without modifying shader source;
 * 2. Allowing to include GLSL files in shaders like in C/C++
 *    without GLSL extensions.
 *
 * Warning: #if #ifdef #ifndef are not processed at all (@todo?). Files with such guards may cause
 * cyclic inclusion and memory overrun. Use #pragma once instead
 *
 * @todo: #pragma cm3d_include
 * @todo: #pragma once
**/

#ifndef CM3D_GLSL_INCLUDE_PRAGMA
#  define CM3D_GLSL_INCLUDE_PRAGMA "cm3d_include"
#endif

#include <Types/BasicString.hpp>

#include <cstring>
#include <utility>
#include <unordered_map>
#include <fstream>

namespace cm3d
{
	class GLSLPreprocessor
	{
	public:
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

		GLSLPreprocessor() = default;

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
			while (*pos && (*pos != ' ' && *pos != '\t' && *pos != '\r' && *pos == '\n'))
				++pos;
			return pos;
		}
		// Go to next line (right after line continuation symbols)
		inline const char *nextLine(const char *pos)
		{
			// Single line termination in GLSL can consist of
			// carriage-return, line-feed or both together.

			while (true) {
				char ch = *pos++;
				if (!ch)
					return NULL;
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
		// Failed: *end isn't changed, NULL returned.
		// @todo test
		inline const char *nextTokenInDir(const char *pos, const char **end)
		{
			const char *tokenBeg = skipSpTabs(pos);
			if (!*tokenBeg)
				return NULL;

			const char *tokenEnd = findSpTabsCrLf(tokenBeg);
			if (tokenEnd == tokenBeg)
				return NULL;
			
			*end = tokenEnd;
			return tokenBeg;
		}

		inline int processDirective(const char *beg, const Directive type)
		{
			if (type == Directive::Pragma)
			{
				const char *wbeg;
				const char *wend;

				if (!(wbeg = nextTokenInDir(beg, &wend)))
					return 1;

				if (!strncmp(wbeg, CM3D_GLSL_INCLUDE_PRAGMA, wend - wbeg))
				{
					const char *filenameBeg;
					const char *filenameEnd;

					if (!(filenameBeg = nextTokenInDir(beg, &filenameEnd)))
						return 1;
					
					// just copypaste the file content in buffer.
				}
			}
		}

		inline int processSource(const String src)
		{	
			nlPos = src.begin();
			currentLine = 0;

			while (true)
			{
				// Update text position
				srcPos = nlPos;
				if (!srcPos || *srcPos == 0)
					break;
				nlPos = nextLine(srcPos);
				++currentLine;
				
				const char *firstSym = skipSpTabs(srcPos);

				if (*firstSym == '#')
				{
					const char *dirBegin = skipSpTabs(firstSym);
					if (*dirBegin == '\r' || *dirBegin == '\n')
						continue;
					
					Directive dir = Directive::None;

					if (!strncmp(dirBegin, "pragma", 6))
						dir = Directive::Pragma, dirBegin += 6;
					else return 1;

					int res = processDirective(dirBegin, dir);
				}
			}
		}
	protected:
		std::unordered_map<String, String> definitions;
		const char *srcPos;
		const char *nlPos;
		size_t currentLine;
		Directive waitFor;
	};
}

#endif
