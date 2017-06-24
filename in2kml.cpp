//
// Created by zhengshi on 5/30/17.
// Get a block of separators from lidar data/ .in file.
//

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

//const int kFileNum = 4;
//const std::vector<std::string> kFileNames = {"torrey", "drone", "drone_new", "morena_long"};
//const std::vector<std::string> kInFileNames = {"data/in/torrey.in", "data/in/drone.in", "data/in/drone_new.in", "data/in/morena_long.in"};
//const std::vector<std::string> kOutFileNames = {"data/kml/torrey_test.kml", "data/kml/drone_test.kml", "data/kml/drone_new_test.kml", "data/kml/morena_long_test.kml"};
//const bool latLon = true;
const int kFileNum = 1;
const std::vector<std::string> kFileNames = {"805.22-8.14c_With_height_Dense"};
const std::vector<std::string> kInFileNames = {"data/in/805.22-8.14c_With_height_Dense.in"};
const std::vector<std::string> kOutFileNames = {"data/kml/805.22-8.14c_With_height_Dense.kml"};
const bool latLon = false;                                   // txt (lat,lon): true, in (lon,lat): false
const double latMin = -90, latMax = 90;
const double lonMin = -180, lonMax = 180;
//const double lonMin = -117.18408, lonMax = -117.00124;
const int mode = 1;                                         // 1: in2kml, 2: txt2kml

int main(int argc, char * argv[]) {
//    int fileNum = argc;
//    char * fileNames[] = argv;

    int fileNum = kFileNum;
    std::vector<std::string> fileNames = kFileNames;
    std::vector<std::string> inFileNames = kInFileNames;
    std::vector<std::string> outFileNames = kOutFileNames;

    std::string line;
    char comma;

    for (int i = 0; i < fileNum; i++) {
        std::fstream inFile, outFile;
        inFile.open(inFileNames[i], std::ios::in);
        outFile.open(outFileNames[i], std::ios::out);

        if (inFile.is_open() && outFile.is_open()) {
            outFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            outFile << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
            outFile << "  <Document>\n";
            outFile << "    <name>" << fileNames[i] << "</name>\n";

            int lineSum, lineId, pointNum, roadType, period;        // Period determines down-sample rate.
            switch (mode) {
                case 1:
                    inFile >> pointNum >> roadType;
                    period = 1;
                    break;
                case 2:
                    inFile >> lineSum;
                    inFile >> lineId >>pointNum >> roadType;
                    period = 20;
                    break;
                default:
                    std::cout << "You need to set a correct mode.\n";
                    exit(1);
            }

            int lineNum = 0;
            while (!inFile.eof()) {
                outFile << "    <Placemark>\n";
                lineNum++;
                outFile << "      <name>Line " << lineNum << "</name>\n";
                outFile << "      <description>" << roadType << "</description>\n";     // type
                // No styleUrl.
                outFile << "      <LineString>\n";
                outFile << "        <tessellate>1</tessellate>\n";                 // Meaning unknown.
                outFile << "        <coordinates>\n";

                bool inRegion = false, lastInRegion = false;
                for (int j = 0; j < pointNum; j++) {
//                    float lon, lat, alt;
//                    inFile >> lon >> comma >> lat >> comma >> alt;
//                    printf("%f %f %f\n", lon, lat, alt);
//                    outFile << "          " << lon << "," << lat << ",0\n";
                    std::string coordinate;
                    inFile >> coordinate;
                    if (j % period == 0) {
                        if (latLon) {                                                   // kml needs to be in format of (lon,lat)
                            unsigned long commaPos1 = coordinate.find(",");
                            unsigned long commaPos2 = coordinate.find(",", commaPos1 + 1);
                            std::string tmpCoordinate = coordinate.substr(commaPos1 + 1, commaPos2 - commaPos1 - 1)
                                                        + "," + coordinate.substr(0, commaPos1)
                                                        + "," + coordinate.substr(commaPos2 + 1);
                            coordinate = tmpCoordinate;
                        }
                        unsigned long commaPos1 = coordinate.find(",");
                        unsigned long commaPos2 = coordinate.find(",", commaPos1 + 1);
                        float lonFloat = std::stof(coordinate.substr(0, commaPos1));
                        float latFloat = std::stof(coordinate.substr(commaPos1 + 1, commaPos2 - commaPos1 - 1));
//                    lastInRegion = inRegion;
                        inRegion = latMin < latFloat && latFloat < latMax && lonMin < lonFloat && lonFloat < lonMax;
//                        if (inRegion ||
//                            lastInRegion) {                                     // Extend by one point. For the convenience of reconnecting.
                        if (inRegion) {
                            outFile << "          " << coordinate << "\n";
                        };
                    }
                }

                outFile << "        </coordinates>\n";
                outFile << "      </LineString>\n";
                outFile << "    </Placemark>\n";

                switch (mode) {
                    case 1:
                        inFile >> pointNum >> roadType;
                        break;
                    case 2:
                        inFile >> lineId >>pointNum >> roadType;
                        break;
                    default:
                        std::cout << "You need to set a correct mode.\n";
                        exit(1);
                }
            }

            outFile << "  </Document>\n";
            outFile << "</kml>\n";
        } else {
            std::cout << "Unable to open file\n";
        }
        inFile.close();
        outFile.close();
    }

    return 0;
}
