// This file is part of libmbmeteor.
// Copyright (C) 2019 Buenia.
//
// libmbmeteor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libmbmeteor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libmbmeteor.  If not, see <https://www.gnu.org/licenses/>.

    
#ifndef LIBMBMETEOR_API_H
#define LIBMBMETEOR_API_H

#if defined(_WIN32) && !defined(LIBMBMETEOR_STATIC)
    #ifdef LIBMBMETEOR_EXPORTS
        #define LIBMBMETEOR_API __declspec(dllexport)
    #else
        #define LIBMBMETEOR_API __declspec(dllimport)
    #endif // LIBMBMETEOR_EXPORTS

    #define LIBMBMETEOR __cdecl
#else
    #define LIBMBMETEOR_API
    #define LIBMBMETEOR
#endif // _WIN32

#endif // LIBMBMETEOR_API_H
