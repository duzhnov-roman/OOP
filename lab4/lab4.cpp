//========================================================================================
//	Лабораторная №4. Концепты. Диапазоны. Трёхстороннее сравнение С++.
//	C++20	         Назначенные инициализаторы. Корутины. Шаблонные лямбды. Атрибуты.
//========================================================================================
// Сборка: g++ -std=c++23 lab4.cpp -o lab4
// (C++23 нужен только для std::optional::or_else и std::ranges::to)
#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <concepts>
#include <ranges>
#include <algorithm>
#include <span>
#include <coroutine>
#include <numeric>
#include <compare>
#include <optional>
#include <cstdio>
#include <stack>
#include <queue>
#include <array>
#include <limits>
#include <cctype>
#include <exception>
#include <thread>
#include <chrono>
#include <random>

using namespace std;

void title(const string& text)
{
    cout << "\n===== " << text << " =====\n";
}


// РАЗДЕЛ 4

// 4.1
struct SimplePoint {
    double x = 0;
    double y = 0;

    auto operator<=>(const SimplePoint& other) const = default;
};

ostream& operator<<(ostream& os, const SimplePoint& p)
{
    return os << "(" << p.x << ", " << p.y << ")";
}

// 4.1
class Point {
    double x, y;
public:
    Point(double x = 0, double y = 0) : x(x), y(y) {}

    double getX() const { return x; }
    double getY() const { return y; }

    double distSq() const { return x * x + y * y; }

    weak_ordering operator<=>(const Point& other) const
    {
        if (distSq() < other.distSq()) return weak_ordering::less;
        if (distSq() > other.distSq()) return weak_ordering::greater;
        return weak_ordering::equivalent;
    }

    bool operator==(const Point& other) const { return distSq() == other.distSq(); }

    friend ostream& operator<<(ostream& os, const Point& p)
    {
        return os << "(" << p.x << ", " << p.y << ")";
    }
};

// 4.2
struct Box {
    double width = 0;
    double height = 0;
    double depth = 0;

    double volume() const { return width * height * depth; }

    bool isValid() const { return width != 0 && height != 0 && depth != 0; }

    partial_ordering operator<=>(const Box& other) const
    {
        if (!isValid() || !other.isValid()) return partial_ordering::unordered;
        if (volume() < other.volume()) return partial_ordering::less;
        if (volume() > other.volume()) return partial_ordering::greater;
        return partial_ordering::equivalent;
    }

    bool operator==(const Box& other) const
    {
        if (!isValid() || !other.isValid()) return false;
        return volume() == other.volume();
    }
};

ostream& operator<<(std::ostream& os, const Box& b)
{
    return os << "Box[" << b.width << "x" << b.height << "x" << b.depth
              << ", V=" << b.volume() << "]";
}

template <typename T>
void printCompare(const T& a, const T& b)
{
    cout << a << " vs " << b << ":  "
              << "<: " << (a < b) << "  >: " << (a > b)
              << "  ==: " << (a == b) << "  !=: " << (a != b) << '\n';
}


// Раздел 1 - CONCEPTS
template <typename T>
concept Printable = requires(std::ostream& os, T val) {
    os << val;
};

template <Printable T>
void printValue(T val)
{
    std::cout << val << '\n';
}

//(stack, priority_queue)
template <typename T>
concept HasTop = requires(T c) {
    c.top();
    c.pop();
    c.empty();
};

//(queue)
template <typename T>
concept HasFront = requires(T c) {
    c.front();
    c.pop();
    c.empty();
};

template <typename C> requires HasTop<C> || HasFront<C>
void printAdapter(C c, const string& name)
{
    cout << name << ": ";
    while (!c.empty()) {
        if constexpr (HasTop<C>)
            cout << c.top() << " ";
        else
            cout << c.front() << " ";
        c.pop();
    }
    cout << '\n';
}


template <typename T, typename K>
concept Summable = std::convertible_to<K, T> && requires(T a, K b) {
    { a + b } -> std::convertible_to<T>;
};

template <typename T, typename K> requires Summable<T, K>
T sumTwo(T a, K b)
{
    return a + b;
}

template <integral T>
constexpr T maxFactorialN() // TODO
{
    T result = 1;
    T n = 1;
    while (result <= numeric_limits<T>::max() / (n + 1)) {
        n++;
        result *= n;
    }
    return n;
}

