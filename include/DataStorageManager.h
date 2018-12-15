#pragma once

#include "Buffer.h"

#include <bitset>
#include <fstream>
#include <string>
#include <vector>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

class DataStorageManager {
 public:
  /// <summary>
  /// Delete the copy constructor due to singleton pattern
  /// </summary>
  DataStorageManager(DataStorageManager const &) = delete;

  /// <summary>
  /// Delete the assignment constructor due to singleton pattern
  /// </summary>
  void operator=(DataStorageManager const &) = delete;

  /// <summary>
  /// Open file in synchronization style
  /// </summary>
  void OpenFile(std::string filePath);

  /// <summary>
  /// Close file that is in current use. This function should only be called
  /// as the database is changed or the program is closed.
  /// </summary>
  void CloseFile();

  /// <summary>
  /// Create a new page in current file
  /// </summary>
  uint32_t CreatePage();

  /// <summary>
  /// Read specified page from database file
  /// </summary>
  void ReadPage(uint32_t pageId, Frame &frame);

  /// <summary>
  /// Write the specified frame into pageid in the database
  /// </summary>
  void WritePage(uint32_t pageId, Frame &frame);

  /// <summary>
  /// Get the number of I/Os between main memory and disk
  /// </summary>
  uint32_t GetNumIOs();

  /// <summary>
  /// DataStorageManager singleton access point
  /// </summary>
  static DataStorageManager &GetSingleton();

 private:
  /// <summary>
  /// Translate the pageid to offset in db file
  /// </summary>
  uint32_t PageIdToOffset(uint32_t pageId);

  /// <summary>
  /// Set the use bit for the specified page
  /// </summary>
  void SetUse(uint32_t pageId);

  /// <summary>
  /// Reset the use bit for the specifited page
  /// </summary>
  void ResetUse(uint32_t pageId);

  /// <summary>
  /// Get the use bit for the specified page
  /// </summary>
  bool IsUsed(uint32_t pageId);

 private:
  /// <summary>
  /// Hide default constructor due to singleton pattern.
  /// Only called by static method GetSingleton in this class.
  /// </summary>
  DataStorageManager();

 private:
  // i/o counter
  uint32_t counter;

  // database file
  std::fstream file;

  // usage bitmap
  uint8_t usemap[PAGE_SIZE * 2];
};