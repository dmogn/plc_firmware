/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 05.11.2011 21:44:00
 */ 

#include "os.h"

memoryStorageFileSystem memory_storage_file_system;

void MemoryStorageSaveFileSystem()
{
	uint8_t *data = &memory_storage_file_system;
	for (int8_t i=0; i<sizeof(memory_storage_file_system); i++)
	{
		EEPROM_write(i, *data);
		data++;
	}
}

void MemoryStoragReadFileSystem()
{
	uint8_t *data = &memory_storage_file_system;
	for (int8_t i=0; i<sizeof(memory_storage_file_system); i++)
	{
		*data = EEPROM_read(i);
		data++;
	}
}

void MemoryStorageInit()
{
	// check file system initialized in EEPROM
	uint8_t eeprom_magic_address = EEPROM_read(0);
	if (eeprom_magic_address == MEMORY_STORAGE_FORMAT_MAGIC_NUMBER)
	{
		// read file system structure from EEPROM:
		MemoryStoragReadFileSystem();
	}
	else
	{
		// init file system
		memory_storage_file_system.magic_number = MEMORY_STORAGE_FORMAT_MAGIC_NUMBER;
		memory_storage_file_system.files_count = 0;
		
		// save file system to EEPROM
		MemoryStorageSaveFileSystem();
	}	
}

uint8_t MemoryStorageWrite(uint8_t key, uint8_t version, void *data, uint16_t size)
{
	uint8_t *data_pointer = data;
	uint8_t file_index = 0;
	for (file_index = 0; file_index<memory_storage_file_system.files_count; file_index++)
		if (memory_storage_file_system.memory_storage_files[file_index].key == key)
		{
			if (memory_storage_file_system.memory_storage_files[file_index].size == size)
			{
				// same size - only overwrite file data
				for (unsigned int i=0; i<size; i++)
				{
					EEPROM_write(sizeof(memory_storage_file_system) + memory_storage_file_system.memory_storage_files[file_index].start_address + i, *data_pointer);
					data_pointer++;
				}
				
				if (memory_storage_file_system.memory_storage_files[file_index].version != version)
				{
					memory_storage_file_system.memory_storage_files[file_index].version = version;
					// write file system updates
					MemoryStorageSaveFileSystem();
				}
				
				return TRUE;
			}
			
			if (file_index == memory_storage_file_system.files_count-1)
			{// last file node - no data shifting
				memory_storage_file_system.files_count--;
				break;
			}
			
			// prepare data shift
			unsigned int shift_size = memory_storage_file_system.memory_storage_files[file_index].size;		    
			unsigned int shift_start_address = memory_storage_file_system.memory_storage_files[file_index].start_address;
			unsigned int shift_data_size = 0;
			
			for (unsigned int shift_index = file_index+1; shift_index<memory_storage_file_system.files_count; shift_index++)
			{
				// update shift data size
				shift_data_size += memory_storage_file_system.memory_storage_files[shift_index].size;
				
				// transfer file system record down in array
				memory_storage_file_system.memory_storage_files[shift_index-1].key = memory_storage_file_system.memory_storage_files[shift_index].key;
				memory_storage_file_system.memory_storage_files[shift_index-1].start_address = memory_storage_file_system.memory_storage_files[shift_index].start_address - shift_size;
				memory_storage_file_system.memory_storage_files[shift_index-1].size = memory_storage_file_system.memory_storage_files[shift_index].size;
				memory_storage_file_system.memory_storage_files[shift_index-1].version = memory_storage_file_system.memory_storage_files[shift_index].version;
			}
			
			// shift all data
			for (unsigned int shift_index=0; shift_index<shift_data_size; shift_data_size++)
			{
				uint8_t data_byte = EEPROM_read(sizeof(memory_storage_file_system) + shift_start_address + shift_size + shift_index);
				EEPROM_write(sizeof(memory_storage_file_system) + shift_start_address + shift_index, data_byte);
			}
			
			memory_storage_file_system.files_count--;
			break;			
		}
		
	// init new file
	if (memory_storage_file_system.files_count >= MEMORY_STORAGE_MAX_FILES_COUNT)
		return FALSE;// no empty file structure
		
	file_index = memory_storage_file_system.files_count;
	
	// check for memory size overhead
	uint16_t current_memory_size;
	if (memory_storage_file_system.files_count == 0)
		current_memory_size = sizeof(memory_storage_file_system);
	else
		current_memory_size = sizeof(memory_storage_file_system) + 
			memory_storage_file_system.memory_storage_files[file_index-1].start_address + 
			memory_storage_file_system.memory_storage_files[file_index-1].size;
			
	if (current_memory_size + size > MEMORY_STORAGE_SIZE)
		return FALSE;// no space for the file
		
	memory_storage_file_system.memory_storage_files[file_index].key = key;
	
	memory_storage_file_system.memory_storage_files[file_index].start_address = 
		memory_storage_file_system.memory_storage_files[file_index-1].start_address + 
		memory_storage_file_system.memory_storage_files[file_index-1].size;
	
	memory_storage_file_system.memory_storage_files[file_index].size = size;
	
	memory_storage_file_system.memory_storage_files[file_index].version = version;
	
	memory_storage_file_system.files_count++;
	
	// write file content
	for (unsigned int i=0; i<size; i++)
	{
		EEPROM_write(sizeof(memory_storage_file_system) + memory_storage_file_system.memory_storage_files[file_index].start_address + i, *data_pointer);
		data_pointer++;
	}
	
	// write file system updates
	MemoryStorageSaveFileSystem();
	
	return TRUE;
}

uint16_t MemoryStorageRead(uint8_t key, void *data, uint16_t maxSize)
{
	uint8_t *data_pointer = data;
	// find key in file system
	uint8_t file_index = 0;
	for (file_index = 0; file_index<memory_storage_file_system.files_count; file_index++)
		if (memory_storage_file_system.memory_storage_files[file_index].key == key)
		{
			uint16_t size;
			if (memory_storage_file_system.memory_storage_files[file_index].size < maxSize)
				size = memory_storage_file_system.memory_storage_files[file_index].size;
			else
				size = maxSize;
			
			for (int i=0; i<size; i++)
			{
				*data_pointer = EEPROM_read(sizeof(memory_storage_file_system) + memory_storage_file_system.memory_storage_files[file_index].start_address + i);
				data_pointer++;
			}
			
			return size;
		}
	
	// error - key not found
	return UINT16_MAX;
}

uint8_t MemoryStorageReadVersion(uint8_t key)
{
	for (unsigned int file_index = 0; file_index<memory_storage_file_system.files_count; file_index++)
		if (memory_storage_file_system.memory_storage_files[file_index].key == key)
		{
			return memory_storage_file_system.memory_storage_files[file_index].version;
		}
	
	// error - key not found
	return UINT8_MAX;
}