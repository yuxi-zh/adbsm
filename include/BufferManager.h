#pragma once

#include "Buffer.h"

#include <list>
#include <map>
#include <mutex>
#include <set>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#ifndef FRAME_SIZE
#define FRAME_SIZE 4096
#endif  // !FRAME_SIZE

class BufferManager {
 private:
  struct CtrlBlk {
    //
    uint32_t pageId;

    //
    uint32_t count;

    //
    bool dirty;

    //
    std::mutex mu;

    void Reset() {
      count = 0;
      dirty = false;
    }
  };

 public:
  /// <summary>
  /// Delete copy constructor due to singleton pattern
  /// </summary>
  BufferManager(BufferManager const &) = delete;

  /// <summary>
  /// Delete assignment constructor due to singleton pattern
  /// </summary>
  void operator=(BufferManager const &) = delete;

  /// <summary>
  /// Check if required page pageId resides in the buffer.
  /// Load required page into buffer first if not then return it.
  /// </summary>
  void *FixPage(uint32_t pageId, uint32_t protectFlag);

  /// <summary>
  /// Allocate a new empty page and load it into buffer.
  /// </summary>
  std::pair<uint32_t, void *> FixNewPage();

  /// <summary>
  /// Decrement the fix count on frame of specified page.
  /// Frame can be remove after count reduces to zero if selected.
  /// </summary>
  void UnfixPage(uint32_t pageId);

  /// <summary>
  /// Mark this page is dirty
  /// </summary>
  void SetDirty(uint32_t pageId);

  /// <summary>
  /// Write out any dirty pages still reside in the buffer
  /// </summary>
  void WriteDirtys();

  /// <summary>
  /// Write out page pageId if it is dirty and in the buffer
  /// </summary>
  void WriteDirtys(uint32_t pageId);

  /// <summary>
  /// BufferMananger singleton access point
  /// </summary>
  /// <return>Reference to global BufferManager singleton</return>
  static BufferManager &GetSingleton();

 private:
  /// <summary>Select a frame to be replaced</summary>
  /// <return>The frame id of selected frame</return>
  uint32_t SelectVictim();

  /// <summary>
  /// Hide default constructor due to singleton pattern.
  /// Only called by static method GetSingleton in this class.
  /// </summary>
  BufferManager();

 private:
  /// Map from pageId to frameId
  std::map<uint32_t, uint32_t> ptof;

  /// frame buffer
  Frame buffer[BUFFER_SIZE];

  /// control block
  CtrlBlk control[BUFFER_SIZE];

  /// list to replace queue for LRU algorithm
  std::list<uint32_t> victims;
};