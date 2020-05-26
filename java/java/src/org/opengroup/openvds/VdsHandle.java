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

import java.io.Closeable;
import java.io.IOException;

/**
 * JNI wrapper for OpenVDS::VDSHandle.
 */
public class VdsHandle extends JniPointer implements Closeable {

    private static native void cpDeleteHandle(long handle);

    private static native long cpGetLayoutHandle(long handle);

    private static native long cpGetAccessManagerHandle(long handle);

    public VdsHandle(long handle, boolean ownHandle) {
        super(handle, ownHandle);
    }

    // Called by JniPointer.release()
    @Override
    protected synchronized void deleteHandle() {
        cpDeleteHandle(_handle);
    }

    public VolumeDataLayout getLayout() {
        return new VolumeDataLayout(cpGetLayoutHandle(_handle));
    }

    public VolumeDataAccessManager getAccessManager() {
        return new VolumeDataAccessManager(cpGetAccessManagerHandle(_handle));
    }

    @Override
    public void close() throws IOException {
        release();
    }
}
