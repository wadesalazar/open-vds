////////////////////////////////////////////////////////////////////////////////
// <copyright>
// Copyright (c) 2016 Hue AS. All rights reserved.
//
// All rights are reserved. Reproduction or transmission in whole or in part, in
// any form or by any means, electronic, mechanical or otherwise, is prohibited
// without the prior written permission of the copyright owner.
// </copyright>
////////////////////////////////////////////////////////////////////////////////

#include "HueBulkDataStore.h"
#include "IO/File.h"
using OpenVDS::File;
using OpenVDS::Error;

class DataStoreFileDescriptor
{
public:
  std::set<int> m_openRevisions;
  int           m_fileIndex;
  FileHeader    m_fileHeader;
  std::string   m_fileName;

  DataStoreFileDescriptor(int fileIndex, FileHeader const &fileHeader, std::string const &fileName) : m_fileIndex(fileIndex), m_fileHeader(fileHeader), m_fileName(fileName) { assert(fileIndex == -1 || fileIndex >= 0); }
};

class DataStoreBuffer : public HueBulkDataStore::Buffer
{
  int64_t m_offset;
  int32_t m_size;
  bool    m_owner;
  void   *m_data;

  mutable bool
    m_dirty;

public:
  DataStoreBuffer(int64_t offset, int32_t size, bool dirty, void *data, bool takeOwnership) : m_offset(offset), m_size(size), m_dirty(dirty), m_data(data), m_owner(takeOwnership) { assert(data && size > 0); }
  DataStoreBuffer(int64_t offset, int32_t size) : m_offset(offset), m_size(size), m_dirty(true), m_data(calloc(1, size)), m_owner(true) { assert(size > 0); }
  DataStoreBuffer() : m_offset(0), m_size(0), m_dirty(false), m_data(NULL), m_owner(false) {}
  virtual ~DataStoreBuffer() { assert(!m_owner || m_data); if (m_owner) free(m_data); }

  int64_t
    Offset() const { return m_offset; }

  void *
    WritableData() { assert(m_owner); m_dirty = true; return m_data; }

  void
    SetOffset(int64_t offset) { m_offset = offset; m_dirty = true; }

  bool
    IsDirty() const { return m_dirty; }

  void
    ClearDirty() const { assert(m_dirty); m_dirty = false; }

  // HueBulkDataStore::Buffer implementation
  virtual const void *
    Data() const { return m_data; }

  virtual int32_t
    Size() const { return m_size; }
};

class FileInterfaceImpl : public HueBulkDataStore::FileInterface
{
  class HueBulkDataStoreImpl
          &m_dataStore;

  DataStoreBuffer
         *m_pageDirectory;

  DataStoreFileDescriptor &
          m_fileDescriptor;

  int     m_revisionNumber;

  int     m_chunkCount;

  std::list<int>
          m_indexPageCacheMRUList;

  int     m_indexPageCacheSize;

  int     m_dirtyIndexPageCount;

  int     m_indexPageCount;

  DataStoreBuffer
          **m_indexPages;

  const int64_t
          IndexPageOffset(int indexPage) { return reinterpret_cast<const int64_t *>(static_cast<const char *>(m_pageDirectory->Data()) + sizeof(PageDirectory) + m_fileDescriptor.m_fileHeader.m_fileMetadataLength)[indexPage]; }

  void    SetIndexPageOffset(int indexPage, int64_t indexPageOffset) { reinterpret_cast<int64_t *>(static_cast<char *>(m_pageDirectory->WritableData()) + sizeof(PageDirectory) + m_fileDescriptor.m_fileHeader.m_fileMetadataLength)[indexPage] = indexPageOffset; }

  void    LimitCachedIndexPages(int indexPageLimit);

  bool    ReadIndexPage(int indexPage, DataStoreBuffer **indexPageBuffer, bool makeWritable);

  bool    ReadChunkInternal(int chunk, HueBulkDataStore::Buffer **buffer, void *metadata, IndexEntry *indexEntry);

  bool    WriteChunkInternal(int chunk, const DataStoreBuffer &buffer, const void *metadata, void *oldMetadata);

public:
  FileInterfaceImpl(HueBulkDataStoreImpl &dataStore, DataStoreBuffer *pageDirectory, DataStoreFileDescriptor &fileDescriptor, int revision, int chunkCount);
  ~FileInterfaceImpl();

  // HueBulkDataStore::FileInterface virtual overrides
  virtual const char*
               GetFileName();
  virtual int  GetRevisionNumber();
  virtual int  GetFileType();
  virtual int  GetChunkMetadataLength();
  virtual int  GetFileMetadataLength();

  virtual HueBulkDataStore::Buffer *
               ReadChunk(int chunk, void *metadata);
  virtual HueBulkDataStore::Buffer *
               ReadChunkData(int chunk);
  virtual bool ReadChunkMetadata(int chunk, void *metadata);
  virtual bool ReadIndexEntry(int chunk, IndexEntry *indexEntry, void *metadata);

  virtual bool WriteChunk(int chunk, const void *data, int size, const void *metadata, void *oldMetadata);
  virtual bool WriteChunkData(int chunk, const void *data, int size);
  virtual bool WriteChunkMetadata(int chunk, const void *metadata, void *oldMetadata);

  virtual int  GetRevision();
  virtual int  GetChunkCount();
  virtual void ReadFileMetadata(void *fileMetadata);
  virtual void WriteFileMetadata(const void *fileMetadata);
  virtual void Rename(const char *fileName);

  virtual int  GetIndexPageIndexForChunk(int chunk) const;
  virtual int  GetIndexEntryIndexForChunk(int chunk) const;

  virtual bool Commit();
};

class HueBulkDataStoreImpl : public HueBulkDataStore
{
  enum
  {
    FILENAME_MAX_CHARS = 1024
  };

  File    m_fileHandle;

  bool    m_readOnly;

  std::string
         m_errorMessage;

  DataStoreBuffer
    *m_header;

  DataStoreBuffer
    *m_fileTable;

  std::list<DataStoreFileDescriptor>
    m_fileDescriptorList;

