#pragma once
#include<coroutine>
#include<functional>
namespace star {
    /*
    * ������C++20�����ϰ汾��Э�̷��ؾ������
    */

    /*
    * �ɼ̳���д��Э�̿�����
    */
    class awaiter
    {
    public:
        //�ж�Э���Ƿ�Ҫ���𣬷���false����𣬷���true�򲻹���
        virtual bool await_ready() { return false; }
        //Э����ͣ������������ǰ����
        virtual void await_suspend(std::coroutine_handle<> h) {}
        //Э�ָ̻������ָ�����
        virtual void await_resume() {}
    };

    /*
    * �ָ�Э��ʱ���̴����Э�̿�����
    */
    class resume_waiter 
    {
    private:
        function<void()> function_handle;
    public:
        template<typename Function,typename... Args>
        resume_waiter(Function&& function,Args&&... args) {
            function_handle = std::bind(std::forward<Function>(function), std::forward<Args>(args)...);
        }
        //Э�����������
        void await_ready() {}
        //Э����ͣ������������ǰ����
        void await_suspend(std::coroutine_handle<> h) {}
        //Э�ָ̻�
        void await_resume() { function_handle(); }
    };

    /*
    * ����Э��ǰ���̴����Э�̴��������
    */
    class suspend_waiter
    {
    private:
        function<void()> function_handle;
    public:
        template<typename Function, typename... Args>
        suspend_waiter(Function&& function, Args&&... args) {
            function_handle = std::bind(std::forward<Function>(function), std::forward<Args>(args)...);
        }
        //Э�����������
        void await_ready() {}
        //Э����ͣ������������ǰ����
        void await_suspend(std::coroutine_handle<> h) { function_handle(); }
        //Э�ָ̻�
        void await_resume() {}
    };

    /*
    * Э�̾���࣬Ĭ�ϲ����޷���ֵ�޳�ʼ����
    */
    template<typename ValueType, bool co_returnValue = false, bool suspend = false >
	class co_handle {
    public:
        /*
        * ���⹫�������ݽṹ
        */
        /* co_yield��co_wait��co_return �������ݽṹ*/
        class promise_type;
        class iterator;
    public:
        co_handle(promise_type* coroutine_promise_ptr) :coroutine_promise_ptr(coroutine_promise_ptr) {}
        iterator begin() { if (suspend) return resume(); return iterator{ this }; }
        nullptr_t end() { return nullptr; }
        iterator resume() { std::coroutine_handle<promise_type>::from_promise(*coroutine_promise_ptr)(); return iterator{ this }; }
        bool isEnd() { return coroutine_promise_ptr->coroutine_end_flag; }
        iterator getIterator() { return iterator{ this }; }
    private:
        promise_type* coroutine_promise_ptr;
        using co_handle_item = co_handle <ValueType, co_returnValue, suspend>;
        class promise_type
        {
        private:
            bool coroutine_end_flag;
            ValueType value;
        public:
            friend class co_handle_item::iterator;
            friend class co_handle_item;
            promise_type() :coroutine_end_flag(false) {};
            //���ؾ��
            co_handle_item get_return_object() { return { this }; }
            //���ó�ʼЭ��״̬�ǹ���������
            std::suspend_never initial_suspend() { return {}; }
            //����Э�̽������״̬Ϊ����״̬
            std::suspend_always final_suspend() noexcept { return {}; }
            //Э�����з���ֵΪ��ʱ�������ñ�־λ
            void return_void() { coroutine_end_flag = true; }
            //����Э����ʱ����ķ���ֵ��������ú󽫵�ǰЭ�̹���
            std::suspend_always yield_value(ValueType yieldValue) { this->value = yieldValue; return {}; }
            //������������
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle_item* handle;
        public:
            iterator(co_handle_item* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            co_handle_item::iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
            ValueType& operator*() { return handle->coroutine_promise_ptr->value; }
        };
	};

    /*
    * �ػ�����汾�޷���ֵ
    */
    template<typename ValueType>
    class co_handle <ValueType, false, true>
    {
    public:
        /*
        * ���⹫�������ݽṹ
        */
        /* co_yield��co_wait��co_return �������ݽṹ*/
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
        using co_handle_item = co_handle <ValueType, false, true>;
        class promise_type
        {
        private:
            bool coroutine_end_flag;
            ValueType value;
        public:
            friend class co_handle_item::iterator;
            friend class co_handle_item;
            promise_type() :coroutine_end_flag(false) {};
            //���ؾ��
            co_handle<ValueType, false, true> get_return_object() { return { this }; }
            //���ó�ʼЭ��״̬�ǹ���������
            std::suspend_always initial_suspend() { return {}; }
            //����Э�̽������״̬Ϊ����״̬
            std::suspend_always final_suspend() noexcept { return {}; }
            //Э�����з���ֵΪ��ʱ�������ñ�־λ
            void return_void() { coroutine_end_flag = true; }
            //����Э����ʱ����ķ���ֵ��������ú󽫵�ǰЭ�̹���
            std::suspend_always yield_value(ValueType yieldValue) { this->value = yieldValue; return {}; }
            //������������
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle_item* handle;
        public:
            iterator(co_handle_item* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            co_handle_item::iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
            ValueType& operator*() { return handle->coroutine_promise_ptr->value; }
        };
    };

