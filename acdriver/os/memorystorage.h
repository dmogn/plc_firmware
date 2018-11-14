/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 05.11.2011 21:44:17
 */ 


#ifndef MEMORYSTORAGE_H_
#define MEMORYSTORAGE_H_

#define MEMORY_STORAGE_MAX_FILES_COUNT 8

#define MEMORY_STORAGE_FORMAT_MAGIC_NUMBER 213

#define MEMORY_STORAGE_SIZE 1024

/** Internal data structure for memory storage */ 
typedef struct
{
	uint8_t key;
	uint8_t version;
	uint16_t start_address;
	uint16_t size;
} memoryStorageFile;

typedef struct
{
	uint8_t magic_number;
	uint8_t files_count;
	memoryStorageFile memory_storage_files[MEMORY_STORAGE_MAX_FILES_COUNT];
} memoryStorageFileSystem;

void MemoryStorageInit();

/** Write data accessible by key and store format version of data. Return TRUE while sucessfull. */
uint8_t MemoryStorageWrite(uint8_t key, uint8_t version, void *data, uint16_t size);
/** Read data by key. Return readed size. Return UINT16_MAX while key not found. */ 
uint16_t MemoryStorageRead(uint8_t key, void *data, uint16_t maxSize);
/** Read version of stored data by key. Return UINT8_MAX while key not found. */
uint8_t MemoryStorageReadVersion(uint8_t key);

#endif /* MEMORYSTORAGE_H_ */