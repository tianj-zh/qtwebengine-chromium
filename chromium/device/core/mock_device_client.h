// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_CORE_MOCK_DEVICE_CLIENT_H_
#define DEVICE_CORE_MOCK_DEVICE_CLIENT_H_

#include "base/memory/scoped_ptr.h"
#include "device/core/device_client.h"

namespace device {

class HidService;
class MockHidService;
class MockUsbService;
class UsbService;

class MockDeviceClient : device::DeviceClient {
 public:
  MockDeviceClient();
  ~MockDeviceClient() override;

  // device::DeviceClient implementation:
  UsbService* GetUsbService() override;
  HidService* GetHidService() override;

  // Accessors for the mock instances.
  MockHidService* hid_service();
  MockUsbService* usb_service();

 private:
  scoped_ptr<MockHidService> hid_service_;
  scoped_ptr<MockUsbService> usb_service_;
};

}  // namespace device

#endif  // DEVICE_CORE_MOCK_DEVICE_CLIENT_H_
