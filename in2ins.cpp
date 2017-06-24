#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include "hdmap.h"
#include <boost/lexical_cast.hpp>
using namespace std;

const int kIntersectionNum = 4;
const std::string kFileNames = "40backward";
const std::string kInterFileName = "data/inter/inter.txt";
const std::string kInFileNames = "data/in/40backward.in";
bool myfunction (double i, double j) {return (i < j); }

int main(int argc, char const *argv[]) {
  int IntersectionNum = kIntersectionNum;
  std::string InterFileName = kInterFileName;
  std::string inFileNames = kInFileNames;
  std::fstream inFile, interFile;
  std::vector<std::fstream *> outFiles;
  inFile.open(inFileNames, std::ios::in);
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


  std::string outFileName = "data/in/" + kFileNames + "_" + to_string(0) + ".in";
  std::fstream outFile0;
  outFile0.open(outFileName, std::ios::out);
  outFiles.push_back(&outFile0);

  outFileName = "data/in/" + kFileNames + "_" + to_string(1) + ".in";
  std::fstream outFile1;
  outFile1.open(outFileName, std::ios::out);
  outFiles.push_back(&outFile1);

  outFileName = "data/in/" + kFileNames + "_" + to_string(2) + ".in";
  std::fstream outFile2;
  outFile2.open(outFileName, std::ios::out);
  outFiles.push_back(&outFile2);

  outFileName = "data/in/" + kFileNames + "_" + to_string(3) + ".in";
  std::fstream outFile3;
  outFile3.open(outFileName, std::ios::out);
  outFiles.push_back(&outFile3);

  outFileName = "data/in/" + kFileNames + "_" + to_string(4) + ".in";
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


  int pointNum, roadType;
  inFile >> pointNum >> roadType;



  while (!inFile.eof()) {
    int lastFlag = 0;
    int flag = lastFlag;
    std::deque<std::string> myDeque;
    for (int j = 0; j < pointNum; j++) {
      std::string coordinate;
      std::string tmpCoordinate;
      double lastlatDouble, lastlonDouble;
      double tmpLat, tmpLon;
      inFile >> coordinate;
      unsigned long commaPos1 = coordinate.find(",");
      unsigned long commaPos2 = coordinate.find(",", commaPos1 + 1);
      double latDouble = stod(coordinate.substr(commaPos1 + 1, commaPos2 - commaPos1 -1));
      double lonDouble = stod(coordinate.substr(0, commaPos1));
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
    inFile >> pointNum >> roadType;
  }


  inFile.close();
  for (int i = 0; i < kIntersectionNum; i++) {
    (*(outFiles[i])).close();
  }

  return 0;
}
