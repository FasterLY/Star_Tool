#pragma once
#include<future>
#include<algorithm>
#include <type_traits>
#include"../thread_tool/thread_pool.h"
namespace star {
	template<typename Iterator>
	void quicksort(Iterator begin, Iterator end) {
		if (std::distance(begin, end) < 2) return;
		Iterator pivot = std::next(begin, std::distance(begin, end) / 2);
		auto const& pivot_value = *pivot;
		Iterator left = begin;
		Iterator right = std::prev(end);
		while (left <= right) {
			while (*left < pivot_value) ++left;
			while (*right > pivot_value) --right;
			if (left <= right) {
				std::iter_swap(left, right);
				++left;
				--right;
			}
		}
		quicksort(begin, right);
		quicksort(left, end);
	}

	template<typename Iterator>
	void quicksort_threads(Iterator begin, Iterator end) {
		if (std::distance(begin, end) < 2) return;
		Iterator pivot = std::next(begin, std::distance(begin, end) / 2);
		auto const& pivot_value = *pivot;
		Iterator left = begin;
		Iterator right = std::prev(end);
		while (left <= right) {
			while (*left < pivot_value) ++left;
			while (*right > pivot_value) --right;
			if (left <= right) {
				std::iter_swap(left, right);
				++left;
				--right;
			}
		}
		std::thread th1(quicksort_threads<Iterator>, left, end);
		quicksort_threads(begin, right);
		th1.join();
	}

	template<typename Iterator>
	void quicksort_threadpool(Iterator begin, Iterator end, thread_pool& pool) {
		if (std::distance(begin, end) < 2) return;
		Iterator pivot = std::next(begin, std::distance(begin, end) / 2);
		auto const& pivot_value = *pivot;
		Iterator left = begin;
		Iterator right = std::prev(end);
		while (left <= right) {
			while (*left < pivot_value) ++left;
			while (*right > pivot_value) --right;
			if (left <= right) {
				std::iter_swap(left, right);
				++left;
				--right;
			}
		}
		pool.submit(quicksort_threadpool<Iterator>, left , end, std::ref(pool));
		quicksort_threadpool(begin, right, pool);
	}
}