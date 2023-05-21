#pragma once
#include<exception>
#include<memory>
#include<mutex>
#include<stack>
namespace star {
	namespace safe_container {
		namespace mutex {
            struct empty_stack : std::exception
            {
                const char* what() const throw()
                {
                    return "empty stack";
                }
            };

            template<typename T>
            class stack
            {
            private:
                std::stack<T> data;
                mutable std::mutex m;
            public:
                stack() {}
                stack(const stack& other);
                stack& operator=(const stack&) = delete;
                void push(T new_value);
                template<typename...Args>
                void emplace(Args&&... args);
                std::shared_ptr<T> pop();
                void pop(T& value);
                bool empty() const;
            };

            template<typename T>
            inline stack<T>::stack(const stack& other)
            {
                std::lock_guard<std::mutex> lock(other.m);
                data = other.data;
            }

            template<typename T>
            inline void stack<T>::push(T new_value)
            {
                std::lock_guard<std::mutex> lock(m);
                data.push(std::move(new_value));
            }

            template<typename T>
            template<typename...Args>
            void stack<T>::emplace(Args&&... args)
            {
                std::lock_guard<std::mutex> lock(m);
                data.emplace(args...);
            }

            template<typename T>
            inline std::shared_ptr<T> stack<T>::pop()
            {
                std::lock_guard<std::mutex> lock(m);
                if (data.empty()) throw empty_stack();
                std::shared_ptr<T> const res(
                    std::make_shared<T>(std::move(data.top())));
                data.pop();
                return res;
            }

            template<typename T>
            inline void stack<T>::pop(T& value)
            {
                std::lock_guard<std::mutex> lock(m);
                if (data.empty()) throw empty_stack();
                value = std::move(data.top());
                data.pop();
            }

            template<typename T>
            inline bool stack<T>::empty() const
            {
                std::lock_guard<std::mutex> lock(m);
                return data.empty();
            }
        }
	}
}