    /*
    * �ػ��޹���汾�з���ֵ
    */
    template<typename ValueType>
    class co_handle <ValueType, true, false>
    {
    public:
        /*
        * ���⹫�������ݽṹ
        */
        /* co_yield��co_wait��co_return �������ݽṹ*/
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
        using co_handle_item = co_handle <ValueType, true, false>;
        class promise_type
        {
        private:
            bool coroutine_end_flag;
            ValueType value;
        public:
            friend class co_handle_item::iterator;
            friend class co_handle_item;
            promise_type() :coroutine_end_flag(false) {};
            //���ؾ��
            co_handle_item get_return_object() { return { this }; }
            //���ó�ʼЭ��״̬�ǹ���������
            std::suspend_never initial_suspend() { return {}; }
            //����Э�̽������״̬Ϊ����״̬
            std::suspend_always final_suspend() noexcept { return {}; }
            //Э�����з���ֵ��Ϊ��
            void return_value(ValueType value) { coroutine_end_flag = true; this->value = value; }
            //����Э����ʱ����ķ���ֵ��������ú󽫵�ǰЭ�̹���
            std::suspend_always yield_value(ValueType yieldValue) { this->value = yieldValue; return {}; }
            //������������
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle_item* handle;
        public:
            iterator(co_handle_item* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            co_handle_item::iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
            ValueType& operator*() { return handle->coroutine_promise_ptr->value; }
        };
    };

    /*
    * �ػ�����汾�з���ֵ
    */
    template<typename ValueType>
    class co_handle <ValueType, true, true>
    {
    public:
        /*
        * ���⹫�������ݽṹ
        */
        /* co_yield��co_wait��co_return �������ݽṹ*/
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
        using co_handle_item = co_handle <ValueType, true, true>;
        class promise_type
        {
        private:
            bool coroutine_end_flag;
            ValueType value;
        public:
            friend class co_handle_item::iterator;
            friend class co_handle_item;
            promise_type() :coroutine_end_flag(false) {};
            //���ؾ��
            co_handle_item get_return_object() { return { this }; }
            //���ó�ʼЭ��״̬�ǹ���������
            std::suspend_always initial_suspend() { return {}; }
            //����Э�̽������״̬Ϊ����״̬
            std::suspend_always final_suspend() noexcept { return {}; }
            //Э�����з���ֵ��Ϊ��
            void return_value(ValueType value) { coroutine_end_flag = true; this->value = value; }
            //����Э����ʱ����ķ���ֵ��������ú󽫵�ǰЭ�̹���
            std::suspend_always yield_value(ValueType yieldValue) { this->value = yieldValue; return {}; }
            //������������
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle_item* handle;
        public:
            iterator(co_handle_item* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            co_handle_item::iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
            ValueType& operator*() { return handle->coroutine_promise_ptr->value; }
        };
    };

    /*
    * �ػ������ݷ��ذ汾
    */
    template<>
    class co_handle<void>
    {
    public:
        /*
        * ���⹫�������ݽṹ
        */
        /* co_yield��co_wait��co_return �������ݽṹ*/
        class promise_type;
        class iterator;
    private:
        /*
        * �������������
        */
        promise_type* coroutine_promise_ptr;
    public:
        /*
        * ����������������
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
            awaiter coroutine_awaiter;
        public:
            friend class co_handle<void>::iterator;
            friend class co_handle<void>;
            promise_type() {};
            //���ؾ��
            co_handle<void> get_return_object() { return { this }; }
            //���ػָ�״̬
            std::suspend_never initial_suspend() { return {}; }
            //���ػָ�״̬
            std::suspend_always final_suspend() noexcept { return {}; }
            //Э�����з���ֵΪ��ʱ�������ñ�־λ
            void return_void() { coroutine_end_flag = true; }
            //����Э�̷���ֵ
            awaiter yield_value(nullptr_t&& _) { return coroutine_awaiter; }
            //������������
            void unhandled_exception() {}
        };
        class iterator
        {
        private:
            co_handle<void>* handle;
        public:
            iterator(co_handle<void>* handle) :handle(handle) {};
            bool operator!=(const nullptr_t& _) { return !handle->coroutine_promise_ptr->coroutine_end_flag; };
            co_handle<void>::iterator& operator++() { std::coroutine_handle<promise_type>::from_promise(*handle->coroutine_promise_ptr)(); return *this; };
        };
    };

}