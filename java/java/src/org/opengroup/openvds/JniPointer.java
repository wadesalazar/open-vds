/*
 * Copyright 2019 The Open Group
 * Copyright 2019 INT, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.opengroup.openvds;

import java.io.File;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * JNI wrapper for pointer to C++ object in OpenVdsJni library.
 *
 * Deletion of C++ object - metod deleteHandle() - must be implemented in
 * derived classes of JniPointer to properly call C++ class destructor.
 */
public abstract class JniPointer {

    static final String JNI_LIB_NAME = "openvdsjava";

    private static final Logger LOGGER = Logger.getLogger(JniPointer.class.getName());
    private static final String ERR_LIBRARY = "JNI library load failed";
    private static long _librarySize;

    private static native String cpLibraryDescription();

    protected abstract void deleteHandle();

    protected long _handle;
    protected boolean _ownHandle;

    static {
        try {
            if (JNI_LIB_NAME.indexOf('/') < 0 && JNI_LIB_NAME.indexOf('\\') < 0) {
                System.loadLibrary(JNI_LIB_NAME);
            } else {
                _librarySize = new File(JNI_LIB_NAME).length();
                System.load(JNI_LIB_NAME);
            }
        } catch (Throwable e) {
            LOGGER.log(Level.SEVERE, ERR_LIBRARY, e);
        }
    }

    public static String libraryDescription() {
        String str = JNI_LIB_NAME;
        if (_librarySize > 0) {
            str += " Size=" + _librarySize;
        }
        String nativeDescr = null;
        try {
            nativeDescr = cpLibraryDescription();
            if (nativeDescr != null) {
                str += " " + nativeDescr;
            }
        } catch (Throwable t) {
            str += " Error: " + t.toString();
        }
        return str;
    }

    public JniPointer(long handle, boolean ownHandle) {
        _handle = handle;
        _ownHandle = ownHandle;
    }

    @Override
    public void finalize() {
        release();
    }

    public synchronized void release() {
        // Deletion of C++ object
        if (_handle != 0) {
            if (_ownHandle) {
                deleteHandle();
            }
            _handle = 0;
        }
    }

    public void setHandle(long handle) {
        _handle = handle;
    }

    public long handle() {
        return _handle;
    }

    public boolean isNull() {
        return _handle == 0;
    }

    public void setOwnHandle(boolean ownHandle) {
        _ownHandle = ownHandle;
    }

    public boolean ownHandle() {
        return _ownHandle;
    }
}
