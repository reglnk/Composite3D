#ifndef CM3D_IO_BINFSTREAM_HPP
#define CM3D_IO_BINFSTREAM_HPP

#include <cm3d/IO/binStream.hpp>

#include <cstdio>
#include <stdexcept>

// @todo: remove WET

namespace cm3d::io
{
	inline void __filestream_seekg(FILE *fp, int64_t off, Way way = Way::set) {
		int cway =
			way == Way::set ? SEEK_SET :
			way == Way::cur ? SEEK_CUR :
			SEEK_END;

		if (fseek(fp, off, cway))
			throw std::runtime_error("fseek error");
	}

	class FileStream: public binStream
	{
	protected:
		FILE *fp;

	public:
		FileStream(FILE *fp): fp(fp) {}
		FileStream(const char *filename, const char *modes = "rb+") {
			fp = fopen(filename, modes);
			if (fp == NULL)
				throw std::runtime_error("failed to open file");
		}

		inline void read(void *dst, size_t count) override {
			if (fread(dst, 1, count, fp) != count)
				throw std::runtime_error("fread error");
		}

		inline void write(void const *src, size_t count) override {
			if (fwrite(src, 1, count, fp) != count)
				throw std::runtime_error("fwrite error");
		}

		inline void seekg(int64_t off, Way way = Way::set) override {
			__filestream_seekg(fp, off, way);
		}

		inline int64_t tellg() override {
			return ftell(fp);
		}

		inline void seekp(int64_t off, Way way = Way::set) override {
			__filestream_seekg(fp, off, way);
		}

		inline int64_t tellp() override {
			return ftell(fp);
		}

		inline ~FileStream() override {
			fclose(fp);
		}
	};

	class FileStreamR: public binStreamR
	{
	protected:
		FILE *fp;

	public:
		FileStreamR(FILE *fp): fp(fp) {}
		FileStreamR(const char *filename, const char *modes = "rb") {
			fp = fopen(filename, modes);
			if (fp == NULL)
				throw std::runtime_error("failed to open file");
		}

		inline void read(void *dst, size_t count) override {
			if (fread(dst, 1, count, fp) != count)
				throw std::runtime_error("fread call failed");
		}

		inline void seekg(int64_t off, Way way = Way::set) override {
			__filestream_seekg(fp, off, way);
		}

		inline int64_t tellg() override {
			return ftell(fp);
		}

		inline ~FileStreamR() override {
			fclose(fp);
		}
	};

	class FileStreamW: public binStreamW
	{
	protected:
		FILE *fp;

	public:
		FileStreamW(FILE *fp): fp(fp) {}
		FileStreamW(const char *filename, const char *modes = "wb") {
			fp = fopen(filename, modes);
			if (fp == NULL)
				throw std::runtime_error("failed to open file");
		}

		inline void write(void const *src, size_t count) override {
			if (fwrite(src, 1, count, fp) != count)
				throw std::runtime_error("fwrite call failed");
		}

		inline void seekp(int64_t off, Way way = Way::set) override {
			__filestream_seekg(fp, off, way);
		}

		inline int64_t tellp() override {
			return ftell(fp);
		}

		inline ~FileStreamW() override {
			fclose(fp);
		}
	};
}

#endif
