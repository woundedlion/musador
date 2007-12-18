#ifndef FACTORY_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define FACTORY_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

template <typename T>
class AbstractFactory
{
public:

    virtual ~AbstractFactory() {}
    virtual T* create() = 0;

};

template <typename BaseType, typename DerivedType>
class ConcreteFactory : public AbstractFactory<BaseType>
{
    BaseType* create() { return new DerivedType(); }
};

#endif