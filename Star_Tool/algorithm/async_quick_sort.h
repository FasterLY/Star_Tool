#pragma once
#include<future>
#include<algorithm>
#include"../thread_tool/thread_pool.h"
namespace star {
	template<typename Iterator>
	void quicksort(Iterator begin, Iterator end) {
		if (end - begin < 2)return;
		Iterator Begin = begin;
		Iterator End = end - 1;
		bool reverse = false;
		while (Begin < End) {
			if (reverse) {
				if (*Begin <= *End) ++Begin;
				else {
					std::swap(*Begin, *End);
					reverse = false;
				}
			}
			else {
				if (*Begin <= *End) End--;
				else {
					std::swap(*Begin, *End);
					reverse = true;
				}
			}
		}
		Begin = reverse ? End : Begin;
		quicksort(begin, Begin);
		quicksort(Begin+1, end);
	}

	template<typename Iterator>
	void quicksort_threads(Iterator begin, Iterator end) {
		if (end - begin < 2)return;
		Iterator Begin = begin;
		Iterator End = end - 1;
		bool reverse = false;
		while (Begin < End) {
			if (reverse) {
				if (*Begin <= *End) ++Begin;
				else {
					std::swap(*Begin, *End);
					reverse = false;
				}
			}
			else {
				if (*Begin <= *End)
					End--;
				else {
					std::swap(*Begin, *End);
					reverse = true;
				}
			}
		}
		Begin = reverse ? End : Begin;
		std::thread th1(quicksort_threads<Iterator>, Begin + 1, end);
		quicksort_threads(begin, Begin);
		th1.join();
	}

	template<typename Iterator>
	void quicksort_threadpool(Iterator begin, Iterator end,thread_pool& pool) {
		if (end - begin < 2)return;
		Iterator Begin = begin;
		Iterator End = end - 1;
		bool reverse = false;
		while (Begin < End) {
			if (reverse) {
				if (*Begin <= *End) ++Begin;
				else {
					std::swap(*Begin, *End);
					reverse = false;
				}
			} 
			else {
				if (*Begin <= *End) End--;
				else {
					std::swap(*Begin, *End);
					reverse = true;
				}
			}
		}
		Begin = reverse ? End : Begin;
		pool.submit(quicksort_threadpool<Iterator>, Begin + 1, end, std::ref(pool));
		quicksort_threadpool(begin, Begin, pool);
	}
}