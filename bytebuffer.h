#pragma once

#include "bytearray.h"
#include <cstdint>
#include <memory>

using namespace std;
using namespace gcommon;

namespace gcommon 
{
    class ByteBuffer : public ByteArray
	{
        private:
			uint32_t m_readPos;
			uint32_t m_writePos;

        public:
            //Creates a new ByteBuffer
            ByteBuffer( uint32_t capacity );

            //Creates a new ByteBuffer cloning another one
            ByteBuffer( const ByteBuffer& other );

            //Creates a new ByteBuffer copying the data from a ByteArray
            ByteBuffer( const ByteArray& byte_array );			

            //Appends a standard byte array at the end of this ByteBuffer
			int writeBuffer(const void* buffer, uint32_t buffer_size);

            //Appends a ByteArray at the end of this ByteBuffer
			int writeByteArray(const ByteArray& byte_array);

            //Appends a 8 bit integer at the end of the ByteBuffer
			int writeInt8(uint8_t value);

            //Appends a 16 bit integer at the end of the ByteBuffer, in network byte order
			int writeInt16(uint16_t value);

            //Appends a 32 bit integer at the end of the ByteBuffer, in network byte order
			int writeInt32(uint32_t value);

            //Appends a 8 bit integer repeated num_bytes times
			int fillBytes(uint32_t num_bytes, uint8_t byte);

            //Reads size bytes and stores them in destination_buffer
            int readBuffer( uint32_t size_to_read, void* destination_buffer );

            //Reads size bytes and stores them in a new ByteArray
            auto_ptr<ByteArray> readByteArray( uint32_t size );

            //Reads a 8 bit integer from the begin of the ByteBuffer
            uint8_t readInt8();

            //Reads a 16 bit integer from the begin of the ByteBuffer, in network byte order
            uint16_t readInt16();

            //Reads a 32 bit integer from the begin of the ByteBuffer, in network byte order
            uint32_t readInt32();

            //Gets the current write position
			uint32_t getWritePosition() const;

            //Sets the write position
            void setWritePosition( uint32_t position );

            //Gets the current read position
			uint32_t getReadPosition() const;

            //Sets the read position
            void setReadPosition( uint32_t position );

            //Resets the pointers
            void reset();

            //Skips the read of the indicated bytes
            void skip( uint32_t bytes );

			~ByteBuffer();
    };
}