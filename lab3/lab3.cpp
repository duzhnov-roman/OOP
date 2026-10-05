// Контейнеры STL: 
//deque, stack, queue, priority_queue
//set, multiset, map, multimap
//Итераторы. Стандартные алгоритмы. Предикаты.

#include <iostream>
#include <deque>
#include <stack>
#include <queue>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <iterator>
#include <cctype>

#include <cstring>

#include "../lab2/MyString.h"
#include "../lab2/lab2.cpp"
#include "Point.h"

template <typename T>
void printElem(const T& elem) {
    cout << elem << " ";
}
// 2. Функтор для for_each (смещение координат Point на заданные dx, dy)
struct OffsetPoint {
    int dx, dy;
    OffsetPoint(int dx = 0, int dy = 0) : dx(dx), dy(dy) {}
    void operator()(Point& p) const {
        p.setX(p.getX() + dx);
        p.setY(p.getY() + dy);
    }
};
// 3. Глобальные переменные и предикат Pred1_1 для find_if
int g_n = 10;
int g_m = 50;
bool Pred1_1(const Point& p) {
    return (p.getX() >= -g_n && p.getX() <= g_m) && 
           (p.getY() >= -g_n && p.getY() <= g_m);
}
// 4. Структура прямоугольника Rect
struct Rect {
    double centerX, centerY;
    Rect(double x = 0, double y = 0) : centerX(x), centerY(y) {}
    double distSq() const {
        return centerX * centerX + centerY * centerY;
    }
    friend ostream& operator<<(ostream& os, const Rect& r) {
        os << "Rect(center: " << r.centerX << ", " << r.centerY << ")";
        return os;
    }
};
// 5. Функция перевода строки в нижний регистр для transform
string toLowerStr(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

using namespace std;	

template <typename T> void cCout(T v, string s)
{
	cout<<"\n\n\t"<<s<<"  # Sequence:\n";
	
	// Итератор любого контейнера

	while (!v.empty()) {
		cout << v.top() << " - ";
		v.pop();
	}
	cout << '\n';
}

template <typename T, typename Container> 
void cCout(queue<T, Container> v, string s)
{
    cout << "\n\n\t" << s << " # Sequence:\n";
    while (!v.empty()) {
        cout << v.front() << " - ";
        v.pop();
    }
    cout << '\n';
}

int main()
{

	//Очередь с двумя концами - контейнер deque

	//Создайте пустой deque с элементами типа Point. С помощью
	//assign заполните deque копиями элементов вектора. С помощью
	//разработанного Вами в предыдущем задании универсального шаблона
	//выведите значения элементов на печать

	deque<Point> dequePoint;
	vector<double> vec1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	dequePoint.assign(vec1.begin() + 2, vec1.begin() + 7);
	pr(dequePoint, "DequePoint Output: ");

	//Создайте deque с элементами типа MyString. Заполните его значениями
	//с помощью push_back(), push_front(), insert()
	//С помощью erase удалите из deque все элементы, в которых строчки
	//начинаются с 'A' или 'a'

	deque<MyString> dequeMyString;
	
	dequeMyString.push_back("Y.");
	dequeMyString.push_front("R. ");
	dequeMyString.insert(dequeMyString.begin() + 1, "Duzhnov");

	pr(dequeMyString, "DequeMyString Output: ");

	for (auto it = dequeMyString.begin(); it != dequeMyString.end(); ) {
		const char* s = it->GetString();
		if (s && (s[0] == 'a' || s[0] == 'A')) {
			it = dequeMyString.erase(it);
		} else {
			++it;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////


	//Напишите шаблон функции для вывода значений stack, queue, priority_queue
	//Подумайте, как "получать" данное "с верхушки"?

	/*
		Для получения значения с верхушки:
			Стек: top()
			Очередь: front()
			Приоритетная очередь: top()
	*/

	//Что происходит с контейнерами после вывода значений?
	// Он будет опустошен, если передавать контейнер не по значению, а по ссылке

	stack<int> stackCout;
	stackCout.push(2);
	stackCout.push(15);
	stackCout.push(4);

	queue<int> queueCout;
	queueCout.push(2);
	queueCout.push(15);
	queueCout.push(4);

	priority_queue<int> prQueueCout;
	prQueueCout.push(2);
	prQueueCout.push(15);
	prQueueCout.push(4);

	cCout(stackCout, "Stack (template): ");
	cout << "\n" << endl;

	cCout(queueCout, "Queue (template): ");
	cout << "\n" << endl;

	cCout(prQueueCout, "Pr. Queue (template): ");
	cout << "\n" << endl;

	////////////////////////////////////////////////////////////////////////////////////
	//stack

	//Создайте стек таким образом, чтобы
	//а) элементы стека стали копиями элементов вектора
	//б) при выводе значений как вектора, так и стека порядок значений был одинаковым 
	{
		vector<Point> vec = { Point(1, 1), Point(2, 2), Point(3, 3) };

		// Так как стек — это LIFO (последним зашел — первым вышел),
		// чтобы первым напечатался первый элемент вектора vec[0],
		// мы должны положить элементы в стек в ОБРАТНОМ порядке (с конца):
		std::stack<Point> st;
		for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
			st.push(*it);
		}

		// Проверяем порядок вывода
		cout << "\nVector:";
		for (const auto& p : vec) cout << " " << p;
			cCout(st, "Stack from vector");
	}

	//Сравнение и копирование стеков
	//а) создайте стек и любым способом задайте значения элементов
	//б) создайте новый стек таким образом, чтобы он стал копией первого
	//в) сравните стеки на равенство
	//г) модифицируйте любой из стеком любым образом (push, pop, top)
	//д) проверьте, какой из стеков больше (подумайте, какой смысл вкладывается в такое сравнение)
	{
		// а) Создаем первый стек
		std::stack<int> s1;
		s1.push(10);
		s1.push(20);
		s1.push(30);

		// б) Копируем первый стек во второй через конструктор копирования
		std::stack<int> s2 = s1;

		// в) Сравниваем на равенство
		cout << "\ns1 == s2: " << boolalpha << (s1 == s2) << "\n"; // true

		// г) Модифицируем второй стек
		s2.pop();        // удалили верхний (30)
		s2.push(50);     // добавили вместо него 50

		// д) Проверяем, какой стек больше
		cout << "s1 < s2: " << (s1 < s2) << "\n";
		cout << "s1 > s2: " << (s1 > s2) << "\n";
	}


	////////////////////////////////////////////////////////////////////////////////////
	//queue

	//Создайте очередь, которая содержит указатели на объекты типа Point,
	//при этом явно задайте базовый контейнер.
	//Измените значения первого и последнего элементов посредством front() и back()
	//Подумайте, что требуется сделать при уничтожении такой очереди?
	{

		std::queue<Point*, std::deque<Point*>> q;
		q.push(new Point(10, 20));
		q.push(new Point(30, 40));
		q.push(new Point(50, 60));

		// Изменяем значения первого и последнего элементов через front() и back()
		q.front()->setX(99);   // первый элемент: точка (10, 20) -> x стал 99
		q.back()->setY(88);    // последний элемент: точка (50, 60) -> y стал 88
		cout << "\nQueue front: " << *q.front() << ", back: " << *q.back() << "\n";

		// Что требуется сделать при уничтожении очереди:
		// 		Обязательно освободить динамическую память, иначе произойдет утечка памяти
		// 		Контейнеры STL автоматически НЕ вызывают delete для «сырых» указателей.
		while (!q.empty()) {
			delete q.front(); // Освобождаем память под объектом Point
			q.pop();          // Удаляем сам указатель из очереди
    }


	
	
	}
	////////////////////////////////////////////////////////////////////////////////////
	//priority_queue
	//а) создайте очередь с приоритетами, которая будет хранить адреса строковых литералов - const char*
	//б) проинициализируйте очередь при создании с помощью вспомогательного массива с элементами const char*
	//в) проверьте "упорядоченность" значений (с помощью pop() ) - если они оказываются не упорядоченными, подумайте:
	//		что сравнивается при вставке?


	{
		const char* arr[] = { "banana", "apple", "cherry", "orange" };
		
		// а) Очередь с приоритетами по умолчанию
		priority_queue<const char*> pq(arr, arr + 4);
		cout << "\nPriority Queue (by default):\n";
		while (!pq.empty()) {
			cout << pq.top() << " ";
			pq.pop();
		}
		cout << "\n"; // ?? ниже
		// ПОЧЕМУ ОНИ НЕ УПОРЯДОЧЕНЫ ПО АЛФАВИТУ:
		// По умолчанию сравниваются АДРЕСА указателей в памяти (числа), а не содержимое строк!
		// Чтобы упорядочить именно по алфавиту, нужен компаратор с strcmp:
		struct CmpStr {
			bool operator()(const char* a, const char* b) const {
				return strcmp(a, b) < 0; // лексикографическое сравнение строк
			}
		};
		priority_queue<const char*, vector<const char*>, CmpStr> pq_alphabet(arr, arr + 4);
		cout << "Priority Queue (alphabetically with comparator):\n";
		while (!pq_alphabet.empty()) {
			cout << pq_alphabet.top() << " ";
			pq_alphabet.pop();
		}
		cout << "\n";

	}
	
	
	////////////////////////////////////////////////////////////////////////////////////
	//set
	//a) создайте множество с элементами типа Point - подумайте, что необходимо определить
	//		в классе Point (и каким образом)
	//б) распечатайте значения элементов с помощью шаблона, реализованного в предыдущей лаб. работе
	//в) попробуйте изменить любое значение...
	//г) Создайте два множества, которые будут содержать одинаковые значения
	//		типа int, но занесенные в разном порядке
	//д) Вставьте в любое множество диапазон элементов из любого другого
	//	контейнера, например, элементов массива	(что происходит, если в массиве имеются дубли?)

		// а) В классе Point ОБЯЗАТЕЛЬНО должен быть перегружен operator< (строгий слабый порядок),
		// так как std::set построен на основе бинарного дерева поиска (красно-черного дерева).
	{
		set<Point> pointSet;
		pointSet.insert(Point(5, 5));
		pointSet.insert(Point(1, 2));
		pointSet.insert(Point(3, 4));
		pointSet.insert(Point(1, 2)); // дубликат автоматически отбросится

		// б) Распечатка значений (элементы отсортируются автоматически)
		cout << "\nSet of Points:\n";
		for (const auto& p : pointSet) cout << p << " ";
		cout << "\n";

		// в) Попробуем изменить значение:
		auto it = pointSet.begin();
		// *it = Point(10, 10);  // ОШИБКА: элементы в set имеют квалификатор const!
		// Прямое изменение элемента сломало бы порядок в бинарном дереве.

		// Сначала удаляем старый элемент, затем вставляем измененный:
		Point modifiedPoint = *it;
		modifiedPoint.setX(10);
		pointSet.erase(it);
		pointSet.insert(modifiedPoint);

		// г) Два множества с одинаковыми int, но занесенными в разном порядке:
		set<int> s1 = { 9, 1, 5, 3 };
		set<int> s2 = { 1, 3, 5, 9 };
		cout << "s1 == s2: " << boolalpha << (s1 == s2) << "\n"; // true (порядок вставки не важен)

		// д) Вставка диапазона из массива:
		int arr[] = { 4, 1, 4, 2, 1, 3, 2 };
		set<int> sFromArr;
		sFromArr.insert(arr, arr + 7);
		cout << "sFromArr (duplicates discarded): ";
		for (int val : sFromArr) cout << val << " "; // 1 2 3 4
		cout << "\n";
	}

	////////////////////////////////////////////////////////////////////////////////////
	//multiset
	{
		// В отличие от set, multiset разрешает хранить одинаковые элементы (дубликаты)
		int arr[] = { 4, 1, 4, 2, 1, 4 };

		multiset<int> ms(arr, arr + 6);
		cout << "\nMultiset: ";
		for (int val : ms) cout << val << " "; // 1 1 2 4 4 4
			cout << "\nNumber of fours: " << ms.count(4) << "\n";

		// Демонстрация удаления:
		// ms.erase(4);          // удалит ВСЕ четверки
		ms.erase(ms.find(4));    // удалит ТОЛЬКО ОДНУ четверку
		cout << "After removing one quad: ";

		for (int val : ms) cout << val << " ";
			cout << "\n";
	}



	////////////////////////////////////////////////////////////////////////////////////
	//map	
	//а) создайте map, который хранит пары "фамилия, зарплата" - pair<const char*, int>,
	//	при этом строки задаются строковыми литералами
	//б) заполните контейнер значениями посредством operator[] и insert()
	//в) распечатайте содержимое

	//е) замените один из КЛЮЧЕЙ на новый (была "Иванова", вышла замуж => стала "Петрова")

	{
		struct CmpStr {
			bool operator()(const char* a, const char* b) const {
				return strcmp(a, b) < 0;
			}
		};

		// а) Создание map
		map<const char*, int, CmpStr> salary;

		// б) Заполнение через operator[] и insert()
		salary["Ivanova"] = 50000;
		salary["Sidorova"] = 75000;
		salary.insert(pair<const char*, int>("Петров", 60000));
		salary.insert(make_pair("Кузнецов", 90000));

		// в) Печать содержимого
		cout << "\nEmployee salariesв:\n";
		for (const auto& item : salary) {
			cout << item.first << " : " << item.second << " руб.\n";
		}
	}


	////////////////////////////////////////////////////////////////////////////////////
	//multimap
	//а) создайте "англо-русский" словарь, где одному и тому же ключу будут соответствовать
	//		несколько русских значений - pair<string,string>, например: strange: чужой, странный...
	//б) Заполните словарь парами с помощью метода insert или проинициализируйте с помощью 
	//		вспомогательного массива пара (пары можно конструировать или создавать с помощью шаблона make_pair)
	//в) Выведите все содержимое словаря на экран
	//г) Выведите на экран только варианты "переводов" для заданного ключа. Подсказка: для нахождения диапазона
	//		итераторов можно использовать методы lower_bound() и upper_bound()


	{
		// а) Создаем англо-русский словарь
		// (У multimap НЕТ operator[], вставка производится только через insert)
		multimap<string, string> dict;

		// б) Заполнение через insert и make_pair
		dict.insert(make_pair("strange", "странный"));
		dict.insert(make_pair("strange", "чужой"));
		dict.insert(make_pair("strange", "незнакомый"));
		dict.insert(make_pair("run", "бежать"));
		dict.insert(make_pair("run", "управлять"));
		dict.insert(make_pair("apple", "яблоко"));

		// в) Вывод всего словаря на экран
		cout << "\nПолный англо-русский словарь:\n";
		for (const auto& item : dict) {
			cout << item.first << " -> " << item.second << "\n";
		}

		// г) Поиск переводов для заданного ключа через lower_bound и upper_bound:
		string key = "strange";
		auto itLow = dict.lower_bound(key); // итератор на первый элемент с ключом "strange"
		auto itUp  = dict.upper_bound(key); // итератор за последний элемент с ключом "strange"

		cout << "\nВарианты перевода для слова '" << key << "':\n";
		for (auto it = itLow; it != itUp; ++it) {
			cout << "  - " << it->second << "\n";
		}
	}

///////////////////////////////////////////////////////////////////

	//Итераторы

	//Реверсивные итераторы. Сформируйте set<Point>. Подумайте, что
	//нужно перегрузить в классе Point. Создайте вектор, элементы которого 
	//являются копиями элементов set, но упорядочены по убыванию
	{
		// В классе Point для set по-прежнему необходим operator<.
		set<Point> pointSet = { Point(1, 1), Point(5, 5), Point(2, 3), Point(4, 2) };
		// В set элементы всегда упорядочены по возрастанию.
		// Чтобы в векторе они оказались по убыванию, инициализируем его 
		// реверсивными итераторами: от rbegin() до rend():
		vector<Point> vecDesc(pointSet.rbegin(), pointSet.rend());
		cout << "\nSet (по возрастанию):\n";
		
		for (const auto& p : pointSet) cout << p << " ";
			cout << "\nVector (по убыванию через реверсивные итераторы):\n";
		
		for (const auto& p : vecDesc) cout << p << " ";
			cout << "\n";
	}
	//Потоковые итераторы. С помощью ostream_iterator выведите содержимое
	//vector и set из предыдущего задания на экран.
	{
		set<Point> pointSet = { Point(10, 20), Point(30, 40), Point(50, 60) };
		vector<Point> vecDesc(pointSet.rbegin(), pointSet.rend());
		
		// ostream_iterator связывает поток cout с алгоритмом copy.
		// Вторым аргументом передается строка-разделитель (например, пробел или переход на новую строку).
		// ВАЖНО: для типа Point обязательно должен быть перегружен operator<< !
		
		cout << "\nВывод set через ostream_iterator:\n";
		copy(pointSet.begin(), pointSet.end(), ostream_iterator<Point>(cout, " "));
		cout << "\n";
		
		cout << "Вывод vector через ostream_iterator:\n";
		copy(vecDesc.begin(), vecDesc.end(), ostream_iterator<Point>(cout, " "));
		cout << "\n";
	}
	//Итераторы вставки. С помощью возвращаемых функциями:
	//back_inserter()
	//front_inserter()
	//inserter()
	//итераторов вставки добавьте элементы в любой из созданных контейнеров. Подумайте:
	//какие из итераторов вставки можно использовать с каждым контейнером.
	{
		vector<int> src = { 10, 20, 30 };
		
		// 1. back_inserter() — внутри вызывает c.push_back(val)
		// Используется с: vector, deque, list
		vector<int> vDest;
		
		copy(src.begin(), src.end(), back_inserter(vDest));
		cout << "\nПосле back_inserter в vector: ";
		
		for (int x : vDest) cout << x << " "; // 10 20 30
			cout << "\n";
		
		// 2. front_inserter() — внутри вызывает c.push_front(val)
		// Используется с: deque, list (у vector НЕТ метода push_front, с ним не работает!)
		// Внимание: при вставке в начало порядок элементов разворачивается!
		deque<int> dDest;
		
		copy(src.begin(), src.end(), front_inserter(dDest));
		cout << "После front_inserter в deque: ";
		
		for (int x : dDest) cout << x << " "; // 30 20 10
			cout << "\n";
		
		// 3. inserter() — внутри вызывает c.insert(it, val)
		// Используется с: ЛЮБЫМИ стандартными контейнерами (vector, deque, list, set, map).
		set<int> sDest = { 1, 100 };
		
		copy(src.begin(), src.end(), inserter(sDest, sDest.begin()));
		cout << "После inserter в set: ";
		
		for (int x : sDest) cout << x << " "; // 1 10 20 30 100
			cout << "\n";



///////////////////////////////////////////////////////////////////

	//Обобщенные алгоритмы (заголовочный файл <algorithm>). Предикаты.

	// алгоритм for_each() - вызов заданной функции для каждого элемента любой последовательности
	//(массив, vector, list...)
	//С помощью алгоритма for_each в любой последовательности с элементами любого типа
	//распечатайте значения элементов
	//Подсказка : неплохо вызываемую функцию определить как шаблон
	{
		vector<int> vInt = { 1, 2, 3, 4, 5 };
		cout << "\nfor_each для vector<int>: ";
		
		for_each(vInt.begin(), vInt.end(), printElem<int>);
			cout << "\n";
	}

	//С помощью алгоритма for_each в любой последовательности с элементами типа Point
	//измените "координаты" на указанное значение (такой предикат тоже стоит реализовать 
	//как шаблон) и выведите результат с помощью предыдущего предиката
	{
		vector<Point> vp = { Point(1, 2), Point(3, 4), Point(5, 6) };
		cout << "\nPoint до смещения: ";
		for_each(vp.begin(), vp.end(), printElem<Point>);
		cout << "\n";
		// Смещаем координаты на (+10, +20)
		
		for_each(vp.begin(), vp.end(), OffsetPoint(10, 20));
			cout << "Point после смещения: ";
		for_each(vp.begin(), vp.end(), printElem<Point>);
			cout << "\n";
	}

	//С помощью алгоритма find() найдите в любой последовательности элементов Point
	//все итераторы на элемент Point с указанным значением.
	{
		vector<Point> vp = { Point(1, 1), Point(5, 5), Point(2, 2), Point(5, 5), Point(9, 9) };
		Point target(5, 5);
		vector<vector<Point>::iterator> foundIterators;
		auto it = vp.begin();
		// find находит первое вхождение, поэтому для поиска ВСЕХ вхождений запускаем цикл:
		while ((it = find(it, vp.end(), target)) != vp.end()) {
			foundIterators.push_back(it);
			++it; // сдвигаемся вперед, чтобы искать дальше
		}
		cout << "\nНайдено совпадений с " << target << ": " << foundIterators.size() << "\n";
	}

	//С помощью алгоритма sort() отсортируйте любую последовательность элементов Point. 
	////По умолчанию алгоритм сортирует последовательность по возрастанию.
	//Что должно быть определено в классе Point?
	// Замечание: обобщенный алгоритм sort не работает со списком, так как
	//это было бы не эффективно => для списка сортировка реализована методом класса!!!
	{
		// В классе Point ОБЯЗАТЕЛЬНО должен быть перегружен operator<
		vector<Point> vp = { Point(7, 2), Point(1, 5), Point(3, 3) };
		sort(vp.begin(), vp.end());
		cout << "\nОтсортированный vector<Point>: ";
		
		for_each(vp.begin(), vp.end(), printElem<Point>);
			cout << "\n";
		
			// Демонстрация для списка:
		list<Point> lp = { Point(7, 2), Point(1, 5), Point(3, 3) };
		// sort(lp.begin(), lp.end()); // ОШИБКА! У списка нет RandomAccessIterator
		lp.sort(); // ПРАВИЛЬНО: сортировка методом самого класса list
	}
	
	//Создайте глобальную функцию вида: bool Pred1_1(const Point& ), которая будет вызываться
	//алгоритмом find_if(), передавая в качестве параметра очередной элемент последовательности.
	//С помощью алгоритма find_if() найдите в любой последовательности элементов Point
	//итератор на элемент Point, удовлетворяющий условию: координаты x и y лежат в промежутке
	//[-n, +m].

	{
		vector<Point> vp = { Point(-20, 100), Point(15, 25), Point(70, 80) };
		auto it = find_if(vp.begin(), vp.end(), Pred1_1);
		if (it != vp.end()) {
			cout << "\nfind_if нашел точку в диапазоне [-" << g_n << ", +" << g_m << "]: " << *it << "\n";
		} else {
			cout << "\nfind_if: точка не найдена\n";
		}
	}

	//С помощью алгоритма sort() отсортируйте любую последовательность элементов Rect,
	//располагая прямоугольники по удалению центра от начала координат.
	{
		vector<Rect> rects = { Rect(5, 5), Rect(1, 1), Rect(10, 2), Rect(2, 2) };
		// Сортировка с лямбда-компаратором по удаленности от (0, 0)
		sort(rects.begin(), rects.end(), [](const Rect& a, const Rect& b) {
			return a.distSq() < b.distSq();
		});
		cout << "\nПрямоугольники по удалению от начала координат:\n";
		for (const auto& r : rects) {
			cout << "  " << r << " (dist^2 = " << r.distSq() << ")\n";
		}
	}

	{//transform
		//Напишите функцию, которая с помощью алгоритма transform переводит 
		//содержимое объекта string в нижний регистр.
		//Подсказка: класс string - это "почти" контейнер, поэтому для него
		// определены методы begin() и end()
		string s = "Hello, WORLD!";
		string lowerS = toLowerStr(s);
		cout << "\nИсходная строка: " << s << " -> в нижнем регистре: " << lowerS << "\n";
		
		//Заполните list объектами string. С помощью алгоритма transform сформируте
		//значения "пустого" set, конвертируя строки в нижний регистр
		list<string> strList = { "APPLE", "BANANA", "ORANGE", "apple" };
		set<string> lowerSet;
		
		// Используем inserter для вставки в пустой set и toLowerStr для конвертации:
		transform(strList.begin(), strList.end(), inserter(lowerSet, lowerSet.begin()), toLowerStr);
		cout << "Результирующий set (в нижнем регистре и без дублей): ";
		for (const auto& str : lowerSet) cout << str << " ";
			cout << "\n";
	}
	{// map
		//Сформируйте любым способом вектор с элементами типа string.
		//Создайте (и распечатайте для проверки) map<string, int>, который будет
		//содержать упорядоченные по алфавиту строки и
		//количество повторений каждой строки в векторе
		vector<string> words = { "banana", "apple", "banana", "orange", "apple", "apple" };
		map<string, int> wordCount;
		for (const auto& w : words) {
			wordCount[w]++; // если слово встречается впервые, создается значение 0 и увеличивается до 1
		}
		cout << "\nЧастотный словарь слов:\n";
		for (const auto& pair : wordCount) {
			cout << "  " << pair.first << " : " << pair.second << " раз(а)\n";
		}
	}
	return 0;
}

