///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007 Rick Taylor
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

#if !defined(OOICONV_CONVERTER_H_INCLUDED_)
#define OOICONV_CONVERTER_H_INCLUDED_

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
		public Omega::IConv::IConverter
{
public:
	Converter();
	virtual ~Converter();

	BEGIN_INTERFACE_MAP(Converter)
		INTERFACE_ENTRY(Omega::IO::IInputStream)
		INTERFACE_ENTRY(Omega::IConv::IConverter)
		INTERFACE_ENTRY(Omega::TypeInfo::IProvideObjectInfo)
	END_INTERFACE_MAP()

private:
	Omega::Threading::Mutex                 m_lock;
	iconv_t                                 m_cd;
	OTL::ObjectPtr<Omega::IO::IInputStream> m_ptrInput;	
	std::string                             m_strIn;
	Omega::string_t                         m_strFrom;

	// IConverter methods
public:
	virtual void SetTranscoding(const Omega::string_t& strFrom, const Omega::string_t& strTo);
	virtual void SetInputStream(Omega::IO::IInputStream* pInStream);

	// IO::IInputStream methods
public:
	virtual Omega::uint32_t ReadBytes(Omega::uint32_t lenBytes, Omega::byte_t* data);
};


#endif // OOICONV_CONVERTER_H_INCLUDED_
