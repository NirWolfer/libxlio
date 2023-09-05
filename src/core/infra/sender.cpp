/*
 * Copyright © 2013-2023 NVIDIA CORPORATION & AFFILIATES. ALL RIGHTS RESERVED.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "core/infra/sender.h"

neigh_send_data::neigh_send_data(iovec *iov, size_t sz, header *hdr, uint32_t mtu,
                                 uint32_t packet_id)
    : m_header(hdr->copy())
    , m_mtu(mtu)
    , m_packet_id(packet_id)
{
    BULLSEYE_EXCLUDE_BLOCK_START
    if (!iov || sz == 0U) {
        m_iov.iov_base = nullptr;
        m_iov.iov_len = 0;
        return;
    }
    BULLSEYE_EXCLUDE_BLOCK_END

    uint8_t *buff = nullptr;
    size_t total_len = 0;

    for (uint32_t i = 0; i < sz; i++) {
        total_len += iov[i].iov_len;
    }

    buff = new uint8_t[total_len];
    BULLSEYE_EXCLUDE_BLOCK_START
    if (!buff) {
        m_iov.iov_base = nullptr;
        m_iov.iov_len = 0;
        return;
    }
    BULLSEYE_EXCLUDE_BLOCK_END

    memcpy_fromiovec(buff, iov, sz, 0, total_len);
    m_iov.iov_base = buff;
    m_iov.iov_len = total_len;
}

neigh_send_data::neigh_send_data(neigh_send_data &&snd_data)
{
    m_header = snd_data.m_header;
    snd_data.m_header = nullptr;

    m_iov = snd_data.m_iov;
    snd_data.m_iov.iov_base = nullptr;
    snd_data.m_iov.iov_len = 0U;

    m_mtu = snd_data.m_mtu;
    m_packet_id = snd_data.m_packet_id;
}

neigh_send_data::~neigh_send_data()
{
    if (m_iov.iov_base) {
        delete[]((uint8_t *)m_iov.iov_base);
    }

    if (m_header) {
        delete m_header;
    }
}
