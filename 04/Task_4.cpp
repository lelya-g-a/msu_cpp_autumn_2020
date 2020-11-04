#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class BigInt
{
private:
    char *bigNum = nullptr;
    size_t size = 0;
    bool neg = false;
public:
    BigInt(int64_t num = 0);
    BigInt(std::string num);
    BigInt(const BigInt &Num);
    BigInt(BigInt &&Num);
    BigInt & operator = (const BigInt &Num);
    BigInt & operator = (BigInt &&Num);
    ~BigInt();
 
    BigInt operator - () const;
    BigInt operator + (const BigInt &Num) const;
    BigInt operator - (const BigInt &Num) const;
    void mul_10_n (const size_t n);
    BigInt operator * (const BigInt &Num) const;

    bool operator == (const BigInt &Num) const;
    bool operator != (const BigInt &Num) const;
    bool operator <  (const BigInt &Num) const;
    bool operator >  (const BigInt &Num) const;
    bool operator <= (const BigInt &Num) const;
    bool operator >= (const BigInt &Num) const;
    
    friend std::ostream & operator << 
        (std::ostream &ostream, const BigInt &Num);
};

//реализации методов класса
BigInt :: BigInt(int64_t num)
{
    // перевод числа в строку
    std::stringstream ss;
    ss << num;
    std::string strNum = ss.str();
    // проверка на отрицательность
    if (strNum[0] == '-')
    {
        neg = true;
        strNum.erase(0,1);
    }
    size = strNum.length();
    bigNum = new char [size];
    // хранение числа в обратном порядке
    for (size_t i = 0; i < size; i++)
    {
        bigNum[i] = strNum[size - 1 - i];
    }
}

BigInt :: BigInt(std::string strNum)
{
    // проверка на отрицательность
    if (strNum[0] == '-')
    {
        neg = true;
        strNum.erase(0,1);
    }
    // является ли строка числом
    bool flag = true;
    for (size_t i = 0; i < strNum.length(); i++)
    {
        flag = flag & isdigit(strNum[i]);
    }
    if (!flag)
    {
        throw "String is not a number";
    }
    //удаление незначащих нулей спереди
    while ((strNum[0] == '0') && (strNum.length() != 1))
    {
        strNum.erase(0,1);
    }
    // если остался один 0
    if (strNum == "0")
    {
        neg = false;
    }
    size = strNum.length();
    bigNum = new char [size];
    // хранение числа в обратносм порядке
    for (size_t i = 0; i < size; i++)
    {
        bigNum[i] = strNum[size - 1 - i];
    }
}

BigInt :: BigInt(const BigInt &Num) : size(Num.size), neg(Num.neg)
{
    bigNum = new char [size];
    for (size_t i = 0; i < size; i++)
    {
        bigNum[i] = Num.bigNum[i];
    }
}

BigInt :: BigInt(BigInt &&Num) : size(std::move(Num.size)), 
                                  neg(std::move(Num.neg))
{
    bigNum = Num.bigNum;
    // обнуляем перемещённый объект
    Num.bigNum = nullptr;
    Num.size = 0;
    Num.neg = false;
}

BigInt & BigInt :: operator = (const BigInt &Num)
{
    // приравнивание самому себе
    if (this == &Num)
    {
        return *this;
    }
    delete [] bigNum;
    size = Num.size;
    neg = Num.neg;
    bigNum = new char [size];
    for (size_t i = 0; i < size; i++)
    {
        bigNum[i] = Num.bigNum[i];
    }
    return *this;
}

BigInt & BigInt :: operator = (BigInt &&Num)
{
    // приравнивание самому себе
    if (this == &Num)
    {
        return *this;
    }
    delete [] bigNum;
    bigNum = Num.bigNum;
    size = std::move(Num.size);
    neg = std::move(Num.neg);
    // обнуляем перемещённый объект
    Num.bigNum = nullptr;
    Num.size = 0;
    Num.neg = false;
    return *this;
}

BigInt :: ~BigInt()
{
    delete [] bigNum;
}

BigInt BigInt :: operator - () const
{
    BigInt Temp = *this;
    // вернуть такое же число, но с другим знаком, если это не 0
    if (!((size == 1) && (bigNum[0] == '0')))
    {
        Temp.neg = !neg;
    }
    return Temp;
}

