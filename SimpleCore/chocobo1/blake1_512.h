/*
 *  Chocobo1/Hash
 *
 *   Copyright 2017 by Mike Tzou (Chocobo1)
 *     https://github.com/Chocobo1/Hash
 *
 *   Licensed under GNU General Public License 3 or later.
 *
 *  @license GPL3 <https://www.gnu.org/licenses/gpl-3.0-standalone.html>
 */

#ifndef CHOCOBO1_BLAKE1_512_H
#define CHOCOBO1_BLAKE1_512_H

#include "gsl/span"

#include <array>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <type_traits>
#include <vector>


namespace Chocobo1
{
	// Use these!!
	// Blake1_512();
}


namespace Chocobo1
{
// users should ignore things in this namespace

namespace Hash
{
#ifndef CONSTEXPR_CPP17_CHOCOBO1_HASH
#if __cplusplus >= 201703L
#define CONSTEXPR_CPP17_CHOCOBO1_HASH constexpr
#else
#define CONSTEXPR_CPP17_CHOCOBO1_HASH
#endif
#endif

#ifndef CHOCOBO1_HASH_BUFFER_IMPL
#define CHOCOBO1_HASH_BUFFER_IMPL
	template <typename T, std::size_t N>
	class Buffer
	{
		public:
			using value_type = T;
			using size_t = std::size_t;
			using reference = T&;
			using iterator = T*;
			using const_iterator = const T*;

			constexpr Buffer() = default;
			constexpr explicit Buffer(const Buffer &) = default;

			constexpr Buffer(const std::initializer_list<T> initList)
			{
#if !defined(NDEBUG)
				// check if out-of-bounds
				m_array.at(m_dataEndIdx + initList.size() - 1);
#endif

				for (const auto &i : initList)
				{
					m_array[m_dataEndIdx] = i;
					++m_dataEndIdx;
				}
			}

			template <typename InputIt>
			constexpr Buffer(const InputIt first, const InputIt last)
			{
				for (InputIt iter = first; iter != last; ++iter)
				{
					this->fill(*iter);
				}
			}

			constexpr T& operator[](const size_t pos)
			{
				return m_array[pos];
			}

			constexpr T operator[](const size_t pos) const
			{
				return m_array[pos];
			}

			constexpr void fill(const T &value, const size_t count = 1)
			{
#if !defined(NDEBUG)
				// check if out-of-bounds
				m_array.at(m_dataEndIdx + count - 1);
#endif

				for (size_t i = 0; i < count; ++i)
				{
					m_array[m_dataEndIdx] = value;
					++m_dataEndIdx;
				}
			}

			template <typename InputIt>
			constexpr void push_back(const InputIt first, const InputIt last)
			{
				for (InputIt iter = first; iter != last; ++iter)
				{
					this->fill(*iter);
				}
			}

			constexpr void clear()
			{
				m_array = {};
				m_dataEndIdx = 0;
			}

			constexpr bool empty() const
			{
				return (m_dataEndIdx == 0);
			}

			constexpr size_t size() const
			{
				return m_dataEndIdx;
			}

			constexpr const T* data() const
			{
				return m_array.data();
			}

			constexpr iterator begin()
			{
				return m_array.data();
			}

			constexpr const_iterator begin() const
			{
				return m_array.data();
			}

			constexpr iterator end()
			{
				if (N == 0)
					return m_array.data();
				return &m_array[m_dataEndIdx];
			}

			constexpr const_iterator end() const
			{
				if (N == 0)
					return m_array.data();
				return &m_array[m_dataEndIdx];
			}

		private:
			std::array<T, N> m_array {};
			size_t m_dataEndIdx = 0;
	};
#endif

#ifndef CHOCOBO1_HASH_UINT128_IMPL
#define CHOCOBO1_HASH_UINT128_IMPL
	class Uint128
	{
		public:
			constexpr Uint128()
				: m_lo(0), m_hi(0)
			{
			}

			constexpr Uint128& operator= (const uint64_t n)
			{
				this->m_lo = n;
				this->m_hi = 0;
				return (*this);
			}

			constexpr Uint128 operator+ (const uint64_t n)
			{
				Uint128 ret = *this;
				ret += n;
				return ret;
			}

