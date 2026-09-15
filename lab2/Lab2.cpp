//=======================================================================
//	Лабораторная №3. Шаблоны функций. Шаблоны классов. Стандартные шаблоны С++.
//				Обработка исключений.
//=======================================================================
//Используйте недостающие файлы из лабораторной 2
#include "MyVector.h"
#include "MyString.h"
#include <iostream>	
#include <vector>	
#include <list>	
#include <algorithm>

using namespace std;

//============= Шаблон функции для вывода с помощью итератора

template <class T> void pr(T& v, string s)
{
	cout<<"\n\n\t"<<s<<"  # Sequence:\n";
	
	// Итератор любого контейнера
	typename T::iterator p;
	int i;

	for (p = v.begin(), i=0;  p != v.end();  p++, i++)
		cout << endl << i+1 <<". "<< *p;
	cout << '\n';
}

template <typename T>
void Swap(T& a, T& b) {
	T temporary_variable;
	temporary_variable = a;
	a = b;
	b = temporary_variable;
}

class StackOverflow {};
class StackUnderflow {};
class StackOutOfRange {
		int index;
	public:
		StackOutOfRange(int bad_index) {
			index = bad_index;
		}

		void Out() {
			cout << "OutOfRange. Bad index - " << index << endl;
		}
};


template <typename T, int size>
class MyStack {
		int capacity;
		T* stack;
	public:
		void Push(T element) {
			if (capacity < size) {
				stack[capacity] = element;
				capacity += 1;
			}
			else {
				throw StackOverflow();
			}
		}

		T Pop() {
			if (capacity > 0) {
				capacity -= 1;
				return stack[capacity];
			}
			else { throw StackUnderflow(); }
		}

		int GetSize() { return size; }
		int Capacity() { return capacity; }

		operator[] (int index) {
			if (index < capacity) {
				return stack[index];
			}
			else {
				throw StackOutOfRange(index);
			}
		}

		MyStack() {
			capacity = 0;
			stack = new T[size];
		}

		~MyStack() {
			delete[] stack;
		}
};

