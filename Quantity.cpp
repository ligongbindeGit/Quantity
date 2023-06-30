#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <new>
//----===
//一包类型
namespace Ligongbinde {
	typedef short unsigned  Int; //正整型
	struct Q;
}
namespace L = Ligongbinde;
//----===
//一包语标
namespace Factory {
	L::Q& mulQ(L::Q const& vA, L::Q const& vB);
	L::Q& createQ(char const* const pch);
}
namespace F = Factory;
//----===
template<typename _tp>
const int CalcDigit() {
	_tp Max = ~~~~~~~_tp();
	if (Max < 0) {//999999999 （4+5=9个9）
		Max = (~(1 << (sizeof(_tp) * 8 - 1)));
	}
	_tp R = 1;
	_tp V = 0;
	int n = 0;
	do {
		V = 9 * R + V;
		if ((V % 9) != 0) break;
		R = R * 10;
		n++;
	} while (V < Max);
	return n;
}
//----===
template<typename _tp>
const _tp CalcValue() {
	_tp R = 1;
	_tp V = 0;
	int n = CalcDigit<_tp>() / 2;
	while (n > 0) {
		V = 9 * R + V;
		R = R * 10;
		n--;
	}
	return V;
};
//Ligongbinde
//一包类型，类标，语型，语标。
//标语句一。
namespace Ligongbinde {
	typedef Int data_type;
	typedef int index_type;
	int  const  TotalDigit = CalcDigit<data_type>() / 2;
	data_type const MaxValue = CalcValue<data_type>();//9999;
	data_type const Radix = CalcValue<data_type>() + 1;
}
//----===
struct Ligongbinde::Q {
private:
	Q(); ~Q(); Q(Q const&);
	Q& operator=(Q const&);

	L::index_type alloc_size;
	L::index_type data_size;
	L::data_type   data_neg;
	L::data_type   data[4 + 3 + 3];
public:
	int datasize() const {
		return data_size;
	}
	int allocsize() const {
		return alloc_size;
	}
	bool  isneg() const {
		return !(data_neg == 0);
	}
	L::data_type& operator[](index_type const& idx) {
		return const_cast<L::data_type&>(__dataref_(idx));
	}
	L::data_type const& operator[](index_type const& idx) const {
		return __dataref_(idx);
	}
	L::data_type const& __dataref_(index_type const& idx)const {
		return data[idx];
	}

	void zero() {
		data_neg = 0;
		data_size = 0;
		for (L::data_type* it = begin(); it < end(); it++) {
			*it = L::data_type();
		}
	}

private:
	L::data_type* begin() {
		return &data[0];
	}
	L::data_type* end() {
		return &data[alloc_size];
	}
public:
	void destory() {
		::delete[](char*)this;
	}

private:
	static
		struct Q* allocQ(int size) {
		struct Q* q = (struct Q*)new char[
			+sizeof(L::index_type) + sizeof(L::index_type) + sizeof(L::data_type) + sizeof(L::data_type) * size
		];
		(*q).alloc_size = size;
		return q;
	}
	friend L::Q& Factory::createQ(char const* const pch);
	friend L::Q& Factory::mulQ(L::Q const& vA, L::Q const& vB);

};

//----===
namespace Factory {
	L::Q& createQ(char const* const pch) {
		// pd
		//    987654321
		//                     pe;

		char const* pd = pch;
		char const* pe = pch;
		if (*pd == '+' || *pd == '-')pd++;
		while (*pe)pe++;

		int bytes = (int)(pe - pd);
		int size = (bytes / L::TotalDigit) + (bytes % L::TotalDigit > 0);
		L::Q& vQ = *L::Q::allocQ(size);
		vQ.zero();
		int i = 0;
		while (pe > pd) {

			L::data_type V = int();
			L::data_type R = 1;
			int n = L::TotalDigit;
			while (pe > pd && n > 0) {
				V = ((--pe)[int()] - '0') * R + V;
				R = R * 10;
				--n;
			}
			vQ[i] = V;
			i++;
		}
		vQ.data_size = i;
		vQ.data_neg = (*pch == '-');

		return vQ;
	};
	//----===
	L::Q& createQ(std::string const& str) {
		return createQ(str.c_str());
	}
	//----===
	L::Q& mulQ(L::Q const& vA, L::Q const& vB) {

		L::Q& vC = *L::Q::allocQ(vA.datasize() + vB.datasize());
		vC.zero();
		for (int iA = 0; iA < vA.datasize(); iA++) {
			for (int iB = 0; iB < vB.datasize(); iB++) {
				vC[iA + iB] += vA[iA] * vB[iB];
			}

			for (int iC = iA + 0; iC < iA + vB.datasize() && iC < vC.allocsize(); iC++) {
				L::data_type high = vC[iC] / L::Radix;
				L::data_type low = vC[iC] % L::Radix;
				vC[iC] = low;
				vC[iC + 1] += high;
			}

		}
		int iC = vC.allocsize();
		while (vC[iC - 1] == 0)iC--;
		vC.data_size = iC;
		vC.data_neg = ((int)vA.isneg() + (int)vB.isneg()) & 1;
		return vC;
	}

}
//----===
L::Q& operator *(L::Q const& a, L::Q const& b) {
	return Factory::mulQ(a, b);
}
//----===
std::ostream& operator <<(std::ostream& O, L::Q const& vQ) {
	L::index_type  i = vQ.datasize() - 1;
	if (vQ.isneg())
		O << '-';

	O << (unsigned int)vQ[i];
	while (--i >= 0) {
		O << std::setw(L::TotalDigit) << std::setfill('0') << (unsigned int)vQ[i];
	}
	return O;
}
//----===
 
/*
    录入2个量 ,计算 他们的乘值,例子:
		115792089237316195423570985008687907853269984665640564039457584007913129639936
	 *    115792089237316195423570985008687907853269984665640564039457584007913129639936
	=
		13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084096
*/
void  Suan()
{
	std::stringstream  ostrA, ostrB, ostrC;
	std::string  strA, strB;

	std::cin >> strA >> strB;

	L::Q& A = Factory::createQ(strA);
	L::Q& B = Factory::createQ(strB);
	L::Q& C = A * B;

	ostrA << A;
	ostrB << B;
	ostrC << C;

	std::cout << std::endl;
	std::cout
		<< " " << std::setw(ostrC.str().length()) << std::setfill(' ') << ostrA.str() << std::endl
		<< " " << std::setw(ostrC.str().length() - ostrB.str().length()) << std::setfill(' ') << "×" << ostrB.str() << std::endl
		<< " " << std::setw(ostrC.str().length()) << std::setfill('-') << "-" << std::endl
		<< "=" << std::setw(ostrC.str().length()) << std::setfill(' ') << ostrC.str() << std::endl;

	A.destory();
	B.destory();
	C.destory();
}
int  main() {
	std::cout << "录入2个量, 例子: 4 4" << std::endl;
	while (1 + 1 == 2) { Suan(); std::cout << "录入2个量" << std::endl; }
}