			constexpr Uint128& operator* (const unsigned int n)
			{
				// only handle `*8` case
				assert(n == 8);

				const uint8_t msb = (m_lo >> 61) & 0xff;
				m_hi = (m_hi << 3) | msb;
				m_lo = m_lo << 3;

				return (*this);
			}

			constexpr Uint128& operator+= (const uint64_t n)
			{
				const uint64_t newLo = (m_lo + n);
				if (newLo < m_lo)
					++m_hi;
				m_lo = newLo;

				return (*this);
			}

			constexpr uint64_t low() const
			{
				return m_lo;
			}

			constexpr uint64_t high() const
			{
				return m_hi;
			}

		private:
			uint64_t m_lo;
			uint64_t m_hi;
	};
#endif


namespace Blake1_512_NS
{
	class Blake1_512
	{
		// https://131002.net/blake/

		public:
			using Byte = uint8_t;
			using ResultArrayType = std::array<Byte, 64>;

			template <typename T>
			using Span = gsl::span<T>;


			constexpr Blake1_512();

			constexpr void reset();
			constexpr Blake1_512& finalize();  // after this, only `toArray()`, `toString()`, `toVector()`, `reset()` are available

			std::string toString() const;
			std::vector<Byte> toVector() const;
			CONSTEXPR_CPP17_CHOCOBO1_HASH ResultArrayType toArray() const;

			constexpr Blake1_512& addData(const Span<const Byte> inData);
			constexpr Blake1_512& addData(const void *ptr, const long int length);
			template <typename T, std::size_t N>
			Blake1_512& addData(const T (&array)[N]);
			template <typename T>
			Blake1_512& addData(const Span<T> inSpan);

		private:
			constexpr void addDataImpl(const Span<const Byte> data, const uint32_t paddingLen = 0);

			static constexpr unsigned int BLOCK_SIZE = 128;

			Buffer<Byte, (BLOCK_SIZE * 2)> m_buffer;  // x2 for paddings
			Uint128 m_sizeCounter;

			uint64_t m_h[8] = {};

			static constexpr uint64_t cTable[16] =
			{
				0x243f6a8885a308d3, 0x13198a2e03707344, 0xa4093822299f31d0, 0x082efa98ec4e6c89, 0x452821e638d01377, 0xbe5466cf34e90c6c, 0xc0ac29b7c97c50dd, 0x3f84d5b5b5470917,
				0x9216d5d98979fb1b, 0xd1310ba698dfb5ac, 0x2ffd72dbd01adfb7, 0xb8e1afed6a267e96, 0xba7c9045f12c7f99, 0x24a19947b3916cf7, 0x0801f2e2858efc16, 0x636920d871574e69
			};
	};

	constexpr uint64_t Blake1_512::cTable[16];


	// helpers
	template <typename T>
	class Loader
	{
		// this class workaround loading data from unaligned memory boundaries
		// also eliminate endianness issues
		public:
			explicit constexpr Loader(const void *ptr)
				: m_ptr(static_cast<const uint8_t *>(ptr))
			{
			}

			constexpr T operator[](const size_t idx) const
			{
				static_assert(std::is_same<T, uint64_t>::value, "");
				// handle specific endianness here
				const uint8_t *ptr = m_ptr + (sizeof(T) * idx);
				return  ( (static_cast<T>(*(ptr + 0)) << 56)
						| (static_cast<T>(*(ptr + 1)) << 48)
						| (static_cast<T>(*(ptr + 2)) << 40)
						| (static_cast<T>(*(ptr + 3)) << 32)
						| (static_cast<T>(*(ptr + 4)) << 24)
						| (static_cast<T>(*(ptr + 5)) << 16)
						| (static_cast<T>(*(ptr + 6)) <<  8)
						| (static_cast<T>(*(ptr + 7)) <<  0));
			}

		private:
			const uint8_t *m_ptr;
	};

	template <typename R, typename T>
	constexpr R ror(const T x, const unsigned int s)
	{
		static_assert(std::is_unsigned<R>::value, "");
		const R mask = -1;
		return ((x >> s) & mask);
	}

	template <typename T>
	constexpr T rotr(const T x, const unsigned int s)
	{
		static_assert(std::is_unsigned<T>::value, "");
		if (s == 0)
			return x;
		return ((x >> s) | (x << ((sizeof(T) * 8) - s)));
	}