  ExtentAllocator
    *m_extentAllocator;

public:
  HueBulkDataStoreImpl();
  ~HueBulkDataStoreImpl();

  // Methods
  void SetErrorMessage(std::string const &errorMessage);

  bool Open(const char *fileName);
  bool CreateNew(const char *fileName, bool overwriteExisting);
  void Close();

  DataStoreBuffer *
    ReadBuffer(int64_t offset, int32_t size);

  DataStoreBuffer *
    CreateBuffer(int32_t size, ExtentAllocator::ExtentType type);

  void MakeWritable(DataStoreBuffer *buffer, ExtentAllocator::ExtentType type);

  bool WriteBuffer(DataStoreBuffer const &buffer);

  bool ReadHeaderAndFileTable();

  const DataStoreHeader &
    GetDataStoreHeader();

  DataStoreFileDescriptor *
    FindFile(int fileIndex);

  DataStoreFileDescriptor *
    FindFile(const char *fileName);

  FileInterface *
    OpenFileInternal(DataStoreFileDescriptor &fileDescriptor, int revision);

  bool BuildExtentAllocatorForPageDirectory(FileHeader const &fileHeader, int64_t pageDirectoryOffset, int32_t chunkCount);

  bool UpdateFileTable(DataStoreFileDescriptor const &fileDescriptor);

  bool CommitFileTable();

  void Flush();

  // HueBulkDataStore virtual overrides
  virtual const char
    *GetErrorMessage();

  virtual class ExtentAllocator &
    GetExtentAllocator();

  virtual bool BuildExtentAllocator();

  virtual bool EnableWriting();

  virtual int  GetFileCount();

  virtual const char*
               GetFileName(int fileIndex);
  virtual int  GetHeadRevisionNumber(int fileIndex);
  virtual int  GetFileType(int fileIndex);
  virtual int  GetChunkMetadataLength(int fileIndex);
  virtual int  GetFileMetadataLength(int fileIndex);

  virtual FileInterface *OpenFile(const char *fileName);
  virtual FileInterface *OpenFileRevision(const char *fileName, int revision);
  virtual void           CloseFile(FileInterface *fileInterface);

  virtual FileInterface *AddFile(const char *fileName, int chunkCount, int indexPageEntryCount, int fileType, int chunkMetadataLength, int fileMetadataLength, bool overwriteExisting);
  virtual bool           RemoveFile(const char *fileName);

  virtual bool           IsOpen();
  virtual bool           IsReadOnly();

  virtual Buffer *       ReadBuffer(struct IndexEntry const &indexEntry) { return ReadBuffer(indexEntry.m_offset, indexEntry.m_length); }
};

FileInterfaceImpl::FileInterfaceImpl(HueBulkDataStoreImpl &dataStore, DataStoreBuffer *pageDirectory, DataStoreFileDescriptor &fileDescriptor, int revision, int chunkCount) :
  m_dataStore(dataStore),
  m_pageDirectory(pageDirectory),
  m_fileDescriptor(fileDescriptor),
  m_revisionNumber(revision),
  m_chunkCount(chunkCount),
  m_indexPageCacheSize(10),
  m_dirtyIndexPageCount(0),
  m_indexPageCount((chunkCount + fileDescriptor.m_fileHeader.m_indexPageEntryCount - 1) / fileDescriptor.m_fileHeader.m_indexPageEntryCount)
{
  m_indexPages = new DataStoreBuffer *[m_indexPageCount];
  memset(m_indexPages, 0, sizeof(DataStoreBuffer *) * m_indexPageCount);
}

FileInterfaceImpl::~FileInterfaceImpl()
{
  for (int indexPage = 0; indexPage < m_indexPageCount; indexPage++)
  {
    delete m_indexPages[indexPage];
  }

  delete[] m_indexPages;
}

void
FileInterfaceImpl::LimitCachedIndexPages(int indexPageLimit)
{
  while (int(m_indexPageCacheMRUList.size()) - m_dirtyIndexPageCount > indexPageLimit)
  {
    std::list<int>::iterator it = m_indexPageCacheMRUList.end();

    while (it != m_indexPageCacheMRUList.begin())
    {
      --it;

      assert(m_indexPages[*it]);

      if (!m_indexPages[*it]->IsDirty())
      {
        delete m_indexPages[*it];
        m_indexPages[*it] = NULL;
        m_indexPageCacheMRUList.erase(it);
        break;
      }
    }
  }
}

bool
FileInterfaceImpl::ReadIndexPage(int indexPage, DataStoreBuffer **indexPageBuffer, bool makeWritable)
{
  int
    indexEntrySize = (int)sizeof(IndexEntry) + m_fileDescriptor.m_fileHeader.m_chunkMetadataLength;

  int
    indexPageEntryCount = m_fileDescriptor.m_fileHeader.m_indexPageEntryCount;

  if (m_indexPages[indexPage])
  {
    assert(std::find(m_indexPageCacheMRUList.begin(), m_indexPageCacheMRUList.end(), indexPage) != m_indexPageCacheMRUList.end());
    m_indexPageCacheMRUList.remove(indexPage);
    m_indexPageCacheMRUList.push_front(indexPage);

    if (makeWritable && !m_indexPages[indexPage]->IsDirty())
    {
      m_dataStore.MakeWritable(m_indexPages[indexPage], ExtentAllocator::IndexPageExtent);
      assert(m_indexPages[indexPage]->IsDirty());
      m_dirtyIndexPageCount++;
    }

    *indexPageBuffer = m_indexPages[indexPage];
    return true;
  }

  const int64_t
    indexPageOffset = IndexPageOffset(indexPage);

  if (indexPageOffset != 0)
  {
    *indexPageBuffer = m_dataStore.ReadBuffer(indexPageOffset, indexPageEntryCount * indexEntrySize);

    if (!indexPageBuffer)
    {
      return false;
    }
  }
  else if (makeWritable)
  {
    *indexPageBuffer = m_dataStore.CreateBuffer(indexPageEntryCount * indexEntrySize, ExtentAllocator::IndexPageExtent);
  }
  else
  {
    *indexPageBuffer = NULL;
  }

  if (*indexPageBuffer)
  {
    LimitCachedIndexPages(m_indexPageCacheSize - 1);

    assert(std::find(m_indexPageCacheMRUList.begin(), m_indexPageCacheMRUList.end(), indexPage) == m_indexPageCacheMRUList.end());
    m_indexPageCacheMRUList.push_front(indexPage);

    m_indexPages[indexPage] = *indexPageBuffer;

    if (makeWritable)
    {
      m_dataStore.MakeWritable(m_indexPages[indexPage], ExtentAllocator::IndexPageExtent);
      assert(m_indexPages[indexPage]->IsDirty());
      m_dirtyIndexPageCount++;
    }
  }

  return true;
}

