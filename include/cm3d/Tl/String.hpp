#ifndef CM3D_BASIC_STRING_HPP
#define CM3D_BASIC_STRING_HPP

/* ====================================================================
 * A lightweight c-string wrapper, intended to be flexible as char*
 * (unlike string from STL) and useful in low-level operations
**/

#include <cm3d/Types/DynArrayIterators.hpp>

#include <cm3d/Core/Malloc.hpp>

#include <cm3d/Utility/Std.hpp>
#include <cm3d/Utility/SCstring.hpp>
#include <cm3d/Utility/Debug.hpp>
#include <cm3d/Utility/Hash.hpp>

#include <utility>

// @todo avoid UB in new standard

namespace cm3d
{
	class String
	{
	protected:
		char *buf;
		size_t len;
		
	public:
		typedef dynArrForwardIterator<char> Iterator;
		typedef dynArrReverseIterator<char> rIterator;
		typedef dynArrForwardIterator<const char> cIterator;
		typedef dynArrReverseIterator<const char> crIterator;

		using valueType = char;
		
		inline String() noexcept: buf(nullptr), len(0) {}
		inline String(const String &s): buf(nullptr), len(s.len)
		{
			if (len) {
				buf = (char *)mAlloc(len + 1);
				strcpy(buf, s.buf);
			}
		}
		inline String(String &&s): buf(s.buf), len(s.len) {
			s.buf = nullptr;
		}
		inline String(const char *s)
		{
			len = strlen(s);
			buf = (char *)mAlloc(len + 1);
			strcpy(buf, s);
		}
		inline String(char *s):
			String((const char *)s) {}

		inline String(const char *s, const size_t slen): buf(nullptr), len(slen)
		{
			buf = (char *)mAlloc(len + 1);
			memcpy(buf, s, slen);
			buf[len] = '\0';
		}
		inline String(const size_t slen): len(slen) {
			buf = (char *)mAlloc(len + 1);
		}
		inline String(char *s, const size_t slen):
			String((const char *)s, slen) {}

		inline ~String() noexcept {
			if (buf)
				free(buf);
		}
/* ==================================================================== */

		inline String &operator =(const char *s)
		{
			len = strlen(s);
			buf = (char *)mRealloc(buf, len + 1);
			strcpy(buf, s);
			return *this;
		}
		inline String &operator =(const String &s)
		{
			len = s.len;
			if (buf)
				free(buf);
			buf = nullptr;
			if (len) {
				buf = (char *)mAlloc(len + 1);
				strcpy(buf, s.buf);
			}
			return *this;
		}
		inline String &operator =(String &&s) noexcept
		{
			len = s.len;
			buf = s.buf;
			s.buf = nullptr;
			return *this;
		}
		inline operator bool() const noexcept {
			return buf != nullptr;
		}
/* ==================================================================== */

		CM3D_CXX14_CONSTEXPR_INLINE const size_t &length() const {
			return len;
		}
		CM3D_CXX14_CONSTEXPR_INLINE size_t &length() {
			return len;
		}

		CM3D_CXX14_CONSTEXPR_INLINE size_t size() const {
			return len;
		}
		CM3D_CXX14_CONSTEXPR_INLINE void clear() {
			len = 0;
			if (buf) {
				free(buf);
				buf = nullptr;
			}
		}
		CM3D_CXX14_CONSTEXPR_INLINE void finalize() {
			len = 0;
			if (buf) {
				free(buf);
				buf = nullptr;
			}
		}
		CM3D_CXX14_CONSTEXPR_INLINE const char *c_str() const {
			return buf;
		}
		CM3D_CXX14_CONSTEXPR_INLINE const char *data() const {
			return buf;
		}
		CM3D_CXX14_CONSTEXPR_INLINE char *data() {
			return buf;
		}

		CM3D_CXX14_CONSTEXPR_INLINE const char &front() const {
			return *buf;
		}
		CM3D_CXX14_CONSTEXPR_INLINE const char &back() const {
			return buf[len - 1];
		}
		CM3D_CXX14_CONSTEXPR_INLINE char &front() {
			return *buf;
		}
		CM3D_CXX14_CONSTEXPR_INLINE char &back() {
			return buf[len - 1];
		}

		CM3D_CXX14_CONSTEXPR_INLINE char &operator [](const size_t pos) {
			return buf[pos];
		}
		CM3D_CXX14_CONSTEXPR_INLINE const char &operator [](const size_t pos) const {
			return buf[pos];
		}

		CM3D_CXX14_CONSTEXPR_INLINE Iterator begin() { return Iterator(buf); }
		CM3D_CXX14_CONSTEXPR_INLINE Iterator end() { return Iterator(buf + len); }
		CM3D_CXX14_CONSTEXPR_INLINE cIterator begin() const { return cIterator(buf); }
		CM3D_CXX14_CONSTEXPR_INLINE cIterator end() const { return cIterator(buf + len); }