BigInt BigInt :: operator + (const BigInt &Num) const
{
    if (neg)
    {
        if (Num.neg)
        {
            // отр. + отр.
            return (-((-*this) + (-Num)));
        }
        else
        {
            // отр. + пол.
            return (-((-*this) - Num));
        }
    }
    else if (Num.neg)
    {
        // пол. + отр.
        return (*this - (-Num));
    }
    else 
    {
        // пол. + пол.
        // сделаем так, чтобы первым шло более длинное число
        if (size < Num.size)
        {
            return (Num + *this);
        }
        else
        {
            std::string res;
            // в эту переменную будем записывать 
            // доп. слагаемое от предыдущего разряда
            char add = 0;
            for (size_t i = 0; i < Num.size; i++)
            {
                int sym;
                sym = (bigNum[i] - '0') + (Num.bigNum[i] - '0') + add;
                if (sym >= 10)
                {
                    add = 1;
                    sym %= 10;
                }
                else
                {
                    add = 0;
                }
                char res_sym = '0' + sym;
                res.insert(res.begin(), res_sym);
            }
            for (size_t i = Num.size; i < size; i++)
            {
                int sym;
                sym = (bigNum[i] - '0') + add;
                if (sym >= 10)
                {
                    add = 1;
                    sym %= 10;
                }
                else
                {
                    add = 0;
                }
                char res_sym = '0' + sym;
                res.insert(res.begin(), res_sym);
            }
            if (add)
            {
                res.insert(res.begin(), '1');
            }
            BigInt Res(res);
            return Res;
        }
    }
}

BigInt BigInt :: operator - (const BigInt &Num) const
{
    if (neg)
    {
        if (Num.neg)
        {
            // отр. - отр.
            return (-((-*this) - (-Num)));
        }
        else
        {
            // отр. - пол.
            return (-((-*this) + Num));
        }
    }
    else if (Num.neg)
    {
        // пол. - отр.
        return (*this + (-Num));
    }
    else 
    {
        // пол. - пол.
        if (*this < Num)
        {
            // из меньшего большее
            return (-(Num - *this));
        }
        else
        {
            // из большего меньшее
            std::string res;
            // в эту переменную будем записывать единицу,
            // если необходимо занять у следующего разряда
            char add = 0;
            for (size_t i = 0; i < Num.size; i++)
            {
                int sym;
                sym = (bigNum[i] - '0') - add - (Num.bigNum[i] - '0');
                if (sym < 0)
                {
                    add = 1;
                    sym += 10;
                }
                else
                {
                    add = 0;
                }
                char res_sym = '0' + sym;
                res.insert(res.begin(), res_sym);
            }
            for (size_t i = Num.size; i < size; i++)
            {
                int sym;
                sym = (bigNum[i] - '0') - add;
                if (sym < 0)
                {
                    add = 1;
                    sym += 10;
                }
                else
                {
                    add = 0;
                }
                char res_sym = '0' + sym;
                res.insert(res.begin(), res_sym);
            }
            BigInt Res(res);
            return Res;
        }
    }
}

void BigInt :: mul_10_n (const size_t n)
{
    // умножение на 10^n
    size += n;
    char *newBigNum = new char [size];
    for (size_t i = 0; i < n; i++)
    {
        newBigNum[i] = '0';
    }
    for (size_t i = n; i < size; i++)
    {
        newBigNum[i] = bigNum[i-n];
    }
    delete [] bigNum;
    bigNum = newBigNum;
}

