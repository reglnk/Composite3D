#ifndef CM3D_BASIC_STRING_HPP
#define CM3D_BASIC_STRING_HPP

/* ====================================================================
 * A lightweight c-string wrapper, intended to be flexible as char*
 * (unlike string from STL) and useful in low-level operations
**/

#include <cm3d/Tl/DynArray.hpp>

#include <cm3d/Core/Malloc.hpp>

#include <cm3d/Utility/Std.hpp>
#include <cm3d/Utility/SCstring.hpp>
#include <cm3d/Utility/Debug.hpp>
#include <cm3d/Utility/Hash.hpp>

#include <utility>

namespace cm3d
{
	class String: public DynArray<char>
	{
	public:
		inline String() = default;
		
		inline String(const size_t slen): DynArray<char>(slen + 1) {
			this->arr[slen] = '\0';
		}
		
		inline String(const char *s) {
			size_t slen = std::strlen(s);
			resize(slen);
			std::memcpy(this->arr, s, slen);
		}
		inline String(char *s):
			String((const char *)s) {}

		inline String(const char *s, const size_t slen): String(slen)
		{
			std::memcpy(this->arr, s, slen);
			this->arr[slen] = '\0';
		}
		inline String(char *s, const size_t slen):
			String((const char *)s, slen) {}
		
		constexpr inline bool ready() const noexcept {
			return this->len;
		}

		constexpr inline size_t length() const {
			return ready() ? this->len - 1 : 0;
		}
		
		constexpr inline size_t size() const = delete;
		
		constexpr inline const char *c_str() const {
			return ready() ? this->data() : "";
		}
		
		constexpr inline operator bool() const noexcept {
			return length();
		}
		
		inline void resize(const size_t newlen) {
			DynArray<char>::resize(newlen + 1);
			this->data()[newlen] = '\0';
		}
		
		inline void reserve(const size_t cap) {
			DynArray<char>::reserve(cap + 1);
		}
		
		constexpr inline Iterator end() { return Iterator(arr + len - 1); }
		constexpr inline cIterator end() const { return cIterator(arr + len - 1); }
		constexpr inline cIterator cEnd() const { return end(); }

		constexpr inline rIterator rBegin() { return rIterator(arr + len - 2); }
		constexpr inline crIterator crBegin() const { return crIterator(arr + len - 2); }
		
		constexpr inline char &back() = delete;
		constexpr inline char &front() = delete;
		
		constexpr inline char back() const {
			if (!ready())
				return '\0';
			return this->arr[length() - 1];
		}
		constexpr inline char front() const {
			if (!ready())
				return '\0';
			return this->arr[0];
		}

		inline char *find(const char Ch) noexcept {
			return std::strchr(this->data(), Ch);
		}
		inline const char *find(const char Ch) const noexcept {
			return std::strchr(this->data(), Ch);
		}
		
		inline const char *find(const char *s) const noexcept {
			return std::strstr(this->data(), s);
		}
		inline const char *find(const String &s) const noexcept {
			return std::strstr(this->data(), s.c_str());
		}
		
/* =============================================================== */

		inline bool operator ==(const char *s) const noexcept {
			return !std::strncmp(c_str(), s, length()) && !s[length()];
		}
		inline bool operator ==(char *s) const noexcept {
			return this->operator ==((const char *)s);
		}
		inline bool operator !=(const char *s) const noexcept {
			return std::strncmp(c_str(), s, length()) || s[length()];
		}
		inline bool operator !=(char *s) const noexcept {
			return this->operator !=((const char *)s);
		}
		inline bool operator ==(const String &s) const noexcept {
			return length() == s.length() && !std::strncmp(c_str(), s.c_str(), length());
		}
		inline bool operator !=(const String &s) const noexcept {
			return length() != s.length() || std::strncmp(c_str(), s.c_str(), length());
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
			const size_t prevlen = length();
			resize(prevlen + slen);
			std::memcpy(data() + prevlen, s, slen);
			return *this;
		}
		inline String &operator +=(const String &s)
		{
			const size_t slen = s.length();
			const size_t prevlen = length();
			resize(prevlen + slen);
			std::memcpy(data() + prevlen, s.c_str(), slen);
			return *this;
		}
	
		inline String concat(const char *s, const size_t slen) const
		{
			String ns(length() + slen);
			std::memcpy(ns.data(), data(), length());
			std::memcpy(ns.data() + length(), s, slen);
			return ns;
		}
		inline String operator +(const char *s) const {
			return concat(s, strlen(s));
		}
		inline String operator +(char *s) const {
			return concat(s, strlen(s));
		}
		inline String operator +(const String &s) const {
			return concat(s.c_str(), s.length());
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
			return cm3d::Hash::u32(s.c_str(), s.length());
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
