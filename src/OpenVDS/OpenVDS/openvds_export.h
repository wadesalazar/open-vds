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

#ifndef OPENVDS_EXPORT_H
#define OPENVDS_EXPORT_H

#ifdef OPENVDS_STATIC_DEFINE
#  define OPENVDS_EXPORT
#  define OPENVDS_NO_EXPORT
#else
#  if _WIN32
#    ifndef OPENVDS_EXPORT
#      ifdef openvds_EXPORTS
          /* We are building this library */
#        define OPENVDS_EXPORT __declspec(dllexport)
#      else
          /* We are using this library */
#        define OPENVDS_EXPORT __declspec(dllimport)
#      endif
#    endif

#    ifndef OPENVDS_NO_EXPORT
#      define OPENVDS_NO_EXPORT 
#    endif
#  else
#    ifndef OPENVDS_EXPORT
#      ifdef openvds_EXPORTS
          /* We are building this library */
#        define OPENVDS_EXPORT __attribute__((visibility("default")))
#      else
          /* We are using this library */
#        define OPENVDS_EXPORT __attribute__((visibility("default")))
#      endif
#    endif

#    ifndef OPENVDS_NO_EXPORT
#      define OPENVDS_NO_EXPORT __attribute__((visibility("hidden")))
#    endif

#  endif
#endif

#ifndef OPENVDS_DEPRECATED
#  if _WIN32
#    define OPENVDS_DEPRECATED __declspec(deprecated)
#  else
#    define OPENVDS_DEPRECATED __attribute__ ((__deprecated__))
#  endif
#endif

#ifndef OPENVDS_DEPRECATED_EXPORT
#  define OPENVDS_DEPRECATED_EXPORT OPENVDS_EXPORT OPENVDS_DEPRECATED
#endif

#ifndef OPENVDS_DEPRECATED_NO_EXPORT
#  define OPENVDS_DEPRECATED_NO_EXPORT OPENVDS_NO_EXPORT OPENVDS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef OPENVDS_NO_DEPRECATED
#    define OPENVDS_NO_DEPRECATED
#  endif
#endif

#endif /* OPENVDS_EXPORT_H */