int
FileInterfaceImpl::GetIndexPageIndexForChunk(int chunk) const
{
  return  chunk / m_fileDescriptor.m_fileHeader.m_indexPageEntryCount;
}

int
FileInterfaceImpl::GetIndexEntryIndexForChunk(int chunk) const
{
  return chunk % m_fileDescriptor.m_fileHeader.m_indexPageEntryCount;
}

bool
FileInterfaceImpl::ReadIndexEntry(int chunk, IndexEntry *indexEntry, void *metadata)
{
  assert(chunk >= 0 && chunk < m_chunkCount);
  assert(indexEntry != NULL);

  int indexPage = GetIndexPageIndexForChunk(chunk);
  int entryIndex = GetIndexEntryIndexForChunk(chunk);

  DataStoreBuffer
    *indexPageBuffer;

  bool
    read = ReadIndexPage(indexPage, &indexPageBuffer, false);

  if (!read)
  {
    return false;
  }

  if (indexPageBuffer)
  {
    size_t
      indexPageEntrySize = sizeof(IndexEntry) + m_fileDescriptor.m_fileHeader.m_chunkMetadataLength;

    const IndexEntry
      *indexPageEntry = reinterpret_cast<const IndexEntry *>(static_cast<const char *>(indexPageBuffer->Data()) + entryIndex * indexPageEntrySize);

    memcpy(indexEntry, indexPageEntry, sizeof(IndexEntry));

    if (metadata)
    {
      memcpy(metadata, indexPageEntry + 1, m_fileDescriptor.m_fileHeader.m_chunkMetadataLength);
    }
  }
  else
  {
    memset(indexEntry, 0, sizeof(IndexEntry));

    if (metadata)
    {
      memset(metadata, 0, m_fileDescriptor.m_fileHeader.m_chunkMetadataLength);
    }
  }

  return true;
}

bool
FileInterfaceImpl::WriteChunkInternal(int chunk, const DataStoreBuffer &buffer, const void *metadata, void *oldMetadata)
{
  assert(chunk >= 0 && chunk < m_chunkCount);

  int indexPage = chunk / m_fileDescriptor.m_fileHeader.m_indexPageEntryCount;
  int entryIndex = chunk % m_fileDescriptor.m_fileHeader.m_indexPageEntryCount;

  DataStoreBuffer
    *indexPageBuffer;

  if (!ReadIndexPage(indexPage, &indexPageBuffer, true))
  {
    return false;
  }

  if (!m_dataStore.WriteBuffer(buffer))
  {
    return false;
  }

  if (!indexPageBuffer->IsDirty())
  {
    m_dirtyIndexPageCount++;
  }

  int
    indexEntrySize = (int)sizeof(IndexEntry) + m_fileDescriptor.m_fileHeader.m_chunkMetadataLength;

  IndexEntry
    *indexPageEntry = reinterpret_cast<IndexEntry *>(static_cast<char *>(indexPageBuffer->WritableData()) + entryIndex * indexEntrySize);

  indexPageEntry->m_offset = buffer.Offset();
  indexPageEntry->m_length = buffer.Size();

  if (m_fileDescriptor.m_fileHeader.m_chunkMetadataLength > 0)
  {
    if (oldMetadata)
    {
      memcpy(oldMetadata, indexPageEntry + 1, m_fileDescriptor.m_fileHeader.m_chunkMetadataLength);
    }
    if (metadata)
    {
      memcpy(indexPageEntry + 1, metadata, m_fileDescriptor.m_fileHeader.m_chunkMetadataLength);
    }
    else
    {
      memset(indexPageEntry + 1, 0, m_fileDescriptor.m_fileHeader.m_chunkMetadataLength);
    }
  }
  return true;
}

const char*
FileInterfaceImpl::GetFileName()
{
  return m_fileDescriptor.m_fileName.c_str();
}

int
FileInterfaceImpl::GetRevisionNumber()
{
  return m_revisionNumber;
}

int
FileInterfaceImpl::GetFileType()
{
  return m_fileDescriptor.m_fileHeader.m_fileType;
}

int
FileInterfaceImpl::GetChunkMetadataLength()
{
  return m_fileDescriptor.m_fileHeader.m_chunkMetadataLength;
}

int
FileInterfaceImpl::GetFileMetadataLength()
{
  return m_fileDescriptor.m_fileHeader.m_fileMetadataLength;
}

HueBulkDataStore::Buffer *
FileInterfaceImpl::ReadChunk(int chunk, void *metadata)
{
  assert(chunk >= 0 && chunk < m_chunkCount);
  assert((m_fileDescriptor.m_fileHeader.m_chunkMetadataLength > 0) == (metadata != NULL));

  IndexEntry
    indexEntry;

  bool
    read = ReadIndexEntry(chunk, &indexEntry, metadata);

  if (!read)
  {
    return NULL;
  }

  return m_dataStore.ReadBuffer(indexEntry.m_offset, indexEntry.m_length);
}

HueBulkDataStore::Buffer *
FileInterfaceImpl::ReadChunkData(int chunk)
{
  assert(chunk >= 0 && chunk < m_chunkCount);

  IndexEntry
    indexEntry;

  bool
    read = ReadIndexEntry(chunk, &indexEntry, NULL);

  if (!read)
  {
    return NULL;
  }

  return m_dataStore.ReadBuffer(indexEntry.m_offset, indexEntry.m_length);
}

