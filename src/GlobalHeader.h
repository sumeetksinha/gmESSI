#include "TeeStream.h"

std::string LogFileName = "Log.txt";
FileTee* out;
FileTee* err;
std::streambuf* old_cerr = std::cerr.rdbuf(&err->buf);

// int main () {

// using namespace std;
// out = new FileTee(cout, "LogFileName");
// err = new FileTee(cerr, "LogFileName");


//   // out << "stdout";
//   // cerr << "stderr\n";

//   cerr.rdbuf(old_cerr);
// //   // watch exception safety
// // }
// }
