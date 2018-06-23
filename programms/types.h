#ifndef TYPES_H
#define TYPES_H

#include "property.hpp"
#include <vector>
#include <stdexcept>
#include <assert.h>

namespace Modules {


    enum ValueType{Brightness, RGB};

    typedef int time_diff_t;

    typedef unsigned char brightness_t;
    static_assert (sizeof (brightness_t)==1, "size of unsigned char is not 1");

    struct rgb_t{
        union{
            brightness_t r;
            brightness_t red;
        };
        union{
            brightness_t g;
            brightness_t green;
        };
        union{
            brightness_t b;
            brightness_t blue;
        };
    };
    static_assert (sizeof (rgb_t)==3, "size of rgb_t is not 3");


    /**
     * @brief typeToEnum return for the brightness_t and rgb_t the right enum value
     * @return the enum value represents the real type
     */
    template<typename VT>
    ValueType typeToEnum(){
        static_assert (std::is_same<VT,brightness_t>::value || std::is_same<VT,rgb_t>::value, "Not a value type");
        if(std::is_same<VT,brightness_t>::value){
            return Brightness;
        }else{
            return RGB;
        }
    }


    /**
     * @brief getSizeOfEnumType return for Brightness and RGB the size of the type brightness_t and rwg_t
     * @param t the type
     * @return the size of the type
     */
    inline size_t getSizeOfEnumType(ValueType t){
        switch(t){
        case Brightness:
            return sizeof(brightness_t);
        case RGB:
            return sizeof(rgb_t);
        }
        assert(false && "Nicht im Enum");
    }

    class PropertyBase : public Serilerizeable{
    protected:
        std::vector<Property*> properties;
    public:
        const std::vector<Property*>& getProperties()const{return properties;}
        void load(const LoadObject &l)override{
            for(auto & p : properties){
                p->load(l);
            }
        }
        void save(SaveObject &s) const override{
            for(auto & p : properties){
                p->save(s);
            }
        }
    };



    class OutputDataProducer{
        const ValueType outputDataType;
    public:
        OutputDataProducer(const ValueType outputDataType):outputDataType(outputDataType){}
        virtual ~OutputDataProducer() = default;
        inline ValueType getOutputType()const{return outputDataType;}        
        virtual unsigned int getOutputLength()const=0;
        /**
         * @brief getOutputData gibt einen Pointer auf die Ausgabedaten zurück, diese müssen sizeOf(Type) * outputLength groß sein
         * @return
         */
        virtual void* getOutputData()=0;
    };

    class InputDataConsumer{
        const ValueType inputDataType;
    public:
        InputDataConsumer(const ValueType inputDataType):inputDataType(inputDataType){}
        virtual ~InputDataConsumer() = default;
        /**
         * @brief getInputType gibt den Typ der Eingabedaten an.
         * @return brighness_t or rgb_t
         */
        inline ValueType getInputType()const{return inputDataType;}
        virtual unsigned int getInputLength()const=0;
        /**
         * @brief setInputData setzt die eingabedaten, angezeigt werden sie mit show
         * @param data Die daten die angezeigt werden soll
         * @param index Der start index ab dem die daten in den input array geschrieben werden
         * @param length die anzahl der daten in void * data
         */
        virtual void setInputData(void*data, unsigned int index, unsigned int length) = 0;
    };



    /**
     * @brief The BoundedArray class maps index acces lower 0 to 0 and index access greater length-1 to length-1
     */
    template <typename Type>
    class BoundedArray{
        Type * data;
        size_t length;
    public:
        BoundedArray(Type* data, size_t length):data(data),length(length){}
        Type& operator[](std::size_t idx)       { return idx<length?data[idx]:data[length-1]; }
        const Type& operator[](std::size_t idx) const { return idx<length?data[idx]:data[length-1]; }
        Type& operator[](int i)       { if(i>=0){if(i<length){return data[i];}else{return data[length-1];}}else{return data[0];}}
        const Type& operator[](int i) const { if(i>=0){if(i<length){return data[i];}else{return data[length-1];}}else{return data[0];}}
    };


    using namespace std::string_literals;

    class IndexOutOfBoundsException : public std::exception{
    public:
        const int min, max, index;
        std::string message;
        IndexOutOfBoundsException(int min, int max, int index):min(min),max(max),index(index),message("IndexOutOfBoundsException min: "s + std::to_string(min) + " max: " + std::to_string(max) + " your index: " + std::to_string(index)){}
        virtual const char* what() const noexcept override{
            return message.c_str();
        }
    };

    /**
     * @brief The CheckedArray class does index bounds checking
     */
    template <typename Type>
    class CheckedArray{
        size_t length;
        Type * data;
    public:
        CheckedArray(Type* data, size_t length):data(data),length(length){}
        Type& operator[](std::size_t idx)       { return idx<length?data[idx]:throw IndexOutOfBoundsException(0,length-1,idx); }
        const Type& operator[](std::size_t idx) const { return idx<length?data[idx]:throw IndexOutOfBoundsException(0,length-1,idx); }
        Type& operator[](int i)       { if(i>=0 && i<length){return data[i];}else{throw IndexOutOfBoundsException(0,length-1,i);}}
        const Type& operator[](int i) const { if(i>=0 && i<length){return data[i];}else{throw IndexOutOfBoundsException(0,length-1,i);}}
    };

    class Named{
    public:
        virtual const char* getName()const = 0;
        virtual ~Named() = default;
    };

}
#endif // TYPES_H