bool
FileInterfaceImpl::ReadChunkMetadata(int chunk, void *metadata)
{
  assert(chunk >= 0 && chunk < m_chunkCount);
  assert(metadata != NULL);

  IndexEntry
    indexEntry;

  return ReadIndexEntry(chunk, &indexEntry, metadata);
}

bool
FileInterfaceImpl::WriteChunk(int chunk, const void *data, int size, const void *metadata, void *oldMetadata)
{
  assert(data && size > 0);
  assert((m_fileDescriptor.m_fileHeader.m_chunkMetadataLength > 0 && metadata) ||
    (m_fileDescriptor.m_fileHeader.m_chunkMetadataLength == 0 && !metadata));

  DataStoreBuffer
    buffer(m_dataStore.GetExtentAllocator().Allocate(size, ExtentAllocator::ChunkExtent), size, true, const_cast<void *>(data), false);

  return WriteChunkInternal(chunk, buffer, metadata, oldMetadata);
}

bool
FileInterfaceImpl::WriteChunkData(int chunk, const void *data, int size)
{
  assert(data && size > 0);

  DataStoreBuffer
    buffer(m_dataStore.GetExtentAllocator().Allocate(size, ExtentAllocator::ChunkExtent), size, true, const_cast<void *>(data), false);

  return WriteChunkInternal(chunk, buffer, NULL, NULL);
}

bool
FileInterfaceImpl::WriteChunkMetadata(int chunk, const void *metadata, void *oldMetadata)
{
  assert(m_fileDescriptor.m_fileHeader.m_chunkMetadataLength > 0 && metadata);

  DataStoreBuffer
    buffer;

  return WriteChunkInternal(chunk, buffer, metadata, oldMetadata);
}

int
FileInterfaceImpl::GetRevision()
{
  return m_revisionNumber;
}

int
FileInterfaceImpl::GetChunkCount()
{
  return m_chunkCount;
}

void
FileInterfaceImpl::ReadFileMetadata(void *fileMetadata)
{
  assert(m_fileDescriptor.m_fileHeader.m_fileMetadataLength > 0 && fileMetadata);

  memcpy(fileMetadata, static_cast<const char *>(m_pageDirectory->Data()) + sizeof(PageDirectory), m_fileDescriptor.m_fileHeader.m_fileMetadataLength);
}

void
FileInterfaceImpl::WriteFileMetadata(const void *fileMetadata)
{
  assert(m_fileDescriptor.m_fileHeader.m_fileMetadataLength > 0 && fileMetadata);

  memcpy(static_cast<char *>(m_pageDirectory->WritableData()) + sizeof(PageDirectory), fileMetadata, m_fileDescriptor.m_fileHeader.m_fileMetadataLength);
}

void
FileInterfaceImpl::Rename(const char *fileName)
{
  m_fileDescriptor.m_fileName = std::string(fileName);
}

bool
FileInterfaceImpl::Commit()
{
  for (int indexPage = 0; indexPage < m_indexPageCount; indexPage++)
  {
    if (m_indexPages[indexPage] && m_indexPages[indexPage]->IsDirty())
    {
      if (!m_dataStore.WriteBuffer(*m_indexPages[indexPage]))
      {
        return false;
      }

      assert(!m_indexPages[indexPage]->IsDirty());
      m_dirtyIndexPageCount--;
      assert(m_dirtyIndexPageCount >= 0);

      if (IndexPageOffset(indexPage) != m_indexPages[indexPage]->Offset())
      {
        m_dataStore.MakeWritable(m_pageDirectory, ExtentAllocator::PageDirectoryExtent);
        SetIndexPageOffset(indexPage, m_indexPages[indexPage]->Offset());
      }
    }
  }

  if (!m_dataStore.WriteBuffer(*m_pageDirectory))
  {
    return false;
  }

  m_fileDescriptor.m_fileHeader.m_headPageDirectoryOffset = m_pageDirectory->Offset();

  if (m_dataStore.UpdateFileTable(m_fileDescriptor))
  {
    if (!m_dataStore.CommitFileTable())
    {
      return false;
    }
  }

  m_dataStore.Flush();
  return true;
}

void
HueBulkDataStoreImpl::SetErrorMessage(std::string const &errorMessage)
{
  m_errorMessage = errorMessage;
}

DataStoreBuffer *
HueBulkDataStoreImpl::ReadBuffer(int64_t offset, int32_t size)
{
  assert(offset >= 0 && size >= 0);

  if (size == 0)
  {
    return NULL;
  }

  void *data = malloc(size);
  Error error;
  if(m_fileHandle.Read(data, offset, size, error))
  {
    return new DataStoreBuffer(offset, size, false, data, true);
  }
  else
  {
    SetErrorMessage("Read error: " + error.string);
    free(data);
    return NULL;
  }
}

DataStoreBuffer *
HueBulkDataStoreImpl::CreateBuffer(int32_t size, ExtentAllocator::ExtentType type)
{
  assert(size > 0);
  return new DataStoreBuffer(m_extentAllocator->Allocate(size, type), size);
}

void
HueBulkDataStoreImpl::MakeWritable(DataStoreBuffer *buffer, ExtentAllocator::ExtentType type)
{
  if (!buffer->IsDirty())
  {
    buffer->SetOffset(m_extentAllocator->Allocate(buffer->Size(), type));
  }
}

bool
HueBulkDataStoreImpl::WriteBuffer(DataStoreBuffer const &buffer)
{
  if (!buffer.IsDirty()) return true;

  assert(buffer.Offset() >= 0 && buffer.Size() > 0);

  Error error;
  if (m_fileHandle.Write(buffer.Data(), buffer.Offset(), buffer.Size(), error))
  {
    buffer.ClearDirty();
    return true;
  }
  else
  {
    SetErrorMessage("Write error: " + error.string);
    return false;
  }
}

