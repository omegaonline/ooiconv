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

#if !defined(OOICONV_CONVERTER_H_INCLUDED_)
#define OOICONV_CONVERTER_H_INCLUDED_

#include <iconv.h>

#if defined(HAVE_CONFIG_H)
#include "ooiconv-autoconf.h"
#endif

#include <Omega/TypeInfo.h>
#include <Omega/IO.h>
#include <OTL/Exception.h>

#include "../include/IConv.h"

namespace Omega
{
	namespace IConv
	{
		extern "C" const Omega::guid_t OID_IConv;
	}
}

class Converter :
		public OTL::ObjectBase,
		public OTL::AutoObjectFactory<Converter,&Omega::IConv::OID_IConv>,
		public OTL::IProvideObjectInfoImpl<Converter>,
		public Omega::IConv::IConverter,
		public Omega::IConv::IStringConverter
{
public:
	Converter();
	virtual ~Converter();

	BEGIN_INTERFACE_MAP(Converter)
		INTERFACE_ENTRY(Omega::IConv::IStringConverter)
		INTERFACE_ENTRY(Omega::IConv::IConverter)
		INTERFACE_ENTRY(Omega::IO::IInputStream)		
		INTERFACE_ENTRY(Omega::TypeInfo::IProvideObjectInfo)
	END_INTERFACE_MAP()

private:
	Omega::Threading::Mutex                 m_lock;
	iconv_t                                 m_cd;
	OTL::ObjectPtr<Omega::IO::IInputStream> m_ptrInput;	
	Omega::string_t                         m_strFrom;
	char                                    m_buffer[1024];
	size_t                                  m_bufpos;

	// IConverter methods
public:
	virtual void SetTranscoding(const Omega::string_t& strFrom, const Omega::string_t& strTo);
	virtual void SetInputStream(Omega::IO::IInputStream* pInStream);

	// IO::IInputStream methods
public:
	virtual Omega::uint32_t ReadBytes(Omega::uint32_t lenBytes, Omega::byte_t* data);
	
	// IStringConverter methods
public:
	virtual Omega::string_t ConvertStream(const Omega::string_t& strEncoding, Omega::IO::IInputStream* inStream);
	virtual Omega::string_t ConvertBuffer(const Omega::string_t& strEncoding, Omega::uint32_t len, const Omega::byte_t* bytes);
};


#endif // OOICONV_CONVERTER_H_INCLUDED_
