#pragma once
#include<coroutine>
#include<functional>
namespace star {
    /*
    * 适用于C++20及以上版本的协程返回句柄对象
    */

    /* 有返回值标志位，用于填入协程句柄的模板参数 */
    class has_return { has_return() = delete; };
    /* 开始时挂起协程的标志位，用于填入协程句柄的模板参数 */
    class suspend { suspend() = delete; };
    /*
    * 可继承重写的协程控制器
    */
    class awaiter
    {
    public:
        //判断协程是否要挂起，返回false则挂起，返回true则不挂起
        virtual bool await_ready() { return false; }
        //协程暂停，函数做挂起前操作
        virtual void await_suspend(std::coroutine_handle<> h) {}
        //协程恢复，做恢复操作
        virtual void await_resume() {}
    };

    /*
    * 恢复协程时进程处理的协程控制器
    */
    class resume_waiter 
    {
    private:
        std::function<void()> function_handle;
    public:
        template<typename Function,typename... Args>
        resume_waiter(Function&& function,Args&&... args) {
            function_handle = std::bind(std::forward<Function>(function), std::forward<Args>(args)...);
        }
        //协程做挂起操作
        void await_ready() {}
        //协程暂停，函数做挂起前操作
        void await_suspend(std::coroutine_handle<> h) {}
        //协程恢复
        void await_resume() { function_handle(); }
    };

    /*
    * 挂起协程前进程处理的协程处理控制器
    */
    class suspend_waiter
    {
    private:
        std::function<void()> function_handle;
    public:
        template<typename Function, typename... Args>
        suspend_waiter(Function&& function, Args&&... args) {
            function_handle = std::bind(std::forward<Function>(function), std::forward<Args>(args)...);
        }
        //协程做挂起操作
        void await_ready() {}
        //协程暂停，函数做挂起前操作
        void await_suspend(std::coroutine_handle<> h) { function_handle(); }
        //协程恢复
        void await_resume() {}
    };

    /*
    * 协程句柄类，默认参数无返回值无初始挂起
    */
    template<typename ValueType,typename... Args>
	class co_handle {
    public:
        /*
        * 对外公布的数据结构
        */
        /* co_yield、co_wait、co_return 处理数据结构*/
        class promise_type;
        class iterator;
    public:
        /* 警告：请勿手动调用该方法，否则会造成不可预料后果 */
        co_handle(promise_type* coroutine_promise_ptr) :coroutine_promise_ptr(coroutine_promise_ptr) {}
        iterator begin() { return iterator{ this }; }
        nullptr_t end() { return nullptr; }
        iterator resume() { std::coroutine_handle<promise_type>::from_promise(*coroutine_promise_ptr)(); return iterator{ this }; }
        bool isEnd() { return coroutine_promise_ptr->coroutine_end_flag; }
        iterator getIterator() { return iterator{ this }; }
    private:
        promise_type* coroutine_promise_ptr;
        using co_handle_item = co_handle <ValueType>;
        class promise_type
        {
        private:
            bool coroutine_end_flag;
            ValueType value;
        public:
            friend class co_handle_item::iterator;
            friend class co_handle_item;
            promise_type() :coroutine_end_flag(false) {};
            //返回句柄
            co_handle_item get_return_object() { return { this }; }
            //设置初始协程状态是挂起还是运行
            std::suspend_never initial_suspend() { return {}; }
            //设置协程结束后的状态为结束状态
            std::suspend_always final_suspend() noexcept { return {}; }
            //协程运行返回值为空时结束设置标志位
            void return_void() { coroutine_end_flag = true; }
            //设置协程临时挂起的返回值，完成设置后将当前协程挂起
            std::suspend_always yield_value(ValueType yieldValue) { this->value = yieldValue; return {}; }
            //意料外句柄错误
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle_item* handle;
        public:
            iterator(co_handle_item* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
            ValueType& operator*() { return handle->coroutine_promise_ptr->value; }
        };
	};

    /*
    * 特化挂起版本无返回值
    */
    template<typename ValueType>
    class co_handle <ValueType, suspend>
    {
    public:
        /*
        * 对外公布的数据结构
        */
        /* co_yield、co_wait、co_return 处理数据结构*/
        class promise_type;
        class iterator;
    public:
        co_handle(promise_type* coroutine_promise_ptr) :coroutine_promise_ptr(coroutine_promise_ptr) {}
        iterator begin() { return resume(); }
        nullptr_t end() { return nullptr; }
        iterator resume() { std::coroutine_handle<promise_type>::from_promise(*coroutine_promise_ptr)(); return iterator{ this }; }
        bool isEnd() { return coroutine_promise_ptr->coroutine_end_flag; }
        iterator getIterator() { return iterator{ this }; }
    private:
        promise_type* coroutine_promise_ptr;
        using co_handle_item = co_handle <ValueType, suspend>;
        class promise_type
        {
        private:
            bool coroutine_end_flag;
            ValueType value;
        public:
            friend class co_handle_item::iterator;
            friend class co_handle_item;
            promise_type() :coroutine_end_flag(false) {};
            //返回句柄
            co_handle_item get_return_object() { return { this }; }
            //设置初始协程状态是挂起还是运行
            std::suspend_always initial_suspend() { return {}; }
            //设置协程结束后的状态为结束状态
            std::suspend_always final_suspend() noexcept { return {}; }
            //协程运行返回值为空时结束设置标志位
            void return_void() { coroutine_end_flag = true; }
            //设置协程临时挂起的返回值，完成设置后将当前协程挂起
            std::suspend_always yield_value(ValueType yieldValue) { this->value = yieldValue; return {}; }
            //意料外句柄错误
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle_item* handle;
        public:
            iterator(co_handle_item* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
            ValueType& operator*() { return handle->coroutine_promise_ptr->value; }
        };
    };

