/* Copyright (C) 2012 Red Hat

 This file is part of IcedTea.

 IcedTea is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 IcedTea is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with IcedTea; see the file COPYING.  If not, write to the
 Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 02110-1301 USA.

 Linking this library statically or dynamically with other modules is
 making a combined work based on this library.  Thus, the terms and
 conditions of the GNU General Public License cover the whole
 combination.

 As a special exception, the copyright holders of this library give you
 permission to link this library with independent modules to produce an
 executable, regardless of the license terms of these independent
 modules, and to copy and distribute the resulting executable under
 terms of your choice, provided that you also meet, for each linked
 independent module, the terms and conditions of the license of that
 module.  An independent module is a module which is not derived from
 or based on this library.  If you modify this library, you may extend
 this exception to your version of the library, but you are not
 obligated to do so.  If you do not wish to do so, delete this
 exception statement from your version. */

#include <UnitTest++.h>

#include <npapi.h>

#include "browser_mock.h"
#include "MemoryLeakDetector.h"

#include "IcedTeaScriptablePluginObject.h"
#include "IcedTeaPluginUtils.h"

static NPP_t dummy_npp = {0,0};

SUITE(IcedTeaScriptablePluginObject) {
    TEST(destructor) {
        MemoryLeakDetector leak_detector;
        IcedTeaScriptablePluginObject* obj = new IcedTeaScriptablePluginObject(&dummy_npp);
        delete obj;
        CHECK(leak_detector.memory_leaks() == 0);
    }

    TEST(get_scriptable_java_object) {
        MemoryLeakDetector leak_detector;
        NPObject* obj = IcedTeaScriptablePluginObject::get_scriptable_java_package_object(&dummy_npp, "DummyPackage");
        browser_functions.releaseobject(obj);
        CHECK(leak_detector.memory_leaks() == 0);
    }
}

SUITE(IcedTeaScriptableJavaObject) {
    TEST(deallocate) {
        MemoryLeakDetector leak_detector;
        IcedTeaScriptableJavaObject* obj = new IcedTeaScriptableJavaObject(&dummy_npp);
        IcedTeaScriptableJavaObject::deAllocate(obj);
        CHECK(leak_detector.memory_leaks() == 0);
    }
}

SUITE(IcedTeaScriptableJavaPackageObject) {
    TEST(deallocate) {
        MemoryLeakDetector leak_detector;
        IcedTeaScriptableJavaPackageObject* obj = new IcedTeaScriptableJavaPackageObject(&dummy_npp);
        IcedTeaScriptableJavaPackageObject::deAllocate(obj);
        CHECK(leak_detector.memory_leaks() == 0);
    }

    TEST(get_scriptable_java_object) {
        MemoryLeakDetector leak_detector;

        NPObject* first_obj = IcedTeaScriptableJavaPackageObject::get_scriptable_java_object(&dummy_npp, "DummyClass", "DummyInstance", false);
        browser_functions.releaseobject(first_obj);

        /* After the first call, the object should be cached in the object map */
        NPObject* second_obj = IcedTeaScriptableJavaPackageObject::get_scriptable_java_object(&dummy_npp, "DummyClass", "DummyInstance", false);

        /* Objects should be the same, because of caching  */
        CHECK(first_obj == second_obj);

        browser_functions.releaseobject(second_obj);

        CHECK(leak_detector.memory_leaks() == 0);
    }
}
