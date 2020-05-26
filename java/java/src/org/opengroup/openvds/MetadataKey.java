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

public class MetadataKey {
    public MetadataType type;
    public String category;
    public String name;

    public MetadataKey(int type, String category, String name) {
        this(MetadataType.values()[type], category, name);
    }

    public MetadataKey(MetadataType type, String category, String name) {
        this.category = category;
        this.name = name;
        this.type = type;
    }

    public MetadataType getType() {
        return type;
    }

    public String getCategory() {
        return category;
    }

    public String getName() {
        return name;
    }
}