    /*
    * 特化无挂起版本有返回值
    */
    template<typename ValueType>
    class co_handle <ValueType, has_return>
    {
    public:
        /*
        * 对外公布的数据结构
        */
        /* co_yield、co_wait、co_return 处理数据结构*/
        class promise_type;
        class iterator;
    public:
        co_handle(promise_type* coroutine_promise_ptr) :coroutine_promise_ptr(coroutine_promise_ptr) {}
        iterator begin() { return iterator{ this }; }
        nullptr_t end() { return nullptr; }
        iterator resume() { std::coroutine_handle<promise_type>::from_promise(*coroutine_promise_ptr)(); return iterator{ this }; }
        bool isEnd() { return coroutine_promise_ptr->coroutine_end_flag; }
        iterator getIterator() { return iterator{ this }; }
    private:
        promise_type* coroutine_promise_ptr;
        using co_handle_item = co_handle <ValueType, has_return>;
        class promise_type
        {
        private:
            bool coroutine_end_flag;
            ValueType value;
        public:
            friend class co_handle_item::iterator;
            friend class co_handle_item;
            promise_type() :coroutine_end_flag(false) {};
            //返回句柄
            co_handle_item get_return_object() { return { this }; }
            //设置初始协程状态是挂起还是运行
            std::suspend_never initial_suspend() { return {}; }
            //设置协程结束后的状态为结束状态
            std::suspend_always final_suspend() noexcept { return {}; }
            //协程运行返回值不为空
            void return_value(ValueType value) { coroutine_end_flag = true; this->value = value; }
            //设置协程临时挂起的返回值，完成设置后将当前协程挂起
            std::suspend_always yield_value(ValueType yieldValue) { this->value = yieldValue; return {}; }
            //意料外句柄错误
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle_item* handle;
        public:
            iterator(co_handle_item* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
            ValueType& operator*() { return handle->coroutine_promise_ptr->value; }
        };
    };

    /*
    * 特化挂起版本有返回值
    */
    template<typename ValueType>
    class co_handle <ValueType, has_return, suspend>
    {
    public:
        /*
        * 对外公布的数据结构
        */
        /* co_yield、co_wait、co_return 处理数据结构*/
        class promise_type;
        class iterator;
    public:
        co_handle(promise_type* coroutine_promise_ptr) :coroutine_promise_ptr(coroutine_promise_ptr) {}
        iterator begin() { return resume();}
        nullptr_t end() { return nullptr; }
        iterator resume() { std::coroutine_handle<promise_type>::from_promise(*coroutine_promise_ptr)(); return iterator{ this }; }
        bool isEnd() { return coroutine_promise_ptr->coroutine_end_flag; }
        iterator getIterator() { return iterator{ this }; }
    private:
        promise_type* coroutine_promise_ptr;
        using co_handle_item = co_handle <ValueType, has_return, suspend>;
        class promise_type
        {
        private:
            bool coroutine_end_flag;
            ValueType value;
        public:
            friend class co_handle_item::iterator;
            friend class co_handle_item;
            promise_type() :coroutine_end_flag(false) {};
            //返回句柄
            co_handle_item get_return_object() { return { this }; }
            //设置初始协程状态是挂起还是运行
            std::suspend_always initial_suspend() { return {}; }
            //设置协程结束后的状态为结束状态
            std::suspend_always final_suspend() noexcept { return {}; }
            //协程运行返回值不为空
            void return_value(ValueType value) { coroutine_end_flag = true; this->value = value; }
            //设置协程临时挂起的返回值，完成设置后将当前协程挂起
            std::suspend_always yield_value(ValueType yieldValue) { this->value = yieldValue; return {}; }
            //意料外句柄错误
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle_item* handle;
        public:
            iterator(co_handle_item* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
            ValueType& operator*() { return handle->coroutine_promise_ptr->value; }
        };
    };

