#include <cassert>
#include <cctype>
#include <fstream>
#include <functional>
#include <iostream>

class TokenParser
{
private:
    std::function <void ()> startCallback;
    std::function <void ()> tokenCallback;
    std::function <void (uint64_t &digit)> digitTokenCallback;
    std::function <void (std::string &str)> strTokenCallback;
    std::function <void ()> finishCallback;
    
    // проверка, является ли слово числом
    bool isDigitToken(const std::string &str)
    {
        bool flag = true;
        for (size_t i = 0; i < str.size(); i++)
        {
            flag = flag & isdigit(str[i]);
        }
        return flag;
    }

public:
    TokenParser();
    void Parse(const std::string &str);
    void SetStartCallback
        (std::function <void ()> func);
    void SetTokenCallback
        (std::function <void ()> func);
    void SetDigitTokenCallback
        (std::function <void (uint64_t &digit)> func);
    void SetStrTokenCallback
        (std::function <void (std::string &str)> func);
    void SetFinishCallback
        (std::function <void ()> func);
};

// вспомогательные функции
void f() {}
void g(uint64_t &digit) {}
void h(std::string &str) {}

// реализация методов класса
TokenParser :: TokenParser()
{
    startCallback = f;
    tokenCallback = f;
    digitTokenCallback = g;
    strTokenCallback = h;
    finishCallback = f;
}

void TokenParser :: Parse(const std::string &str = {})
{
    startCallback();
    size_t i = 0;
    while (i != str.length())
    {
        std::string word;
        while (!isspace(str[i]) && (i != str.length()))
        {
            word = word + str[i++];
        }
        tokenCallback();
        if (isDigitToken(word))
        {
            uint64_t digit = std::stoull(word);
            digitTokenCallback(digit);
        }
        else
        {
            strTokenCallback(word);
        }
        while (isspace(str[i]) && (i != str.length()))
        {
            i++;
        }
    }
    finishCallback();
}

void TokenParser :: SetStartCallback
    (std::function <void ()> func)
{
    startCallback = func;
}

void TokenParser :: SetTokenCallback
    (std::function <void ()> func)
{
    tokenCallback = func;
}

void TokenParser :: SetDigitTokenCallback
    (std::function <void (uint64_t &digit)> func)
{
    digitTokenCallback = func;
}

void TokenParser :: SetStrTokenCallback
    (std::function <void (std::string &str)> func)
{
    strTokenCallback = func;
}

void TokenParser :: SetFinishCallback
    (std::function <void ()> func)
{
    finishCallback = func;
}


// проверка работы программы

// для проверки работы программы
// стандартный вывод заменяется на файл, 
// за время работы туда выводится всё в одну строку,
// в конце строка из этого файла сравнивается с ожидаемым результатом

// работа при вызове без задания параметров
void testEmpty()
{
    std::string file_name = "output.txt";
    std::ofstream out(file_name);
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());
    
    TokenParser P;
    P.Parse();
    
    std::cout.rdbuf(coutbuf);
    out.close();

    std::ifstream f(file_name);
    std::string str;
    getline(f, str);
    f.close();
    assert(str == "\0");
}

// проверка установки начального и конечного callback
void testStartFinish()
{
    std::string file_name = "output.txt";
    std::ofstream out(file_name);
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());
    
    TokenParser P;
    P.SetStartCallback
        ( [](){std::cout << "hello";} );
    P.SetFinishCallback
        ( [](){std::cout << "!";} );
    P.Parse();
    
    std::cout.rdbuf(coutbuf);
    out.close();

    std::ifstream f(file_name);
    std::string str;
    getline(f, str);
    f.close();
    assert(str == "hello!");
}

// проверка разделения на строки и числа
// проверка установки обработки чисел и строк
// проверка callback для всех токенов
void testParse()
{
    std::string file_name = "output.txt";
    std::ofstream out(file_name);
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());
    
    std::string text = "1 text 2 text 3";
    TokenParser P;
    P.SetTokenCallback
        ( [](){std::cout << "!";} );
    P.SetDigitTokenCallback
        ( [](uint64_t num){std::cout << "n";} );
    P.SetStrTokenCallback
        ( [](std::string str){std::cout << "t";} );
    P.Parse(text);
    
    std::cout.rdbuf(coutbuf);
    out.close();

    std::ifstream f(file_name);
    std::string str;
    getline(f, str);
    f.close();
    assert(str == "!n!t!n!t!n");
}

// проверка на обработку чисел как int64_t
void testDigit()
{
    std::string file_name = "output.txt";
    std::ofstream out(file_name);
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());
    
    std::string text = "0 9 text 99999";
    TokenParser P;
    P.SetDigitTokenCallback
        ( [](uint64_t num){std::cout << num + 1;} );
    P.Parse(text);
    
    std::cout.rdbuf(coutbuf);
    out.close();

    std::ifstream f(file_name);
    std::string str;
    getline(f, str);
    f.close();
    assert(str == "110100000");
}


int main()
{
    testEmpty();
    testStartFinish();
    testParse();
    testDigit();
    
    std::cout << "Tests are successful!" << std::endl;
    
    return 0;
}
