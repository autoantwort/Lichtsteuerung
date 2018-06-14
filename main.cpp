#include <QCoreApplication>
#include <QDebug>
#include <id.h>
#include <device.h>
#include "channel.h"
#include "applicationdata.h"
#include "programm.h"
#include <QMetaProperty>
#include "dmxchannelfilter.h"
#include <chrono>
#include <QEasingCurve>
#include <cmath>
#include <QCryptographicHash>
#include "usermanagment.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "device.h"
#include <QQmlContext>
#include <QFileInfo>
#include <limits>
#include "modelmanager.h"
#include "channelprogrammeditor.h"
#include "mapview.h"
#include "mapeditor.h"
#include "controlitem.h"
#include "controlpanel.h"
#include "errornotifier.h"
#include <QQuickView>
#include <QLibrary>
#include "HardwareInterface.h"
#include "settings.h"
#include <QDir>
#include "driver.h"
#include "graph.h"
#include <QTimer>
#include "oscillogram.h"
#include "colorplot.h"

#include "aubio/aubio.h"

#define WIN_ONLY(a)

#ifdef Q_OS_WIN

#ifndef _MSC_VER
#include "audio_fft.h"
#else /*_MSC_VER*/
#include "audio_fft.h"
#endif /*_MSC_VER*/
#define WIN_ONLY(a) a

int channel;
const int fft_size=960;
AudioFFT audioFft(fft_size);
std::atomic_bool done (false);
float fftOutput[fft_size/2];
volatile float fftOutputV[fft_size/2];
float data960[960*2];
volatile float data960V[960*2];
aubio_tempo_t * tempo = nullptr;
aubio_onset_t * onset = nullptr;
aubio_onset_t * onset2 = nullptr;
#define AUDIO_FFT_SIZE 1024
void init(int c){
    channel = c;
    Colorplot::getLast()->setBlockSize(300);
    tempo = new_aubio_tempo("default",AUDIO_FFT_SIZE,480,48000);
    onset = new_aubio_onset("energy",AUDIO_FFT_SIZE,480,48000);
    onset2 = new_aubio_onset("hfc",AUDIO_FFT_SIZE,480,48000);
    qDebug()<< "init tempo";
}

fvec_t * input_data = new_fvec(AUDIO_FFT_SIZE);
fvec_t * output_data = new_fvec(1);
fvec_t * onset_input_data = new_fvec(480);
fvec_t * onset_output_data = new_fvec(1);
fvec_t * onset_output_data2 = new_fvec(1);

int counter480, counter960;
void callback(float* data, unsigned int frames, bool*d){
    if(data){
        if(channel>2){
            int index = -1;
            for (int i = 0; i < frames*2; ++i) {
                data[i] = data[++index];
                data[++i] = data[++index];
                index+=6;
            }
        }
        auto d = qDebug();
        for(int i = 0;i<frames*2;i++){
            data[i] *= 400;
        }
        /*for(int i = 0; i<frames;++i){
            data960[i] = data[i*2];
        }*/



        if(frames==480){
            //memmove(data960,data960+960,960);
            //memmove(data960+960,data,960);
            for(int i = 0; i<960;++i){
                data960[i] = data960[i+960];
            }
            for(int i = 0; i<960;++i){
                data960[i+960] = data[i];
            }
            counter480++;
        }else{
            //memcpy(data960,data,960*2);
            counter960++;
        }
        audioFft.analyse(data960,channel,fftOutput);
        for(int i = 0; i < fft_size/2;++i){
            fftOutputV[i] = fftOutput[i];
        }
        if(Graph::getLast())
            Graph::getLast()->showData(fftOutput,960/2);

        if(tempo){
            for (int i = 0;i<AUDIO_FFT_SIZE-960;++i) {
                input_data->data[i] = input_data->data[i+480];
            }
            for (int i = 0; i< std::min(960,AUDIO_FFT_SIZE);++i) {
                input_data->data[i+std::max(AUDIO_FFT_SIZE-960,0)] = data960[i*2];
            }
            for (int i = 0 ;i<480;++i) {
                onset_input_data->data[i] = data960[(480+i)*2];
            }

            aubio_onset_do(onset,onset_input_data,onset_output_data);

            aubio_onset_do(onset2,onset_input_data,onset_output_data2);
            if(onset_output_data->data[0] > 0.001f){
                qDebug() << "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMM Onset with "<< onset_output_data->data[0] << "  MMMMMMMMMMMMMMMMMMMMMM";
            }

            aubio_tempo_do(tempo,input_data,output_data);
            int beat = aubio_tempo_was_tatum(tempo);
            qDebug()<<"Beat: "<< output_data->data[0] << ' '
                   <<beat<< " BPM:"<<aubio_tempo_get_bpm(tempo);
            if(beat == 1){
                qDebug()<< " ---- Tantum ----";
            }else if(output_data->data[0] > 0.2f){
                qDebug()<< " ########### BEAT #########";
            }
        }

        if(Colorplot::getLast()){
            Colorplot::getLast()->startBlock();
            for (int i = 0; i < 285; ++i) {
                Colorplot::getLast()->pushDataToBlock(fftOutputV[i]);
            }
            qDebug() << fftOutputV[30];
            for(int i = 0 ; i< 5 ; ++i){
                Colorplot::getLast()->pushDataToBlock(onset_output_data2->data[0]*5000);
            }
            for(int i = 0 ; i< 5 ; ++i){
                Colorplot::getLast()->pushDataToBlock(onset_output_data->data[0]*5000);
            }
            for(int i = 0 ; i< 5 ; ++i){
                Colorplot::getLast()->pushDataToBlock(output_data->data[0]*10000);
            }
            Colorplot::getLast()->endBlock();
        }

        for(int i = 0 ; i < 960*2;++i){
            data960V[i] = data960[i];
        }
        if(Oscillogram::getLast())
            Oscillogram::getLast()->showData(data960V,960*2);
            /*for(int i = 0; i< fft_size/2;++i){
                std::cout << fftOutput[i]<< ' ';
            }
            std::cout<<std::endl;*/

        //qDebug()<<"480 : "<<counter480 << "  960 : "<<counter960;
    }
    *d = done.load();
}





