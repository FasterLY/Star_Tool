#pragma once


#include"byte_array.h"
#include<string.h>


/*
* ����һ�����������ֽ���Э��Ĵ���װ�أ�ֻ�е�װ��������ܽ�����ȡ�����崦��
*/

namespace star {
	class byte_loader {
	private:
		char* from, * loading;
		unsigned int size, loaded_size;
	public:
		byte_loader(unsigned int size);
		~byte_loader();
		/*
			�������˵����
				Resource	���������ͷָ��
				Load_len	��������ĳ���
				off			���������ƫ����
		*/
		unsigned int load(char* Resource, unsigned int Load_len, unsigned int off);	//��������ɹ��Ĵ�С
		byte_array getArray();												//����������й��ֻ࣬����װ����ʱ�Ż�
		bool isFull();														//װ�����Ƿ�װ��
		unsigned int getSize();												//�õ�װ�����ܴ�С
		unsigned int getLoaded();												//�õ���ǰ�Ѿ�װ�صĴ�С
	};


}