BigInt BigInt :: operator * (const BigInt &Num) const
{
    if (neg)
    {
        if (Num.neg)
        {
            // отр. * отр.
            return ((-*this) * (-Num));
        }
        else
        {
            // отр. * пол.
            return (-((-*this) * Num));
        }
    }
    else if (Num.neg)
    {
        // пол. * отр.
        return (-(*this * (-Num)));
    }
    // пол. * пол.
    // если два обычных числа
    if ((size == 1) && (Num.size == 1))
    {
        BigInt Res((bigNum[0] - '0') * (Num.bigNum[0] - '0'));
        return Res;
    }
    std::string num1;
    std::string num2;
    for (size_t i = 0; i < size; i++)
    {
        num1.insert(num1.begin(), bigNum[i]);
    }
    for (size_t i = 0; i < Num.size; i++)
    {
        num2.insert(num2.begin(), Num.bigNum[i]);
    }
    // приведение к одинаковой длине кратной 2
    // добавлением незначащих нулей
    int64_t diff = (size - Num.size);
    size_t len;
    if (diff < 0)
    {
        num1.insert(0, Num.size % 2 - diff, '0');
        num2.insert(0, (Num.size % 2), '0');
        len = Num.size + Num.size % 2;
    }
    else
    {
        num2.insert(num2.begin(), (size % 2 + diff), '0');
        num1.insert(num1.begin(), (size % 2), '0');
        len = size + size % 2;
    }
    // разбиваем числа на половинки
    BigInt Num11(num1.substr(len/2));
    BigInt Num12(num1.substr(0, len/2));
    BigInt Num21(num2.substr(len/2));
    BigInt Num22(num2.substr(0, len/2));

    BigInt Res1 = (Num11 * Num21);
    BigInt Res2 = (Num12 * Num22);
    BigInt Res3 = (((Num11 + Num12) * (Num21 + Num22)) - Res1 - Res2);
    Res3.mul_10_n(len/2);
    Res2.mul_10_n(len);
    
    return (Res1 + Res3 + Res2);
}

bool BigInt :: operator == (const BigInt &Num) const
{
    if ((size != Num.size) || (neg != Num.neg))
    {
        return 0;
    }
    bool flag = 1;
    for (size_t i = 0; i < size; i++)
    {
        flag = flag && (bigNum[i] == Num.bigNum[i]);
    }
    return flag;
}

bool BigInt :: operator != (const BigInt &Num) const
{
    return !(*this == Num);
}

bool BigInt :: operator < (const BigInt &Num) const
{
    if (*this == Num)
    {
        return 0;
    }
    if (neg)
    {
        if (Num.neg)
        {
            return ((- Num) < (- *this));
        }
        else
        {
            return 1;
        }
    }
    else if (Num.neg)
    {
        return 0;
    }
    else
    {
        if (size != Num.size)
        {
            return (size < Num.size);
        }
        else
        {
            size_t i = size - 1;
            while (bigNum[i] == Num.bigNum[i])
            {
                i--;
            }
            return (bigNum[i] < Num.bigNum[i]);
        }
    }
    bool flag = 1;
    
    return flag;
}

bool BigInt :: operator > (const BigInt &Num) const
{
    return !((*this < Num) || (*this == Num));
}

bool BigInt :: operator <= (const BigInt &Num) const
{
    return !(*this > Num);
}

bool BigInt :: operator >= (const BigInt &Num) const
{
    return !(*this < Num);
}


// друг класса
std::ostream & operator << (std::ostream &ostream, const BigInt &Num)
{
    if (Num.neg)
    {
        ostream << '-';
    }
    for (size_t i = Num.size; i > 0; i--)
    {
        ostream << Num.bigNum[i-1];
    }
    return ostream;
}


// проверка работы программы
// создание объектов, вывод и арифметические операции
void Test1()
{
    BigInt N1;
    BigInt N2(123);
    BigInt N3("-4567");
    BigInt N4 = 89;
    BigInt N5 = N1 + N4;
    
    std::string file_name = "output.txt";
    std::ofstream outFile(file_name);
    outFile << (-N1) << ' ' << N5 << ' ' << (N3 * (-N2) - N4 + N3);
    outFile.close();

    std::ifstream inFile(file_name);
    std::string str;
    while (inFile)
    {
        std::string line;
        getline(inFile, line);
        str = str + line;
    }
    inFile.close();
    assert(str == "0 89 557085");
}

// логические операции
void Test2()
{
    BigInt N1 = 4321;
    BigInt N2("4321");
    BigInt N3;
    
    assert(N1 == N2);
    assert(N1 != (-N2));
    assert(N1 >  N3);
    assert((-N1) <  N3);
    assert(N1 >= N2);
    assert(N1 <= (N2 + N1));
}

// присваивание и перемещение
void Test3()
{
    BigInt N1 = 4321;
    BigInt N2;
    BigInt N3;
    N2 = N1;
    BigInt N4 = std::move(N1);
    N3 = std::move(N2);
    assert(N4 == N3);
}

int main()
{
    Test1();
    Test2();
    Test3();
    
    std::cout << "All tests are successful!" << std::endl;
    
    return 0;
}
