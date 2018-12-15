﻿#include "BufferManager.h"
#include "DataStorageManager.h"

#include <algorithm>
#include <cassert>
#include <iostream>

void *BufferManager::FixPage(uint32_t pageId, uint32_t protectFlag) {
  auto &DSM = DataStorageManager::GetSingleton();
  if (ptof.find(pageId) == ptof.end()) {
    // pageId没有加载到缓冲区
    // 分配新的frame用于存放pageId所对应的页内容
    auto frameId = SelectVictim();
    DSM.ReadPage(pageId, buffer[frameId]);
    control[frameId].Reset();
    ptof[pageId] = frameId;
  }
  // pageId已经加载到缓冲区
  control[ptof[pageId]].count += 1;
  return buffer[ptof[pageId]].field;
}

void *BufferManager::FixNewPage() {
  auto &DSM = DataStorageManager::GetSingleton();
  uint32_t pageId = DSM.CreatePage();
  return FixPage(pageId, 0);
}

void BufferManager::UnfixPage(uint32_t pageId) {
  if (ptof.find(pageId) == ptof.end()) {
    // pageId没有加载到缓冲区
    throw new std::runtime_error("[UnfixPage]: page not found.");
  }
  auto frameId = ptof[pageId];
  if (--control[frameId].count == 0) {
    // 尝试将当前页写回磁盘
    WriteDirtys(pageId);
    // 当前frame已经不在被引用
    victims.push_back(frameId);
  }
}

uint32_t BufferManager::SelectVictim() {
  if (victims.empty()) {
    // 没有可用的缓冲区frame
    throw new std::runtime_error("[AllocateFrame]: no avaliable frame");
  }

  auto frameId = victims.front();
  victims.pop_front();

  return frameId;
}

void BufferManager::SetDirty(uint32_t pageId) {
  auto frameId = ptof[pageId];
  control[frameId].dirty = true;
}

void BufferManager::WriteDirtys() {
  auto &DSM = DataStorageManager::GetSingleton();
  for (auto idmap : ptof) {
    if (control[idmap.first].dirty) {
      DSM.WritePage(idmap.first, buffer[idmap.second]);
    }
  }
}

void BufferManager::WriteDirtys(uint32_t pageId) {
  auto &DSM = DataStorageManager::GetSingleton();
  auto frameId = ptof.at(pageId);
  if (control[frameId].dirty) {
    DSM.WritePage(pageId, buffer[frameId]);
  }
}

BufferManager &BufferManager::GetSingleton() {
  static BufferManager Mgr;
  return Mgr;
}

BufferManager::BufferManager() {
  for (int I = 0, N = BUFFER_SIZE; I != N; I++) {
    victims.push_back(I);
  }
}