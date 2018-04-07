#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>

union ValueUnion{
    float floatValue;
    int intValue;
    ValueUnion(float f):floatValue(f){}
    ValueUnion(int i):intValue(i){}
};

template<typename ValueType>
class TypedProperty;

class Property
{
protected:
    ValueUnion min;
    ValueUnion max;
    ValueUnion value;
private:
    std::string name;
    std::string description;
public:
    const enum {Int, Float} Type;
    Property(int min, int max, int value):min(min),max(max),value(value),Type(Int){}
    Property(float min, float max, float value):min(min),max(max),value(value),Type(Float){}

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

    template<typename asType>
    TypedProperty<asType> * as(){
        if(std::is_same<asType,int>::value){
            if(Type == Int){
                return static_cast<TypedProperty<asType>*>(this);
            }
        }else if(std::is_same<asType,float>::value){
            if(Type==Float){
                return static_cast<TypedProperty<asType>*>(this);
            }
        }
        throw std::runtime_error("Bad Type.");
    }
};

template<typename ValueType>
class TypedProperty : public Property{
    static_assert (std::is_same<ValueType,float>::value||std::is_same<ValueType,int>::value,"Wrong Type. Con be only int or float");
public:
    void setValue( const ValueType _value){
        value = std::min(getMax(),std::max(_value,getMin()));
    }
    ValueType getValue() const {
        return value;
    }

    void setMin( const ValueType _min){
        min = _min;
    }
    ValueType getMin() const {
     return min;
    }

    void setMax( const ValueType _max){
        max = _max;
    }
    ValueType getMax() const {
     return max;
    }
};

#endif // VARIABLE_H
