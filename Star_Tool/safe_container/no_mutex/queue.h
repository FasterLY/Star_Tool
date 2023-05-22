#pragma once
#include<atomic>
#include<memory>
namespace star {
	namespace safe_container {
		namespace no_mutex {
			/**
			*   本节无锁队列详细代码摘自C++并行实战（第二版）内容并整理代码头部分布并修复了部分代码
			**/
			template<typename T>
			class queue
			{
				/*
				* 本节无锁队列详细代码摘自C++并行实战（第二版）内容并整理代码头部分布并修复了部分代码
				*/
			public:
				queue();
				~queue();
				template<typename...Args>
				void emplace(Args&&... args);
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


			/*
			* 创建空队列，对头节点创建一个引用数量为1，节点指针为初始化即数据指针为空的数据节点
			*/
			template<typename T>
			inline queue<T>::queue() :head{ counted_node_ptr{1,new node{}} }, tail{ head.load() } {}

			template<typename T>
			inline queue<T>::~queue()
			{
				while (this->pop() != nullptr);
				delete (this->head.load()).ptr;
			}


			template<typename T>
			template<typename ...Args>
			inline void queue<T>::emplace(Args && ...args)
			{
				std::unique_ptr<T> new_data{std::make_unique<T>(args...)};							//数据暂存
				counted_node_ptr new_next;															//新的引用节点
				new_next.ptr = new node{};															//设置引用节点的新数据节点
				new_next.external_count = 1;														//设置引用节点的引用数量
				counted_node_ptr old_tail = tail.load();											//暂存旧的尾部引用节点
				while (true)
				{
					increase_external_count(tail, old_tail);										//对尾部节点的引用加1并更新暂存尾部节点
					T* old_data = nullptr;
					if (old_tail.ptr->data.compare_exchange_strong(old_data, new_data.get()))		//若尾部节点数据的数据指针为空则用新数据节点替换
					{
						counted_node_ptr old_next{};												//创建旧的尾部引用节点的下一引用节点的暂存
						if (!old_tail.ptr->next.compare_exchange_strong(
							old_next, new_next))													//旧的尾部引用的数据指向下一个引用数据为空则用新的引用节点替换
						{
							delete new_next.ptr;													//若替换失败代表旧的尾部引用节点的下一引用节点已经被替换了
							new_next = old_next;													//则这个新引用节点就没有意义了，删除即可。
						}
						set_new_tail(old_tail, new_next);											//设置更新新的尾部节点
						new_data.release();															//取消数据节点的托管，将指针交给节点管理
						break;
					}
					else
					{
						counted_node_ptr old_next{};
						if (old_tail.ptr->next.compare_exchange_strong(
							old_next, new_next))
						{
							old_next = new_next;
							new_next.ptr = new node;
						}
						set_new_tail(old_tail, old_next);
					}
				}
			}

			template<typename T>
			inline void queue<T>::push(T data)
			{
				std::unique_ptr<T> new_data{ std::make_unique<T>(data) };											//数据暂存
				counted_node_ptr new_next;															//新的引用节点
				new_next.ptr = new node{};															//设置引用节点的新数据节点
				new_next.external_count = 1;														//设置引用节点的引用数量
				counted_node_ptr old_tail = tail.load();											//暂存旧的尾部引用节点
				while(true)															
				{								
					increase_external_count(tail, old_tail);										//对尾部节点的引用加1并更新暂存尾部节点
					T* old_data = nullptr;
					if (old_tail.ptr->data.compare_exchange_strong(old_data, new_data.get()))		//若尾部节点数据的数据指针为空则用新数据节点替换
					{
						counted_node_ptr old_next{};												//创建旧的尾部引用节点的下一引用节点的暂存
						if (!old_tail.ptr->next.compare_exchange_strong(
							old_next, new_next))													//旧的尾部引用的数据指向下一个引用数据为空则用新的引用节点替换
						{
							delete new_next.ptr;													//若替换失败代表旧的尾部引用节点的下一引用节点已经被替换了
							new_next = old_next;													//则这个新引用节点就没有意义了，删除即可。
						}
						set_new_tail(old_tail, new_next);											//设置更新新的尾部节点
						new_data.release();															//取消数据节点的托管，将指针交给节点管理
						break;
					}
					else
					{
						counted_node_ptr old_next{};
						if (old_tail.ptr->next.compare_exchange_strong(
							old_next, new_next))
						{
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


			/*
			*	更新引用节点的外部引用数量使其加一并，更新旧引用节点的数据
			*/
			template<typename T>
			inline void queue<T>::increase_external_count(std::atomic<counted_node_ptr>& counter, counted_node_ptr& old_counter)
			{
				counted_node_ptr new_counter;				//设置临时节点变量
				do {
					new_counter = old_counter;
					++new_counter.external_count;
				} while (!counter.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));
					//更新直到提高尾部节点引用成功
				old_counter.external_count = new_counter.external_count;
			}

			/*
			*	
			*/
			template<typename T>
			inline void queue<T>::free_external_counter(counted_node_ptr& old_node_ptr)
			{
				node* const ptr = old_node_ptr.ptr;										//获取临时尾部节点的数据节点指针
				int const count_increase = old_node_ptr.external_count - 2;				//
				node_counter old_counter = ptr->count.load(std::memory_order_relaxed);	//获取数据节点的内部引用计数器
				node_counter new_counter;												//设置新的临时计数器
				do {
					new_counter = old_counter;											//将旧的内部引用器存到临时引用器中
					--new_counter.external_counters;									//将新的临时计数器的外部应用器数量减1
					new_counter.internal_count += count_increase;						//将
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
					//若尾部节点不为old_tail也会退出循环，否则用new_tail替换掉tail
				if (old_tail.ptr == current_tail_ptr) {		//判断尾部节点是否已经被替换
					free_external_counter(old_tail);		//若已经被替换，
				}
				else {
					current_tail_ptr->release_ref();		//未被替换。释放
				}
			}

			template<typename T>
			struct queue<T>::node
			{
				std::atomic<T*> data;
				std::atomic<node_counter> count;
				std::atomic<counted_node_ptr> next;
				node();
				void release_ref();
			};

			template<typename T>
			inline queue<T>::node::node()
				:data(nullptr), next()
			{
				memset(this, 0, sizeof(node));
				node_counter new_count;
				new_count.internal_count = 0;
				new_count.external_counters = 2;
				count.store(new_count);
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
				counted_node_ptr();
				counted_node_ptr(int external_count, node* ptr);
			};

			template<typename T>
			struct queue<T>::node_counter
			{
				unsigned internal_count : 30;
				unsigned external_counters : 2;
			};

			template<typename T>
			inline queue<T>::counted_node_ptr::counted_node_ptr()
			{
				memset(this, 0, sizeof(counted_node_ptr));
				external_count = 0;
				ptr = nullptr;
			}

			template<typename T>
			inline queue<T>::counted_node_ptr::counted_node_ptr(int external_count, node* ptr)
				:external_count(external_count), ptr(ptr)
			{}
		}
	}
}