bool
HueBulkDataStoreImpl::ReadHeaderAndFileTable()
{
  m_header = ReadBuffer(0, sizeof(DataStoreHeader));
  if (!m_header || strncmp(GetDataStoreHeader().m_magic, "HueDataStore", sizeof(GetDataStoreHeader().m_magic)) != 0)
  {
    SetErrorMessage("File is not a HueDataStore");
    return false;
  }

  if (GetDataStoreHeader().m_fileTableCount != 0)
  {
    m_fileTable = ReadBuffer(GetDataStoreHeader().m_fileTableOffset, (sizeof(FileHeader) + GetDataStoreHeader().m_fileNameLength) * GetDataStoreHeader().m_fileTableCount);
    if (!m_fileTable)
    {
      SetErrorMessage("Couldn't read file table");
      return false;
    }
  }
  else
  {
    m_fileTable = NULL;
  }

  for (int fileIndex = 0; fileIndex < GetDataStoreHeader().m_fileTableCount; fileIndex++)
  {
    const FileHeader *
      fileHeader = reinterpret_cast<const FileHeader *>(static_cast<const char *>(m_fileTable->Data()) + (sizeof(FileHeader) + GetDataStoreHeader().m_fileNameLength) * fileIndex);

    const char *
      fileName = reinterpret_cast<const char *>(fileHeader + 1);

    const char *
      fileNameEnd = static_cast<const char *>(memchr(fileName, '\0', GetDataStoreHeader().m_fileNameLength));

    int
      fileNameLength = fileNameEnd ? int(fileNameEnd - fileName) : GetDataStoreHeader().m_fileNameLength;

    m_fileDescriptorList.push_back(DataStoreFileDescriptor(fileIndex, *fileHeader, std::string(fileName, fileNameLength)));
  }

  return true;
}

const DataStoreHeader &
HueBulkDataStoreImpl::GetDataStoreHeader()
{
  assert(m_header);
  return *static_cast<const DataStoreHeader *>(m_header->Data());
}

DataStoreFileDescriptor *
HueBulkDataStoreImpl::FindFile(int fileIndex)
{
  assert(m_header && "Datastore must be open");
  assert(fileIndex >= 0 && fileIndex < int(m_fileDescriptorList.size()));

  std::list<DataStoreFileDescriptor>::iterator
    it = m_fileDescriptorList.begin();

  std::advance(it, fileIndex);
  return &*it;
}

DataStoreFileDescriptor *
HueBulkDataStoreImpl::FindFile(const char *fileName)
{
  std::list<DataStoreFileDescriptor>::iterator fileDescriptor;

  for (fileDescriptor = m_fileDescriptorList.begin(); fileDescriptor != m_fileDescriptorList.end(); ++fileDescriptor)
  {
    if (fileDescriptor->m_fileName == fileName)
    {
      break;
    }
  }

  if (fileDescriptor == m_fileDescriptorList.end())
  {
    return NULL;
  }
  else
  {
    return &*fileDescriptor;
  }
}

HueBulkDataStore::FileInterface *
HueBulkDataStoreImpl::OpenFileInternal(DataStoreFileDescriptor &fileDescriptor, int revision)
{
  int64_t
    pageDirectoryOffset = fileDescriptor.m_fileHeader.m_headPageDirectoryOffset;

  if (pageDirectoryOffset < 0)
  {
    SetErrorMessage("Illegal page directory offset in file header");
    return NULL;
  }

  int
    chunkCount = fileDescriptor.m_fileHeader.m_headChunkCount;

  if (chunkCount < 0)
  {
    SetErrorMessage("Illegal chunk count in file header");
    return NULL;
  }

  int
    revisionNumber = fileDescriptor.m_fileHeader.m_headRevisionNumber;

  if (revisionNumber < 0)
  {
    SetErrorMessage("Illegal revision number in file header");
    return NULL;
  }

  while (revisionNumber > revision && pageDirectoryOffset)
  {
    DataStoreBuffer
      *pageDirectoryBuffer = ReadBuffer(pageDirectoryOffset, sizeof(PageDirectory));

    if (!pageDirectoryBuffer)
    {
      SetErrorMessage("Couldn't read page directory");
      return NULL;
    }

    pageDirectoryOffset = static_cast<const PageDirectory *>(pageDirectoryBuffer->Data())->m_previousPageDirectoryOffset;
    chunkCount = static_cast<const PageDirectory *>(pageDirectoryBuffer->Data())->m_previousChunkCount;
    revisionNumber = static_cast<const PageDirectory *>(pageDirectoryBuffer->Data())->m_previousRevisionNumber;

    if (pageDirectoryOffset < 0)
    {
      SetErrorMessage("Illegal previous page directory offset");
      return NULL;
    }

    if (chunkCount < 0)
    {
      SetErrorMessage("Illegal previous chunk count");
      return NULL;
    }

    if (revisionNumber < 0)
    {
      SetErrorMessage("Illegal previous revision number");
      return NULL;
    }

    delete pageDirectoryBuffer;
  }

  if (!pageDirectoryOffset || revisionNumber != revision)
  {
    return NULL;
  }

  DataStoreBuffer
    *pageDirectory = NULL;

  int
    indexPageCount = (chunkCount + fileDescriptor.m_fileHeader.m_indexPageEntryCount - 1) / fileDescriptor.m_fileHeader.m_indexPageEntryCount;

  int
    pageDirectorySize = (int)sizeof(PageDirectory) + fileDescriptor.m_fileHeader.m_fileMetadataLength + (int)sizeof(int64_t) * indexPageCount;

  pageDirectory = ReadBuffer(pageDirectoryOffset, pageDirectorySize);

  if (!pageDirectory)
  {
    SetErrorMessage("Couldn't read page directory");
    return NULL;
  }

  return new FileInterfaceImpl(*this, pageDirectory, fileDescriptor, revisionNumber, chunkCount);
}

HueBulkDataStoreImpl::HueBulkDataStoreImpl() :
  m_readOnly(true),
  m_header(NULL),
  m_fileTable(NULL),
  m_extentAllocator(NULL)
{
}

HueBulkDataStoreImpl::~HueBulkDataStoreImpl()
{
  assert(!m_fileHandle.IsOpen() && "Must close the datastore before deleting it");

  delete m_header;
  delete m_fileTable;
  delete m_extentAllocator;
}

const char *
HueBulkDataStoreImpl::GetErrorMessage()
{
  return m_errorMessage.c_str();
}

