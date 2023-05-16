#pragma once
#include<string.h>
#include<iostream>
namespace star {

	class byte_array {		//一个动态数组的拷贝与托管类，将动态数组托管给该类进行管理
	private:
		char* from;
		unsigned int len;
	public:
		/*
			输入参数设置：
				from		传入数组头指针地址
				len			传入数组长度
				NeedCopy	是否将数组拷贝备份，若传入的头指针为动态数组头指针，则应该置为false
		*/
		byte_array(char* from, unsigned int len, bool NeedCopy);		//标准构造函数
		byte_array(unsigned int len);
		byte_array(const byte_array& CopySource);					//拷贝构造函数
		byte_array(byte_array&& MoveSource) noexcept;				//移动构造函数
		template<std::size_t Size>
		byte_array(char (&from)[Size]);								//定义默认数组隐式转化函数，只接受数组，不接受指针
		byte_array& operator=(const byte_array& CopySource);		//拷贝赋值函数
		byte_array& operator=(byte_array&& MoveSource) noexcept;	//移动赋值函数
		operator char* () const;									//定义隐式类型转化（危险符号，析构后该指针会空指针异常，设计该接口仅仅为了向下兼容，新设计请勿使用）
		char& operator [](unsigned int index);						//数组使用符号
		byte_array();												//空数组构造函数
		~byte_array();
		virtual unsigned int getLength();							//取得数组长度
		char* getbytes();											//取得数组头指针
	};

	class byte_builder {
	private:
		class byte_element {	//链表子元类
		public:
			char* from;
			unsigned int len;
			byte_element* next;
			byte_element(char* from, unsigned int len, bool NeedCopy);  //若传入的参数就是动态数组时才将NeedCopy置为false，将动态数组交给元保管
			byte_element(const char* from, unsigned int len);  //若传入的参数就是动态数组时才将NeedCopy置为false，将动态数组交给元保管
			byte_element();	//创建空单元，一般仅仅在byte_builder创建链表头时使用
			~byte_element();	//析构函数
		};
		byte_element* head;
		byte_element* tail;
		unsigned int len;		//数组总长度
		void Recursive_Clear(byte_element* deleteFrom);//递归清除链表数据
		void Recursive_Collect(byte_element* CopyFrom, char* Collection, int offset);	//递归整理动态数组
	public:
		byte_builder();
		byte_builder(char* from, unsigned int len, bool NeedCopy);
		~byte_builder();
		void pusb_back(const char* from, unsigned int len);
		void pusb_back(char* from, unsigned int len, bool NeedCopy);
		byte_array getArray();		//获取数组
		unsigned int getLength();		//获取数组的总长度
		void collect();			//整理动态数组指令
		void clear();			//清空数组
	};

	
}
