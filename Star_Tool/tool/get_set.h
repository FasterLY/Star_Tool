#pragma once
#include<functional>
namespace star {
    template<typename DATA_TYPE>
    class get_set
    {
    public:
        /*
        * data:源数据的引用
        * 使用默认依赖过滤器，传入数据引用
        */
        get_set(DATA_TYPE& data);
        /*
        * data:源数据的引用
        * getter: 依赖获取过滤器
        * setter: 依赖注入过滤器
        */
        get_set(DATA_TYPE& data, std::function<DATA_TYPE(DATA_TYPE)> getter, std::function<DATA_TYPE(DATA_TYPE)> setter);
        /*
        * 修改setter，流式依赖注入器
        * setter:修改后的依赖注入过滤器
        * 返回值:get_set本身
        */
        get_set<DATA_TYPE>& change_setter(std::function<DATA_TYPE(DATA_TYPE)> setter);
        /*
        * 修改getter，流式依赖注入器
        * getter:修改后的依赖获取过滤器
        * 返回值:get_set本身
        */
        get_set<DATA_TYPE>& change_getter(std::function<DATA_TYPE(DATA_TYPE)> getter); 
        /*
        * 重载=运算符，data会经过移动(move)并经过setter过滤器后赋值给引用数据
        */
        get_set& operator =(DATA_TYPE data);
        /*
        * 重载隐式转化，隐式转化为数据类型，会移动(move)并经过getter过滤后返回
        */
        operator DATA_TYPE();
        /*
        * 会对源数据进行move移动进入getter过滤器后并返回源数据通过移动构造的对象，原来的数据可能无法正常使用。
        */
        DATA_TYPE move();
        /*
        * 对源数据进行copy复制拷贝后经过getter过滤器后返回拷贝函数构造的对象(一般情况下与隐式转化结果相同）
        */
        DATA_TYPE copy();
        /*
        *  返回源数据的引用。
        *  !!危险方法，在修改引用时会略过setter过滤器，这应仅仅用于调用DATA_TYPE的函数。
        */
        DATA_TYPE& ref();
        /*
        * 源数据的类型
        */
        using type = DATA_TYPE;
    private:
        DATA_TYPE& data;
        std::function<DATA_TYPE(DATA_TYPE)> getter;
        std::function<DATA_TYPE(DATA_TYPE)> setter;
    public:     //不重要的公有函数
        /*
        * 拷贝函数已被删除
        */
        get_set(const get_set<DATA_TYPE>& CopySource) = delete;
        /*
        * 移动构造函数已被删除
        */
        get_set(get_set<DATA_TYPE>&& MoveSource) = delete;
        /*
        * 拷贝赋值函数已删除
        */
        get_set& operator =(const get_set<DATA_TYPE>& CopySource) = delete;
        /*
        * 移动赋值函数已删除
        */
        get_set& operator =(get_set<DATA_TYPE>&& MoveSource) = delete;
        /*
        * 默认析构函数
        */
        ~get_set();
    };

    template<typename DATA_TYPE>
    inline get_set<DATA_TYPE>::get_set(DATA_TYPE& data) :data(data)
    {
        this->getter = [](DATA_TYPE data) {
            return std::move(data);
        };
        this->setter = [](DATA_TYPE data) {
            return std::move(data);
        };
    }

    template<typename DATA_TYPE>
    inline get_set<DATA_TYPE>::get_set(DATA_TYPE& data, std::function<DATA_TYPE(DATA_TYPE)> getter, std::function<DATA_TYPE(DATA_TYPE)> setter)
        : data(data), getter(getter), setter(setter) {}

    template<typename DATA_TYPE>
    inline get_set<DATA_TYPE>::~get_set()
    {}

    template<typename DATA_TYPE>
    inline get_set<DATA_TYPE>& get_set<DATA_TYPE>::change_setter(std::function<DATA_TYPE(DATA_TYPE)> setter)
    {
        this->setter = std::move(setter);
        return *this;
    }

    template<typename DATA_TYPE>
    inline get_set<DATA_TYPE>& get_set<DATA_TYPE>::change_getter(std::function<DATA_TYPE(DATA_TYPE)> getter)
    {
        this->getter = std::move(getter);
        return *this;
    }

    template<typename DATA_TYPE>
    inline get_set<DATA_TYPE>& get_set<DATA_TYPE>::operator=(DATA_TYPE data)
    {
        this->data = setter(std::move(data));
        return *this;
    }

    template<typename DATA_TYPE>
    inline get_set<DATA_TYPE>::operator DATA_TYPE()
    {
        return getter(this->data);
    }

    template<typename DATA_TYPE>
    inline DATA_TYPE get_set<DATA_TYPE>::move()
    {
        return getter(std::move(this->data));
    }

    template<typename DATA_TYPE>
    inline DATA_TYPE get_set<DATA_TYPE>::copy()
    {
        return getter(this->data);
    }

    template<typename DATA_TYPE>
    inline DATA_TYPE& get_set<DATA_TYPE>::ref()
    {
        return this->data;
    }
}