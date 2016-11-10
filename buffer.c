#include "buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "define.h"

buffer_t *alloc_buffer()
{
    buffer_t *buffer = zmalloc(sizeof(buffer_t));
    if (buffer == NULL) {
        goto err;
    }
    buffer->buff = zmalloc(DEFAULT_BUFF_SIZE);
    buffer->size = DEFAULT_BUFF_SIZE;
    buffer->read_idx = 0;
    buffer->write_idx = 0;

    return buffer;

err:
    if (buffer) {
        zfree(buffer->buff);
        zfree(buffer);
    }

    return NULL;
}

void free_buffer(buffer_t *buffer)
{
    if (buffer) {
        zfree(buffer->buff);
        zfree(buffer);
    }
}

void check_buffer_size(buffer_t *buffer, size_t avlid_size)
{
    if (buffer->read_idx > DEFAULT_BUFF_SIZE) {
        size_t data_len = get_readable_size(buffer);
        memmove(buffer->buff, buffer->buff + buffer->read_idx, data_len);
        buffer->read_idx = 0;
        buffer->write_idx = data_len;
    }
    if (get_writeable_size(buffer) < avlid_size) {
        size_t new_size = buffer->size + avlid_size;
        buffer->buff = zrealloc(buffer->buff, new_size);
        buffer->size = new_size;
    }
}

size_t get_readable_size(buffer_t *buffer)
{
    assert(buffer->size >= buffer->write_idx);
    assert(buffer->read_idx <= buffer->write_idx);
    return buffer->write_idx - buffer->read_idx;
}

size_t get_writeable_size(buffer_t *buffer)
{
    assert(buffer->size >= buffer->write_idx);
    assert(buffer->read_idx <= buffer->write_idx);
    return buffer->size - buffer->write_idx;
}