#endif /*Q_OS_WIN*/

int main(int argc, char *argv[])
{

    std::thread captureAudioThread;

    class CatchingErrorApplication : public QGuiApplication{
    public:
        CatchingErrorApplication(int &argc, char **argv):QGuiApplication(argc,argv){}
        virtual ~CatchingErrorApplication(){}
        virtual bool notify(QObject* receiver, QEvent* event)
        {
            try {
                return QGuiApplication::notify(receiver, event);
            } catch (std::exception &e) {
                qCritical("Error %s sending event %s to object %s (%s)",
                    e.what(), typeid(*event).name(), qPrintable(receiver->objectName()),
                    receiver->metaObject()->className());
                QString error = QString("Error ") +e.what()+"sending event "+typeid(*event).name()+" to object "+ qPrintable(receiver->objectName())+" "+ receiver->metaObject()->className();
                ErrorNotifier::get()->newError(error);
            } catch (...) {
                qCritical("Error <unknown> sending event %s to object %s (%s)",
                    typeid(*event).name(), qPrintable(receiver->objectName()),
                    receiver->metaObject()->className());
                QString error = QString("Error ") + "<unknown> sending event "+typeid(*event).name()+" to object "+ qPrintable(receiver->objectName())+" "+ receiver->metaObject()->className();
                ErrorNotifier::get()->newError(error);
            }

            return false;
        }
    };



//    auto defaultFormat = QSurfaceFormat::defaultFormat();
//    defaultFormat.setSamples(8);
//    QSurfaceFormat::setDefaultFormat(defaultFormat);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    CatchingErrorApplication app(argc, argv);
    QQmlApplicationEngine engine;
    ControlPanel::setQmlEngine(&engine);
    //qmlRegisterType<const ChannelVector*>("my.models",1,0,"ChannelVector");
    qmlRegisterType<ChannelProgrammEditor>("custom.licht",1,0,"ChannelProgrammEditor");
    qmlRegisterType<MapView>("custom.licht",1,0,"MapView");
    qmlRegisterType<MapEditor>("custom.licht",1,0,"MapEditor");
    qmlRegisterType<ControlPanel>("custom.licht",1,0,"ControlPanel");
    qmlRegisterType<Graph>("custom.licht",1,0,"Graph");
    qmlRegisterType<Oscillogram>("custom.licht",1,0,"Oscillogram");
    qmlRegisterType<Colorplot>("custom.licht",1,0,"Colorplot");
    //qmlRegisterType<ErrorNotifier>("custom.licht",1,0,"ErrorNotifier");
    qmlRegisterType<ControlItem>("custom.licht.template",1,0,"ControlItemTemplate");
    qmlRegisterType<ControlItemData>("custom.licht.template",1,0,"ControlItemData");
    qmlRegisterType<DimmerGroupControlItemData>("custom.licht",1,0,"DimmerGroupControlItemData");
    qmlRegisterType<DMXChannelFilter>("custom.licht",1,0,"DMXChannelFilter");
    qRegisterMetaType<DMXChannelFilter::Operation>("Operation");
    qmlRegisterUncreatableType<UserManagment>("custom.licht",1,0,"Permission","Singletone in c++");
    qRegisterMetaType<UserManagment::Permission>("Permission");
    Settings settings;
    settings.setJsonSettingsFilePath("QTJSONFile.json");
    QFile file(settings.getJsonSettingsFilePath());
    file.copy(settings.getJsonSettingsFilePath()+"_"+QDateTime::currentDateTime().toString("dd.MM.YYYY HH:mm:ss"));

//#warning Dont use IDBase<xxxxx>::getAllIDBases() in this file. It will crash the aplication when its closing

    auto after = ApplicationData::loadData(file);


    QStringList dataList;
    const QMetaObject &mo = QEasingCurve::staticMetaObject;
    QMetaEnum metaEnum = mo.enumerator(mo.indexOfEnumerator("Type"));
    for (int i = 0; i < QEasingCurve::NCurveTypes - 1; ++i) {
        dataList.append(metaEnum.key(i));
    }

    app.connect(&app,&QGuiApplication::lastWindowClosed,[&](){
        QFile savePath(settings.getJsonSettingsFilePath());
        ApplicationData::saveData(savePath);
        Driver::stopAndUnloadDriver();
        qDebug()<<"is captureAudioThread jionable : "<<captureAudioThread.joinable();
        if(captureAudioThread.joinable()){
            WIN_ONLY(done.store(true);)
            captureAudioThread.join();
        }
        del_aubio_tempo(tempo);
        tempo = nullptr;
        del_fvec(input_data);
        del_fvec(output_data);
        del_aubio_onset(onset);
        onset = nullptr;
        del_aubio_onset(onset2);
        onset2 = nullptr;
        del_fvec(onset_input_data);
        del_fvec(onset_output_data);
        del_fvec(onset_output_data2);
    });
    settings.connect(&settings,&Settings::driverFilePathChanged,[&](){
        Driver::loadAndStartDriver(settings.getDriverFilePath());
    });
    settings.connect(&settings,&Settings::updatePauseInMsChanged,[&](){
        if(Driver::getCurrentDriver()){
            Driver::getCurrentDriver()->setWaitTime(std::chrono::milliseconds(settings.getUpdatePauseInMs()));
        }
    });


    engine.rootContext()->setContextProperty("ModelManager",new ModelManager());
    engine.rootContext()->setContextProperty("easingModel",dataList);
    engine.rootContext()->setContextProperty("ErrorNotifier",ErrorNotifier::get());
    engine.setObjectOwnership(ErrorNotifier::get(),QQmlEngine::CppOwnership);
    engine.rootContext()->setContextProperty("devicePrototypeModel",IDBaseDataModel<DevicePrototype>::singletone());
    engine.rootContext()->setContextProperty("deviceModel",IDBaseDataModel<Device>::singletone());
    engine.rootContext()->setContextProperty("programmModel",IDBaseDataModel<Programm>::singletone());
    engine.rootContext()->setContextProperty("programmPrototypeModel",IDBaseDataModel<ProgrammPrototype>::singletone());
    engine.rootContext()->setContextProperty("userModel",IDBaseDataModel<User>::singletone());
    engine.rootContext()->setContextProperty("UserManagment",UserManagment::get());
    engine.rootContext()->setContextProperty("Settings",&settings);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));


    // laden erst nach dem laden des qml ausführen
    after();

