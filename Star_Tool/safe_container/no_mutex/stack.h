#pragma once
#include<atomic>
#include<memory>
namespace star {
	namespace safe_container {
		namespace no_mutex {
			template<class T>
			class stack {
				/*
				*当前无锁栈容器是否无锁取决于标准库内shared_ptr是否是无锁实现。
				*/
			private:
				struct node {
					std::shared_ptr<T> data;
					std::shared_ptr<node> next;
					node() :data(nullptr), next(nullptr) {}
					node(T Data) :data(std::make_shared<T>(std::move(Data))), next(nullptr) {}
					node(std::shared_ptr<T> Data_ptr) :data(Data_ptr), next(nullptr) {}
				};
				std::atomic<std::shared_ptr<node>> head;
			public:
				stack();
				void push(T data);
				template<typename...Args>
				void emplace(Args&&... args);
				void push(std::shared_ptr<T> data_ptr);
				std::shared_ptr<T> pop();
			};

			template<class T>
			inline stack<T>::stack() :head(nullptr) {}

			template<class T>
			inline void stack<T>::push(T data)
			{
				std::shared_ptr<node> new_node(std::make_shared<node>(std::move(data)));
				new_node->next = head.load();
				while (!(head.compare_exchange_weak(new_node->next, new_node)));
			}

			template<class T>
			inline void stack<T>::push(std::shared_ptr<T> data_ptr)
			{
				std::shared_ptr<node> new_node(std::make_shared<node>(data_ptr));
				new_node->next = head.load();
				while (!(head.compare_exchange_weak(new_node->next, new_node)));
			}

			template<class T>
			template<typename ...Args>
			inline void stack<T>::emplace(Args && ...args)
			{
				std::shared_ptr<node> new_node(std::make_shared<node>(std::make_shared<T>(args...)));
				new_node->next = head.load();
				while (!(head.compare_exchange_weak(new_node->next, new_node)));
			}

			template<class T>
			inline std::shared_ptr<T> stack<T>::pop() {
				std::shared_ptr<node> old_head = head.load();
				while (old_head && !head.compare_exchange_weak(old_head, old_head->next));
				return old_head ? old_head->data : nullptr;
			}
		}
	}
}
