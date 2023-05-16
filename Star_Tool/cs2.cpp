#pragma once
#include<atomic>
#include<memory>
#include<iostream>
#include<random>
#include<algorithm>
#include<chrono>
#include<signal.h>
#include<vector>
#include"safe_container/no_mutex/stack.h"
#include"safe_container/no_mutex/queue.h"
#include"thread_tool/thread_pool.h"
#include"algorithm/async_quick_sort.h"
#include<iostream>

int async_cs(int argc, char* argv[]) {
	using namespace std;
	std::default_random_engine engine;
	std::uniform_int_distribution distance(0, 1000000);
	vector<int> vec;
	int num_size = std::stoi(argv[1]);
	int thread_size = std::stoi(argv[2]);
	star::thread_pool pool(thread_size);
	vec.reserve(num_size);
	for (int i = 0; i < num_size; i++) {
		vec.push_back(distance(engine));
	}
	vector<int> vec2 = vec;
	vector<int> vec3 = vec;
	vector<int> vec4 = vec;
	auto time_point1 = chrono::system_clock::now();
	star::quicksort(vec2.begin(), vec2.end());
	auto time_point2 = chrono::system_clock::now();
	star::quicksort_threads(vec3.begin(), vec3.end());
	auto time_point3 = chrono::system_clock::now();
	star::quicksort_threadpool(vec4.begin(), vec4.end(), pool);
	pool.join();
	auto time_point4 = std::chrono::system_clock::now();

	auto dur1 = time_point2 - time_point1;
	auto dur2 = time_point3 - time_point2;
	auto dur3 = time_point4 - time_point3;
	cout << "General quick sort method (unit: microseconds):        " << std::chrono::duration_cast<chrono::microseconds>(dur1).count() << endl;
	cout << "Multi-threaded quick sort method (unit: microseconds): " << std::chrono::duration_cast<chrono::microseconds>(dur2).count() << endl;
	cout << "Thread-pool quick sort method (in microseconds):       " << std::chrono::duration_cast<chrono::microseconds>(dur3).count() << endl;
	vec2.shrink_to_fit();
}

namespace star {
	namespace safe_container {
		namespace no_mutex {
			template<typename T>
			class queue
			{
			public:
				queue();
				~queue();
				void push(T data);
				std::unique_ptr<T> pop();
			private:
				struct node;
				struct counted_node_ptr;
				struct node_counter;
				static void increase_external_count(std::atomic<counted_node_ptr>& counter, counted_node_ptr& old_counter);
				static void free_external_counter(counted_node_ptr& old_node_ptr);
				void set_new_tail(counted_node_ptr& old_tail, counted_node_ptr const& new_tail);
				std::atomic<counted_node_ptr> head;
				std::atomic<counted_node_ptr> tail;
			};

			template<typename T>
			inline queue<T>::queue() :head(), tail(head.load()) {}

			template<typename T>
			inline queue<T>::~queue()
			{
				while (node* const old_head = head.load()) {
					head.store(old_head->next());
					delete old_head;
				}
			}

			template<typename T>
			inline void queue<T>::push(T data)
			{
				std::unique_ptr<T> new_data(std::make_unique<T>(data));
				counted_node_ptr new_next;
				new_next.ptr = new node;
				new_next.external_count = 1;
				counted_node_ptr old_tail = tail.load();
				while (true) {
					increase_external_count(tail, old_tail);
					T* old_data = nullptr;
					if (old_tail.ptr->data.compare_exchange_strong(old_data, new_data.get())) {
						counted_node_ptr old_next = { 0 };
						if (!old_tail.ptr->next.compare_exchange_strong(old_next, new_next)) {
							delete new_next.ptr;
							new_next = old_next;
						}
						set_new_tail(old_tail, new_next);
						new_data.release();
						break;
					}
					else {
						counted_node_ptr old_next = { 0 };
						if (old_tail.ptr->next.compare_exchange_strong(old_next, new_next)) {
							old_next = new_next;
							new_next.ptr = new node;
						}
						set_new_tail(old_tail, old_next);
					}
				}
			}

			template<typename T>
			inline std::unique_ptr<T> queue<T>::pop() {
				counted_node_ptr old_head = head.load(std::memory_order_relaxed);
				while (true) {
					increase_external_count(head, old_head);
					node* const ptr = old_head.ptr;
					if (ptr == tail.load().ptr) {
						return nullptr;
					}
					counted_node_ptr next = ptr->next.load();
					if (head.compare_exchange_strong(old_head, next)) {
						T* const res = ptr->data.exchange(nullptr);
						free_external_counter(old_head);
						return std::unique_ptr<T>(res);
					}
					ptr->release_ref();
				}
			}

			template<typename T>
			inline void queue<T>::increase_external_count(std::atomic<counted_node_ptr>& counter, counted_node_ptr& old_counter)
			{
				counted_node_ptr new_counter;
				do {
					new_counter = old_counter;
					++new_counter.external_count;
				} while (!counter.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));
				old_counter.external_count = new_counter.external_count;
			}

			template<typename T>
			inline void queue<T>::free_external_counter(counted_node_ptr& old_node_ptr)
			{
				node* const ptr = old_node_ptr.ptr;
				int const count_increase = old_node_ptr.external_count - 2;
				node_counter old_counter = ptr->count.load(std::memory_order_relaxed);
				node_counter new_counter;
				do {
					new_counter = old_counter;
					--new_counter.external_counters;
					new_counter.internal_count += count_increase;
				} while (!ptr->count.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));
				if (!new_counter.internal_count && !new_counter.external_counters) {
					delete ptr;
				}
			}

			template<typename T>
			inline void queue<T>::set_new_tail(counted_node_ptr& old_tail, counted_node_ptr const& new_tail)
			{
				node* const current_tail_ptr = old_tail.ptr;
				while (!tail.compare_exchange_weak(old_tail, new_tail) && old_tail.ptr == current_tail_ptr);
				if (old_tail.ptr == current_tail_ptr) {
					free_external_counter(old_tail);
				}
				else {
					current_tail_ptr->release_ref();
				}
			}

			template<typename T>
			struct queue<T>::node
			{
				std::atomic<T*> data;
				std::atomic<node_counter> count;
				std::atomic <counted_node_ptr> next;
				node();
				void release_ref();
			};

			template<typename T>
			inline queue<T>::node::node() {
				node_counter new_count;
				new_count.internal_count = 0;
				new_count.external_counters = 2;
				count.store(new_count);
				counted_node_ptr buf;
				buf.external_count = 0;
				buf.ptr = nullptr;
				next.store(buf);
			}

			template<typename T>
			inline void queue<T>::node::release_ref()
			{
				node_counter old_counter = count.load(std::memory_order_relaxed);
				node_counter new_counter;
				do {
					new_counter = old_counter;
					--new_counter.internal_count;
				} while (!count.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));
				if (!new_counter.internal_count && !new_counter.external_counters) {
					delete this;
				}
			}

			template<typename T>
			struct queue<T>::counted_node_ptr
			{
				int external_count;
				node* ptr;
			};

			template<typename T>
			struct queue<T>::node_counter
			{
				unsigned internal_count : 30;
				unsigned external_counters : 2;
			};
		}
	}

}