#ifdef Q_OS_WIN
    typedef int (*capture)(void(*)(int),void(*)(float*,unsigned int, bool*)) ;
    auto func = reinterpret_cast<capture>(QLibrary::resolve(settings.getAudioCaptureFilePath(),"captureAudio"));
    if(func){
        captureAudioThread = std::thread([&](){
            qDebug()<<"Start capture";
            qDebug()<<"Capture Result  :  "<<func(&init,&callback);

        });
    }

#endif /*Q_OS_WIN*/


    // Treiber laden
//#define USE_DUMMY_DRIVER
#ifndef USE_DUMMY_DRIVER
    /*if(!Driver::loadAndStartDriver(settings.getDriverFilePath())){
        ErrorNotifier::showError("Cant start driver.");
    }
    Driver::getCurrentDriver()->setWaitTime(std::chrono::milliseconds(1500000));
    */
#else
#include "test/DriverDummy.h"

    DriverDummy driver;
    driver.setSetValuesCallback([](unsigned char* values, int size, double time){
        DMXChannelFilter::initValues(values,size);
        Programm::fill(values,size,time);
        DMXChannelFilter::filterValues(values,size);
    });
    driver.setWaitTime(std::chrono::seconds(5));
    driver.init();
    driver.start();
#endif


    QTimer timer;
    timer.setInterval(15);
    QObject::connect(&timer,&QTimer::timeout,[&](){
        if(Graph::getLast())
            Graph::getLast()->update();
        if(Oscillogram::getLast())
            Oscillogram::getLast()->update();
        if(Colorplot::getLast())
            Colorplot::getLast()->update();
    });
    timer.start();

    //ControlPanel::getLastCreated()->addDimmerGroupControl();
    return app.exec();
}
