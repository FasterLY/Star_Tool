#pragma once
#include<memory>
#include<mutex>
#include<condition_variable>
#include<queue>
namespace star {
	namespace safe_container {
		namespace mutex {
            /**
            *   本节有锁队列详细代码摘自C++并行实战（第二版）内容并整理代码头部分布
            **/
            template<typename T>
            class queue
            {
            private:
                mutable std::mutex mut;
                std::queue<std::shared_ptr<T> > data_queue;
                std::condition_variable data_cond;
            public:
                queue() {}
                void wait_and_pop(T& value);
                bool try_pop(T& value);
                std::shared_ptr<T> wait_and_pop();
                std::shared_ptr<T> try_pop();
                bool empty() const;
                void push(T new_value);
                template<typename...Args>
                void emplace(Args... args);
                void push(std::shared_ptr<T> new_value);
            };

            template<typename T>
            inline void queue<T>::wait_and_pop(T& value)
            {
                std::unique_lock<std::mutex> lk(mut);
                data_cond.wait(lk, [this] {return !data_queue.empty(); });
                value = std::move(*data_queue.front());
                data_queue.pop();
            }

            template<typename T>
            inline bool queue<T>::try_pop(T& value)
            {
                std::lock_guard<std::mutex> lk(mut);
                if (data_queue.empty())
                    return false;
                value = std::move(*data_queue.front());
            }

            template<typename T>
            inline std::shared_ptr<T> queue<T>::wait_and_pop()
            {
                std::unique_lock<std::mutex> lk(mut);
                data_cond.wait(lk, [this] {return !data_queue.empty(); });
                std::shared_ptr<T> res = data_queue.front();
                data_queue.pop();
                return res;
            }

            template<typename T>
            inline std::shared_ptr<T> queue<T>::try_pop()
            {
                std::lock_guard<std::mutex> lk(mut);
                if (data_queue.empty())
                    return std::shared_ptr<T>();
                std::shared_ptr<T> res = data_queue.front();
                data_queue.pop();
                return res;
            }

            template<typename T>
            inline bool queue<T>::empty() const
            {
                std::lock_guard<std::mutex> lk(mut);
                return data_queue.empty();
            }

            template<typename T>
            inline void queue<T>::push(T new_value)
            {
                std::shared_ptr<T> data(std::make_shared<T>(std::move(new_value)));
                std::lock_guard<std::mutex> lk(mut);
                data_queue.push(data);
                data_cond.notify_one();
            }

            template<typename T>
            inline void queue<T>::push(std::shared_ptr<T> new_value)
            {
                std::lock_guard<std::mutex> lk(mut);
                data_queue.push(new_value);
                data_cond.notify_one();
            }

            template<typename T>
            template<typename ...Args>
            inline void queue<T>::emplace(Args ...args)
            {
                std::shared_ptr<T> data(std::make_shared<T>(args...));
                std::lock_guard<std::mutex> lk(mut);
                data_queue.push(data);
                data_cond.notify_one();
            }
        }
	}
}