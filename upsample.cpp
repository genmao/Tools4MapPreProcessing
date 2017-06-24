//
// Created by zhengshi on 6/16/17.
//

#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <limits>
#include <cmath>
//#include <math.h>
// uncomment to disable assert()
// #define NDEBUG
#include <cassert>
#include "utils.h"

const int kSampleRate = 300;

const std::string kIntactDataFileName = "data/txt/map_all.txt";
const std::string kSparseLineFileName = "data/in/805.22-8.14c_With_height.in";
const std::string kDenseLineFileName = "data/in/805.22-8.14c_With_height_Dense.in";

// (point on line 0, points on line 1, ...)
static std::vector<std::vector<point2d>> allLines;          // Format: x, y, height
static std::vector<std::vector<point2d>> sparseLines;       // Format: x, y, height
static std::vector<std::vector<point2d>> denseLines;        // Format: lat, lon, height
static std::vector<int> denseLinesType;
static std::vector<std::vector<point2d>> allLinesLatLon;    // Format: lat, lon, height
// (block corner) -> ([(lineNumber, pointNumber), (), ...])
// Note: point2d equal is determined by its x and y only.
static std::map<point2d, std::vector<std::pair<int, int>>> allPointsByBlocks;

char comma;
std::string doodle;


void utils::readInIntactData(std::fstream& intactDataFile) {
    std::cout << std::setprecision(15);

    allLines.clear();
    int numOfLine;
    intactDataFile >> numOfLine;
    for (int lineNumber = 0; lineNumber < numOfLine; lineNumber++) {
        int lineId, numOfPoints, roadType;
        intactDataFile >> lineId >> numOfPoints >> roadType;
//        std::cm bout << lineId << ' ' << numOfPoints << ' ' << roadType << '\n';
        std::vector<point2d> line;
        std::vector<point2d> lineLatLon;
        for (int pointNumber = 0; pointNumber < numOfPoints; pointNumber++) {
            double lat, lon, height, x, y;
            intactDataFile >> lat >> comma >> lon >> comma >> height;       // Format: lat,lon,height
//            std::cout << lat << ',' << lon << ',' << height << '\n';
            latlon2xy(lat, lon, x, y);
            point2d point(x, y);
            point.h = height;
            line.push_back(point);
            point2d blockCornerPoint(floor(x / blockSize) * blockSize, floor(y / blockSize) * blockSize);
            allPointsByBlocks[blockCornerPoint].push_back(std::make_pair(lineNumber, pointNumber));
                                            // No guarantee that lineIds are unique, so use lineNumber instead.
            point2d pointLatLon(lat, lon);
            pointLatLon.h = height;
            lineLatLon.push_back(pointLatLon);
        }
        allLines.push_back(line);
        allLinesLatLon.push_back(lineLatLon);
    }
    intactDataFile >> doodle;
    if (!intactDataFile.eof()) {
        std::cout << "Not all data are read in.\n";
    }

    std::cout.copyfmt(std::ios(NULL));
}

// Store lines of type 0, 1 in sparseLines, and lines of type 2, 3, 4 in denseLines (regarded as having been processed)
void utils::readInSparseLine(std::fstream& sparseLineFile) {
    std::cout << std::setprecision(15);

    sparseLines.clear();
    int numOfPoints, roadType;
    sparseLineFile >> numOfPoints >> roadType;
    std::vector<int> linesType01;
    while (!sparseLineFile.eof()) {
        std::vector<point2d> line;
        std::vector<point2d> lineLatLon;
        for (int pointIter = 0; pointIter < numOfPoints; pointIter++) {
            double lat, lon, height, x, y;
            sparseLineFile >> lon >> comma >> lat >> comma >> height;       // Format: lon, lat, height
            latlon2xy(lat, lon, x, y);
            point2d point(x, y);
            point.h = height;
            line.push_back(point);
            point2d pointLatLon(lat, lon);
            pointLatLon.h = height;
            lineLatLon.push_back(pointLatLon);
        }
        if (roadType < 2) {                // Ignore partitions, start point, and end point.
            linesType01.push_back(roadType);
            sparseLines.push_back(line);
        } else {                            // Redirect to output file.
            denseLinesType.push_back(roadType);
            denseLines.push_back(lineLatLon);
        }
        sparseLineFile >> numOfPoints >> roadType;
//        std::cout << numOfPoints << '\n';
    }
    for (auto iter : linesType01)
        denseLinesType.push_back(iter);

    std::cout.copyfmt(std::ios(NULL));
}


