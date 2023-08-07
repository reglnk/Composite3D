#include <cm3d/Types/String.hpp>
#include <cm3d/Core/GLSLPreprocessor.hpp>
#include <cm3d/Core/FileSystem.hpp>

#include <cstdio>
#include <cstring>
#include <utility>
#include <unordered_map>

#ifndef CM3D_GLSL_PREPROCESSOR_MAX_INCLUDE_DEPTH
#  define CM3D_GLSL_PREPROCESSOR_MAX_INCLUDE_DEPTH 20
#endif

namespace cm3d
{
	namespace GLSLPreprocessor
	{
		int deleteComments(const char *srcbeg, const char *srcend, DynArray<char> *output)
		{
			for (const char *findCh = srcbeg; findCh != srcend; )
			{
				auto cmt = findComment(srcbeg, srcend, findCh);

				size_t curSize = output->length();
				output->resize(curSize + (cmt - findCh));
				memcpy(output->begin() + curSize, findCh, 1 + cmt - findCh);

				if (cmt == srcend)
					break;
				
				if (cmt[1] == '/')
					findCh = findCrLf(cmt);
				else {
					auto cmtClose = strstr(cmt + 2, "*/");
					if (!cmtClose)
						return 1;
					findCh = cmtClose + 2;
				}
			}
			return 0;
		}

		int processDirective (
			const char *beg, // the pointer to a space/tab after #directive
			const Directive type,
			DynArray<char> *dest, // output
			const FileSystem::sPath &srcPath, // the name of CWD of file containing the code currently being processed
			const size_t recursionDepth
			// parseData *, // for future use
		) {
			if (type == Directive::Pragma)
			{
				const char *wbeg;
				const char *wend;

				if (!(wbeg = nextTokenInDir(beg, &wend)))
					return Status::ParseError;

				if (!strncmp(wbeg, CM3D_GLSL_INCLUDE_PRAGMA, wend - wbeg))
				{
					if (recursionDepth > CM3D_GLSL_PREPROCESSOR_MAX_INCLUDE_DEPTH)
						return Status::RecursionDepthExceeded;

					const char *filenameBeg;
					const char *filenameEnd;

					filenameBeg = skipSpTabs(wend);
					
					bool isGlobal;
					if (*filenameBeg == '<')
						isGlobal = true;
					else if (*filenameBeg == '"')
						isGlobal = false;
					else return Status::ParseError;
					++filenameBeg;

					filenameEnd = strchr(filenameBeg, isGlobal ? '>' : '"');
					if (!filenameEnd || filenameBeg == filenameEnd)
						return Status::ParseError;

					// #pragma cm3d_include <somefile.glsl>
					//                   beg_^        end_^
					if (filenameEnd - filenameBeg <= 0)
						return Status::ParseError;
					
					// Considering cm3d_include "somefile" (or <somefile>):
					// incase of angled brackets, a path relative to the process working directory will be picked
					// incase of quotes, make this path relative to the directory

					FileSystem::sPath fileName(filenameBeg, filenameEnd - filenameBeg);
					
					if (!isGlobal)
						fileName = FileSystem::concat(srcPath, fileName);
					if (!FileSystem::isNormalized(fileName))
						fileName = FileSystem::normalize(fileName);
					
					FileSystem::sPath innerPath(fileName);
					FileSystem::toNative(&fileName);

					// Then read the file, preprocess and append its content to destination
					FILE *fp;
					do {
						fp = fopen(fileName.c_str(), "rb");
						
						if (!fp) return Status::FSError;
						if (fseek(fp, 0, SEEK_END)) break;
						sSize fileSize = ftell(fp);
						if (fseek(fp, 0, SEEK_SET)) break;

						String fileBuf(fileSize);
						if (fread(fileBuf.data(), 1, fileSize, fp) != fileSize)
							break;
						fileBuf[fileSize] = '\0';
						fclose(fp);

						DynArray<char> outBuf;
						FileSystem::parentDir(&innerPath);
						processSource(fileBuf.begin(), fileBuf.end(), &outBuf, innerPath, recursionDepth + 1);

						const size_t prevSize = dest->length();
						dest->resize(prevSize + outBuf.length());
						memcpy(dest->begin() + prevSize, outBuf.begin(), outBuf.length());

						return Status::DirectivePass;
					} while (0);
					
					fclose(fp);
					return Status::FSError;
				}
				return Status::DirectiveSkip;
			}
			return Status::DirectiveSkip;
		}

		int processSourceBlock (
			const char *src, // current source code position
			DynArray<char> *dest, // the preprocessed text is appended here
			const FileSystem::sPath &srcPath,
			const size_t recursionDepth
		) {
			parseData dat;
			dat.nlPos = src;

			while (true)
			{
				// Update text position
				dat.srcPos = dat.nlPos;
				if (*dat.srcPos == 0)
					break;
				dat.nlPos = nextLine(dat.srcPos);
				++dat.currentLine;
				
				const char *firstSym = skipSpTabs(dat.srcPos);

				if (*firstSym == '#')
				{
					Directive dir = Directive::None;
					const char *dirBegin = skipSpTabs(firstSym + 1);
					
					if (*dirBegin == '\r' || *dirBegin == '\n')
						continue;

					if (!strncmp(dirBegin, "pragma", 6)) {
						dir = Directive::Pragma;
						dirBegin += 6;
					}

					int pdRes;
					if (dir != Directive::None)
					{
						pdRes = processDirective(dirBegin, dir, dest, /* &dat, */ srcPath);
						if (pdRes & Status::ErrorMask)
							return pdRes;
						if (pdRes == Status::DirectivePass)
							continue;
					}
				}
				size_t prevLen = dest->length();
				dest->resize(prevLen + (dat.nlPos - dat.srcPos));
				memcpy(dest->begin() + prevLen, dat.srcPos, dat.nlPos - dat.srcPos);
			}
			return Status::Success;
		}
	};
}
