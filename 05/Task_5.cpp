#include <cassert>
#include "serializer.h"

struct Data
{
    bool a;
    bool b;
    uint64_t c;
    bool d;
    uint64_t e;

    template <class Serializer>
    Error serialize(Serializer& serializer)
    {
        return serializer(a, b, c, d, e);
    }
    
    template <class Deserializer>
    Error deserialize(Deserializer& deserializer)
    {
        return deserializer(a, b, c, d, e);
    }
};

struct DataErr1
{
    bool a;
    uint64_t b;
    uint64_t c;

    template <class Serializer>
    Error serialize(Serializer& serializer)
    {
        return serializer(a, b, c);
    }
    
    template <class Deserializer>
    Error deserialize(Deserializer& deserializer)
    {
        return deserializer(a, b, c);
    }
};

struct DataErr2
{
    bool a;
    std::string b;
    uint64_t c;

    template <class Serializer>
    Error serialize(Serializer& serializer)
    {
        return serializer(a, b, c);
    }
    
    template <class Deserializer>
    Error deserialize(Deserializer& deserializer)
    {
        return deserializer(a, b, c);
    }
};


// корректная работа сериализатора и десериализатора
void Test1()
{
    Data x {false, true, 123, true, 90};
    Data y {false, false, 0, false, 0};
    
    std::stringstream stream;
    Serializer serializer(stream);
    Deserializer deserializer(stream);
    
    assert(serializer.save(x)   == Error::NoError);
    assert(deserializer.load(y) == Error::NoError);
    assert(x.a == y.a);
    assert(x.b == y.b);
    assert(x.c == y.c);
    assert(x.d == y.d);
    assert(x.e == y.e);
}


// попытка выгрузить не ту структуру, которая была сохранена
void Test2()
{
    Data x {false, true, 123, true, 90};
    DataErr1 y {false, 0, 0};
    
    std::stringstream stream;
    Serializer serializer(stream);
    Deserializer deserializer(stream);
    
    assert(serializer.save(x)   == Error::NoError);
    assert(deserializer.load(y) == Error::CorruptedArchive);
}


// попытка подать сериализатору неправильные типы данных
void Test3()
{
    DataErr2 x {false, "str", 0};
    
    std::stringstream stream;
    Serializer serializer(stream);
    
    assert(serializer.save(x) == Error::CorruptedArchive);
}


// проверка работы десериализатора без вызова сериализатора
void Test4()
{
    Data x {false, true, 123, true, 90};
    
    std::stringstream stream;
    Deserializer deserializer(stream);
    
    assert(deserializer.load(x) == Error::CorruptedArchive);
}

int main()
{
    Test1();
    Test2();
    Test3();
    Test4();

    return 0;
}
