#ifndef SCANNER_HPP
#define SCANNER_HPP

namespace Modules {

    struct PointToResult{
        unsigned char dmxValueForTilt = 128;
        unsigned char dmxValueForRotation = 128;
        bool canBeReached = false;
    };

    class IScanner{
    public:
        virtual void setScannerPosition(float x, float y, float z) = 0;
        virtual void setScannerTilt(float tiltInDegree) = 0;
        virtual void setScannerRotationInRoom(float scannerRotationInDegrees) = 0;
        virtual void setDmxValuesForTiltAngles(float angle1, unsigned char dmxValue1,float angle2, unsigned char dmxValue2) = 0;
        virtual void setDmxValueForRotation(float rotation, unsigned char dmxValue) = 0;
        virtual PointToResult computeDmxValuesForPointTo(float x, float y, float z = 0) = 0;
        virtual ~IScanner() = default;
    };

}

#endif // SCANNER_HPP
