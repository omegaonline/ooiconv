///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011 Rick Taylor
//
// This file is part of OOIConv, the Omega Online IConv wrapper library.
//
// OOIConv is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OOIConv is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OOIConv.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
//
// This library wraps the GNU LIBICONV Library from the Free Software Foundation, Inc.
//
// The GNU LIBICONV Library is free software; you can redistribute it
// and/or modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// The GNU LIBICONV Library is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with the GNU LIBICONV Library; see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc., 51 Franklin Street,
// Fifth Floor, Boston, MA 02110-1301, USA.
//
///////////////////////////////////////////////////////////////////////////////////

#include "Converter.h"

BEGIN_LIBRARY_OBJECT_MAP()
	OBJECT_MAP_ENTRY(Converter)
END_LIBRARY_OBJECT_MAP()

OMEGA_DEFINE_OID(Omega::IConv, OID_IConv, "{77A15551-E8F4-46D1-8991-7CDC07BE8EC3}");

using namespace Omega;
using namespace OTL;

Converter::Converter() :
		m_cd(iconv_t(-1)),
		m_bufpos(0)
{}

Converter::~Converter()
{
	if (m_cd != iconv_t(-1))
		iconv_close(m_cd);
}

void Converter::SetTranscoding(const string_t& strFrom, const string_t& strTo)
{
	iconv_t cd = iconv_open(strTo.c_str(),strFrom.c_str());
	if (cd == iconv_t(-1))
	{
		if (errno == EINVAL)
		{
			// No conversion
			throw INotFoundException::Create(string_t::constant("Cannot find suitable conversion from encoding '{0}' to encoding '{1}'") % strFrom % strTo);
		}

		throw ISystemException::Create(errno);
	}

	Threading::Guard guard(m_lock);

	if (m_cd != iconv_t(-1))
		iconv_close(m_cd);

	m_cd = cd;
	m_strFrom = strFrom;
}

void Converter::SetInputStream(IO::IInputStream* pInStream)
{
	Threading::Guard guard(m_lock);

	m_ptrInput = pInStream;
	m_bufpos = 0;
}

uint32_t Converter::ReadBytes(uint32_t lenBytes, byte_t* data)
{
	size_t outBytes = lenBytes;
	char*  outBuf = reinterpret_cast<char*>(data);

	Threading::Guard guard(m_lock);

	if (!m_ptrInput)
		return 0;

	for (int last_err = 0;outBytes != 0;)
	{
		// Read more...
		if (m_bufpos < outBytes)
		{
			// Try not to over-read... obviously this will struggle when the output encoding is wider than the input encoding.
			size_t r = outBytes - m_bufpos;
			if (r > sizeof(m_buffer) - m_bufpos)
				r = sizeof(m_buffer) - m_bufpos;

			r = m_ptrInput->ReadBytes(static_cast<uint32_t>(r),reinterpret_cast<byte_t*>(m_buffer + m_bufpos));
			if (r != 0)
				m_bufpos += r;
			else
			{
				// No more...
				if (last_err == EINVAL)
					throw INotFoundException::Create(string_t::constant("Conversion stopped due to an incomplete character or shift sequence at the end of the input buffer."));
				break;
			}

			// Clear last error
			last_err = 0;
		}

		ICONV_CONST char* inBuf = m_buffer;
		size_t inBytes = m_bufpos;

		if (iconv(m_cd,&inBuf,&inBytes,&outBuf,&outBytes) == size_t(-1))
		{
			if (errno == E2BIG)
				break;

			switch (errno)
			{
			case EINVAL:
				// Remember...
				last_err = errno;
				break;

			case EILSEQ:
				// Illegal input
				throw INotFoundException::Create(string_t::constant("Conversion stopped due to an input byte '{0}' that does not belong to the input codeset '{1}'.") % static_cast<uint8_t>(*inBuf) % m_strFrom);

			default:
				throw ISystemException::Create(errno);
			}
		}

		// Drop off what we have used
		memmove(m_buffer,inBuf,inBytes);
		m_bufpos = inBytes;
	}

	return static_cast<uint32_t>(lenBytes - outBytes);
}

string_t Converter::ConvertStream(const string_t& strEncoding, IO::IInputStream* inStream)
{
	SetTranscoding(strEncoding,Omega::string_t::constant("UTF-8"));
	SetInputStream(inStream);

	byte_t szBuf[1024] = {0};
	for (string_t ret;;)
	{
		uint32_t r = ReadBytes(static_cast<uint32_t>(sizeof(szBuf)),szBuf);
		if (r == 0)
			return ret;

		ret += string_t(reinterpret_cast<const char*>(szBuf),r);
	}
}

string_t Converter::ConvertBuffer(const string_t& strEncoding, uint32_t len, const byte_t* bytes)
{
	ObjectPtr<IO::IInputStream> ptrIn = IO::IInputStream::Create(len,bytes,false);

	return ConvertStream(strEncoding,ptrIn);
}
