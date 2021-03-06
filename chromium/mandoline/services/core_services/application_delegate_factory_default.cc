// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mandoline/services/core_services/application_delegate_factory.h"

#include "mojo/shell/public/cpp/application_delegate.h"

namespace core_services {

scoped_ptr<mojo::ApplicationDelegate> CreatePlatformSpecificApplicationDelegate(
    const std::string& url) {
  return nullptr;
}

}  // namespace core_services
