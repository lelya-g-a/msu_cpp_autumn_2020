#include <cassert>
#include <fstream>
#include <iostream>

// прокси-класс для перегрузки операции [][]
class MatrixProxy
{
private:
    size_t size;
    int *row;
public:
    MatrixProxy(int *rowMatr, size_t sizeN)
    {
        size = sizeN;
        row = rowMatr;
    }
    int & operator [] (size_t index)
    {
        if ((index >= size) || (index < 0))
        {
            throw std::out_of_range("Bad columns index");
        }
        return row[index];
    }
    const int & operator [] (size_t index) const
    {
        if ((index >= size) || (index < 0))
        {
            throw std::out_of_range("Bad columns index");
        }
        return row[index];
    }
};

class Matrix
{
private:
    size_t rows;
    size_t cols;
    int **matr;
public:
    Matrix(size_t rowsN, size_t colsN);
    Matrix(const Matrix &M);
    Matrix & operator = (const Matrix & M);
    size_t GetRowsNum () const;
    size_t GetColsNum () const;
    MatrixProxy operator [] (size_t index);
    const MatrixProxy operator [] (size_t index) const;
    Matrix & operator *= (int mult);
    friend std::ostream & operator << 
        (std::ostream &ostream, const Matrix &M);
    Matrix operator + (const Matrix &M) const;
    bool operator == (const Matrix &M) const;
    bool operator != (const Matrix &M) const;
    ~Matrix();
};

// реализация методов класса Matrix
Matrix :: Matrix(size_t rowsN, size_t colsN) : rows(rowsN), cols(colsN)
{
    matr = new int *[rows];
    for (size_t i = 0; i < rows; i++)
    {
        matr[i] = new int [cols];
    }
}

Matrix :: Matrix(const Matrix &M)
{
    rows = M.rows;
    cols = M.cols;
    matr = new int *[rows];
    for (size_t i = 0; i < rows; i++)
    {
        matr[i] = new int [cols];
        for (size_t j = 0; j < cols; j++)
        {
            matr[i][j] = M.matr[i][j];
        }
    }
}

Matrix & Matrix :: operator = (const Matrix & M)
{
    for (size_t i = 0; i < rows; i++)
    {
        delete [] matr[i];
    }
    delete [] matr;
    rows = M.rows;
    cols = M.cols;
    matr = new int *[rows];
    for (size_t i = 0; i < rows; i++)
    {
        matr[i] = new int [cols];
        for (size_t j = 0; j < cols; j++)
        {
            matr[i][j] = M.matr[i][j];
        }
    }
    return *this;
}

size_t Matrix :: GetRowsNum () const
{
    return rows;
}

size_t Matrix :: GetColsNum () const
{
    return cols;
}

MatrixProxy Matrix :: operator [] (size_t index)
{
    if ((index >= rows) || (index < 0))
    {
        throw std::out_of_range("Bad rows index");
    }
    return MatrixProxy (matr[index], cols);
}

const MatrixProxy Matrix :: operator [] (size_t index) const
{
    if ((index >= rows) || (index < 0))
    {
        throw std::out_of_range("Bad rows index");
    }
    return MatrixProxy (matr[index], cols);
}

Matrix & Matrix :: operator *= (int mult)
{
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            matr[i][j] *= mult;
        }
    }
    return *this;
}

Matrix Matrix :: operator + (const Matrix &M) const
{
    if ((rows != M.rows) || (cols != M.cols))
    {
        throw "Wrong matrix sizes";
    }
    Matrix Temp(rows, cols);
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            Temp.matr[i][j] = matr[i][j] + M.matr[i][j];
        }
    }
    return Temp;
}

bool Matrix :: operator == (const Matrix &M) const
{
    if ((rows != M.rows) || (cols != M.cols))
    {
        return 0;
    }
    bool flag = 1;
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            flag = flag && (matr[i][j] == M.matr[i][j]);
        }
    }
    return flag;
}

