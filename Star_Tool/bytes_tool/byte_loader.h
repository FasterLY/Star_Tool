#pragma once


#include"byte_array.h"
#include<string.h>


/*
* 该类一般用于网络字节流协议的传输装载，只有当装载满后才能将数组取出整体处理
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
			输入参数说明：
				Resource	导入数组的头指针
				Load_len	导入数组的长度
				off			导入数组的偏移量
		*/
		unsigned int load(char* Resource, unsigned int Load_len, unsigned int off);	//返回载入成功的大小
		byte_array getArray();												//返回数组的托管类，只有在装载满时才会
		bool isFull();														//装载类是否装满
		unsigned int getSize();												//得到装载类总大小
		unsigned int getLoaded();												//得到当前已经装载的大小
	};


}
