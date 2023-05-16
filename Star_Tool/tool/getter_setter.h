#pragma once
#include<functional>
namespace star {
    template<typename DATA_TYPE>
    class getter_setter
    {
    public:
        getter_setter();
        getter_setter(DATA_TYPE data);
        template<typename...Args>
        getter_setter(Args...args);
        getter_setter(DATA_TYPE data, std::function<DATA_TYPE(DATA_TYPE)> getter, std::function<DATA_TYPE(DATA_TYPE)> setter);
        getter_setter(std::function<DATA_TYPE(DATA_TYPE)> getter, std::function<DATA_TYPE(DATA_TYPE)> setter);
        getter_setter(const getter_setter<DATA_TYPE>& CopySource);
        getter_setter(getter_setter<DATA_TYPE>&& MoveSource);
        ~getter_setter();
        getter_setter<DATA_TYPE>& change_setter(std::function<DATA_TYPE(DATA_TYPE)> setter);    //修改setter，流式依赖注入器
        getter_setter<DATA_TYPE>& change_getter(std::function<DATA_TYPE(DATA_TYPE)> getter);    //修改getter，流式依赖注入器
        getter_setter& operator =(DATA_TYPE data);
        operator DATA_TYPE();
        DATA_TYPE move();
        DATA_TYPE copy();
        DATA_TYPE& ref();       //！！危险方法，在修改引用时会略过setter过滤器，这应仅仅用于调用DATA_TYPE的函数。
        using type = DATA_TYPE;
    private:
        DATA_TYPE data;
        std::function<DATA_TYPE(DATA_TYPE)> getter;
        std::function<DATA_TYPE(DATA_TYPE)> setter;
    };

    template<typename DATA_TYPE>
    inline getter_setter<DATA_TYPE>::getter_setter() :data{}
    {
        this->getter = [](DATA_TYPE data) {
            return std::move(data);
        };
        this->setter = [](DATA_TYPE data) {
            return std::move(data);
        };
    }

    template<typename DATA_TYPE>
    inline getter_setter<DATA_TYPE>::getter_setter(DATA_TYPE data) :data(std::move(data))
    {
        this->getter = [](DATA_TYPE data) {
            return std::move(data);
        };
        this->setter = [](DATA_TYPE data) {
            return std::move(data);
        };
    }

    template<typename DATA_TYPE>
    template<typename ...Args>
    inline getter_setter<DATA_TYPE>::getter_setter(Args ...args) : data(std::forward<Args>(args)...)
    {
        this->getter = [](DATA_TYPE data) {
            return std::move(data);
        };
        this->setter = [](DATA_TYPE data) {
            return std::move(data);
        };
    }

    template<typename DATA_TYPE>
    inline getter_setter<DATA_TYPE>::getter_setter(DATA_TYPE data, std::function<DATA_TYPE(DATA_TYPE)> getter, std::function<DATA_TYPE(DATA_TYPE)> setter)
        : data(std::move(data)), getter(getter), setter(setter) {}

    template<typename DATA_TYPE>
    inline getter_setter<DATA_TYPE>::getter_setter(std::function<DATA_TYPE(DATA_TYPE)> getter, std::function<DATA_TYPE(DATA_TYPE)> setter)
        : data{}, getter(getter), setter(setter) {}

    template<typename DATA_TYPE>
    inline getter_setter<DATA_TYPE>::getter_setter(const getter_setter<DATA_TYPE>& CopySource)
        : data(CopySource.data), getter(CopySource.getter), setter(CopySource.setter) {}

    template<typename DATA_TYPE>
    inline getter_setter<DATA_TYPE>::getter_setter(getter_setter<DATA_TYPE>&& MoveSource)
        : data(std::move(MoveSource.data)), getter(MoveSource.getter), setter(MoveSource.setter) {}

    template<typename DATA_TYPE>
    inline getter_setter<DATA_TYPE>::~getter_setter()
    {

    }

    template<typename DATA_TYPE>
    inline getter_setter<DATA_TYPE>& getter_setter<DATA_TYPE>::change_setter(std::function<DATA_TYPE(DATA_TYPE)> setter)
    {
        this->setter = std::move(setter);
        return *this;
    }

    template<typename DATA_TYPE>
    inline getter_setter<DATA_TYPE>& getter_setter<DATA_TYPE>::change_getter(std::function<DATA_TYPE(DATA_TYPE)> getter)
    {
        this->getter = std::move(getter);
        return *this;
    }

    template<typename DATA_TYPE>
    inline getter_setter<DATA_TYPE>& getter_setter<DATA_TYPE>::operator=(DATA_TYPE data)
    {
        this->data = setter(std::move(data));
        return *this;
    }

    template<typename DATA_TYPE>
    inline getter_setter<DATA_TYPE>::operator DATA_TYPE()
    {
        return getter(this->data);
    }

    template<typename DATA_TYPE>
    inline DATA_TYPE getter_setter<DATA_TYPE>::move()
    {
        return getter(std::move(this->data));
    }

    template<typename DATA_TYPE>
    inline DATA_TYPE getter_setter<DATA_TYPE>::copy()
    {
        return getter(this->data);
    }

    template<typename DATA_TYPE>
    inline DATA_TYPE& getter_setter<DATA_TYPE>::ref()
    {
        return this->data;
    }
}