template <integral T>
T factorial(T n)
{
    if (n < 0 || n > maxFactorialN<T>()) { // TODO
        cout << "Error: n must be in [0, " << maxFactorialN<T>() << "]\n";
        return 0;
    }
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}


//раздел 3 - consteval и constexpr

// 3.1 Корни уравнения a*x^2 + b*x + c = 0 (только на этапе компиляции)
struct Roots {
    int count;
    double x1;
    double x2;
};

consteval double constSqrt(double v)
{
    if (v == 0) return 0;
    double x = v;
    for (int i = 0; i < 100; i++)
        x = 0.5 * (x + v / x);
    return x;
}

consteval Roots solveQuadratic(double a, double b, double c)
{
    if (a == 0) {
        if (b == 0) return {0, 0, 0};
        return {1, -c / b, -c / b};
    }
    double d = b * b - 4 * a * c;
    if (d < 0) return {0, 0, 0};
    if (d == 0) return {1, -b / (2 * a), -b / (2 * a)};
    double sq = constSqrt(d);
    return {2, (-b + sq) / (2 * a), (-b - sq) / (2 * a)};
}

// 3.2 n-е число Фибоначчи
constexpr long long fibonacci(int n)
{
    long long a = 0, b = 1;
    for (int i = 0; i < n; i++) {
        long long next = a + b;
        a = b;
        b = next;
    }
    return a;
}


// РАЗДЕЛ 5 - Designated Initializers

struct Point3D {
    double x = 0;
    double y = 0;
    double z = 0;
};

void printPoint3D(const string& name, const Point3D& p)
{
    cout << name << " = (" << p.x << ", " << p.y << ", " << p.z << ")\n";
}

struct Config {
    string host = "localhost";
    int port = 8080;
    bool ssl = false;
    int timeout = 30;
};


// РАЗДЕЛ 6 - std::span

// 6.1
void printSpan(span<int> s)
{
    for (int x : s)
        cout << x << " ";
    cout << '\n';
}

