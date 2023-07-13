#ifndef SCANNER_H
#define SCANNER_H

#include "modelvector.h"
#include "modules/scanner.hpp"
#include <QMatrix4x4>
#include <qmath.h>

class Scanner : public QObject, public Modules::IScanner {
    Q_OBJECT
    QString name;
    QVector3D scannerPosition;
    float scannerRotationInRoom = 0;
    float scannerTilt = 0;
    QMatrix4x4 transformationMatrix;
    float rotationForDmxValue;
    float angleForDmxTiltValue1;
    float angleForDmxTiltValue2;
    unsigned char dmxValueForTiltAngle1;
    unsigned char dmxValueForTiltAngle2;
    unsigned char dmxValueForRotation;

public:
    static ModelVector<Scanner *> &getSingletonList() {
        static ModelVector<Scanner *> model;
        return model;
    }
    static Scanner *getByName(const std::string &name) {
        const auto &vector = getSingletonList().getVector();
        for (const auto &scanner : vector) {
            if (scanner->name.toStdString() == name) {
                return scanner;
            }
        }
        return nullptr;
    }
    static Scanner *getByNameOrCreate(const std::string &name) {
        auto scanner = getByName(name);
        if (!scanner) {
            scanner = new Scanner();
            scanner->name = QString::fromStdString(name);
            getSingletonList().push_back(scanner);
        }
        return scanner;
    }

public:
    Scanner() = default;
    Scanner(QVector3D scannerPosition, float scannerRotationInRoomInDegree, float scannerTiltInDegree);
    ~Scanner() override = default;
    const QMatrix4x4 &getTransformationMatrix() const { return transformationMatrix; }
    /**
     *          ______
     *        _/    _/  <- Scanner
     *      _/    _/
     *    _/    _/ ^
     *  _/    _/   | this angle
     * /_____/     ˇ
     * -------------------------  <- the ground
     *
     * @brief setScannerTilt sets the angle how tipped the scanner is
     * @param tilt The angle how tipped the scanner is in degree betreen 0 and 90 degree
     */
    void setScannerTilt(float tilt) override {
        Q_ASSERT(tilt >= 0 && tilt <= 90);
        if (std::abs(scannerTilt - tilt) < std::numeric_limits<float>::epsilon() * 2) return;
        scannerTilt = tilt;
        recomputeTransformation();
    }
    /**
     *
     *-y
     * ^  .___.
     * |  |___|
     * |  |   |
     * |  |   | <- Scanner angle in Room -90 Degree
     * |  |   |
     * |  |___|
     * |
     *  ----------------------------> x
     * |
     * |    /'\
     * |   /   \
     * |    \   \ <- Scanner angle in Room ca 50 Degree
     * |     \  /\
     * |      \/ /
     * ˇ       \/
     * y
     *
     * generally:
     *
     *         -90
     *          ^
     *          |
     * -180/<---+---> 0
     *  180     |
     *          ˇ
     *          90
     *
     *
     * @brief setScannerRotationInRoom Sets the Rotation in the Room in Degree for the scanner
     * @param scannerRotationInDegrees thr rotation in the room in degrees
     */
    void setScannerRotationInRoom(float scannerRotationInDegrees) override {
        if (std::abs(scannerRotationInRoom - scannerRotationInDegrees) < std::numeric_limits<float>::epsilon() * 2) return;
        scannerRotationInRoom = scannerRotationInDegrees;
        recomputeTransformation();
    }

    void setScannerPosition(float x, float y, float z) override {
        scannerPosition = QVector3D(x, y, z);
        recomputeTransformation();
    }

    void setDmxValuesForTiltAngles(float angle1, unsigned char dmxValue1, float angle2, unsigned char dmxValue2) override {
        angleForDmxTiltValue1 = angle1;
        dmxValueForTiltAngle1 = dmxValue1;
        angleForDmxTiltValue2 = angle2;
        dmxValueForTiltAngle2 = dmxValue2;
    }
    void setDmxValueForRotation(float rotation, unsigned char dmxValue) override {
        rotationForDmxValue = rotation;
        dmxValueForRotation = dmxValue;
    }
    Modules::PointToResult computeDmxValuesForPointTo(float x, float y, float z) override;
    std::pair<float, float> computeAngleForStepMotorsForPositionOnMap(float x, float y, float z = 0);

protected:
    std::pair<bool, unsigned char> getDMXValueForRotation(float rotation);
    std::pair<bool, unsigned char> getDMXValueForTilt(float tilt);
    void recomputeTransformation();
    std::pair<float, float> computeAngleForStepMotors(QVector3D positionInRoom);
};

#endif // SCANNER_H
