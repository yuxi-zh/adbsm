#include "DataStorageManager.h"

#include <algorithm>
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

DataStorageManager::DataStorageManager() : counter(0) {}

DataStorageManager &DataStorageManager::GetSingleton() {
  static DataStorageManager DSM;
  return DSM;
}

void DataStorageManager::CreateFile(std::string filePath) {
  std::fstream file(filePath, std::fstream::out);
  file.seekp(2 * PAGE_SIZE - 1);
  file.write("", 1);
  file.close();
}

void DataStorageManager::OpenFile(std::string filePath) {
  counter = 0;
  file.open(filePath, std::fstream::out | std::fstream::in);
  file.read(reinterpret_cast<char *>(usemap), sizeof(usemap));
}

void DataStorageManager::ReadPage(uint32_t pageId, Frame &frame) {
  counter += 1;
  std::cerr << "[ReadPage] : " << pageId << std::endl;
  file.seekg(PageIdToOffset(pageId));
  file.read(reinterpret_cast<char *>(frame.field), sizeof(frame.field));
}

void DataStorageManager::WritePage(uint32_t pageId, Frame &frame) {
  counter += 1;
  std::cerr << "[WritePage] : " << pageId << std::endl;
  file.seekp(PageIdToOffset(pageId));
  file.write(reinterpret_cast<char *>(frame.field), sizeof(frame.field));
}

void DataStorageManager::CloseFile() { file.close(); }

uint32_t DataStorageManager::GetNumIOs() { return counter; }

void DataStorageManager::SetUse(uint32_t pageId) {
  usemap[pageId / 8] |= (1 << (pageId % 8));
}

void DataStorageManager::ResetUse(uint32_t pageId) {
  usemap[pageId / 8] &= ~(1 << (pageId % 8));
}

bool DataStorageManager::IsUsed(uint32_t pageId) {
  return usemap[pageId / 8] & (1 << (pageId % 8));
}

uint32_t DataStorageManager::CreatePage() {
  for (int i = 0, n = 2 * PAGE_SIZE; i != n; ++i) {
    if (usemap[i] != 0xff) {
      for (int j = 0, m = 8; j != m; ++j) {
        if ((usemap[i] & (1 << j)) == 0) {
          uint32_t pageId = i * 8 + j;
          SetUse(pageId);
          std::cerr << "[CreatePage] : " << pageId << std::endl;
          return pageId;
        }
      }
    }
  }

  throw std::runtime_error("[Create Page]: no avaiable page");
}

uint32_t DataStorageManager::PageIdToOffset(uint32_t pageId) {
  return (pageId + 2) * PAGE_SIZE;
}
