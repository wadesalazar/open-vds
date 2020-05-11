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

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.logging.Level;
import java.util.logging.Logger;

import static org.opengroup.openvds.BufferUtils.checkDirectBuffer;

public class Cleaner {
    private static Method cleanerMethod = null;
    private static Method cleanMethod = null;
    private static Method viewedBufferMethod = null;
    private static Method freeMethod = null;
    private static Method invokeCleanerMethod = null;
    private static Object unsafeObject = null;

    private static void prepareMethods() {
        try {
            final Class<?> unsafeClass = Class.forName("sun.misc.Unsafe");
            final Field theUnsafeField = unsafeClass.getDeclaredField("theUnsafe");
            theUnsafeField.setAccessible(true);
            unsafeObject = theUnsafeField.get(null);
            invokeCleanerMethod = unsafeClass.getMethod("invokeCleaner", java.nio.ByteBuffer.class);
            invokeCleanerMethod.setAccessible(true);
        } catch (ClassNotFoundException | NoSuchFieldException | IllegalAccessException | NoSuchMethodException e) {
            invokeCleanerMethod = null;
            unsafeObject = null;
            return;
        }
    }

    private static Method loadMethod(String className, String methodName) {
        try {
            Method method = Class.forName(className).getMethod(methodName);
            method.setAccessible(true);// according to the Java documentation, by default, a reflected object is not accessible
            return method;
        } catch (NoSuchMethodException ex) {
            return null; // the method was not found
        } catch (SecurityException ex) {
            return null; // setAccessible not allowed by security policy
        } catch (ClassNotFoundException ex) {
            return null; // the direct buffer implementation was not found
        } catch (Throwable t) {
            if (t.getClass().getName().equals("java.lang.reflect.InaccessibleObjectException")) {
                return null;// the class is in an unexported module
            } else {
                throw t;
            }
        }
    }

    static {
        // Oracle JRE / OpenJDK
        cleanerMethod = loadMethod("sun.nio.ch.DirectBuffer", "cleaner");
        cleanMethod = loadMethod("sun.misc.Cleaner", "clean");
        viewedBufferMethod = loadMethod("sun.nio.ch.DirectBuffer", "viewedBuffer");
        if (viewedBufferMethod == null) {
            // They changed the name in Java 7
            viewedBufferMethod = loadMethod("sun.nio.ch.DirectBuffer", "attachment");
        }
        // Java >= 9
        prepareMethods();

        try {
            // Apache Harmony (allocated directly, to not trigger allocator used
            // logic in BufferUtils)
            freeMethod = ByteBuffer.allocateDirect(1).getClass().getMethod("free");
        } catch (NoSuchMethodException ex) {
        } catch (SecurityException ex) {
        }
    }

    /**
     * This methods tries to release resources backing the given direct buffer. This is a best effort methods meant to
     * manage all commonly allocated direct byte buffers depending on the JRE versions and platforms.
     * If the proper release methods fails this methods return false.
     *
     * @param toBeDestroyed The buffer to release resources
     * @return true if the appropriate release methods have been successfully executed. False otherwise.
     */
    public static boolean tryRelease(Buffer toBeDestroyed) {
        try{
            checkDirectBuffer(toBeDestroyed);
        }catch(IllegalArgumentException e){
            return false;
        }

        try {
            if (freeMethod != null) {
                freeMethod.invoke(toBeDestroyed);
            } else {
                //TODO load the methods only once, store them into a cache (only for Java >= 9)
                Method localCleanerMethod;
                if (cleanerMethod == null) {
                    localCleanerMethod = loadMethod(toBeDestroyed.getClass().getName(), "cleaner");
                } else {
                    localCleanerMethod = cleanerMethod;
                }
                if (localCleanerMethod == null) {
                    Logger.getLogger(BufferUtils.class.getName()).log(Level.SEVERE,
                            "Buffer cannot be destroyed: {0}", toBeDestroyed);
                    return false;
                } else {
                    Object cleaner = localCleanerMethod.invoke(toBeDestroyed);
                    if (cleaner != null) {
                        Method localCleanMethod;
                        if (cleanMethod == null) {
                            if (cleaner instanceof Runnable) {
                                // jdk.internal.ref.Cleaner implements Runnable in Java 9
                                localCleanMethod = loadMethod(Runnable.class.getName(), "run");
                            } else {
                                // sun.misc.Cleaner does not implement Runnable in Java < 9
                                localCleanMethod = loadMethod(cleaner.getClass().getName(), "clean");
                            }
                        } else {
                            localCleanMethod = cleanMethod;
                        }
                        if (localCleanMethod == null) {
                            if (invokeCleanerMethod != null && unsafeObject != null) {
                                invokeCleanerMethod.invoke(unsafeObject, toBeDestroyed);
                            } else {
                                Logger.getLogger(BufferUtils.class.getName()).log(Level.SEVERE,
                                        "Buffer cannot be destroyed: {0}", toBeDestroyed);
                                return false;
                            }
                        } else {
                            localCleanMethod.invoke(cleaner);
                        }
                    } else {
                        Method localViewedBufferMethod;
                        if (viewedBufferMethod == null) {
                            localViewedBufferMethod = loadMethod(toBeDestroyed.getClass().getName(), "viewedBuffer");
                        } else {
                            localViewedBufferMethod = viewedBufferMethod;
                        }
                        if (localViewedBufferMethod == null) {
                            Logger.getLogger(BufferUtils.class.getName()).log(Level.SEVERE,
                                    "Buffer cannot be destroyed: {0}", toBeDestroyed);
                            return false;
                        } else {
                            // Try the alternate approach of getting the viewed buffer first
                            Object viewedBuffer = localViewedBufferMethod.invoke(toBeDestroyed);
                            if (viewedBuffer != null && viewedBuffer instanceof Buffer) {
                                if (!tryRelease((Buffer) viewedBuffer)) return false;
                                // Else nothing to do, we may be on android which has an
                                // internal system for freeing direct buffers
                            } else {
                                Logger.getLogger(BufferUtils.class.getName()).log(Level.SEVERE,
                                        "Buffer cannot be destroyed: {0}", toBeDestroyed);
                                return false;
                            }
                        }
                    }
                }
            }

            setInvalidCapacity(toBeDestroyed);
        } catch (IllegalAccessException | IllegalArgumentException | InvocationTargetException | SecurityException | NoSuchFieldException ex) {
            Logger.getLogger(BufferUtils.class.getName()).log(Level.SEVERE, "{0}", ex);

            return false;
        }
        return true;
    }

    /**
     * Set an invalid capacity to the provided buffer. The capacity can then be checked by the regular getCapacity()
     * to check if the buffer have been previoulsy released.
     *
     * @param toBeDestroyed
     * @throws NoSuchFieldException
     * @throws IllegalAccessException
     */
    private static void setInvalidCapacity(Buffer toBeDestroyed) throws NoSuchFieldException, IllegalAccessException {
        final Field capacity = Buffer.class.getDeclaredField("capacity");
        capacity.setAccessible(true);
        capacity.setInt(toBeDestroyed, -1);
    }
}