	//
	constexpr Blake1_512::Blake1_512()
	{
		static_assert((CHAR_BIT == 8), "Sorry, we don't support exotic CPUs");
		reset();
	}

	constexpr void Blake1_512::reset()
	{
		m_buffer.clear();
		m_sizeCounter = 0;

		m_h[0] = 0x6a09e667f3bcc908;
		m_h[1] = 0xbb67ae8584caa73b;
		m_h[2] = 0x3c6ef372fe94f82b;
		m_h[3] = 0xa54ff53a5f1d36f1;
		m_h[4] = 0x510e527fade682d1;
		m_h[5] = 0x9b05688c2b3e6c1f;
		m_h[6] = 0x1f83d9abfb41bd6b;
		m_h[7] = 0x5be0cd19137e2179;
	}

	constexpr Blake1_512& Blake1_512::finalize()
	{
		const Uint128 sizeCounterBits = (m_sizeCounter + (m_buffer.size() * 8));
		const uint64_t sizeCounterBitsL = sizeCounterBits.low();
		const uint64_t sizeCounterBitsH = sizeCounterBits.high();

		// append 1 bit
		m_buffer.fill(1 << 7);

		// append paddings
		const size_t len = BLOCK_SIZE - ((m_buffer.size() + 16) % BLOCK_SIZE);
		m_buffer.fill(0, (len + 16));

		m_buffer[m_buffer.size() - 17] |= 1;

		// append size in bits
		for (int i = 0; i < 8; ++i)
		{
			m_buffer[m_buffer.size() - 16 + i] = ror<Byte>(sizeCounterBitsH, (8 * (7 - i)));
			m_buffer[m_buffer.size() -  8 + i] = ror<Byte>(sizeCounterBitsL, (8 * (7 - i)));
		}

		addDataImpl({m_buffer.begin(), m_buffer.end()}, (len + 17));
		m_buffer.clear();

		return (*this);
	}

	std::string Blake1_512::toString() const
	{
		const auto a = toArray();
		std::string ret;
		ret.reserve(2 * a.size());
		for (const auto c : a)
		{
			char buf[3];
			snprintf(buf, sizeof(buf), "%02x", c);
			ret.append(buf);
		}

		return ret;
	}

	std::vector<Blake1_512::Byte> Blake1_512::toVector() const
	{
		const auto a = toArray();
		return {a.begin(), a.end()};
	}

	CONSTEXPR_CPP17_CHOCOBO1_HASH Blake1_512::ResultArrayType Blake1_512::toArray() const
	{
		const Span<const uint64_t> state(m_h);
		const int dataSize = sizeof(decltype(state)::value_type);

		int retCounter = 0;
		ResultArrayType ret {};
		for (const auto i : state)
		{
			for (int j = (dataSize - 1); j >= 0; --j)
				ret[retCounter++] = ror<Byte>(i, (j * 8));
		}

		return ret;
	}

	constexpr Blake1_512& Blake1_512::addData(const Span<const Byte> inData)
	{
		Span<const Byte> data = inData;

		if (!m_buffer.empty())
		{
			const size_t len = std::min<size_t>((BLOCK_SIZE - m_buffer.size()), data.size());  // try fill to BLOCK_SIZE bytes
			m_buffer.push_back(data.begin(), (data.begin() + len));

			if (m_buffer.size() < BLOCK_SIZE)  // still doesn't fill the buffer
				return (*this);

			addDataImpl({m_buffer.begin(), m_buffer.end()});
			m_buffer.clear();

			data = data.subspan(len);
		}

		const size_t dataSize = data.size();
		if (dataSize < BLOCK_SIZE)
		{
			m_buffer = {data.begin(), data.end()};
			return (*this);
		}

		const size_t len = dataSize - (dataSize % BLOCK_SIZE);  // align on BLOCK_SIZE bytes
		addDataImpl(data.first(len));

		if (len < dataSize)  // didn't consume all data
			m_buffer = {(data.begin() + len), data.end()};

		return (*this);
	}

	constexpr Blake1_512& Blake1_512::addData(const void *ptr, const long int length)
	{
		// gsl::span::index_type = long int
		return addData({static_cast<const Byte*>(ptr), length});
	}

	template <typename T, std::size_t N>
	Blake1_512& Blake1_512::addData(const T (&array)[N])
	{
		return addData({reinterpret_cast<const Byte*>(array), (sizeof(T) * N)});
	}

