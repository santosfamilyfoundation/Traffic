#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <iosfwd>
#include <string>
#include <vector>

static const double pi = 3.14159265358979323846;

static const std::string separator = " ";
const char commentChar = '#';

/** Loads lines of numbers from a text file, separated by some character */
std::vector<std::vector<float> > loadNumbers(const std::string& filename, const std::string& separator = ::separator);

/** Gets line in a file, skipping comments (starting with '#') (wrapper around getline).
 * Warning: returns an empty string if all the lines to the end of the file are comments. */
std::string getlineComment(std::ifstream& f);

/** Converts a string to an integer. */
int toInt(const std::string& s);

/** Converts a string to a float. */
float toFloat(const std::string& s);

/** Converts a string to a double. */
double toDouble(const std::string& s);

/** Opens file for writing with fixed scientific precision. */
void openWriteScientificPrecision(std::ofstream& out, const std::string& filename, const int& precision);

void openWritePrecision(std::ofstream& out, const std::string& filename, const int& precision);

/** Opens files and checks how it went. */
bool openCheck(std::ifstream& f, const std::string& filename, const std::string& callingFunctionName);
bool openCheck(std::ofstream& f, const std::string& filename, const std::string& callingFunctionName);


// inline 

inline double square(const int& a) { return a*a;}

/** Implements key bindings, for example for cvWaitKey(). */
inline bool forwardKey(const int& pressedKey) { return (((char)pressedKey) == '+');}

inline bool backwardKey(const int& pressedKey) { return (((char)pressedKey) == '-');}

inline bool saveKey(const int& pressedKey) { return (((char)pressedKey) == 's' || ((char)pressedKey) == 'S');}

inline bool interruptionKey(const int& pressedKey) { return (((char)pressedKey) == 'q' || ((char)pressedKey) == 'Q');}

inline bool skipKey(const int& pressedKey) { return (((char)pressedKey) == 'n' || ((char)pressedKey) == 'N');}


// templates

/** Splits a string on the separator. The resulting string items are added to the result (the list is not cleared). */
template<class Seq>
  inline void split(Seq& result, const std::string& text, const std::string& separator = ::separator) {
  int n = text.length();

  int start = text.find_first_not_of(separator);
  while ((start >= 0) && (start < n)) {
    int stop = text.find_first_of(separator, start);
    if ((stop < 0) || (stop > n))
      stop = n;
    result.push_back(text.substr(start, stop - start));
    start = text.find_first_not_of(separator, stop+1);
  }
}

/** Converts strings to numbers. */
template<typename T>
bool fromString(T & result, const std::string & s) {
  std::istringstream iss(s);
  iss >> result;
  return iss.good() || iss.eof();
}

#endif
