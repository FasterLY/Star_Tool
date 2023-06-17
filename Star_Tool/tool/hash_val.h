#pragma once
#include<functional>
namespace star {
	class hash {
	private:
		template<typename T>
		inline static void hash_combine(std::size_t& seed, const T& val) {
			seed ^= std::hash<T>::_Do_hash(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		template<typename T>
		inline static void hash_val(std::size_t& seed, const T& val) {
			hash::hash_combine(seed, val);
		}
		template<typename T, typename... Types>
		inline static void hash_val(std::size_t& seed, const T& val, const Types&...args) {
			hash::hash_combine(seed, val);
			hash::hash_val(seed, args...);
		}
	public:
		template<typename... Types>
		friend inline std::size_t hash_val(const Types&... args);
	};
	template<typename... Types>
	std::size_t hash_val(const Types&... args) {
		std::size_t seed = 0;
		hash::hash_val(seed, args...);
		return seed;
	}
}