	template <typename T>
	Blake1_512& Blake1_512::addData(const Span<T> inSpan)
	{
		return addData({reinterpret_cast<const Byte*>(inSpan.data()), inSpan.size_bytes()});
	}

	constexpr void Blake1_512::addDataImpl(const Span<const Byte> data, const uint32_t paddingLen)
	{
		assert((data.size() % BLOCK_SIZE) == 0);

		for (size_t iter = 0, iend = static_cast<size_t>(data.size() / BLOCK_SIZE); iter < iend; ++iter)
		{
			const Loader<uint64_t> m(static_cast<const Byte *>(data.data() + (iter * BLOCK_SIZE)));

			// TODO: cTable was here, move it back when static variable in constexpr function is allowed

			uint64_t v[16] =
			{
				m_h[0], m_h[1], m_h[2], m_h[3], m_h[4], m_h[5], m_h[6], m_h[7],
				cTable[0], cTable[1], cTable[2], cTable[3], cTable[4], cTable[5], cTable[6], cTable[7]
			};

			const uint32_t nonPaddingBits = (BLOCK_SIZE - paddingLen) * 8;
			m_sizeCounter += nonPaddingBits;
			if (nonPaddingBits > 0)
			{
				const uint64_t t0 = m_sizeCounter.low();
				const uint64_t t1 = m_sizeCounter.high();
				v[12] ^= t0;
				v[13] ^= t0;
				v[14] ^= t1;
				v[15] ^= t1;
			}

			#ifdef blakeMix
			#error "macro name clash"
			#else
			#define blakeMix(a, b, c, d, x, y, constant1, constant2) \
				a = (a + b + (x ^ constant1)); \
				d = rotr((d ^ a), 32); \
				c = (c + d); \
				b = rotr((b ^ c), 25); \
				a = (a + b + (y ^ constant2)); \
				d = rotr((d ^ a), 16); \
				c = (c + d); \
				b = rotr((b ^ c), 11); \

			// had to unroll loop manually :(
			blakeMix(v[0], v[4], v[8], v[12], m[0], m[1], cTable[1], cTable[0]);
			blakeMix(v[1], v[5], v[9], v[13], m[2], m[3], cTable[3], cTable[2]);
			blakeMix(v[2], v[6], v[10], v[14], m[4], m[5], cTable[5], cTable[4]);
			blakeMix(v[3], v[7], v[11], v[15], m[6], m[7], cTable[7], cTable[6]);
			blakeMix(v[0], v[5], v[10], v[15], m[8], m[9], cTable[9], cTable[8]);
			blakeMix(v[1], v[6], v[11], v[12], m[10], m[11], cTable[11], cTable[10]);
			blakeMix(v[2], v[7], v[8], v[13], m[12], m[13], cTable[13], cTable[12]);
			blakeMix(v[3], v[4], v[9], v[14], m[14], m[15], cTable[15], cTable[14]);

			blakeMix(v[0], v[4], v[8], v[12], m[14], m[10], cTable[10], cTable[14]);
			blakeMix(v[1], v[5], v[9], v[13], m[4], m[8], cTable[8], cTable[4]);
			blakeMix(v[2], v[6], v[10], v[14], m[9], m[15], cTable[15], cTable[9]);
			blakeMix(v[3], v[7], v[11], v[15], m[13], m[6], cTable[6], cTable[13]);
			blakeMix(v[0], v[5], v[10], v[15], m[1], m[12], cTable[12], cTable[1]);
			blakeMix(v[1], v[6], v[11], v[12], m[0], m[2], cTable[2], cTable[0]);
			blakeMix(v[2], v[7], v[8], v[13], m[11], m[7], cTable[7], cTable[11]);
			blakeMix(v[3], v[4], v[9], v[14], m[5], m[3], cTable[3], cTable[5]);

			blakeMix(v[0], v[4], v[8], v[12], m[11], m[8], cTable[8], cTable[11]);
			blakeMix(v[1], v[5], v[9], v[13], m[12], m[0], cTable[0], cTable[12]);
			blakeMix(v[2], v[6], v[10], v[14], m[5], m[2], cTable[2], cTable[5]);
			blakeMix(v[3], v[7], v[11], v[15], m[15], m[13], cTable[13], cTable[15]);
			blakeMix(v[0], v[5], v[10], v[15], m[10], m[14], cTable[14], cTable[10]);
			blakeMix(v[1], v[6], v[11], v[12], m[3], m[6], cTable[6], cTable[3]);
			blakeMix(v[2], v[7], v[8], v[13], m[7], m[1], cTable[1], cTable[7]);
			blakeMix(v[3], v[4], v[9], v[14], m[9], m[4], cTable[4], cTable[9]);

			blakeMix(v[0], v[4], v[8], v[12], m[7], m[9], cTable[9], cTable[7]);
			blakeMix(v[1], v[5], v[9], v[13], m[3], m[1], cTable[1], cTable[3]);
			blakeMix(v[2], v[6], v[10], v[14], m[13], m[12], cTable[12], cTable[13]);
			blakeMix(v[3], v[7], v[11], v[15], m[11], m[14], cTable[14], cTable[11]);
			blakeMix(v[0], v[5], v[10], v[15], m[2], m[6], cTable[6], cTable[2]);
			blakeMix(v[1], v[6], v[11], v[12], m[5], m[10], cTable[10], cTable[5]);
			blakeMix(v[2], v[7], v[8], v[13], m[4], m[0], cTable[0], cTable[4]);
			blakeMix(v[3], v[4], v[9], v[14], m[15], m[8], cTable[8], cTable[15]);

			blakeMix(v[0], v[4], v[8], v[12], m[9], m[0], cTable[0], cTable[9]);
			blakeMix(v[1], v[5], v[9], v[13], m[5], m[7], cTable[7], cTable[5]);
			blakeMix(v[2], v[6], v[10], v[14], m[2], m[4], cTable[4], cTable[2]);
			blakeMix(v[3], v[7], v[11], v[15], m[10], m[15], cTable[15], cTable[10]);
			blakeMix(v[0], v[5], v[10], v[15], m[14], m[1], cTable[1], cTable[14]);
			blakeMix(v[1], v[6], v[11], v[12], m[11], m[12], cTable[12], cTable[11]);
			blakeMix(v[2], v[7], v[8], v[13], m[6], m[8], cTable[8], cTable[6]);
			blakeMix(v[3], v[4], v[9], v[14], m[3], m[13], cTable[13], cTable[3]);

			blakeMix(v[0], v[4], v[8], v[12], m[2], m[12], cTable[12], cTable[2]);
			blakeMix(v[1], v[5], v[9], v[13], m[6], m[10], cTable[10], cTable[6]);
			blakeMix(v[2], v[6], v[10], v[14], m[0], m[11], cTable[11], cTable[0]);
			blakeMix(v[3], v[7], v[11], v[15], m[8], m[3], cTable[3], cTable[8]);
			blakeMix(v[0], v[5], v[10], v[15], m[4], m[13], cTable[13], cTable[4]);
			blakeMix(v[1], v[6], v[11], v[12], m[7], m[5], cTable[5], cTable[7]);
			blakeMix(v[2], v[7], v[8], v[13], m[15], m[14], cTable[14], cTable[15]);
			blakeMix(v[3], v[4], v[9], v[14], m[1], m[9], cTable[9], cTable[1]);

			blakeMix(v[0], v[4], v[8], v[12], m[12], m[5], cTable[5], cTable[12]);
			blakeMix(v[1], v[5], v[9], v[13], m[1], m[15], cTable[15], cTable[1]);
			blakeMix(v[2], v[6], v[10], v[14], m[14], m[13], cTable[13], cTable[14]);
			blakeMix(v[3], v[7], v[11], v[15], m[4], m[10], cTable[10], cTable[4]);
			blakeMix(v[0], v[5], v[10], v[15], m[0], m[7], cTable[7], cTable[0]);
			blakeMix(v[1], v[6], v[11], v[12], m[6], m[3], cTable[3], cTable[6]);
			blakeMix(v[2], v[7], v[8], v[13], m[9], m[2], cTable[2], cTable[9]);
			blakeMix(v[3], v[4], v[9], v[14], m[8], m[11], cTable[11], cTable[8]);

			blakeMix(v[0], v[4], v[8], v[12], m[13], m[11], cTable[11], cTable[13]);
			blakeMix(v[1], v[5], v[9], v[13], m[7], m[14], cTable[14], cTable[7]);
			blakeMix(v[2], v[6], v[10], v[14], m[12], m[1], cTable[1], cTable[12]);
			blakeMix(v[3], v[7], v[11], v[15], m[3], m[9], cTable[9], cTable[3]);
			blakeMix(v[0], v[5], v[10], v[15], m[5], m[0], cTable[0], cTable[5]);
			blakeMix(v[1], v[6], v[11], v[12], m[15], m[4], cTable[4], cTable[15]);
			blakeMix(v[2], v[7], v[8], v[13], m[8], m[6], cTable[6], cTable[8]);
			blakeMix(v[3], v[4], v[9], v[14], m[2], m[10], cTable[10], cTable[2]);

			blakeMix(v[0], v[4], v[8], v[12], m[6], m[15], cTable[15], cTable[6]);
			blakeMix(v[1], v[5], v[9], v[13], m[14], m[9], cTable[9], cTable[14]);
			blakeMix(v[2], v[6], v[10], v[14], m[11], m[3], cTable[3], cTable[11]);
			blakeMix(v[3], v[7], v[11], v[15], m[0], m[8], cTable[8], cTable[0]);
			blakeMix(v[0], v[5], v[10], v[15], m[12], m[2], cTable[2], cTable[12]);
			blakeMix(v[1], v[6], v[11], v[12], m[13], m[7], cTable[7], cTable[13]);
			blakeMix(v[2], v[7], v[8], v[13], m[1], m[4], cTable[4], cTable[1]);
			blakeMix(v[3], v[4], v[9], v[14], m[10], m[5], cTable[5], cTable[10]);

			blakeMix(v[0], v[4], v[8], v[12], m[10], m[2], cTable[2], cTable[10]);
			blakeMix(v[1], v[5], v[9], v[13], m[8], m[4], cTable[4], cTable[8]);
			blakeMix(v[2], v[6], v[10], v[14], m[7], m[6], cTable[6], cTable[7]);
			blakeMix(v[3], v[7], v[11], v[15], m[1], m[5], cTable[5], cTable[1]);
			blakeMix(v[0], v[5], v[10], v[15], m[15], m[11], cTable[11], cTable[15]);
			blakeMix(v[1], v[6], v[11], v[12], m[9], m[14], cTable[14], cTable[9]);
			blakeMix(v[2], v[7], v[8], v[13], m[3], m[12], cTable[12], cTable[3]);
			blakeMix(v[3], v[4], v[9], v[14], m[13], m[0], cTable[0], cTable[13]);

			blakeMix(v[0], v[4], v[8], v[12], m[0], m[1], cTable[1], cTable[0]);
			blakeMix(v[1], v[5], v[9], v[13], m[2], m[3], cTable[3], cTable[2]);
			blakeMix(v[2], v[6], v[10], v[14], m[4], m[5], cTable[5], cTable[4]);
			blakeMix(v[3], v[7], v[11], v[15], m[6], m[7], cTable[7], cTable[6]);
			blakeMix(v[0], v[5], v[10], v[15], m[8], m[9], cTable[9], cTable[8]);
			blakeMix(v[1], v[6], v[11], v[12], m[10], m[11], cTable[11], cTable[10]);
			blakeMix(v[2], v[7], v[8], v[13], m[12], m[13], cTable[13], cTable[12]);
			blakeMix(v[3], v[4], v[9], v[14], m[14], m[15], cTable[15], cTable[14]);

			blakeMix(v[0], v[4], v[8], v[12], m[14], m[10], cTable[10], cTable[14]);
			blakeMix(v[1], v[5], v[9], v[13], m[4], m[8], cTable[8], cTable[4]);
			blakeMix(v[2], v[6], v[10], v[14], m[9], m[15], cTable[15], cTable[9]);
			blakeMix(v[3], v[7], v[11], v[15], m[13], m[6], cTable[6], cTable[13]);
			blakeMix(v[0], v[5], v[10], v[15], m[1], m[12], cTable[12], cTable[1]);
			blakeMix(v[1], v[6], v[11], v[12], m[0], m[2], cTable[2], cTable[0]);
			blakeMix(v[2], v[7], v[8], v[13], m[11], m[7], cTable[7], cTable[11]);
			blakeMix(v[3], v[4], v[9], v[14], m[5], m[3], cTable[3], cTable[5]);

			blakeMix(v[0], v[4], v[8], v[12], m[11], m[8], cTable[8], cTable[11]);
			blakeMix(v[1], v[5], v[9], v[13], m[12], m[0], cTable[0], cTable[12]);
			blakeMix(v[2], v[6], v[10], v[14], m[5], m[2], cTable[2], cTable[5]);
			blakeMix(v[3], v[7], v[11], v[15], m[15], m[13], cTable[13], cTable[15]);
			blakeMix(v[0], v[5], v[10], v[15], m[10], m[14], cTable[14], cTable[10]);
			blakeMix(v[1], v[6], v[11], v[12], m[3], m[6], cTable[6], cTable[3]);
			blakeMix(v[2], v[7], v[8], v[13], m[7], m[1], cTable[1], cTable[7]);
			blakeMix(v[3], v[4], v[9], v[14], m[9], m[4], cTable[4], cTable[9]);

			blakeMix(v[0], v[4], v[8], v[12], m[7], m[9], cTable[9], cTable[7]);
			blakeMix(v[1], v[5], v[9], v[13], m[3], m[1], cTable[1], cTable[3]);
			blakeMix(v[2], v[6], v[10], v[14], m[13], m[12], cTable[12], cTable[13]);
			blakeMix(v[3], v[7], v[11], v[15], m[11], m[14], cTable[14], cTable[11]);
			blakeMix(v[0], v[5], v[10], v[15], m[2], m[6], cTable[6], cTable[2]);
			blakeMix(v[1], v[6], v[11], v[12], m[5], m[10], cTable[10], cTable[5]);
			blakeMix(v[2], v[7], v[8], v[13], m[4], m[0], cTable[0], cTable[4]);
			blakeMix(v[3], v[4], v[9], v[14], m[15], m[8], cTable[8], cTable[15]);

			blakeMix(v[0], v[4], v[8], v[12], m[9], m[0], cTable[0], cTable[9]);
			blakeMix(v[1], v[5], v[9], v[13], m[5], m[7], cTable[7], cTable[5]);
			blakeMix(v[2], v[6], v[10], v[14], m[2], m[4], cTable[4], cTable[2]);
			blakeMix(v[3], v[7], v[11], v[15], m[10], m[15], cTable[15], cTable[10]);
			blakeMix(v[0], v[5], v[10], v[15], m[14], m[1], cTable[1], cTable[14]);
			blakeMix(v[1], v[6], v[11], v[12], m[11], m[12], cTable[12], cTable[11]);
			blakeMix(v[2], v[7], v[8], v[13], m[6], m[8], cTable[8], cTable[6]);
			blakeMix(v[3], v[4], v[9], v[14], m[3], m[13], cTable[13], cTable[3]);

			blakeMix(v[0], v[4], v[8], v[12], m[2], m[12], cTable[12], cTable[2]);
			blakeMix(v[1], v[5], v[9], v[13], m[6], m[10], cTable[10], cTable[6]);
			blakeMix(v[2], v[6], v[10], v[14], m[0], m[11], cTable[11], cTable[0]);
			blakeMix(v[3], v[7], v[11], v[15], m[8], m[3], cTable[3], cTable[8]);
			blakeMix(v[0], v[5], v[10], v[15], m[4], m[13], cTable[13], cTable[4]);
			blakeMix(v[1], v[6], v[11], v[12], m[7], m[5], cTable[5], cTable[7]);
			blakeMix(v[2], v[7], v[8], v[13], m[15], m[14], cTable[14], cTable[15]);
			blakeMix(v[3], v[4], v[9], v[14], m[1], m[9], cTable[9], cTable[1]);

			#undef blakeMix
			#endif

			m_h[0] ^= (v[0] ^ v[8]);
			m_h[1] ^= (v[1] ^ v[9]);
			m_h[2] ^= (v[2] ^ v[10]);
			m_h[3] ^= (v[3] ^ v[11]);
			m_h[4] ^= (v[4] ^ v[12]);
			m_h[5] ^= (v[5] ^ v[13]);
			m_h[6] ^= (v[6] ^ v[14]);
			m_h[7] ^= (v[7] ^ v[15]);
		}
	}
}
}
	using Blake1_512 = Hash::Blake1_512_NS::Blake1_512;
}

#endif  // CHOCOBO1_BLAKE1_512_H
