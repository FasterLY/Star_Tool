#pragma once
#include<functional>
namespace star {
    template<typename DATA_TYPE>
    class get_set
    {
    public:
        get_set(DATA_TYPE& data);
        get_set(DATA_TYPE& data, std::function<DATA_TYPE(DATA_TYPE)> getter, std::function<DATA_TYPE(DATA_TYPE)> setter);
        get_set(const get_set<DATA_TYPE>& CopySource) = delete;
        get_set(get_set<DATA_TYPE>&& MoveSource) = delete;
        ~get_set();
        get_set<DATA_TYPE>& change_setter(std::function<DATA_TYPE(DATA_TYPE)> setter);    //�޸�setter����ʽ����ע����
        get_set<DATA_TYPE>& change_getter(std::function<DATA_TYPE(DATA_TYPE)> getter);    //�޸�getter����ʽ����ע����
        get_set& operator =(DATA_TYPE data);
        operator DATA_TYPE();
        DATA_TYPE move();
        DATA_TYPE copy();
        DATA_TYPE& ref();       //����Σ�շ��������޸�����ʱ���Թ�setter����������Ӧ�������ڵ���DATA_TYPE�ĺ�����
        using type = DATA_TYPE;
    private:
        DATA_TYPE& data;
        std::function<DATA_TYPE(DATA_TYPE)> getter;
        std::function<DATA_TYPE(DATA_TYPE)> setter;
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