bool Matrix :: operator != (const Matrix &M) const
{
    if ((rows != M.rows) || (cols != M.cols))
    {
        return 1;
    }
    bool flag = 1;
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            flag = flag && (matr[i][j] == M.matr[i][j]);
        }
    }
    return !flag;
}

Matrix :: ~Matrix()
{
    for (size_t i = 0; i < rows; i++)
    {
        delete [] matr[i];
    }
    delete [] matr;
}

// друг класса
std::ostream & operator << (std::ostream &ostream, const Matrix &M)
{
    for (size_t i = 0; i < M.rows; i++)
    {
        for (size_t j = 0; j < M.cols; j++)
        {
            ostream << M.matr[i][j] << "\t";
        }
        ostream << std::endl;
    }
    return ostream;
}


// проверка работы программы
// получение количества строк и столбцов матрицы
void sizeTest()
{
    Matrix M(10, 100);
    
    assert(M.GetRowsNum() == 10);
    assert(M.GetColsNum() == 100);
}

// получение элемента и присваивание ему значения
void elemTest()
{
    Matrix M(2, 3);
    M[0][0] = 1;
    M[0][1] = 2;
    M[0][2] = 3;
    M[1][0] = 4;
    M[1][1] = 5;
    M[1][2] = 6;
    
    int elem = M[0][2];
    
    assert(elem == 3);
    assert(M[1][1] == 5);
}

// умножение на число и складывание матриц
void multAddTest()
{
    Matrix M1(2, 2);
    M1[0][0] = 1;
    M1[0][1] = 2;
    M1[1][0] = 3;
    M1[1][1] = 4;
    
    Matrix M2(2, 2);
    M2[0][0] = 5;
    M2[0][1] = 6;
    M2[1][0] = 7;
    M2[1][1] = 8;
    
    M1 *= 3;
    Matrix M = M1 + M2;
    
    assert(M[1][1] == 20);
}

// проверка операций сравнения (равенство/неравенство) матриц
void compTest()
{
    Matrix M1(2, 2);
    M1[0][0] = 1;
    M1[0][1] = 2;
    M1[1][0] = 3;
    M1[1][1] = 4;
    
    Matrix M2(2, 2);
    M2[0][0] = 5;
    M2[0][1] = 6;
    M2[1][0] = 7;
    M2[1][1] = 8;
    
    assert((M1 == M2) == 0);
    assert((M1 != M2) == 1);
}

// проверка вывода матрицы
void printTest()
{
    Matrix M(2, 2);
    M[0][0] = 1;
    M[0][1] = 2;
    M[1][0] = 3;
    M[1][1] = 4;
    
    std::string file_name = "output.txt";
    std::ofstream outFile(file_name);
    outFile << M;
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
    assert(str == "1\t2\t3\t4\t");
}

// проверка исключений
void excepTest()
{
    Matrix M1(2, 2);
    Matrix M2(3, 4);
    std::string report;
    
    try
    {
        M1[0][2];
    }
    catch (const std::out_of_range &excep) 
    {
        report = excep.what();
    }
    assert(report == "Bad columns index");
    
    try
    {
        M1[3][0];
    }
    catch (const std::out_of_range &excep) 
    {
        report = excep.what();
    }
    assert(report == "Bad rows index");
    
    try
    {
        Matrix M = M1 + M2;
    }
    catch (const char *str) 
    {
        report = str;
    }
    assert(report == "Wrong matrix sizes");
}

// проверка работы конструктора копирования и оператора присваивания
void copyAssignTest()
{
    Matrix M1(2, 2);
    M1[0][0] = 1;
    M1[0][1] = 2;
    M1[1][0] = 3;
    M1[1][1] = 4;
    
    Matrix M2 = M1;
    
    Matrix M3(4, 3);
    M3 = M2;
    
    assert((M2 == M1) == 1);
    assert((M3 == M1) == 1);
}


int main()
{
    sizeTest();
    elemTest();
    multAddTest();
    compTest();
    printTest();
    excepTest();
    copyAssignTest();
    
    std::cout << "All tests are successful!" << std::endl;
    
    return 0;
}
