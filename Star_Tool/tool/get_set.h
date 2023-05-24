#pragma once
#include<functional>
namespace star {
    template<typename DATA_TYPE>
    class get_set
    {
    public:
        /*
        * data:Դ���ݵ�����
        * ʹ��Ĭ��������������������������
        */
        get_set(DATA_TYPE& data);
        /*
        * data:Դ���ݵ�����
        * getter: ������ȡ������
        * setter: ����ע�������
        */
        get_set(DATA_TYPE& data, std::function<DATA_TYPE(DATA_TYPE)> getter, std::function<DATA_TYPE(DATA_TYPE)> setter);
        /*
        * �޸�setter����ʽ����ע����
        * setter:�޸ĺ������ע�������
        * ����ֵ:get_set����
        */
        get_set<DATA_TYPE>& change_setter(std::function<DATA_TYPE(DATA_TYPE)> setter);
        /*
        * �޸�getter����ʽ����ע����
        * getter:�޸ĺ��������ȡ������
        * ����ֵ:get_set����
        */
        get_set<DATA_TYPE>& change_getter(std::function<DATA_TYPE(DATA_TYPE)> getter); 
        /*
        * ����=�������data�ᾭ���ƶ�(move)������setter��������ֵ����������
        */
        get_set& operator =(DATA_TYPE data);
        /*
        * ������ʽת������ʽת��Ϊ�������ͣ����ƶ�(move)������getter���˺󷵻�
        */
        operator DATA_TYPE();
        /*
        * ���Դ���ݽ���move�ƶ�����getter�������󲢷���Դ����ͨ���ƶ�����Ķ���ԭ�������ݿ����޷�����ʹ�á�
        */
        DATA_TYPE move();
        /*
        * ��Դ���ݽ���copy���ƿ����󾭹�getter�������󷵻ؿ�����������Ķ���(һ�����������ʽת�������ͬ��
        */
        DATA_TYPE copy();
        /*
        *  ����Դ���ݵ����á�
        *  !!Σ�շ��������޸�����ʱ���Թ�setter����������Ӧ�������ڵ���DATA_TYPE�ĺ�����
        */
        DATA_TYPE& ref();
        /*
        * Դ���ݵ�����
        */
        using type = DATA_TYPE;
    private:
        DATA_TYPE& data;
        std::function<DATA_TYPE(DATA_TYPE)> getter;
        std::function<DATA_TYPE(DATA_TYPE)> setter;
    public:     //����Ҫ�Ĺ��к���
        /*
        * ���������ѱ�ɾ��
        */
        get_set(const get_set<DATA_TYPE>& CopySource) = delete;
        /*
        * �ƶ����캯���ѱ�ɾ��
        */
        get_set(get_set<DATA_TYPE>&& MoveSource) = delete;
        /*
        * ������ֵ������ɾ��
        */
        get_set& operator =(const get_set<DATA_TYPE>& CopySource) = delete;
        /*
        * �ƶ���ֵ������ɾ��
        */
        get_set& operator =(get_set<DATA_TYPE>&& MoveSource) = delete;
        /*
        * Ĭ����������
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