class ExtentAllocator &
  HueBulkDataStoreImpl::GetExtentAllocator()
{
  return *m_extentAllocator;
}

bool
HueBulkDataStoreImpl::BuildExtentAllocatorForPageDirectory(FileHeader const &fileHeader, int64_t pageDirectoryOffset, int32_t chunkCount)
{
  int
    indexPageCount = (chunkCount + fileHeader.m_indexPageEntryCount - 1) / fileHeader.m_indexPageEntryCount;

  int
    pageDirectorySize = (int)sizeof(PageDirectory) + fileHeader.m_fileMetadataLength + (int)sizeof(int64_t) * indexPageCount;

  DataStoreBuffer *
    pageDirectoryBuffer = ReadBuffer(pageDirectoryOffset, pageDirectorySize);

  if (!pageDirectoryBuffer)
  {
    SetErrorMessage("Couldn't read page directory");
    return false;
  }

  m_extentAllocator->AddReference(pageDirectoryOffset, pageDirectorySize, ExtentAllocator::PageDirectoryExtent);

  const PageDirectory
    *pageDirectory = static_cast<const PageDirectory *>(pageDirectoryBuffer->Data());

  if (pageDirectory->m_previousPageDirectoryOffset)
  {
    bool
      success = BuildExtentAllocatorForPageDirectory(fileHeader, pageDirectory->m_previousPageDirectoryOffset, pageDirectory->m_previousChunkCount);

    if (!success)
    {
      return false;
    }
  }

  const int64_t
    *indexPageOffsets = reinterpret_cast<const int64_t *>(static_cast<const char *>(pageDirectoryBuffer->Data()) + sizeof(PageDirectory) + fileHeader.m_fileMetadataLength);

  int
    indexEntrySize = (int)sizeof(IndexEntry) + fileHeader.m_chunkMetadataLength;

  for (int indexPage = 0; indexPage < indexPageCount; indexPage++)
  {
    if (indexPageOffsets[indexPage])
    {
      int
        indexPageEntryCount = fileHeader.m_indexPageEntryCount;

      DataStoreBuffer *
        indexPageBuffer = ReadBuffer(indexPageOffsets[indexPage], indexPageEntryCount * indexEntrySize);

      if (!indexPageBuffer)
      {
        SetErrorMessage("Couldn't read index page");
        return false;
      }

      m_extentAllocator->AddReference(indexPageOffsets[indexPage], indexPageEntryCount * indexEntrySize, ExtentAllocator::IndexPageExtent);

      for (int indexPageEntryIndex = 0; indexPageEntryIndex < indexPageEntryCount; indexPageEntryIndex++)
      {
        const IndexEntry
          *indexPageEntry = reinterpret_cast<const IndexEntry *>(static_cast<const char *>(indexPageBuffer->Data()) + indexPageEntryIndex * indexEntrySize);

        if (indexPageEntry->m_offset)
        {
          m_extentAllocator->AddReference(indexPageEntry->m_offset, indexPageEntry->m_length, ExtentAllocator::ChunkExtent);
        }
      }

      delete indexPageBuffer;
    }
  }

  delete pageDirectoryBuffer;
  return true;
}

bool
HueBulkDataStoreImpl::BuildExtentAllocator()
{
  assert(m_header && m_fileTable && "Can only be called on a successfully opened datastore");

  if (m_extentAllocator)
  {
    return true;
  }

  Error error;

  int64_t
    fileSize = m_fileHandle.Size(error);

  if(fileSize < 0)
  {
    SetErrorMessage(error.string);
    return false;
  }

  m_extentAllocator = new ExtentAllocator(fileSize);

  m_extentAllocator->AddReference(m_header->Offset(), m_header->Size(), ExtentAllocator::DataStoreHeaderExtent);
  m_extentAllocator->AddReference(m_fileTable->Offset(), m_fileTable->Size(), ExtentAllocator::FileTableExtent);

  for (std::list<DataStoreFileDescriptor>::const_iterator it = m_fileDescriptorList.begin(); it != m_fileDescriptorList.end(); ++it)
  {
    if (it->m_fileHeader.m_headPageDirectoryOffset)
    {
      bool
        success = BuildExtentAllocatorForPageDirectory(it->m_fileHeader, it->m_fileHeader.m_headPageDirectoryOffset, it->m_fileHeader.m_headChunkCount);

      if (!success)
      {
        return false;
      }
    }
    else
    {
      // file has no page directory?!
    }
  }

  return true;
}

