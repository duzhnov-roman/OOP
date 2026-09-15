#include "MyString.h"

#include <iostream>	
#include <string.h>	

using namespace std;

void MyString::Copy (const char* s)
{
	delete [] m_pStr;
	// Динамически выделяем требуемое количество памяти.
	int len = strlen(s) + 1;
	m_pStr = new char[len];
	// + 1, так как нулевой байт тоже нужно скопировать
	// Если память выделена, копируем строку-аргумент в строку-член класса
	if (m_pStr)
		strncpy(m_pStr, s, len);
}

MyString::MyString() {
	m_pStr = nullptr;
}

// Определение конструктора.
MyString::MyString (const char* s)
{
	m_pStr = 0;
	Copy(s);
}

// Определение деструктора.
MyString::~MyString()
{
	// Освобождение памяти, занятой в конструкторе для строки-члена класса
	delete[] m_pStr;
}

void MyString::print() {
	cout << m_pStr << endl;
}

// Метод класса
char* MyString::GetString()
{
	return m_pStr;
}

int MyString::GetLength()
{
	return strlen(m_pStr) + 1;
}

MyString& MyString::operator= (MyString& v) {
	m_pStr = 0;
	Copy(v.GetString());
}