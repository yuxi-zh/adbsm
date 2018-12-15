#include "BufferManager.h"
#include "DataStorageManager.h"

#include <exception>
#include <fstream>
#include <iostream>

using namespace std;

class TestDriver {
 public:
  /// <summary>
  /// Create a temporary database file for test
  /// and materialize it with required 50000 pages
  /// </summary>
  void InitTest() {
    // Materialize database file
    for (int i = 0; i < 50000; i++) {
      BufferManager::GetSingleton().FixNewPage();
    }

    // Open trace file
    trace.exceptions(ifstream::failbit | ifstream::badbit);
    trace.open("data-5w-50w-zipf.txt");
  }

  /// <summary>
  /// Call interface funtion according to given trace file
  /// </summary>
  void RunTest() {
    while (!trace.eof()) {
      uint32_t action, pageId;
      char seperator;
      trace >> action >> seperator >> pageId;
      switch (action) {
        case 0:  // read page
          BufferManager::GetSingleton().FixPage(pageId, 0);
          break;
        case 1:  // write page
          BufferManager::GetSingleton().FixPage(pageId, 0);
          break;
        default:
          throw runtime_error("Invalid action index");
      }
      BufferManager::GetSingleton().UnfixPage(pageId);
    }
  }

  /// <summary>
  /// Print out the total I/Os between main memory and disk
  /// </summary>
  void ReportMertics() {
    cout << "I/Os between main memory and disk: ";
    cout << DataStorageManager::GetSingleton().GetNumIOs() << endl;
  }

  /// <summary>
  /// Release system resource allocated for the test
  /// </summary>
  void UninitTest() { trace.close(); }

 private:
  ifstream trace;
};

int main(int argc, char const *argv[]) {
  TestDriver driver;
  driver.InitTest();
  driver.RunTest();
  driver.ReportMertics();
  driver.UninitTest();
  return 0;
}