int main()
{

	//===========================================================
	// Шаблоны функций
	//===========================================================
	// Создайте шаблон функции перестановки двух параметров - Swap().
	// Проверьте работоспособность созданного шаблона с помощью
	// приведенного ниже фрагмента.
	{
		int i = 1, j = -1;
		cout << "Before Swap: i - " << i << "; j - " << j << endl;
		Swap (i, j);
		cout << "After Swap: i - " << i << "; j - " << j << endl;
		cout << '\n';

		double a = 0.5, b = -5.5;
		cout << "Before Swap: a - " << a << "; b - " << b << endl;
		Swap (a, b);
		cout << "After Swap: a - " << a << "; b - " << b << endl;
		cout << '\n';

		Vector u(1,2), w(-3,-4);
		cout << "Before Swap: u =>" << endl; u.print(); cout << " w => " << endl; w.print();
		Swap(u, w);
		cout << "After Swap: u =>" << endl; u.print(); cout << " w => " << endl; w.print();
		cout << '\n';
		
		MyString s1 ("Your fault"), s2 ("My forgiveness");
		cout << "Before Swap: s1 =>" << endl; s1.print(); cout << " s2 => " << endl; s2.print();
		Swap (s1, s2);
		cout << "After Swap: s1 =>" << endl; s1.print(); cout << " s2 => " << endl; s2.print();
		cout << '\n';
	}
	//===========================================================
	// Шаблоны классов
	//===========================================================
	// Создайте шаблон класса MyStack для хранения элементов любого типа T.
	// В качестве основы для стека может быть выбран массив.
	// Для задания максимального размера стека может быть использован
	// параметр-константа шаблона
	// Обязательными операциями со стеком являются "Push" и "Pop","GetSize" и "Capacity"
	// Необязательной - может быть выбор по индексу (operator[]).
	// Для того, чтобы гарантировать корректное выполнение этих операций 
	// следует генерировать исключительные ситуации.
	
	// С помощью шаблона MyStack создайте стек переменных типа int, затем
	// стек переменных типа double и, наконец, стек из переменных типа Vector 
	// Если вы подготовите три класса для обработки исключений,
	// то следующий фрагмент должен работать
	try
	{
		cout << "\tTest MyStack\n";
		MyStack <int, 3> stack;

		cout << "\nInteger Stack capacity: " << stack.GetSize();

		stack.Push(1);
		stack.Push(2);
		stack.Push(3);
		
		cout << "\nInteger Stack has: " << stack.Capacity() << " elements";

		//stack.Push(4);	// Здесь должно быть "выброшено" исключение

		cout << "\nInteger Stack pops: " << stack.Pop();
		cout << "\nInteger Stack pops: " << stack.Pop();
		
		cout << "\nInteger Stack has: " << stack.Capacity() << " elements";
		stack.Pop();
		//stack.Pop();		// Здесь должно быть "выброшено" исключение
		stack.Push(2);
		
		//int i = stack[3];	// Здесь должно быть "выброшено" исключение
		
		MyStack<Vector, 5> ptStack;
		
		cout << "\nVector Stack capacity: " << ptStack.GetSize();
		
		ptStack.Push(Vector(1,1));
		ptStack.Push(Vector(2,2));
		
		cout << "\nVector Stack pops: ";
		// Используйте метод класса Vector для вывода элемента
		ptStack.Pop().Out();
		
		cout << "\nVector Stack has: " << ptStack.Capacity() << " elements";
	}
	catch (StackOverflow)
	{
		cout << "\nStack overflow";
	}
	catch (StackUnderflow)
	{
		cout << "\nStack underflow";
	}
	catch (StackOutOfRange o)
	{
		o.Out();
	}

	//=======================================================================
	// Контейнеры стандартной библиотеки. Последовательности типа vector
	//=======================================================================
	
	// Создайте пустой вектор целых чисел. Узнайте его размер с помощью метода size(),
	// С помощью метода push_back() заполните вектор какими-либо значениями.
	// Получите новый размер вектора и выведите значения его элементов.
	// В процессе работы с вектором вы можете кроме количества реально заполненных
	// элементов (size()) узнать максимально возможное количество элементов (max_size()),
	// а также зарезервированную память (capacity()).

	vector<int> v;
	int n = v.size();
	cout << "\n\nN (size): " << n << endl;
	v.push_back(-1);
	v.push_back(-2);
	for (int x : v) {
		cout << x << " ";
	}
	cout << '\n';

	v.push_back(-5);
	v.push_back(-7);
	n = v.size();
	cout << "N (size): " << n << endl;
	n = v.capacity();
	cout << "N (capacity): " << n << endl;
	n = v.max_size();
	cout << "N (max_size): " << n << endl;
		
	// Так как мы часто будем выводить последовательности, то целесообразно
	// создать шаблон функции для вывода любого контейнера.
	// Проанализируйте коды такого шабдлона (pr), который приведен выше
	// Используйте его для вывода вашего вектора

	pr (v, "Vector of ints");
	
	// Используем другой конструктор для создания вектора вещественных
	// с начальным размером в 2 элемента и заполнением (222.).
	// Проверим параметры вектора. Затем изменим размер вектора и его заполнение
	// (метод - resize()) и вновь проверим параметры.
	
	vector<double> vd(2, 222.0);
	pr (vd, "Vector of doubles");
	n = vd.size();
	cout << "\n\nN (size): " << n << endl;
	n = vd.capacity();
	cout << "N (capacity): " << n << endl;
	n = vd.max_size();
	cout << "N (max_size): " << n << endl;

	vd.resize(5, 333.3);
	pr (vd, "After resize");
	n = vd.size();
	cout << "N (size): " << n << endl;
	n = vd.capacity();
	cout << "N (capacity): " << n << endl;
	n = vd.max_size();
	cout << "N (max_size): " << n << endl;
	
	// Используя метод at(), а также операцию выбора [], измените значения
	// некоторых элементов вектора и проверьте результат.
	vd.at(1) = 99.;
	pr (vd, "After at");

	vd[1] = 999.;
	pr (vd, "After []");
	
	// Создайте вектор вещественных, который является копией существующего.
	vector<double> wd(vd);
	pr (wd, "Copy");
	
	// Создайте вектор, который копирует часть существующей последовательности
	vector<double> ud(vd.begin(), vd.begin() + 2);
	pr (ud, "Copy part");

	// Создайте вектор вещественных, который является копией части обычного массива.
	double ar[] = { 0., 1., 2., 3., 4., 5. };

	vector<double> va1(begin(ar), begin(ar) + 2);
	pr (va1, "Copy part of array");

	vector<double> va2(ar, ar + 2);
	pr (va2, "Copy part of array");
	
	// Создайте вектор символов, который является копией части обычной строки
	char s[] = "Array is a succession of chars";
	
	vector<char> vc(s + 2, s + 5); 
	pr (vc, "Copy part of c-style string");

	// Создайте вектор элементов типа Vector и инициализируйте
	// его вектором с координатами (1,1).
	vector<Vector> vv(2, Vector(1, 1)); 
	
	cout << "\n\nvector of Vectors\n";
	for (int i=0;  i < vv.size();  i++)
		vv[i].Out();

	// Создайте вектор указателей на Vector и инициализируйте его адресами
	// объектов класса Vector
	Vector* shared_vec1 = new Vector(1, 1);
	Vector* shared_vec2 = new Vector(1, 5);
	Vector* shared_vec3 = new Vector(4, 2);
	Vector* shared_vec4 = new Vector(3, 2);
	Vector* shared_vec5 = new Vector(8, 9);
	Vector* shared_vec6 = new Vector(3, 3);
	vector<Vector*> vp(2, shared_vec1);
	vector<Vector*> vp_for_copy(2, shared_vec1);
	
	cout << "\n\nvector of pointers to Vector\n";
	
	for (int i=0;  i < vp.size();  i++)
		vp[i]->Out();

	// Научитесь пользоваться методом assign и операцией
	// присваивания = для контейнеров типа vector.
	vp.assign({shared_vec1}); 
	vp_for_copy = {shared_vec2, shared_vec3, shared_vec4};

	cout << "\n\nAfter assign\n";
	for (int i=0;  i < vp.size();  i++)
		vp[i]->Out();
	
	vp.assign(vp_for_copy.begin(), vp_for_copy.begin() + 2);

	cout << "\n\nAfter assign\n";
	for (int i=0;  i < vp.size();  i++)
		vp[i]->Out();

	vp.assign({shared_vec5, shared_vec6}); 

	cout << "\n\nAfter assign\n";
	for (int i=0;  i < vp.size();  i++)
		vp[i]->Out();


	// Декларируйте новый вектор указателей на Vector и инициализируйте его 
	// с помощью второй версии assign
	//vpNew.assign 
	
	vector<Vector*> vpNew;


	vpNew.assign(vp.begin(), vp.end());

	cout << "\n\nNew vector after assign\n";
	for (size_t i = 0; i < vpNew.size(); i++) {
		vpNew[i]->Out();
		cout << " ";
	}
	cout << endl;	

	delete shared_vec1;
	delete shared_vec2;
	delete shared_vec3;
	delete shared_vec4;
	delete shared_vec5;
	delete shared_vec6;


	// На базе шаблона vector создание двухмерный массив и
	// заполните его значениями разными способами.
	// Первый вариант - прямоугольная матрица
	// Второй вариант - ступенчатая матрица

	//========= Прямоугольная матрица
	vector<vector<double>> matrix(3, vector<double>(6));
	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix[i].size(); j++) {
			matrix[i][j] = i * j;
		}
	}
	cout << "\n\n\tTest vector of vector<double>\n";
	for (int i=0;  i < matrix.size();  i++)
	{
		cout << endl;
		for (int j=0;  j < matrix[i].size();  j++)
			cout << matrix[i][j] << "  ";
	}	

	
	//========= Ступенчатая матрица
	vector<vector<double>> vdd(5, vector<double>(5));
	for (int i=0;  i < vdd.size();  i++)
		vdd[i] = vector<double>(i+1, double(i));
	
	cout << "\n\n\tTest vector of vector<double>\n";
	for (int i=0;  i < vdd.size();  i++)
	{
		cout << endl;
		for (int j=0;  j < vdd[i].size();  j++)
			cout << vdd[i][j] << "  ";
	}	
  

	//===================================
	// Простейшие действия с контейнерами
	//===================================
	//3б. Получение значения первого и последнего элементов последовательности.
	//Получение размера последовательности. Присваивание значений
	//элементов одной последовательности элементам другой - assign().

	//Создайте и проинициализируйте вектор из элементов char. Размер -
	//по желанию.
	vector<char> vchar({'v', 's', 'a', 'w'});

	//Получите значение первого элемента вектора ( front() )
	cout << "Front(): " << vchar.front() << endl;

	//Получите значение последнего элемента вектора ( back() )
	cout << "Back(): " << vchar.back() << endl;

	//Получите размер вектора
	cout << "Size: " << vchar.size() << endl;

	//Присвойте вектору любой диапазон из значений массива cMas.
	char cMas[] = {'c', 'd', 'a', 'y', 'g', 'p', 'v'};
	vchar.assign(cMas + 1, cMas + 4);
	pr(vchar, "Assigned range from cMas");

	//Проверьте размер вектора, первый и последний элементы.
	cout << "Size: " << vchar.size() << endl;
	cout << "Front: " << vchar.front() << endl;
	cout << "Back: " << vchar.back() << endl;

	//3в. Доступ к произвольным элементам вектора с проверкой - at()
	//и без проверки - []
	//Создайте неинициализированный вектор из 8 элементов char - vChar2.
	//С помощью at() присвойте четным элементам вектора значения
	//элементов vChar1 из предыдущего задания,
	//а с помощью [] присвойте нечетным элементам вектора vChar2 значения
	//массива {'K','U','K','U'}.

	vector<char> vChar2(8, ' ');
	vChar2[0] = vchar.at(0);
	vChar2[1] = 'K'; 
	vChar2[2] = vchar.at(2);
	vChar2[3] = 'U';
	vChar2[5] = 'K';
	vChar2[7] = 'U';

	pr(vChar2, "Playing around with [] and at()");
	
	//Попробуйте "выйти" за границы вектора с помощью at() и
	//с помощью []. Обратите внимание: что происходит при
	//попытке обращения к несуществующему элементу в обоих случаях
	// vChar2[6] = vchar.at(15); // terminate called after throwing an instance of 'std::out_of_range'
  	// what():  vector::_M_range_check: __n (which is 15) >= this->size() (which is 3)
	// vChar2[6] = vchar[15]; // Ошибка не вылазит, UB
  
	//3г.Добавьте в конец вектора vChar2  - букву Z (push_back()). Для
	//расширения кругозора можете ее сразу же и выкинуть (pop_back())

	vChar2.push_back('Z');
	pr(vChar2, "Push back \'z\'");
	vChar2.pop_back();
	pr(vChar2, "Pop back");

	//3д. Вставка-удаление элемента последовательности insert() - erase()
	//Очистка последовательности - clear()

	//Вставьте перед каждым элементом вектора vChar2 букву 'W'
	for(int i = vChar2.size(); i >= 0; i--) {
		vChar2.insert(vChar2.begin() + i, 'W');
	}
	pr(vChar2, "Added W");

	//Вставьте перед 5-ым элементом вектора vChar2 3 буквы 'X'
	vChar2.insert(vChar2.begin() + 5, 3, 'X');
	pr(vChar2, "Added X");

	//Вставьте перед 2-ым элементом вектора vChar2 с третьего по
	//шестой элементы массива "aaabbbccc"
	char ar_vChar2[] = "aaabbbccc";
	vChar2.insert(vChar2.begin() + 2, ar_vChar2 + 2, ar_vChar2 + 6);
	pr(vChar2, "Added ar_vChar2");
  
	//Сотрите c первого по десятый элементы vChar2
	vChar2.erase(vChar2.begin() + 1, vChar2.begin() + 11);
	pr(vChar2, "Erace method");

	//Уничтожьте все элементы последовательности - clear()
	vChar2.clear();
	pr(vChar2, "Clear method");

