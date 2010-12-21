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

#if !defined(OMEGA_GNU_ICONV_H_INCLUDED_)
#define OMEGA_GNU_ICONV_H_INCLUDED_

#if !defined(OMEGA_IO_H_INCLUDED_)
#include "./IO.h"
#endif

namespace Omega
{
	namespace IConv
	{
		interface IConverter : public IO::IInputStream
		{
			virtual void SetTranscoding(const string_t& strFrom, const string_t& strTo) = 0;
			virtual void SetInputStream(IO::IInputStream* pInStream) = 0;

			static string_t ToString(const string_t& strEncoding, IO::IInputStream* inStream);
		};

		interface INoConversionException : public IException
		{
		};

		interface IIllegalInputException : public IException
		{
		};

		interface IIncompleteInputException : public IException
		{
		};
	}
}

OMEGA_DEFINE_INTERFACE_DERIVED
(
	Omega::IConv, IConverter, Omega::IO, IInputStream, "{E585E6D4-2469-40C7-AF6D-7F3A4E8D0790}",

	// Methods
	OMEGA_METHOD_VOID(SetTranscoding,2,((in),const string_t&,strFrom,(in),const string_t&,strTo))
	OMEGA_METHOD_VOID(SetInputStream,1,((in),IO::IInputStream*,pInStream))
)

OMEGA_DEFINE_INTERFACE_DERIVED
(
	Omega::IConv, INoConversionException, Omega, IException, "{4DD88926-34CE-4B8F-8917-D060F3BB9C95}",

	OMEGA_NO_METHODS()
)

OMEGA_DEFINE_INTERFACE_DERIVED
(
	Omega::IConv, IIllegalInputException, Omega, IException, "{33ECC494-91C1-4D12-A301-FCFA8856A2EE}",

	OMEGA_NO_METHODS()
)

OMEGA_DEFINE_INTERFACE_DERIVED
(
	Omega::IConv, IIncompleteInputException, Omega, IException, "{C27AF18A-BC91-48DD-8591-EAD2853ACEAD}",

	OMEGA_NO_METHODS()
)

#if defined(OOICONV_INTERNAL)

#define OOICONV_EXPORTED_FUNCTION(ret_type,name,param_count,params) \
	OMEGA_LOCAL_FUNCTION(ret_type,name,param_count,params)

#else

#define OOICONV_EXPORTED_FUNCTION(ret_type,name,param_count,params) \
	OMEGA_EXPORTED_FUNCTION_IMPL(ret_type,name,param_count,params)

#endif

OOICONV_EXPORTED_FUNCTION(Omega::string_t,OOIConv_ToString,2,((in),const Omega::string_t&,strEncoding,(in),Omega::IO::IInputStream*,pInStream))
inline Omega::string_t Omega::IConv::IConverter::ToString(const string_t& strEncoding, IO::IInputStream* pInStream)
{
	return OOIConv_ToString(strEncoding,pInStream);
}

#endif // OMEGA_GNU_ICONV_H_INCLUDED_
