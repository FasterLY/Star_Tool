#pragma once
#include<string.h>
#include<iostream>
namespace star {

	class byte_array {		//һ����̬����Ŀ������й��࣬����̬�����йܸ�������й���
	private:
		char* from;
		unsigned int len;
	public:
		/*
			����������ã�
				from		��������ͷָ���ַ
				len			�������鳤��
				NeedCopy	�Ƿ����鿽�����ݣ��������ͷָ��Ϊ��̬����ͷָ�룬��Ӧ����Ϊfalse
		*/
		byte_array(char* from, unsigned int len, bool NeedCopy);		//��׼���캯��
		byte_array(unsigned int len);
		byte_array(const byte_array& CopySource);					//�������캯��
		byte_array(byte_array&& MoveSource) noexcept;				//�ƶ����캯��
		template<std::size_t Size>
		byte_array(char (&from)[Size]);								//����Ĭ��������ʽת��������ֻ�������飬������ָ��
		byte_array& operator=(const byte_array& CopySource);		//������ֵ����
		byte_array& operator=(byte_array&& MoveSource) noexcept;	//�ƶ���ֵ����
		operator char* () const;									//������ʽ����ת����Σ�շ��ţ��������ָ����ָ���쳣����Ƹýӿڽ���Ϊ�����¼��ݣ����������ʹ�ã�
		char& operator [](unsigned int index);						//����ʹ�÷���
		byte_array();												//�����鹹�캯��
		~byte_array();
		virtual unsigned int getLength();							//ȡ�����鳤��
		char* getbytes();											//ȡ������ͷָ��
	};

	class byte_builder {
	private:
		class byte_element {	//������Ԫ��
		public:
			char* from;
			unsigned int len;
			byte_element* next;
			byte_element(char* from, unsigned int len, bool NeedCopy);  //������Ĳ������Ƕ�̬����ʱ�Ž�NeedCopy��Ϊfalse������̬���齻��Ԫ����
			byte_element(const char* from, unsigned int len);  //������Ĳ������Ƕ�̬����ʱ�Ž�NeedCopy��Ϊfalse������̬���齻��Ԫ����
			byte_element();	//�����յ�Ԫ��һ�������byte_builder��������ͷʱʹ��
			~byte_element();	//��������
		};
		byte_element* head;
		byte_element* tail;
		unsigned int len;		//�����ܳ���
		void Recursive_Clear(byte_element* deleteFrom);//�ݹ������������
		void Recursive_Collect(byte_element* CopyFrom, char* Collection, int offset);	//�ݹ�����̬����
	public:
		byte_builder();
		byte_builder(char* from, unsigned int len, bool NeedCopy);
		~byte_builder();
		void pusb_back(const char* from, unsigned int len);
		void pusb_back(char* from, unsigned int len, bool NeedCopy);
		byte_array getArray();		//��ȡ����
		unsigned int getLength();		//��ȡ������ܳ���
		void collect();			//����̬����ָ��
		void clear();			//�������
	};

	
}
