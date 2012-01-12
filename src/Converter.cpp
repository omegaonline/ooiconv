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

namespace
{
	class NoConversionException :
			public ExceptionImpl<IConv::INoConversionException>
	{
	public:
		BEGIN_INTERFACE_MAP(NoConversionException)
			INTERFACE_ENTRY_CHAIN(ExceptionImpl<IConv::INoConversionException>)
		END_INTERFACE_MAP()
	};

	class IllegalInputException :
			public ExceptionImpl<IConv::IIllegalInputException>
	{
	public:
		BEGIN_INTERFACE_MAP(IllegalInputException)
			INTERFACE_ENTRY_CHAIN(ExceptionImpl<IConv::IIllegalInputException>)
		END_INTERFACE_MAP()
	};

	class IncompleteInputException :
			public ExceptionImpl<IConv::IIncompleteInputException>
	{
	public:
		BEGIN_INTERFACE_MAP(IncompleteInputException)
			INTERFACE_ENTRY_CHAIN(ExceptionImpl<IConv::IIncompleteInputException>)
		END_INTERFACE_MAP()
	};
}

Converter::Converter() :
		m_cd(iconv_t(-1))
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
			ObjectImpl<NoConversionException>* pNew = ObjectImpl<NoConversionException>::CreateInstance();
			pNew->m_strDesc = Omega::string_t::constant("Cannot convert from encoding '{0}' to encoding '{1}'") % strFrom % strTo;
			throw static_cast<IConv::INoConversionException*>(pNew);
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
	void* TODO; // This could all be made faster by using a dynamic buffer array rather than a string_t for m_strIn

	Threading::Guard guard(m_lock);

	m_ptrInput = pInStream;
	m_strIn.Clear();
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
		if (m_strIn.Length() < outBytes)
		{
			// Try not to over-read... obviously this will struggle when the output encoding is wider than the input encoding.
			byte_t buf[1024] = {0};
			size_t r = outBytes;
			if (r > sizeof(buf))
				r = sizeof(buf);

			r = m_ptrInput->ReadBytes(static_cast<uint32_t>(r),buf);
			if (r != 0)
				m_strIn += Omega::string_t(reinterpret_cast<char*>(buf),r);
			else
			{
				// No more...
				if (last_err == EINVAL)
				{
					// Incomplete input
					ObjectImpl<IncompleteInputException>* pNew = ObjectImpl<IncompleteInputException>::CreateInstance();
					pNew->m_strDesc = Omega::string_t::constant("Input conversion stopped due to an incomplete character or shift sequence at the end of the input buffer.");
					throw static_cast<IConv::IIncompleteInputException*>(pNew);
				}
				break;
			}

			// Clear last error
			last_err = 0;
		}

		const char* inBuf = m_strIn.c_str();
		size_t inBytes = m_strIn.Length();

#if defined(ICONV_CONST)
		if (iconv(m_cd,const_cast<char**>(&inBuf),&inBytes,&outBuf,&outBytes) == size_t(-1))
#else
		if (iconv(m_cd,&inBuf,&inBytes,&outBuf,&outBytes) == size_t(-1))
#endif
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
				{
					// Illegal input
					ObjectImpl<IllegalInputException>* pNew = ObjectImpl<IllegalInputException>::CreateInstance();
					pNew->m_strDesc = Omega::string_t::constant("Input conversion stopped due to an input byte '{0}' that does not belong to the input codeset '{1}'.") % static_cast<uint8_t>(*inBuf) % m_strFrom;
					throw static_cast<IConv::IIllegalInputException*>(pNew);
				}

			default:
				throw ISystemException::Create(errno);
			}
		}

		// Drop off what we have used
		m_strIn = m_strIn.Mid(0,inBuf - m_strIn.c_str());
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
