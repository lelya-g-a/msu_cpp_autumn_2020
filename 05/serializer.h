// классы Error, Serializer и Deserializer
#include <sstream>

enum class Error
{
    NoError,
    CorruptedArchive
};

class Serializer
{
    static constexpr char Separator = ' ';
public:
    explicit Serializer(std::ostream &out) : out_(out) 
    {
    }

    template <class T>
    Error save(T& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator () (ArgsT... args);
    
private:
    std::ostream &out_;
    bool used = false;
    
    template <class T>
    Error process (T&& elem);
    
    template <class T, class ... ArgsT>
    Error process (T&& elem, ArgsT&&... args);
    
    Error saveElem(bool elem);
    Error saveElem(uint64_t elem);
    Error saveElem(...);
};


class Deserializer
{
    static constexpr char Separator = ' ';
public:
    explicit Deserializer(std::istream &in) : in_(in) 
    {
    }

    template <class T>
    Error load(T& object)
    {
        return object.deserialize(*this);
    }

    template <class... ArgsT>
    Error operator () (ArgsT&... args)
    {
        return process(std::forward<ArgsT>(args)...);
    }
    
private:
    std::istream &in_;
    
    template <class T>
    Error process (T&& elem);
    
    template <class T, class ... ArgsT>
    Error process (T&& elem, ArgsT&&... args);
    
    Error loadElem(bool &elem);
    Error loadElem(uint64_t &elem);
    Error loadElem(...);
};


// реализация методов класса Serializer

template <class... ArgsT>
Error Serializer :: operator () (ArgsT... args)
{
    if (used)
    {
        return Error::CorruptedArchive;
    }
    else
    {
        used = true;
        return process(std::forward<ArgsT>(args)...);
    }
}

template <class T>
Error Serializer :: process (T&& elem)
{
    return saveElem(elem);
}

template <class T, class ... ArgsT>
Error Serializer :: process (T&& elem, ArgsT&&... args)
{
    if (saveElem(elem) == Error::NoError)
    {
        return process(std::forward<ArgsT>(args)...);
    }
    else
    {
        return Error::CorruptedArchive;
    }
}

Error Serializer :: saveElem(bool elem) 
{
    std::string text;
    
    if (elem == true)
    {
        text = "true";
    }
    else
    {
        text = "false";
    }
    
    out_ << text << Separator;
    
    return Error::NoError;
}

Error Serializer :: saveElem(uint64_t elem) 
{
    out_ << elem << Separator;
    
    return Error::NoError;
}

Error Serializer :: saveElem(...) 
{
    return Error::CorruptedArchive;
}


// реализация методов класса Deserializer

template <class T>
Error Deserializer :: process (T&& elem)
{
    return loadElem(elem);
}

template <class T, class ... ArgsT>
Error Deserializer :: process (T&& elem, ArgsT&&... args)
{
    if (loadElem(elem) == Error::NoError)
    {
        return process(std::forward<ArgsT>(args)...);
    }
    else
    {
        return Error::CorruptedArchive;
    }
}

Error Deserializer :: loadElem(bool &elem) 
{
    std::string obj;
    in_ >> obj;
    if (obj == "true")
    {
        elem = true;
    }
    else if (obj == "false")
    {
        elem = false;
    }
    else
    {
        return Error::CorruptedArchive;
    }

    return Error::NoError;
}

Error Deserializer :: loadElem(uint64_t &elem) 
{
    std::string obj;
    in_ >> obj;
    try
    {
        elem = std::stoull(obj);
    }
    catch (std::invalid_argument &err)
    {
        return Error::CorruptedArchive;
    }
    catch (std::out_of_range &err)
    {
        return Error::CorruptedArchive;
    }
        
    return Error::NoError;
}

Error Deserializer :: loadElem(...) 
{
    return Error::CorruptedArchive;
}
