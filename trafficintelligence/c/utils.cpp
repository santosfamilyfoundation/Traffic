#include "utils.hpp"

#include <boost/foreach.hpp>

#include <iostream>
#include <fstream>

using namespace std;

std::vector<std::vector<float> > loadNumbers(const string& filename, const string& separator /* = " " */) {
  ifstream in(filename.c_str());
  std::vector<std::vector<float> > result;

  if (::openCheck(in, filename, "loadNumbers")) {  
    while (!in.eof()) {
      string line = ::getlineComment(in);
      std::vector<string> tokens;
      ::split(tokens, line, separator);
      std::vector<float> numbers;
      BOOST_FOREACH(string s, tokens)
	numbers.push_back(::toFloat(s));
      if (!numbers.empty())
	result.push_back(numbers);
    }
  }

  return result;
}

int toInt(const std::string& s) { int i; fromString(i, s); return i;} //atoi

float toFloat(const std::string& s) { float x; fromString(x, s); return x;}// lexical_cast<float>(s)

double toDouble(const std::string& s) { double x; fromString(x, s); return x;}

string getlineComment(ifstream& f) {
  string s;
  getline(f, s);
  while ((!f.eof()) && (s[0] == ::commentChar)) {
    getline(f, s);
  }
  
  if (!s.empty() && s[0] == ::commentChar)
    s.clear();
  return s;
}

void openWriteScientificPrecision(ofstream& out, const string& filename, const int& precision) {
  ::openWritePrecision(out, filename, precision);
  out.setf(ios::scientific);
}

void openWritePrecision(ofstream& out, const string& filename, const int& precision) {
  out.open(filename.c_str(), ios::binary);
  ::openCheck(out, filename, "openWritePrecision");
  out.precision(precision);
}

bool openCheck(ifstream& f, const string& filename, const string& callingFunctionName) {
  if (!f.is_open()) {
    cerr << "Pb opening file " << filename << " in " << callingFunctionName << endl;
    return false;
  } else
    return true;
}

bool openCheck(ofstream& f, const string& filename, const string& callingFunctionName) {
  if (!f.is_open()) {
    cerr << "Pb opening file " << filename << " in " << callingFunctionName << endl;
    return false;
  } else
    return true;
}