bool
HueBulkDataStoreImpl::UpdateFileTable(DataStoreFileDescriptor const &fileDescriptor)
{
  bool
    changed = false;

  int
    fileTableEntryCount = GetDataStoreHeader().m_fileTableCount;

  size_t
    fileTableEntrySize = sizeof(FileHeader) + GetDataStoreHeader().m_fileNameLength,
    fileTableSize = fileTableEntrySize * fileTableEntryCount;

  assert((m_fileTable ? m_fileTable->Size() : 0) == fileTableSize);

  int32_t
    fileNameLength = std::max(int32_t(fileDescriptor.m_fileName.length()), GetDataStoreHeader().m_fileNameLength);

  // Create new file table if necessary
  if (fileDescriptor.m_fileIndex == -1 || fileNameLength != GetDataStoreHeader().m_fileNameLength)
  {
    int
      newFileTableEntrySize = (int)sizeof(FileHeader) + std::max((int)fileDescriptor.m_fileName.length(), GetDataStoreHeader().m_fileNameLength);

    DataStoreBuffer
      *fileTable = CreateBuffer(newFileTableEntrySize * (fileTableEntryCount + 1), ExtentAllocator::FileTableExtent);

    for (int file = 0; file < fileTableEntryCount; file++)
    {
      memcpy(static_cast<char *>(fileTable->WritableData()) + newFileTableEntrySize * file, static_cast<const char *>(m_fileTable->Data()) + fileTableEntrySize * file, fileTableEntrySize);
    }

    delete m_fileTable;
    m_fileTable = fileTable;

    // Update header
    DataStoreHeader
      *header = static_cast<DataStoreHeader *>(m_header->WritableData());

    if (fileDescriptor.m_fileIndex == -1)
    {
      const_cast<DataStoreFileDescriptor &>(fileDescriptor).m_fileIndex = header->m_fileTableCount++;
    }

    header->m_fileNameLength = fileNameLength;

    fileTableEntryCount = header->m_fileTableCount;
    fileTableEntrySize = newFileTableEntrySize,
      fileTableSize = fileTableEntrySize * fileTableEntryCount;

    changed = true;
  }

  const FileHeader *
    fileHeader = reinterpret_cast<const FileHeader *>(static_cast<const char *>(m_fileTable->Data()) + fileTableEntrySize * fileDescriptor.m_fileIndex);

  const char *
    fileName = reinterpret_cast<const char *>(fileHeader + 1);

  if (memcmp(fileHeader, &fileDescriptor.m_fileHeader, sizeof(FileHeader)) != 0 ||
    strncmp(fileName, fileDescriptor.m_fileName.c_str(), GetDataStoreHeader().m_fileNameLength) != 0)
  {
    changed = true;
  }

  if (changed)
  {
    MakeWritable(m_fileTable, ExtentAllocator::FileTableExtent);

    FileHeader *
      fileHeader = reinterpret_cast<FileHeader *>(static_cast<char *>(m_fileTable->WritableData()) + fileTableEntrySize * fileDescriptor.m_fileIndex);

    char *
      fileName = reinterpret_cast<char *>(fileHeader + 1);

    memcpy(fileHeader, &fileDescriptor.m_fileHeader, sizeof(FileHeader));
    strncpy(fileName, fileDescriptor.m_fileName.c_str(), GetDataStoreHeader().m_fileNameLength);
  }

  return changed;
}

bool
HueBulkDataStoreImpl::CommitFileTable()
{
  if (m_fileTable)
  {
    bool
      isSuccess = WriteBuffer(*m_fileTable);

    if (!isSuccess)
    {
      return false;
    }

    if (GetDataStoreHeader().m_fileTableOffset != m_fileTable->Offset())
    {
      static_cast<DataStoreHeader *>(m_header->WritableData())->m_fileTableOffset = m_fileTable->Offset();
    }
  }

  return WriteBuffer(*m_header);
}

void
HueBulkDataStoreImpl::Flush()
{
  m_fileHandle.Flush();
}

bool
HueBulkDataStoreImpl::EnableWriting()
{
  if (!m_readOnly) return true;

  Error error;
  if(!m_fileHandle.EnableWriting(error))
  {
    SetErrorMessage(error.string);
    return false;
  }

  m_readOnly = false;
  return BuildExtentAllocator();
}

int
HueBulkDataStoreImpl::GetFileCount()
{
  return int(m_fileDescriptorList.size());
}

const char*
HueBulkDataStoreImpl::GetFileName(int fileIndex)
{
  return FindFile(fileIndex)->m_fileName.c_str();
}

int
HueBulkDataStoreImpl::GetHeadRevisionNumber(int fileIndex)
{
  return FindFile(fileIndex)->m_fileHeader.m_headRevisionNumber;
}

int
HueBulkDataStoreImpl::GetFileType(int fileIndex)
{
  return FindFile(fileIndex)->m_fileHeader.m_fileType;
}

int
HueBulkDataStoreImpl::GetChunkMetadataLength(int fileIndex)
{
  return FindFile(fileIndex)->m_fileHeader.m_chunkMetadataLength;
}

int
HueBulkDataStoreImpl::GetFileMetadataLength(int fileIndex)
{
  return FindFile(fileIndex)->m_fileHeader.m_fileMetadataLength;
}

HueBulkDataStore::FileInterface *
HueBulkDataStoreImpl::OpenFile(const char *fileName)
{
  DataStoreFileDescriptor *
    fileDescriptor = FindFile(fileName);

  if (!fileDescriptor)
  {
    SetErrorMessage("File not found");
    return NULL;
  }
  else
  {
    return OpenFileInternal(*fileDescriptor, fileDescriptor->m_fileHeader.m_headRevisionNumber);
  }
}

HueBulkDataStore::FileInterface *
HueBulkDataStoreImpl::OpenFileRevision(const char *fileName, int revision)
{
  DataStoreFileDescriptor *
    fileDescriptor = FindFile(fileName);

  if (!fileDescriptor)
  {
    SetErrorMessage("File not found");
    return NULL;
  }
  else
  {
    return OpenFileInternal(*fileDescriptor, revision);
  }
}

HueBulkDataStore::FileInterface *
HueBulkDataStoreImpl::AddFile(const char *fileName, int chunkCount, int indexPageEntryCount, int fileType, int chunkMetadataLength, int fileMetadataLength, bool overwriteExisting)
{
  assert(!m_readOnly && m_extentAllocator);

  DataStoreFileDescriptor *
    fileDescriptor = FindFile(fileName);

  if (fileDescriptor)
  {
    if (!overwriteExisting)
    {
      SetErrorMessage("File already exists");
      return NULL;
    }
    else if (!fileDescriptor->m_openRevisions.empty())
    {
      SetErrorMessage("Cannot overwrite a file which is open");
      return NULL;
    }
  }

  if (chunkCount < 0)
  {
    SetErrorMessage("Illegal chunk count");
    return NULL;
  }

  if (indexPageEntryCount <= 0)
  {
    SetErrorMessage("Illegal index page entry count");
    return NULL;
  }

  if (chunkMetadataLength < 0 || chunkMetadataLength > 1000)
  {
    SetErrorMessage("Illegal chunk metadata length");
    return NULL;
  }

  if (fileMetadataLength < 0 || fileMetadataLength > 1000)
  {
    SetErrorMessage("Illegal file metadata length");
    return NULL;
  }

  int
    indexPageCount = (chunkCount + indexPageEntryCount - 1) / indexPageEntryCount;

  int
    pageDirectorySize = (int)sizeof(PageDirectory) + fileMetadataLength + (int)sizeof(int64_t) * indexPageCount;

  DataStoreBuffer
    *pageDirectory = CreateBuffer(pageDirectorySize, ExtentAllocator::PageDirectoryExtent);

  FileHeader
    fileHeader = { 0 };

  fileHeader.m_headPageDirectoryOffset = pageDirectory->Offset();
  fileHeader.m_headChunkCount = chunkCount;
  fileHeader.m_headRevisionNumber = 0;
  fileHeader.m_indexPageEntryCount = indexPageEntryCount;
  fileHeader.m_fileType = fileType;
  fileHeader.m_chunkMetadataLength = chunkMetadataLength;
  fileHeader.m_fileMetadataLength = fileMetadataLength;

  if (fileDescriptor)
  {
    fileDescriptor->m_fileHeader = fileHeader;
  }
  else
  {
    m_fileDescriptorList.push_back(DataStoreFileDescriptor(-1, fileHeader, fileName));
    fileDescriptor = &m_fileDescriptorList.back();
  }

  return new FileInterfaceImpl(*this, pageDirectory, *fileDescriptor, 0, chunkCount);
}

