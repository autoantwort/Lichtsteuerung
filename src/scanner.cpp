#include "scanner.h"

Scanner::Scanner(QVector3D scannerPosition, float scannerRotationInRoomInDegree, float scannerTiltInDegree)
    : scannerPosition(scannerPosition)
    , scannerRotationInRoom(scannerRotationInRoomInDegree)
    , scannerTilt(scannerTiltInDegree) {
    recomputeTransformation();
}

Modules::PointToResult Scanner::computeDmxValuesForPointTo(float x, float y, float z) {
    auto result = computeAngleForStepMotorsForPositionOnMap(x, y, z);
    auto tilt = getDMXValueForTilt(result.second);
    auto rotation = getDMXValueForRotation(result.first);
    return {tilt.second, rotation.second, tilt.first && rotation.first};
}

std::pair<float, float> Scanner::computeAngleForStepMotorsForPositionOnMap(float x, float y, float z) {
    auto p = computeAngleForStepMotors(QVector3D(x, y, z));
    return {qRadiansToDegrees(p.first), 90 + qRadiansToDegrees(p.second)};
}

float interpolate(float x1, float y1, float x2, float y2, float x) {
    float m = (y2 - y1) / (x2 + x1);
    float b = y1 - (m * x1);
    return m * x + b;
}

std::pair<bool, unsigned char> convertToDmxValue(float dmxValue) {
    dmxValue = std::round(dmxValue);
    if (dmxValue < 0) {
        return {false, 0};
    } else if (dmxValue > 255) {
        return {false, 255};
    } else {
        return {true, static_cast<unsigned char>(dmxValue)};
    }
}

std::pair<bool, unsigned char> Scanner::getDMXValueForRotation(float rotation) {
    auto dmxValue = interpolate(0, 128, rotationForDmxValue, dmxValueForRotation, rotation);
    return convertToDmxValue(dmxValue);
}

std::pair<bool, unsigned char> Scanner::getDMXValueForTilt(float tilt) {
    auto dmxValue = interpolate(angleForDmxTiltValue1, dmxValueForTiltAngle1, angleForDmxTiltValue2, dmxValueForTiltAngle2, tilt);
    return convertToDmxValue(dmxValue);
}

void Scanner::recomputeTransformation() {
    QMatrix4x4 trans;
    // reverse order:
    trans.rotate(-scannerTilt, 0, 1, 0);          // scanner neigen
    trans.scale(-1, 1, 1);                        // x-Achse flippen
    trans.rotate(scannerRotationInRoom, 0, 0, 1); // Scanner richtig in Raum drehen
    trans.scale(1, 1, -1);                        // z-Achse flippen
    trans.translate(-scannerPosition);            // scanner in den Ursprung
    transformationMatrix = trans;
}

std::pair<float, float> Scanner::computeAngleForStepMotors(QVector3D positionInRoom){
    auto vecFromScanner = transformationMatrix.map(positionInRoom);
    vecFromScanner.normalize();
    auto beta = std::asin(-std::sqrt((vecFromScanner.x() - 1) / -2.f));
    auto b = std::cos(beta) * std::sin(beta);
    auto alpha = std::asin(vecFromScanner.y() / (2.f * b));
    return {alpha, beta};
}
