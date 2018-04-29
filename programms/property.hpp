#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <stdexcept>

namespace Modules {


    template<typename ValueType>
    class NumericProperty;

    class StringProperty;

    class Property
    {
    private:
        std::string name;
        std::string description;
    public:
        const enum Type {Int, Float, String} type;
    protected:
        Property(Type t):type(t){}
    public:

        void setName( const std::string _name){
         name = _name;
        }
        std::string getName() const {
         return name;
        }

        void setDescription( const std::string _description){
         description = _description;
        }
        std::string getDescription() const {
         return description;
        }

        NumericProperty<int> * asInt(){
            if(type == Int){
                return reinterpret_cast<NumericProperty<int>*>(this);
            }
            return nullptr;
        }
        NumericProperty<float> * asFloat(){
            if(type == Int){
                return reinterpret_cast<NumericProperty<float>*>(this);
            }
            return nullptr;
        }
        StringProperty * asString(){
            if(type == String){
                return reinterpret_cast<StringProperty*>(this);
            }
            return nullptr;
        }
    };

    namespace defail{

        template<typename t>
        Property::Type toEnum(){
            static_assert (std::is_same<t,float>::value||std::is_same<t,int>::value,"Wrong Type. Con be only int or float");
            if(std::is_same<t,int>::value){
                return Property::Int;
            }else {
                return Property::Float;
            }
        }
    }

    template<typename Type_t>
    class NumericProperty : public Property{
        static_assert (std::is_same<Type_t,float>::value||std::is_same<Type_t,int>::value,"Wrong Type. Con be only int or float");
        Type_t min;
        Type_t max;
        Type_t value;
    public:        
        NumericProperty(Type_t min, Type_t max, Type_t value):Property(defail::toEnum<Type_t>()),min(min),max(max),value(value){}
        void setValue( const Type_t _value){
            value = std::min(getMax(),std::max(_value,getMin()));
        }
        Type_t getValue() const {
            return value;
        }

        void setMin( const Type_t _min){
            min = _min;
        }
        Type_t getMin() const {
         return min;
        }

        void setMax( const Type_t _max){
            max = _max;
        }
        Type_t getMax() const {
         return max;
        }
    };

    class StringProperty : public Property{
    protected:
        std::string value;
    public:
        StringProperty(std::string value):Property(Property::String),value(value){}
        /**
         * @brief setValuetry to set the value, if that fails, false is returned
         * @param value the new value
         * @return true  for succes
         */
        virtual bool setValue(std::string value){
            this->value = value;
            return true;
        }
        virtual ~StringProperty() = default;
    };

}

#endif // VARIABLE_H