bool
HueBulkDataStoreImpl::RemoveFile(const char *fileName)
{
  std::list<DataStoreFileDescriptor>::iterator it;

  for (it = m_fileDescriptorList.begin(); it != m_fileDescriptorList.end(); ++it)
  {
    if (it->m_fileName == fileName)
    {
      break;
    }
  }

  if (it == m_fileDescriptorList.end())
  {
    SetErrorMessage("File not found");
    return false;
  }

  int
    fileIndex = it->m_fileIndex;

  m_fileDescriptorList.erase(it);

  // Create new file table if necessary
  if (fileIndex != -1)
  {
    for (it = m_fileDescriptorList.begin(); it != m_fileDescriptorList.end(); ++it)
    {
      if (it->m_fileIndex > fileIndex)
      {
        it->m_fileIndex--;
      }
    }

    int
      fileTableEntryCount = GetDataStoreHeader().m_fileTableCount;

    int
      fileTableEntrySize = (int)sizeof(FileHeader) + GetDataStoreHeader().m_fileNameLength,
      fileTableSize = fileTableEntrySize * fileTableEntryCount;

    DataStoreBuffer
      *fileTable = CreateBuffer(fileTableEntrySize * (fileTableEntryCount - 1), ExtentAllocator::FileTableExtent);

    for (int file = 0; file < fileTableEntryCount - 1; file++)
    {
      memcpy(static_cast<char *>(fileTable->WritableData()) + fileTableEntrySize * file, static_cast<const char *>(m_fileTable->Data()) + fileTableEntrySize * (file >= fileIndex ? file + 1 : file), fileTableEntrySize);
    }

    delete m_fileTable;
    m_fileTable = fileTable;

    // Update header
    DataStoreHeader
      *header = static_cast<DataStoreHeader *>(m_header->WritableData());

    header->m_fileTableCount--;
  }

  return CommitFileTable();
}

void
HueBulkDataStoreImpl::CloseFile(HueBulkDataStore::FileInterface *fileInterface)
{
  delete static_cast<FileInterfaceImpl *>(fileInterface);
}

bool
HueBulkDataStoreImpl::IsOpen()
{
  return m_fileHandle.IsOpen();
}

bool
HueBulkDataStoreImpl::IsReadOnly()
{
  return m_readOnly;
}

bool
HueBulkDataStoreImpl::Open(const char *fileName)
{
  assert(!m_fileHandle.IsOpen() && "Already open");

  Error error;
  if(!m_fileHandle.Open(fileName, false, false, false, error))
  {
    SetErrorMessage("Open error: " + error.string);
    return false;
  }

  if (!ReadHeaderAndFileTable())
  {
    Close();
    return false;
  }

  return true;
}

bool
HueBulkDataStoreImpl::CreateNew(const char *fileName, bool overwriteExisting)
{
  assert(!m_fileHandle.IsOpen() && "Already open");

  Error error;
  if(!m_fileHandle.Open(fileName, true, overwriteExisting, true, error))
  {
    SetErrorMessage("Open error: " + error.string);
    return false;
  }

  m_extentAllocator = new ExtentAllocator(sizeof(DataStoreHeader));

  m_header = CreateBuffer(sizeof(DataStoreHeader), ExtentAllocator::DataStoreHeaderExtent);
  assert(m_header->Offset() == 0);

  DataStoreHeader
    *header = static_cast<DataStoreHeader *>(m_header->WritableData());

  strncpy(header->m_magic, "HueDataStore", sizeof(header->m_magic));
  header->m_version = (1 << 16) + 0; // (major << 16) + minor

  m_fileTable = NULL;

  m_readOnly = false;
  return true;
}

void
HueBulkDataStoreImpl::Close()
{
  if(m_fileHandle.IsOpen())
  {
    m_fileHandle.Close();
  }

  m_readOnly = true;

  delete m_header;
  delete m_fileTable;
  delete m_extentAllocator;

  m_header = NULL;
  m_fileTable = NULL;
  m_extentAllocator = NULL;

  m_fileDescriptorList.clear();
}

HueBulkDataStore *
HueBulkDataStore::Open(const char *fileName)
{
  HueBulkDataStoreImpl *dataStore = new HueBulkDataStoreImpl();

  dataStore->Open(fileName);

  return dataStore;
}

HueBulkDataStore *
HueBulkDataStore::CreateNew(const char *fileName, bool overwriteExisting)
{
  HueBulkDataStoreImpl *dataStore = new HueBulkDataStoreImpl();

  dataStore->CreateNew(fileName, overwriteExisting);

  return dataStore;
}

void
HueBulkDataStore::Close(HueBulkDataStore *hueBulkDataStore)
{
  HueBulkDataStoreImpl *dataStore = static_cast<HueBulkDataStoreImpl *>(hueBulkDataStore);

  dataStore->Close();
  delete dataStore;
}

void
HueBulkDataStore::ReleaseBuffer(HueBulkDataStore::Buffer *buffer)
{
  assert(buffer);
  delete static_cast<DataStoreBuffer *>(buffer);
}

HueBulkDataStore::~HueBulkDataStore()
{}
