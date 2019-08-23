/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
****************************************************************************/

#ifndef WAVELET_H
#define WAVELET_H

#define WAVELET_MIN_COMPRESSION_TOLERANCE 0.01f

#define WAVELET_OLD_DATA_VERSION (666)
#define WAVELET_DATA_VERSION_1_0 (667)
#define WAVELET_DATA_VERSION_1_1 (668)
#define WAVELET_DATA_VERSION_1_2 (669) // added 1 bit mask support for novalue
#define WAVELET_DATA_VERSION_1_3 (670) // added handling of 0 values
#define WAVELET_DATA_VERSION_1_4 (671) // progressive wavelet transform

#define WAVELET_MIN_COMPRESSED_HEADER (6 * 4)

#endif