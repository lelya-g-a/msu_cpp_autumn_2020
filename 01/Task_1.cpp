#include <cassert>
#include <iostream>

class Allocator
{
private:
    char* mem = nullptr;
    size_t memSize = 0;
    size_t offset = 0;
public:
    void makeAllocator(size_t maxSize);
    char* alloc(size_t size);
    void reset();
    ~ Allocator();
};

// реализация методов класса
void Allocator :: makeAllocator(size_t maxSize)
{
    try
    {
        //если makeAllocator() уже вызывался для объекта
        if (mem != nullptr)
        {
            throw -1;
        }
        //выделение памяти для объекта
        mem = new char [memSize = maxSize];
    }
    catch(std::bad_alloc &e)
    {
        //некорректная работа new
        std::cerr << "Failed allocation: " << e.what() << std::endl;
    }
    catch(int a)
    {
        //std::cerr << "Memory is already allocated" << std::endl;
        throw -1;
    }
}

char* Allocator :: alloc(size_t size)
{
    //проверка на превышение количества памяти
    if (offset + size > memSize)
    {
        return nullptr;
    }
    char* tmp = mem + offset;
    offset += size;
    return tmp;
}

void Allocator :: reset()
{
    //если ещё не был вызван метод makeAllocator()
    if (mem == nullptr)
    {
        //std::cerr << "Memory is not allocated" << std::endl;
        throw -1;
    }
    //обнуление всей использованной памяти и установка сдвига на начало
    for (size_t i = 0; i < offset; i++)
    {
        *(mem + i) = 0;
    }
    offset = 0;
}

Allocator :: ~ Allocator()
{
    //очищение памяти
    delete [] mem;
}



//тестирование корректности работы
//вызов метода makeAllocator() дважды должен привести к ошибке
void twiceMakeAllocatorTest()
{
    int err = 0;
    Allocator A;
    A.makeAllocator(100);
    try
    {
        A.makeAllocator(10);
    }
    catch (int a) 
    {
        err = a;
    }
    assert(err == -1);
}

//вызов alloc() с запросом на большее количество памяти, чем есть
void badAllocTest1()
{
    Allocator A;
    A.makeAllocator(10);
    char* mem = A.alloc(15);
    assert(mem == nullptr);
}

//граничный случай работы alloc()
void badAllocTest2()
{
    Allocator A;
    A.makeAllocator(10);
    char* mem = A.alloc(10);
    assert(mem != nullptr);
}

//вызов alloc() несколько раз, сумма превышает допустимы размер
void manyAllocTest1()
{
    Allocator A;
    A.makeAllocator(10);
    char* mem = A.alloc(5);
    mem = A.alloc(6);
    assert(mem == nullptr);
}

//вызов alloc() несколько раз, сумма не превышает допустимы размер
void manyAllocTest2()
{
    Allocator A;
    A.makeAllocator(10);
    char* mem = A.alloc(5);
    mem = A.alloc(4);
    assert(mem != nullptr);
}

//корректная работа alloc()
void allocTest()
{
    Allocator A;
    A.makeAllocator(10);
    char* mem1 = A.alloc(5);
    char* mem2 = A.alloc(5);
    assert(mem1 + 5 == mem2);
}

//вызов reset() без makeAllocator() должен привести к ошибке
void badResetTest()
{
    int err = 0;
    Allocator A;
    try
    {
        A.reset();
    }
    catch(int a)
    {
        err = a;
    }
    assert(err == -1);
}

//корректная работа reset()
void resetTest()
{
    Allocator A;
    A.makeAllocator(10);
    char* mem1 = A.alloc(5);
    A.reset();
    char* mem2 = A.alloc(5);
    assert(mem1 == mem2);
}

//возможность переиспользования памяти
void reuseTest()
{
    Allocator A;
    A.makeAllocator(10);
    char* mem = A.alloc(5);
    mem = A.alloc(4);
    A.reset();
    mem = A.alloc(5);
    assert(mem != nullptr);
}

int main()
{
    //вызов тестов
    twiceMakeAllocatorTest();
    badAllocTest1();
    badAllocTest2();
    manyAllocTest1();
    manyAllocTest2();
    allocTest();
    badResetTest();
    resetTest();
    reuseTest();
    
    //программа не завершилась раньше => все тесты пройдены
    std::cout << "Tests are successful!" << std::endl;
    
    return 0;
}
