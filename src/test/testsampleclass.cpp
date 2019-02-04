#include "testsampleclass.h"
#include "audio/sample.h"


namespace Test{


    void testSampleClass(){
        using namespace Audio;
        Sample<int,10> s;
        using namespace std;
        std::array<int,10> v;
        std::iota(v.begin(),v.end(),0);
        //for_each(v.cbegin(),v.cend(),[](auto i ){qDebug() << i;});
        s.addData(v.begin(),v.end());
        //for_each(s.getArray().cbegin(),s.getArray().cend(),[](auto i ){qDebug() << i;});
        for (int i = 0; i < 10; ++i) {
            Q_ASSERT(s.getArray().at(i)==i);
        }
        s.addData(v.begin(),v.end(),1,1);
        //for_each(s.getArray().cbegin(),s.getArray().cend(),[](auto i ){qDebug() << i;});
        for (int i = 5, j=1; i < 10; ++i,j+=2) {
            //qDebug()<<i << ':'<<j;
            Q_ASSERT(s.getArray().at(i)==j);
        }
        s.addData(v.begin(),v.end(),4,4);
        //qDebug();
        //for_each(s.getArray().cbegin(),s.getArray().cend(),[](auto i ){qDebug() << i;});
        Q_ASSERT(s.data()[8]==4);
        Q_ASSERT(s.data()[9]==9);
        s.addData(v.begin(),v.end(),8,2);
        //qDebug();
        //for_each(s.getArray().cbegin(),s.getArray().cend(),[](auto i ){qDebug() << i;});
        Q_ASSERT(s.data()[8]==9);
        Q_ASSERT(s.data()[9]==2);
    }
}
