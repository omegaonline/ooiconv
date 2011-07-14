///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007,2011 Rick Taylor
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
		};
		
		interface IStringConverter : public IObject
		{
			virtual string_t ConvertStream(const string_t& strEncoding, IO::IInputStream* inStream) = 0;
			virtual string_t ConvertBuffer(const string_t& strEncoding, uint32_t len, const byte_t* bytes) = 0;
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

OMEGA_DEFINE_INTERFACE
(
	Omega::IConv, IStringConverter, "{B8595F79-2794-4853-999B-BE903BBA7BF8}",

	// Methods
	OMEGA_METHOD(string_t,ConvertStream,2,((in),const string_t&,strEncoding,(in),IO::IInputStream*,inStream))
	OMEGA_METHOD(string_t,ConvertBuffer,3,((in),const string_t&,strEncoding,(in),uint32_t,len,(in)(size_is(len)),const byte_t*,bytes))
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

#endif // OMEGA_GNU_ICONV_H_INCLUDED_