    /*
    * 特化挂起版本有返回值
    */
    template<typename ValueType>
    class co_handle <ValueType, suspend, has_return>
    {
    public:
        /*
        * 对外公布的数据结构
        */
        /* co_yield、co_wait、co_return 处理数据结构*/
        class promise_type;
        class iterator;
    public:
        co_handle(promise_type* coroutine_promise_ptr) :coroutine_promise_ptr(coroutine_promise_ptr) {}
        iterator begin() { return resume(); }
        nullptr_t end() { return nullptr; }
        iterator resume() { std::coroutine_handle<promise_type>::from_promise(*coroutine_promise_ptr)(); return iterator{ this }; }
        bool isEnd() { return coroutine_promise_ptr->coroutine_end_flag; }
        iterator getIterator() { return iterator{ this }; }
    private:
        promise_type* coroutine_promise_ptr;
        using co_handle_item = co_handle <ValueType, suspend, has_return>;
        class promise_type
        {
        private:
            bool coroutine_end_flag;
            ValueType value;
        public:
            friend class co_handle_item::iterator;
            friend class co_handle_item;
            promise_type() :coroutine_end_flag(false) {};
            //返回句柄
            co_handle_item get_return_object() { return { this }; }
            //设置初始协程状态是挂起还是运行
            std::suspend_always initial_suspend() { return {}; }
            //设置协程结束后的状态为结束状态
            std::suspend_always final_suspend() noexcept { return {}; }
            //协程运行返回值不为空
            void return_value(ValueType value) { coroutine_end_flag = true; this->value = value; }
            //设置协程临时挂起的返回值，完成设置后将当前协程挂起
            std::suspend_always yield_value(ValueType yieldValue) { this->value = yieldValue; return {}; }
            //意料外句柄错误
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle_item* handle;
        public:
            iterator(co_handle_item* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
            ValueType& operator*() { return handle->coroutine_promise_ptr->value; }
        };
    };

    /*
    * 特化无数据返回版本的无初始挂起版本
    */
    template<>
    class co_handle<void>
    {
    public:
        /*
        * 对外公布的数据结构
        */
        /* co_yield、co_wait、co_return 处理数据结构*/
        class promise_type;
        class iterator;
    private:
        /*
        * 对象变量数据区
        */
        promise_type* coroutine_promise_ptr;
        using co_handle_item = co_handle<void>;
    public:
        /*
        * 函数区（方法区）
        */
        co_handle(promise_type* coroutine_promise_ptr) :coroutine_promise_ptr(coroutine_promise_ptr) {}
        iterator begin() { return iterator{ this }; }
        nullptr_t end() { return nullptr; }
        iterator resume() { std::coroutine_handle<promise_type>::from_promise(*coroutine_promise_ptr)(); return iterator{ this }; }
        bool isEnd() { return coroutine_promise_ptr->coroutine_end_flag; }
        iterator getIterator() { return iterator{ this }; }
        class promise_type
        {
        private:
            bool coroutine_end_flag = false;
        public:
            friend class co_handle_item::iterator;
            friend class co_handle_item;
            promise_type() {};
            //返回句柄
            co_handle_item get_return_object() { return { this }; }
            //返回恢复状态
            std::suspend_never initial_suspend() { return {}; }
            //返回恢复状态
            std::suspend_always final_suspend() noexcept { return {}; }
            //协程运行返回值为空时结束设置标志位
            void return_void() { coroutine_end_flag = true; }
            //设置协程返回值
            std::suspend_always yield_value(nullptr_t&& _) { return {}; }
            //意料外句柄错误
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle_item* handle;
        public:
            iterator(co_handle_item* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
        };
    };
    /*
    * 特化无数据返回版本的有初始挂起版本
    */
    template<>
    class co_handle<suspend>
    {
    public:
        /*
        * 对外公布的数据结构
        */
        /* co_yield、co_wait、co_return 处理数据结构*/
        class promise_type;
        class iterator;
    private:
        /*
        * 对象变量数据区
        */
        promise_type* coroutine_promise_ptr;
        using co_handle_item = co_handle<suspend>;
    public:
        /*
        * 函数区（方法区）
        */
        co_handle(promise_type* coroutine_promise_ptr) :coroutine_promise_ptr(coroutine_promise_ptr) {}
        iterator begin() { return iterator{ this }; }
        nullptr_t end() { return nullptr; }
        iterator resume() { std::coroutine_handle<promise_type>::from_promise(*coroutine_promise_ptr)(); return iterator{ this }; }
        bool isEnd() { return coroutine_promise_ptr->coroutine_end_flag; }
        iterator getIterator() { return iterator{ this }; }
        class promise_type
        {
        private:
            bool coroutine_end_flag = false;
        public:
            friend class co_handle_item::iterator;
            friend class co_handle_item;
            promise_type() {};
            //返回句柄
            co_handle_item get_return_object() { return { this }; }
            //返回恢复状态
            std::suspend_always initial_suspend() { return {}; }
            //返回恢复状态
            std::suspend_always final_suspend() noexcept { return {}; }
            //协程运行返回值为空时结束设置标志位
            void return_void() { coroutine_end_flag = true; }
            //设置协程返回值
            std::suspend_always yield_value(nullptr_t&& _) { return {}; }
            //意料外句柄错误
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle_item* handle;
        public:
            iterator(co_handle_item* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
        };
    };
}