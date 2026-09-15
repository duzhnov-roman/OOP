// Добавьте в класс все необходимые конструкторы и методы необходимые для функционирования в данной лабораторной

class MyString
{
private:
	char* m_pStr;		// Элемент данных класса (адрес строки)
public:
    MyString ();
	MyString (const char* s);	// Объявление конструктора
    ~MyString();		// Объявление деструктора

	// template <>
	// void Swap<const char*>(const char* first_string, const char* second_string) {
	// 	const char temporary_string = strcpy(first_string);
	// 	first_string = 
	// }

	void print();

	void Copy (const char* s);
	char* GetString();	// Объявление метода (accessor)
	int GetLength();	// Объявление метода (длина строки)

	MyString& operator= (MyString& v);
};
