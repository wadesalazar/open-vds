#ifndef HUEBULKDATASTORE_H
#define HUEBULKDATASTORE_H
////////////////////////////////////////////////////////////////////////////////
// <copyright>
// Copyright (c) 2016 Hue AS. All rights reserved.
//
// All rights are reserved. Reproduction or transmission in whole or in part, in
// any form or by any means, electronic, mechanical or otherwise, is prohibited
// without the prior written permission of the copyright owner.
// </copyright>
////////////////////////////////////////////////////////////////////////////////
#include <cstdlib>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		
#define NOMINMAX
#include <windows.h>
#define fseeko _fseeki64
#define ftello _ftelli64
#else
#include <sys/stat.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <cassert>

#include <algorithm>
#include <list>
#include <string>
#include <set>
#include <errno.h>
#include <string.h>

#include "HueBulkDataStoreFormat.h"
#include "ExtentAllocator.h"

class HueBulkDataStore
{
public:
  class Buffer
  {
  protected:
    Buffer() {}
    ~Buffer() {}
  public:
    virtual const void *
      Data() const = 0;

    virtual int  Size() const = 0;
  };

  class FileInterface
  {
  public:
    virtual const char*  
                 GetFileName() = 0;
    virtual int  GetRevisionNumber() = 0;
    virtual int  GetFileType() = 0;
    virtual int  GetChunkMetadataLength() = 0;
    virtual int  GetFileMetadataLength() = 0;

    virtual Buffer *  ReadChunk(int chunk, void *metadata) = 0;
    virtual Buffer *  ReadChunkData(int chunk) = 0;
    virtual bool      ReadChunkMetadata(int chunk, void *metadata) = 0;
    virtual bool      ReadIndexEntry(int chunk, struct IndexEntry *indexEntry, void *metadata) = 0;

    virtual bool WriteChunk(int chunk, const void *data, int size, const void *metadata, void *oldMetadata = NULL) = 0;
    virtual bool WriteChunkData(int chunk, const void *data, int size) = 0;
    virtual bool WriteChunkMetadata(int chunk, const void *metadata, void *oldMetadata = NULL) = 0;
    virtual bool WriteIndexEntry(int chunk, const struct IndexEntry &indexEntry, const void *metadata, void *oldMetadata = NULL) = 0;

    virtual int  GetRevision() = 0;
    virtual int  GetChunkCount() = 0;
    virtual void ReadFileMetadata(void *fileMetadata) = 0;
    virtual void WriteFileMetadata(const void *fileMetadata) = 0;
    virtual void Rename(const char *fileName) = 0;

    virtual int  GetIndexPageIndexForChunk(int chunk) const = 0;
    virtual int  GetIndexEntryIndexForChunk(int chunk) const = 0;

    virtual bool Commit() = 0;
    virtual ~FileInterface() {}
  };

  virtual  ~HueBulkDataStore();
  virtual const char *GetErrorMessage() = 0;

  virtual class ExtentAllocator &  GetExtentAllocator() = 0;

  virtual bool BuildExtentAllocator() = 0;

  virtual bool EnableWriting() = 0;

  virtual int  GetFileCount() = 0;

  virtual const char* 
               GetFileName(int fileIndex) = 0;
  virtual int  GetHeadRevisionNumber(int fileIndex) = 0;
  virtual int  GetFileType(int fileIndex) = 0;
  virtual int  GetChunkMetadataLength(int fileIndex) = 0;
  virtual int  GetFileMetadataLength(int fileIndex) = 0;

  virtual FileInterface *OpenFile(const char *fileName) = 0;
  virtual FileInterface *OpenFileRevision(const char *fileName, int revision) = 0;
  virtual void          CloseFile(FileInterface *fileInterface) = 0;

  virtual FileInterface *AddFile(const char *fileName, int chunkCount, int indexPageEntryCount, int fileType, int chunkMetadataLength, int fileMetadataLength, bool overwriteExisting) = 0;
  virtual bool          RemoveFile(const char *fileName) = 0;
  virtual bool          IsOpen() = 0;
  virtual bool          IsReadOnly() = 0;

  virtual Buffer*  ReadBuffer(struct IndexEntry const &indexEntry) = 0;
  virtual Buffer*  CreateBuffer(struct IndexEntry &indexEntry, int size) = 0;
  virtual bool     WriteBuffer(Buffer *buffer) = 0;

  static HueBulkDataStore *Open(const char *fileName);
  static HueBulkDataStore *CreateNew(const char *fileName, bool overwriteExisting);
  static void              Close(HueBulkDataStore *hueBulkDataStore);

  static void  ReleaseBuffer(Buffer *buffer);
};

#endif //HUEBULKDATASTORE_H