		CM3D_CXX14_CONSTEXPR_INLINE cIterator cBegin() const { return cIterator(buf); }
		CM3D_CXX14_CONSTEXPR_INLINE cIterator cEnd() const { return cIterator(buf + len); }

		CM3D_CXX14_CONSTEXPR_INLINE rIterator rBegin() { return rIterator(buf + len - 1); }
		CM3D_CXX14_CONSTEXPR_INLINE rIterator rEnd() { return rIterator(buf - 1); }
		CM3D_CXX14_CONSTEXPR_INLINE crIterator crBegin() const { return crIterator(buf + len - 1); }
		CM3D_CXX14_CONSTEXPR_INLINE crIterator crEnd() const { return crIterator(buf - 1); }

		inline char *find(const char Ch) const noexcept {
			return strchr(buf, Ch);
		}
		inline const char *find(const char *s) const noexcept {
			return strstr(buf, s);
		}
		inline const char *find(const String &s) const noexcept {
			return strstr(buf, s.c_str());
		}
		inline void resize(const size_t newSize) {
			buf = (char *)mRealloc(buf, newSize + 1);
			len = newSize;
		}
/* =============================================================== */

		inline bool operator ==(const char *s) const noexcept {
			return !strncmp(buf, s, len) && !s[len];
		}
		inline bool operator ==(char *s) const noexcept {
			return this->operator ==((const char *)s);
		}
		inline bool operator !=(const char *s) const noexcept {
			return strncmp(buf, s, len) || s[len];
		}
		inline bool operator !=(char *s) const noexcept {
			return this->operator !=((const char *)s);
		}
		inline bool operator ==(const String &s) const noexcept {
			return len == s.size() && !strncmp(buf, s.c_str(), len);
		}
		inline bool operator !=(const String &s) const noexcept {
			return len != s.size() || strncmp(buf, s.c_str(), len);
		}
		
#	ifndef CM3D_BASIC_STRING_DISABLE_STL_COMPAT
		// Only intended to make hash
		inline bool operator >(const String &s) const;

		// Only intended to make hash
		inline bool operator >=(const String &s) const;

		// Only intended to make hash
		inline bool operator <(const String &s) const;

		// Only intended to make hash
		inline bool operator <=(const String &s) const;
#	endif

		inline String &operator +=(const char *s)
		{
			const size_t slen = strlen(s);
			buf = (char *)mRealloc(buf, len + slen + 1);
			memcpy(buf + len, s, slen);
			len += slen;
			buf[len] = '\0';
			return *this;
		}
		inline String &operator +=(const String &s)
		{
			const size_t slen = s.size();
			buf = (char *)mRealloc(buf, len + slen + 1);
			memcpy(buf + len, s.c_str(), slen);
			len += slen;
			buf[len] = '\0';
			return *this;
		}
	
		inline String concat(
			const char *s, const size_t slen
		) const noexcept {
			String ns;
			ns.len = len + slen;
			ns.buf = (char *)mAlloc(ns.len + 1);

			memcpy(ns.buf, buf, len);
			memcpy(ns.buf + len, s, slen + 1);

			return ns;
		}
		inline String operator +(const char *s) const noexcept {
			return concat(s, strlen(s));
		}
		inline String operator +(char *s) const noexcept {
			return concat(s, strlen(s));
		}
		inline String operator +(const String &s) const noexcept {
			return concat(s.c_str(), s.size());
		}

		friend inline bool operator ==(const char *cs, const String &s) noexcept {
			return s == cs;
		}
		friend inline bool operator !=(const char *cs, const String &s) noexcept {
			return s != cs;
		}
	};
	namespace Hash
	{
		inline uint32_t u32(const String &s) noexcept
		{
			return cm3d::Hash::u32(s.c_str(), s.size());
		}
	}
}

#ifndef CM3D_BASIC_STRING_DISABLE_STL_COMPAT

#include <cstdint>
#include <cstddef>
#include <functional>

namespace std
{
	template<> struct hash<cm3d::String>
	{
		size_t operator ()(const cm3d::String &e) const
		{
			uint32_t h = cm3d::Hash::u32(e);
			
// @todo produce really 8-byte hash
#		if __SIZEOF_SIZE_T__ == 8
			h ^= static_cast<size_t>(cm3d::Hash::u32(h)) << 32;
#		endif
			return h;
		}
	};
}

namespace cm3d
{
	// Only intended to make hash
	inline bool String::operator >(const String &s) const {
		return std::hash<String>()(*this) > std::hash<String>()(s);
	}
	// Only intended to make hash
	inline bool String::operator >=(const String &s) const {
		return  std::hash<String>()(*this) >= std::hash<String>()(s);
	}
	// Only intended to make hash
	inline bool String::operator <(const String &s) const {
		return std::hash<String>()(*this) < std::hash<String>()(s);
	}
	// Only intended to make hash
	inline bool String::operator <=(const String &s) const {
		return  std::hash<String>()(*this) <= std::hash<String>()(s);
	}
}

#endif // CM3D_BASIC_STRING_DISABLE_STL_COMPAT

#endif // CM3D_BASIC_STRING_HPP