double distance(point2d point1, point2d point2) {
    return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2));
}
// Consider the given point and its closest point are on two corners of a block. It is sufficient to search for 5*5 blocks around!!!
void locatePoint(point2d point, int& lineNumber, int& pointNumber) {
    lineNumber = -1, pointNumber = -1;
    double blockCornerX = floor(point.x / blockSize) * blockSize, blockCornerY = floor(point.y / blockSize) * blockSize;
    double closestDistance = std::numeric_limits<double>::max();

    for (int offsetX = -2; offsetX <= 2; offsetX++) {
        for (int offsetY = -2; offsetY <= 2; offsetY++) {
            point2d blockCornerPoint(blockCornerX + offsetX * blockSize, blockCornerY + offsetY * blockSize);
            for (std::pair<int, int> pointInBlockId : allPointsByBlocks[blockCornerPoint]) {
                point2d pointInBlock = allLines[pointInBlockId.first][pointInBlockId.second];
                // Can't use == operator because of precision loss of google map.
                if (distance(pointInBlock, point) < closestDistance) {
                    closestDistance = distance(pointInBlock, point);
                    lineNumber = pointInBlockId.first, pointNumber = pointInBlockId.second;
                }
            }
        }
    }


}
void utils::handleSparseLine(int sampleRate) {
    for (int lineIter = 0; lineIter < sparseLines.size(); lineIter++) {
        assert(sparseLines[lineIter].size() >= 2);
        std::vector<point2d> lineLatLon;
        int count = 0;
        for (int pointIter = 0; pointIter < sparseLines[lineIter].size() - 1; pointIter++) {
            point2d point1 = sparseLines[lineIter][pointIter], point2 = sparseLines[lineIter][pointIter + 1];
            int lineNumber1, pointNumber1, lineNumber2, pointNumber2;
            locatePoint(point1, lineNumber1, pointNumber1);   // 1 input, 2 output.
            locatePoint(point2, lineNumber2, pointNumber2);
//            double u,v;
//            xy2latlon(point1.x, point1.y, u, v);
//            printf("           %lf,%lf,%lf,%lf\n", u, v,point1.x, point1.y);
//            xy2latlon(allLines[lineNumber1][pointNumber1].x, allLines[lineNumber1][pointNumber1].y, u, v);
//            printf("           %lf,%lf,%f,%f\n", u, v,allLines[lineNumber1][pointNumber1].x, allLines[lineNumber1][pointNumber1].y);
//            printf("%d %d, %d %d\n", lineNumber1, pointNumber1, lineNumber2, pointNumber2);
            assert(pointNumber1 != -1 && pointNumber2 != -1 && "Point on sparse line is not found in intact data.");
            if (pointIter == 0) {
                lineLatLon.push_back(allLinesLatLon[lineNumber1][pointNumber1]);
            }
            // Dealing with segment (pointIter, pointIter+1].
            if (lineNumber1 == lineNumber2) {
                for (int pointNumber = pointNumber1 + 1; pointNumber <= pointNumber2; pointNumber++) {
                    if (count % sampleRate == 0 || pointNumber == pointNumber2)
                        lineLatLon.push_back(allLinesLatLon[lineNumber1][pointNumber]);
                    count++;
                }
                lineLatLon.push_back(allLinesLatLon[lineNumber2][pointNumber2]);
            } else {
// //                The method generate denser points, with the risk of producing acute angles.
//                for (int pointNumber = pointNumber1 + 1; pointNumber < allLines[lineNumber1].size(); pointNumber++) {
//                    if (count % sampleRate == 0)
//                        lineLatLon.push_back(allLinesLatLon[lineNumber1][pointNumber]);
//                    count++;
//                }
//                for (int pointNumber = 0; pointNumber <= pointNumber2; pointNumber++) {
//                    if (count % sampleRate == 0 || pointNumber == pointNumber2)
//                        lineLatLon.push_back(allLinesLatLon[lineNumber2][pointNumber]);
//                    count++;
//                }

// //               This method is more robust, at a cost of dropping more points.
//                lineLatLon.push_back(allLinesLatLon[lineNumber1][pointNumber1+1]);
                lineLatLon.push_back(allLinesLatLon[lineNumber2][pointNumber2]);
            }
        }
        denseLines.push_back(lineLatLon);
    }
}


void utils::outputDenseLine(std::fstream& denseLineFile) {
    denseLineFile << std::setprecision(15);
    for (int lineIter = 0; lineIter < denseLines.size(); lineIter++) {
        denseLineFile << denseLines[lineIter].size() << ' ' << denseLinesType[lineIter] << '\n';
        for (int pointIter = 0; pointIter < denseLines[lineIter].size(); pointIter++) {
            point2d point = denseLines[lineIter][pointIter];
            double lat = point.x, lon = point.y;
            denseLineFile << lon << ',' << lat << ',' << point.h << '\n';
        }
    }
    std::cout.copyfmt(std::ios(NULL));
}


void utils::restoreDenseLine(int sampleRate = 1) {
    std::fstream intactDataFile, sparseLineFile, denseLineFile;
    intactDataFile.open(kIntactDataFileName, std::ios::in);
    sparseLineFile.open(kSparseLineFileName, std::ios::in);
    denseLineFile.open(kDenseLineFileName, std::ios::out);
    if (!intactDataFile.is_open() || !sparseLineFile.is_open() || !denseLineFile.is_open()) {
        std::cout << "Cannot open files. Please check their names and paths.\n";
        return;
    }

    denseLines.clear();
    readInIntactData(intactDataFile);
    readInSparseLine(sparseLineFile);
    handleSparseLine(sampleRate);
    outputDenseLine(denseLineFile);

    intactDataFile.close();
    sparseLineFile.close();
    denseLineFile.close();
}


int main() {
    utils::restoreDenseLine(kSampleRate);
}