// ///////////////////////////////////////////////////////////////////
// /*
// 	//Задание 4. Списки. Операции, характерные для списков.
// 	//Создайте два пустых списка из элементов Vector - ptList1 и
// 	//ptList2
	list<Vector> ptList1;
	list<Vector> ptList2;

// 	//Наполните оба списка значениями с помощью методов push_back(),
// 	//push_front, insrert()

	Vector vec1(1, 1);
	Vector vec2(1, 5);
	Vector vec3(4, 2);
	Vector vec4(3, 2);
	Vector vec5(8, 9);
	Vector vec6(3, 3);

	ptList1.insert(ptList1.begin(), vec3); 
    ptList2.insert(ptList2.begin(), vec4);

    ptList1.insert(ptList1.begin(), vec1); 
    ptList2.insert(ptList2.begin(), vec2);

    ptList1.push_back(vec5); 
    ptList2.push_back(vec6);

	pr(ptList1, "List1 (not sort): ");
	pr(ptList2, "List2 (not sort): ");

	//Отсортируйте списки - sort().
	//Подсказка: для того, чтобы работала сортировка, в классе Vector
	//должен быть переопределен оператор "<"

	ptList1.sort();
	ptList2.sort();

	pr(ptList1, "List1 (sort): ");
	pr(ptList2, "List2 (sort): ");

	//Объедините отсортированные списки - merge(). Посмотрите: что
	//при этом происходит со вторым списком.

	ptList1.merge(ptList2);
	pr(ptList1, "List1 (after merge): ");
	pr(ptList2, "List2 (after merge): ");

	//Исключение элемента из списка - remove()
	//Исключите из списка элемент с определенным значением.
	//Подсказка: для этого необходимо также переопределить
	//в классе Vector оператор "=="
	ptList1.remove(vec1);
	pr(ptList1, "List1 (after remove (1,1)): ");