// 6.2
double average(span<const double> data)
{
    if (data.empty()) return 0.0;
    double sum = accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

// 6.3
struct Product {
    string name;
    double price;
};


void reducePrices(span<Product> products, double amount)
{
    for (Product& p : products)
        p.price = max(0.0, p.price - amount);
}


//========================================================================================
// РАЗДЕЛ 7: Coroutines
//========================================================================================

class Generator {
public:
    struct promise_type {
        int current = 0;   // последнее выданное число

        Generator get_return_object()
        {
            return Generator(std::coroutine_handle<promise_type>::from_promise(*this));
        }
        // Корутина не стартует сама, а ждёт первого next()
        std::suspend_always initial_suspend() { return {}; }
        // После завершения корутина остаётся приостановленной до destroy()
        std::suspend_always final_suspend() noexcept { return {}; }
        // co_yield v: запоминаем число и засыпаем
        std::suspend_always yield_value(int v)
        {
            current = v;
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    explicit Generator(std::coroutine_handle<promise_type> h) : handle(h) {}

    // Копировать нельзя (иначе destroy() вызовется дважды), перемещать можно
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& other) noexcept : handle(other.handle) { other.handle = nullptr; }
    Generator& operator=(Generator&&) = delete;

    ~Generator()
    {
        if (handle) handle.destroy();
    }

    // true - получено новое число, false - корутина закончилась
    bool next()
    {
        if (!handle || handle.done()) return false;
        handle.resume();
        return !handle.done();
    }

    int value() const { return handle.promise().current; }

private:
    std::coroutine_handle<promise_type> handle;
};

Generator count_to(int limit)
{
    for (int i = 1; i <= limit; i++)   // при limit < 1 цикл не выполнится ни разу
        co_yield i;
}


// РАЗДЕЛ 9: [[likely]] / [[unlikely]] и std::jthread

enum class Status { Success, Timeout, CriticalError };

void handle_status(Status s)
{
    switch (s) {
        [[likely]] case Status::Success:
            cout << "Status: Success\n";
            break;
        [[unlikely]] case Status::Timeout:
            cout << "Status: Timeout\n";
            break;
        [[unlikely]] case Status::CriticalError:
            cout << "Status: CriticalError\n";
            break;
    }
}

// Поток генерирует случайные числа, пока не попросят остановиться
void generateNumbers(std::stop_token token, int minValue, int maxValue)
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(minValue, maxValue);

    while (!token.stop_requested()) {
        std::cout << dist(gen) << " " << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::cout << "\nThread stopped\n";
}


// РАЗДЕЛ 10: Range-for с инициализатором

vector<int> getNumbers()
{
    mt19937 gen(random_device{}());
    uniform_int_distribution<int> dist(1, 100);

    vector<int> numbers(10);
    for (int& x : numbers)
        x = dist(gen);
    return numbers;
}


// РАЗДЕЛ 11: std::optional

// Поэлементное деление a[i] / b[i]. При ошибке - std::nullopt
template <typename C1, typename C2>
std::optional<std::vector<int>> divideContainers(const C1& a, const C2& b)
{
    if (a.size() != b.size()) return std::nullopt;

    std::vector<int> result;
    for (std::size_t i = 0; i < a.size(); i++) {
        if (b[i] == 0) return std::nullopt;
        result.push_back(a[i] / b[i]);
    }
    return result;
}

// Делит контейнеры и выводит либо результат, либо индексы с ошибками (через or_else)
template <typename C1, typename C2>
void divideAndPrint(const C1& a, const C2& b)
{
    auto result = divideContainers(a, b).or_else([&]() -> std::optional<std::vector<int>> {
        std::cout << "Division failed at indices: ";
        std::size_t maxSize = std::max(a.size(), b.size());
        for (std::size_t i = 0; i < maxSize; i++) {
            // индекс есть только в одном контейнере, либо делитель равен 0
            if (i >= a.size() || i >= b.size() || b[i] == 0)
                std::cout << i << " ";
        }
        std::cout << '\n';
        return std::nullopt;
    });

    if (result) {
        std::cout << "Result: ";
        for (int x : *result)
            std::cout << x << " ";
        std::cout << '\n';
    }
}


int main()
{
    std::cout << std::boolalpha;


// Задание 1.1
// Определите концепт Printable, который проверяет, что тип T можно вывести
// через std::cout (т.е. оператор << определён для std::ostream и T).
// Затем напишите шаблонную функцию printValue(T val), которая принимает
// только типы, удовлетворяющие концепту Printable, и выводит значение.
// Перепишите шаблон функции для вывода значений stack, queue, priority_queue из Лаб. 3 через концепты и constexpr в единую функцию
    {
        title("1.1 Printable");
        printValue(42);
        printValue(3.14);
        printValue("hello");
        printValue(string("string"));
        printValue(Point(1, 2));
        // printValue(std::vector<int>{1, 2});

        stack<Point> st;
        queue<Point> q;
        priority_queue<int> pq;
        for (int i = 1; i <= 4; i++) {
            st.push(Point(i, i));
            q.push(Point(i, i));
            pq.push(i * 10);
        }
        printAdapter(st, "stack");
        printAdapter(q, "queue");
        printAdapter(pq, "priority_queue");
    }

// Задание 1.2
// Напишите концепт Summable, который проверяет, что два значения типа T или K
// можно сложить оператором +, где K можно привести к T, результат приводится к типу T.
// Затем напишите шаблонную функцию sumTwo, возвращающую их сумму.
    {
        title("1.2 Summable");
        cout << "sumTwo(5, 7) = " << sumTwo(5, 7) << '\n';
        cout << "sumTwo(2.5, 3) = " << sumTwo(2.5, 3) << '\n';
        cout << "sumTwo(string(\"abc\"), \"def\") = "
                  << sumTwo(std::string("abc"), "def") << '\n';
        // sumTwo(5, std::string("x"));
    }

// Задание 1.3
// Используйте стандартный концепт std::integral для ограничения вычислений рекурсивной функции
// factorial(T n), которая вычисляет факториал только для целых чисел и указывает максимальное значение n.
    {
        title("1.3 factorial");
        cout << "Max n for int: " << maxFactorialN<int>() << '\n';
        cout << "Max n for long long: " << maxFactorialN<long long>() << '\n';
        cout << "factorial(5) = " << factorial(5) << '\n';
        cout << "factorial(12) = " << factorial(12) << '\n';
        cout << "factorial(20LL) = " << factorial(20LL) << '\n';
        cout << "factorial(13): ";
        factorial(13);
        // factorial(5.0);
    }



// Задание 2.1
// Создайте вектор из целых чисел, заполните вектор какими-либо значениями.
// С помощью std::views::filter и std::views::transform:
//   - оставьте только чётные числа,
//   - умножьте каждое из них на 3,
//   - выведите результат на экран.
    vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    {
        title("2.1 filter + transform");
        auto result = numbers
            | views::filter([](int x) { return x % 2 == 0; })
            | views::transform([](int x) { return x * 3; });

        for (int x : result)
            cout << x << " ";
        cout << '\n';
    }

// Задание 2.2
// С помощью std::views::drop и std::views::take выведите элементы вектора
// начиная с 3-го и заканчивая 6-м (включительно).
    {
        title("2.2 drop + take");

        for (int x : numbers | views::drop(2) | views::take(4))
            cout << x << " ";
        cout << '\n';
    }

// Задание 2.3
// Создайте вектор строк (std::string_view), заполните вектор какими-либо значениями.
// Используйте std::ranges::sort для сортировки вектора строк в алфавитном порядке.
// Объедините вектор строк в единый объект string_view используя std::views::join.
// Затем используйте views::split и views::transform создайте новое представление ranges::transform_view
// в котором строки приведены к верхнему регистру.
// Используйте std::ranges::find, чтобы найти введенную пользователем строку в полученном представлении и выведите её индекс.
    {
        title("2.3 sort + join + split + find");
        vector<string_view> words = {"orange", "apple", "kiwi", "banana", "lemon"};

        ranges::sort(words);
        cout << "Sorted: ";
        for (auto w : words)
            cout << w << " ";
        cout << '\n';

        string joinedStr = words
            | views::transform([](std::string_view w) { return std::string(w) + " "; })
            | views::join
            | ranges::to<string>();

        string_view joined = joinedStr;
        joined.remove_suffix(1);
        cout << "Joined: \"" << joined << "\"\n";

        auto upperWords = joined
            | views::split(' ')
            | views::transform([](auto part) {
                  string word;
                  for (char c : part)
                      word += static_cast<char>(toupper(static_cast<unsigned char>(c)));
                  return word;
              });

        cout << "Upper: ";
        for (const auto& w : upperWords)
            cout << w << " ";
        cout << '\n';

        string input;
        cout << "Enter a word to find: ";
        cin >> input;
        for (char& c : input)   
            c = static_cast<char>(toupper(static_cast<unsigned char>(c)));

        auto it = ranges::find(upperWords, input);
        if (it != upperWords.end())
            cout << "Found at index " << ranges::distance(upperWords.begin(), it) << '\n';
        else
            cout << "Not found\n";
    }


// РАЗДЕЛ 3 - consteval и constexpr

// Задание 3.1
// Напишите функцию вычисления квадратного уравнения через consteval
// ТОЛЬКО на этапе компиляции. Создайте constexpr переменную со значением и выведите результат.
    {
        title("3.1 consteval");
        constexpr Roots r1 = solveQuadratic(1, -3, 2);   // x^2 - 3x + 2 = 0
        constexpr Roots r2 = solveQuadratic(1, 2, 1);    // x^2 + 2x + 1 = 0
        constexpr Roots r3 = solveQuadratic(1, 0, 1);    // x^2 + 1 = 0

        cout << "x^2 - 3x + 2 = 0: count = " << r1.count
                  << ", x1 = " << r1.x1 << ", x2 = " << r1.x2 << '\n';
        cout << "x^2 + 2x + 1 = 0: count = " << r2.count << ", x = " << r2.x1 << '\n';
        cout << "x^2 + 1 = 0: count = " << r3.count << " (no real roots)\n";
        // double a = 1; solveQuadratic(a, 2, 1); // ошибка: аргумент не константа времени компиляции
    }

// Задание 3.2
// Напишите constexpr-функцию fibonacci(int n), возвращающую n-е число
// Фибоначчи итеративно. Убедитесь, что она работает как в compile-time
// (constexpr переменная), так и в run-time (обычный вызов).
// fibonacci(10) = 55,  fibonacci(0) = 0,  fibonacci(1) = 1
    {
        title("3.2 fibonacci");
        constexpr long long fib10 = fibonacci(10);
        static_assert(fibonacci(0) == 0);
        static_assert(fibonacci(1) == 1);
        static_assert(fibonacci(10) == 55);
        cout << "compile-time fibonacci(10) = " << fib10 << '\n';

        int n = 20;
        cout << "run-time fibonacci(" << n << ") = " << fibonacci(n) << '\n';
        cout << "run-time: ";
        for (int i = 0; i <= 10; i++)
            cout << fibonacci(i) << " ";
        cout << '\n';
    }


// РАЗДЕЛ 4 - Трёхстороннее сравнение (Оператор космического корабля <=>)

// Задание 4.1
// Добавьте перегрузку оператора <=> для класса Point
// Проверьте результаты сравнений точек между собой: < > == !=
// Модифицируйте оператор через std::weak_ordering, где критерием сравнения будет являться
// порядок точек плоскости, определяемый удалённостью от центра.
    {
        title("4.1 Point <=> (default, by x then y)");
        SimplePoint a{1, 5}, b{2, 0}, c{1, 5};
        printCompare(a, b);
        printCompare(a, c);

        title("4.1 Point <=> (weak_ordering, by distance)");
        Point p1(3, 4), p2(4, 3), p3(1, 1), p4(0, 6);
        printCompare(p1, p2);
        printCompare(p1, p3);
        printCompare(p1, p4);
    }

// Задание 4.2
// Создайте структуру Box с полями double width, height и depth.
// Добавьте метод double volume() const.
// Определите оператор <=>, сравнивающий Box по объёму вручную, с неопределенным unordered поведением для width, height, depth значения равного 0.
// Проверьте результаты сравнений Box между собой: < > == !=
// Отсортируйте вектор из трёх Box (std::ranges::sort) и выведите объёмы.
    {
        title("4.2 Box <=>");
        Box b1{2, 3, 4};    // V = 24
        Box b2{1, 2, 3};    // V = 6
        Box b3{4, 3, 2};    // V = 24
        Box bad{0, 5, 5};   // нулевая сторона -> unordered

        printCompare(b1, b2);
        printCompare(b1, b3);
        printCompare(b1, bad);   // все сравнения false, кроме !=

        auto order = b1 <=> bad;
        cout << "b1 <=> bad is unordered: " << (order == partial_ordering::unordered) << '\n';

        vector<Box> boxes = {{5, 5, 5}, {1, 2, 3}, {2, 2, 2}};
        ranges::sort(boxes);
        cout << "Sorted volumes: ";
        for (const Box& b : boxes)
            cout << b.volume() << " ";
        cout << '\n';
    }


// РАЗДЕЛ 5: Designated Initializers (Назначенные инициализаторы)

// Задание 5.1
// Объявите структуру Point3D { double x = 0; double y = 0; double z = 0; }.
// Создайте три объекта, используя designated initializers:
//   - p1: только x = 1.0
//   - p2: x = 2.0, z = 5.0
//   - p3: все три поля (3.0, 4.0, 7.0)
// Выведите координаты каждого объекта.
    {
        title("5.1 Point3D");
        Point3D p1{.x = 1.0};
        Point3D p2{.x = 2.0, .z = 5.0};
        Point3D p3{.x = 3.0, .y = 4.0, .z = 7.0};
        printPoint3D("p1", p1);
        printPoint3D("p2", p2);
        printPoint3D("p3", p3);
    }

// Задание 5.2
// Объявите структуру Config:
//   std::string host = "localhost"; int port = 8080;
//   bool ssl = false; int timeout = 30;
// Создайте объект, установив через designated initializers только
// host = "example.com" и ssl = true. Выведите все поля.
    {
        title("5.2 Config");
        Config cfg{.host = "example.com", .ssl = true};
        cout << "host = " << cfg.host << '\n'
                  << "port = " << cfg.port << '\n'
                  << "ssl = " << cfg.ssl << '\n'
                  << "timeout = " << cfg.timeout << '\n';
    }


// РАЗДЕЛ 6: std::span

// Задание 6.1
// Напишите функцию printSpan(std::span<int> s), которая выводит все элементы
// через пробел. Вызовите её с:
//   1) int arr[] = {1, 2, 3, 4, 5, ...}
//   2) std::vector<int>{10, 20, 30, ....}
    {
        title("6.1 printSpan");
        int arr[] = {1, 2, 3, 4, 5, 6};
        vector<int> vec = {10, 20, 30, 40, 50};
        printSpan(arr);
        printSpan(vec);
    }

// Задание 6.2
// Напишите функцию double average(std::span<const double> data).
// Если data пустой — вернуть 0.0. Иначе вернуть среднее арифметическое.
// Проверьте на {1.0, 2.5, 3.0, 4.5, 5.0}.
    {
        title("6.2 average");
        vector<double> data = {1.0, 2.5, 3.0, 4.5, 5.0};
        vector<double> empty;
        cout << "average = " << average(data) << '\n';
        cout << "average(empty) = " << average(empty) << '\n';
    }

// Задание 6.3
// Создайте структуру Product содержащую имя товара и цену. Создайте std::array из товаров заполненных произвольным образом.
// Реализовать функцию, которая уменьшает цену каждого переданного товара на введенное значение, но не ниже нуля.
// Создать std::span, представляющий весь массив. Через subspan выбрать два товара, начиная с индекса 1.
// Уменьшить цены выбранных товаров на указанную константу. Вывести названия и цены всех товаров из исходного массива.
    {
        title("6.3 Product + subspan");
        array<Product, 4> products = {{
            {"Bread", 50},
            {"Milk", 90},
            {"Cheese", 300},
            {"Apple", 20},
        }};

        double discount;
        cout << "Enter discount: ";
        cin >> discount;

        span<Product> all(products);
        span<Product> selected = all.subspan(1, 2);
        reducePrices(selected, discount);

        for (const Product& p : products)
            cout << p.name << ": " << p.price << '\n';
    }


// РАЗДЕЛ 7: Coroutines (Корутины)

// Задание 7.1
// Напишите корутину, которая по очереди выдаёт целые числа от 1 до 5.
// Необходимо реализовать класс Generator. Он должен содержать:
// - Вложенный тип promise_type.
// - Дескриптор корутины std::coroutine_handle<promise_type>.
// - Метод next(), который возобновляет корутину и возвращает true, если получено очередное число, или false, если выполнение завершено.
// - Метод value(), возвращающий последнее выданное число.
// - Деструктор, освобождающий корутину через destroy().
// Реализуйте тип promise_type и необходимые методы:
// - get_return_object().
// - initial_suspend() — корутина должна начинать выполнение только при вызове next().
// - final_suspend() — корутина должна оставаться приостановленной до освобождения её дескриптора.
// - yield_value(int) — сохраняет число и приостанавливает выполнение.
// - return_void().
// - unhandled_exception() — для простоты вызывает std::terminate().
// Для приостановки используется std::suspend_always.
// Реализуйте корутину count_to. Она должна в цикле выдавать числа через co_yield.
// Если limit меньше 1, она не должна выдавать ни одного числа.
// Подсказка: в next() проверяйте, что корутина ещё не завершилась, прежде чем вызывать resume(). Вызывайте value() только после того, как next() вернул true.
    {
        title("7.1 Generator");
        Generator gen = count_to(5);
        std::cout << "count_to(5): ";
        while (gen.next())
            std::cout << gen.value() << " ";
        std::cout << '\n';
        std::cout << "next() after end: " << gen.next() << '\n';

        Generator empty = count_to(0);
        std::cout << "count_to(0): ";
        while (empty.next())
            std::cout << empty.value() << " ";
        std::cout << "(nothing)\n";
    }


// РАЗДЕЛ 8: Шаблонные лямбды (C++20)

// Задание 8.1
// Напиши лямбду, которая преобразует элементы std::array с помощью переданной операции и возвращает новый массив.
// Тип элементов результата может отличаться от типа исходных элементов. Заготовка шаблонной лямбды: auto transform_array = []<typename T, std::size_t N, typename F>,
// где T — тип исходных элементов; N — размер массива; F — тип переданной операции.
// Выведите элементы массива.
    {
        title("8.1 transform_array");
        auto transform_array = []<typename T, size_t N, typename F>(const array<T, N>& arr, F op) {
            using R = decltype(op(arr[0]));
            array<R, N> result{};
            for (size_t i = 0; i < N; i++)
                result[i] = op(arr[i]);
            return result;
        };

        std::array<int, 5> ints = {1, 2, 3, 4, 5};

        auto halves = transform_array(ints, [](int x) { return x / 2.0; });
        auto texts = transform_array(ints, [](int x) { return "#" + to_string(x); });
        for (double x : halves)
            cout << x << " ";
        cout << '\n';
        for (const string& s : texts)
            cout << s << " ";
        cout << '\n';
    }

// Задание 8.2
// Напишите лямбду-счётчик: при каждом вызове возвращает следующее число
// начиная с 0 (0, 1, 2, 3, ...). Используйте захват count = 0 + mutable.
    {
        title("8.2 counter");
        auto counter = [count = 0]() mutable { return count++; };
        for (int i = 0; i < 5; i++)
            cout << counter() << " ";
        cout << '\n';
    }

// Задание 8.3
// Напиши одной лямбдой на C++20, которая вычисляет сумму элементов двух массивов (std::array) разных типов с явными шаблонными параметрами.
    {
        title("8.3 sum of two arrays");
        auto sumArrays = []<typename T1, size_t N1, typename T2, size_t N2>(
                             const array<T1, N1>& a, const array<T2, N2>& b) {
            common_type_t<T1, T2> sum = 0;
            for (const T1& x : a) sum += x;
            for (const T2& x : b) sum += x;
            return sum;
        };

        array<int, 3> a = {1, 2, 3};
        array<double, 4> b = {0.5, 1.5, 2.5, 3.5};
        cout << "sum = " << sumArrays(a, b) << '\n';
        cout << "sum (explicit) = " << sumArrays.operator()<int, 3, double, 4>(a, b) << '\n';
    }


// РАЗДЕЛ 9: Атрибуты [[likely]] / [[unlikely]] и std::jthread

// Задание 9.1
// Напишите функцию handle_status(Status n), которая проверяет типы статусов в операторе switch/case:
// Status::Success пометьте ветку [[likely]]
// Status::Timeout пометьте ветку [[unlikely]]
// Status::CriticalError пометьте ветку [[unlikely]]
    {
        title("9.1 handle_status");
        handle_status(Status::Success);
        handle_status(Status::Timeout);
        handle_status(Status::CriticalError);
    }

// Задание 9.2
// Используя std::jthread, запустите поток, который выводит генерирует случаные числа в заданном пользователем диапазоне.
// Передайте токен управления std::to_token в функцию с потоком. Задайте задержку между генерациями this_thread::sleep_for в 200мс
// Напишите явный запрос остановки потока из основной функции через 3 секунды.
    {
        title("9.2 jthread");
        int minValue, maxValue;
        std::cout << "Enter min and max: ";
        std::cin >> minValue >> maxValue;
        if (minValue > maxValue)
            std::swap(minValue, maxValue);

        // jthread сам передаёт std::stop_token первым аргументом в generateNumbers
        std::jthread worker(generateNumbers, minValue, maxValue);

        std::this_thread::sleep_for(std::chrono::seconds(3));
        worker.request_stop();   // явный запрос остановки
        worker.join();
    }


// РАЗДЕЛ 10: Range-for с инициализатором и std::views::iota

// Задание 10.1
// Напишите функцию getNumbers(), возвращающую vector<int> со случайными значениями.
// Используйте range-for с инициализатором C++20. Выведите все элементы.
    {
        title("10.1 range-for with initializer");
        for (auto nums = getNumbers(); int x : nums)
            cout << x << " ";
        cout << '\n';
    }

// Задание 10.2
// Используйте std::views::iota для создания последовательности 1..n.
// Вычислите сумму через std::accumulate и выведите.
    {
        title("10.2 iota + accumulate");
        int n = 10;
        auto seq = views::iota(1, n + 1);
        int sum = accumulate(seq.begin(), seq.end(), 0);
        cout << "sum 1.." << n << " = " << sum << '\n';
    }

// РАЗДЕЛ 11: std::optional

// Задание 11.1
// Напишите шаблонную функцию принимающую два контейнера из целых чисел и выполняющую поэлементное деление всех элементов одного контейнера на другой
// В случае, если произошла ошибка: разный размер контейнеров, либо деление на ноль, то вернуть std::nullopt и применить монадическую операцию or_else
// Вывести номера индексов, с которыми не получилось выполнить деление.
    {
        title("11.1 optional + or_else");
        vector<int> a = {10, 20, 30, 40};
        vector<int> b = {2, 5, 3, 8};
        vector<int> withZero = {2, 0, 3, 0};
        array<int, 6> longer = {1, 2, 3, 4, 5, 6};

        cout << "a / b:        ";
        divideAndPrint(a, b);
        cout << "a / withZero: ";
        divideAndPrint(a, withZero);
        cout << "a / longer:   ";
        divideAndPrint(a, longer);
    }

    return 0;
}
