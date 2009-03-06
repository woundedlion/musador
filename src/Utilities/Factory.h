#ifndef FACTORY_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define FACTORY_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

namespace Util
{
    /// @class AbstractFactory
    /// @brief Abstract base class for Factory objects
    template <typename T>
    class AbstractFactory
    {
    public:

        /// @brief Destructor
        virtual ~AbstractFactory() {}

        /// @brief Create an instance of the type for which this Factory is specialized.
        virtual T* create() = 0;

    };

    /// @class ConcreteFactory
    /// @brief Factory class used for creating polymorphic objects of type DerivedType
    template <typename BaseType, typename DerivedType>
    class ConcreteFactory : public AbstractFactory<BaseType>
    {
        /// @brief Create an instance of DerivedType
        /// @returns A pointer to BaseType pointing to the newly created object of DeriveType
        BaseType* create() { return new DerivedType(); }
    };
}
#endif