// */
///////////////////////////////////////////////////////////////////

	//Задание 5. Стандартные алгоритмы.Подключите заголовочный файл
	// <algorithm>
	//5а. Выведите на экран элементы ptList1 из предыдущего
	//задания с помощью алгоритма for_each()

	for_each(ptList1.begin(), ptList1.end(), [](const Vector& x) {
		cout << x << '\n';
	});

	//5б.С помощью алгоритма find() найдите итератор на элемент Vector с
	//определенным значением. С помощью алгоритма find_if() найдите
	//итератор на элемент, удовлетворяющий определенному условию, 
	//например, обе координаты точки должны быть больше 2.
	//Подсказка: напишите функцию-предикат, которая проверяет условие
	//и возвращает boolean-значение (предикат может быть как глобальной
	//функцией, так и методом класса)

	Vector vec_to_find(1, 5);
	auto it_find = find(ptList1.begin(), ptList1.end(), vec_to_find);

	if (it_find != ptList1.end()) {
		cout << "Result: " << *it_find << endl;
	}
	else {
		cout << "No result" << endl;
	}


	//Создайте список из указателей на элеметы Vector. С помощью 
	//алгоритма find_if() и предиката (можно использовать предикат - 
	//метод класса Vector, определенный в предыдущем задании) найдите в
	//последовательности элемент, удовлетворяющий условию

	auto it_find_if = find_if(ptList1.begin(), ptList1.end(), [](const Vector& v) {
		return v.getX() > 2 && v.getY() > 2;
	});

	if (it_find_if != ptList1.end()) {
		cout << "Result (with the condition): " << *it_find << endl;
	}
	else {
		cout << "No result" << endl;
	}
	
	///

	list<Vector*> ptrList;

	Vector v1(5, 5), v2(-1, -1), v3(3, 4);
	ptrList.push_back(&v1);
	ptrList.push_back(&v2);
	ptrList.push_back(&v3);

	auto it_ptr = find_if(ptrList.begin(), ptrList.end(), [](const Vector* v) {
		return v->getX() > 0 && v->getY() > 0;
	});

	if (it_ptr != ptrList.end()) {
		cout << "Found by index: " << **it_ptr << '\n';
	}

	//5в. Создайте список элементов Vector. Наполните список
	//значениями. С помощью алгоритма replace() замените элемент
	//с определенным значением новым значением. С помощью алгоритма
	//replace_if() замените элемент, удовлетворяющий какому-либо
	//условию на определенное значение. Подсказка: условие
	//задается предикатом.


 	list<Vector> repList = { Vector(1, 1), Vector(2, 2), Vector(1, 1), Vector(3, 3) };

	cout << "before replace:\n";
	for (const auto& v : repList) cout << v << " ";
	cout << '\n';

	replace(repList.begin(), repList.end(), Vector(1, 1), Vector(9, 9));

	replace_if(repList.begin(), repList.end(), [](const Vector& v) {
		return v.getX() == 2;
	}, Vector(8, 8));

	cout << "After replace and replace_if:\n";
	for (const auto& v : repList) cout << v << " ";
	cout << '\n';

  


	//5г. Создайте вектор строк (string). С помощью алгоритма count()
	//сосчитайте количество одинаковых строк. С помощью алгоритма
	//count_if() сосчитайте количество строк, начинающихся с заданной
	//буквы

	vector<string> words = { "apple", "apricot", "banana", "apple", "cherry", "avocado" };

	int count_apple = count(words.begin(), words.end(), "apple");
	cout << "The word 'apple' appears: " << count_apple << "\n";

	int count_a = count_if(words.begin(), words.end(), [](const string& s) {
		return !s.empty() && s.front() == 'a';
	});
	cout << "Words starting with 'a': " << count_a << '\n';

	//5д. С помощью алгоритма count_if() сосчитайте количество строк,
	//которые совпадают с заданной строкой. Подсказка: смотри тему
	//объекты-функции


	struct StringMatcher {
		string target;
		
		StringMatcher(string t) : target(t) {}
		
		bool operator()(const string& s) const {
			return s == target;
		}
	};

	int count_matcher = count_if(words.begin(), words.end(), StringMatcher("apple"));

	cout << "count_if found it with a functor 'apple': " << count_matcher << "\n";

// 	cout <<"\n\n";
}