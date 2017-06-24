#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
using namespace std;

const int kIntersectionNum = 4;
const int kFileNum = 5;
const std::string kFileName = "40backward";
const std::string kInterFileName = "data/inter/inter.txt";
const std::string kTxtFileNames = "data/txt/40_miles_backward.txt";
const std::vector<std::string> kFileNames = {"40backward_0", "40backward_1", "40backward_2", "40backward_3", "40backward_4", "40backward_5"};
const std::vector<std::string> kInFileNames = {"data/in/40backward_0.in", "data/in/40backward_1.in", "data/in/40backward_2.in", "data/in/40backward_3.in", "data/in/40backward_4.in"};
const std::vector<std::string> kOutFileNames = {"data/kml/40backward_0.kml", "data/kml/40backward_1.kml", "data/kml/40backward_2.kml", "data/kml/40backward_3.kml", "data/kml/40backward_4.kml"};
const bool latLon = false;
bool myfunction (double i, double j) {return (i < j); }

int main(int argc, char const *argv[]) {
  int IntersectionNum = kIntersectionNum;
  std::string InterFileName = kInterFileName;
  std::string TxtFileNames = kTxtFileNames;
  std::fstream inFile, interFile;
  std::vector<std::fstream *> outFiles;
  inFile.open(TxtFileNames, std::ios::in);
  interFile.open(InterFileName, std::ios::in);
  std::vector<double> cutLons;
  std::string cutLon;
  //cout.precision(10);
  while (!interFile.eof()) {
    /* code */
    interFile >> cutLon;
    double cutlonDouble = std::stod(cutLon);
    //cout << cutLon << endl;
    cutLons.push_back(cutlonDouble);
  }
  cutLons.pop_back();
  //cout << cutLons[0] << endl;
  std::sort(cutLons.begin(), cutLons.end(), myfunction);
  //cout << cutLons[0] << endl;


  std::string outFileName = "data/in/" + kFileName + "_" + to_string(0) + ".in";
  std::fstream outFile0;
  outFile0.open(outFileName, std::ios::out);
  outFiles.push_back(&outFile0);

  outFileName = "data/in/" + kFileName + "_" + to_string(1) + ".in";
  std::fstream outFile1;
  outFile1.open(outFileName, std::ios::out);
  outFiles.push_back(&outFile1);

  outFileName = "data/in/" + kFileName + "_" + to_string(2) + ".in";
  std::fstream outFile2;
  outFile2.open(outFileName, std::ios::out);
  outFiles.push_back(&outFile2);

  outFileName = "data/in/" + kFileName + "_" + to_string(3) + ".in";
  std::fstream outFile3;
  outFile3.open(outFileName, std::ios::out);
  outFiles.push_back(&outFile3);

  outFileName = "data/in/" + kFileName + "_" + to_string(4) + ".in";
  std::fstream outFile4;
  outFile4.open(outFileName, std::ios::out);
  outFiles.push_back(&outFile4);
  /*
  for (int i = 1; i < kIntersectionNum + 1; i++) {
    std::string outFileName = "data/in/" + kFileNames + to_string(i) + ".in";
    std::fstream outFile;
    outFile << "hello world\n";
    outFile.open(outFileName, std::ios::out);
    outFiles.push_back(&outFile);
  }
  */

  /* txt to ins */

  int lineSum, lineId, pointNum, roadType;
  inFile >> lineSum;
  inFile >> lineId >> pointNum >> roadType;



  while (!inFile.eof()) {
    int lastFlag = 0;
    int flag = lastFlag;
    std::deque<std::string> myDeque;
    for (int j = 0; j < pointNum; j++) {
      std::string coordinate;
      std::string tmpCoordinate;
      double lastlatDouble, lastlonDouble;
      double latDouble, lonDouble;
      double tmpLat, tmpLon;
      inFile >> coordinate;
      if (j % 1 == 0) {      //downsample
        if (!latLon) {
            unsigned long commaPos1 = coordinate.find(",");
            unsigned long commaPos2 = coordinate.find(",", commaPos1 + 1);
            std::string tmpCoordinate = coordinate.substr(commaPos1 + 1, commaPos2 - commaPos1 - 1)
                                        + "," + coordinate.substr(0, commaPos1)
                                        + "," + coordinate.substr(commaPos2 + 1);
            coordinate = tmpCoordinate;
        }
        unsigned long commaPos1 = coordinate.find(",");
        unsigned long commaPos2 = coordinate.find(",", commaPos1 + 1);
        latDouble = stod(coordinate.substr(commaPos1 + 1, commaPos2 - commaPos1 -1));
        lonDouble = stod(coordinate.substr(0, commaPos1));
        //cout << "hello" << endl;
        for (int i = 0; i < cutLons.size(); i++) {
          if (lonDouble <= cutLons[i]) {
            flag = i;
            if (myDeque.empty()) {
              lastFlag = flag;
            }
            if (flag != lastFlag) {
              int size = myDeque.size() + 1;
              *outFiles[lastFlag] << size << " " << roadType << endl;
              while (!myDeque.empty()){
                *outFiles[lastFlag] << "          " << myDeque.front() << "\n";
                myDeque.pop_front();
              }
              tmpLon = cutLons[i];
              tmpLat = (lastlatDouble - latDouble) / (lastlonDouble - lonDouble) * tmpLon
                       + latDouble - (lastlatDouble - latDouble) / (lastlonDouble - lonDouble) * lonDouble;
              tmpCoordinate = to_string(tmpLon) + "," + to_string(tmpLat) + "," + "0";
              *outFiles[lastFlag] << "          " << tmpCoordinate << "\n";
              myDeque.push_back(tmpCoordinate);
            }
            myDeque.push_back(coordinate);
            //cout << coordinate << endl;
            lastFlag = flag;
            break;
          } else if (i == (cutLons.size() - 1)) {
            flag = i + 1;
            if (myDeque.empty()) {
              lastFlag = flag;
            }
            if (flag != lastFlag) {
              int size = myDeque.size() + 1;
              *outFiles[lastFlag] << size << " " << roadType << endl;
              while (!myDeque.empty()){
                *outFiles[lastFlag] << "          " << myDeque.front() << "\n";
                myDeque.pop_front();
              }
              tmpLon = cutLons[i];
              tmpLat = (lastlatDouble - latDouble) / (lastlonDouble - lonDouble) * tmpLon
                       + latDouble - (lastlatDouble - latDouble) / (lastlonDouble - lonDouble) * lonDouble;
              tmpCoordinate = to_string(tmpLon) + "," + to_string(tmpLat) + "," + "0";
              *outFiles[lastFlag] << "          " << tmpCoordinate << "\n";
              myDeque.push_back(tmpCoordinate);
            }
            myDeque.push_back(coordinate);
            lastFlag = flag;
            break;
          }
        }
      }
      lastlatDouble = latDouble;
      lastlonDouble = lonDouble;
    }
    //cout << "hello" << endl;
    if (!myDeque.empty()) {
      //cout << "hello" << endl;
      //cout << lastFlag << endl;
      //*outFiles[0] << "hello world\n";
      *outFiles[lastFlag] << myDeque.size() << " " << roadType << endl;
      //cout << "hello" << endl;
      while (!myDeque.empty()){
        *outFiles[lastFlag] << "          " << myDeque.front() << "\n";
        myDeque.pop_front();
      }
    }
    inFile >> lineId >> pointNum >> roadType;
  }


  inFile.close();
  for (int i = 0; i < kIntersectionNum; i++) {
    (*(outFiles[i])).close();
  }



/* ins2kmls */
  int fileNum = kFileNum;
  std::vector<std::string> fileNames = kFileNames;
  std::vector<std::string> inFileNames = kInFileNames;
  std::vector<std::string> outFileNames = kOutFileNames;

  for (int i = 0; i < fileNum; i++) {
      std::fstream inFile, outFile;
      inFile.open(inFileNames[i], std::ios::in);
      outFile.open(outFileNames[i], std::ios::out);

      if (inFile.is_open() && outFile.is_open()) {
          outFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
          outFile << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
          outFile << "  <Document>\n";
          outFile << "    <name>" << fileNames[i] << "</name>\n";

//            int pointNum, roadType;
//            inFile >> pointNum >> roadType;
//            printf("%d %d\n", pointNum, roadType);
          int lineSum, lineId, pointNum, roadType;
          //inFile >> lineSum;
          //inFile >> lineId >>pointNum >> roadType;
          inFile >> pointNum >> roadType;

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
//                float lon, lat, alt;
//                inFile >> lon >> comma >> lat >> comma >> alt;
//                printf("%f %f \n", lon, lat);
//                outFile << "          " << lon << "," << lat << ",0\n";
                  std::string coordinate;
                  inFile >> coordinate;
                  if (j % 20 == 0) {                                              // Down sample.
                      outFile << "          " << coordinate << "\n";
                  }
              }

              outFile << "        </coordinates>\n";
              outFile << "      </LineString>\n";
              outFile << "    </Placemark>\n";

//                inFile >> pointNum >> roadType;
//                printf("%d %d\n", pointNum, roadType);
            //  inFile >> lineId >> pointNum >> roadType;
                inFile >> pointNum >> roadType;
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
