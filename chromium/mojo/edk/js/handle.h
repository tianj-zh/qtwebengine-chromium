// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_EDK_JS_HANDLE_H_
#define MOJO_EDK_JS_HANDLE_H_

#include <stdint.h>

#include "base/observer_list.h"
#include "gin/converter.h"
#include "gin/handle.h"
#include "gin/wrappable.h"
#include "mojo/public/cpp/system/core.h"

namespace mojo {
namespace edk {
class HandleCloseObserver;

// Wrapper for mojo Handles exposed to JavaScript. This ensures the Handle
// is Closed when its JS object is garbage collected.
class HandleWrapper : public gin::Wrappable<HandleWrapper> {
 public:
  static gin::WrapperInfo kWrapperInfo;

  static gin::Handle<HandleWrapper> Create(v8::Isolate* isolate,
                                           MojoHandle handle) {
    return gin::CreateHandle(isolate, new HandleWrapper(handle));
  }

  mojo::Handle get() const { return handle_.get(); }
  mojo::Handle release() { return handle_.release(); }
  void Close();

  void AddCloseObserver(HandleCloseObserver* observer);
  void RemoveCloseObserver(HandleCloseObserver* observer);

 protected:
  HandleWrapper(MojoHandle handle);
  ~HandleWrapper() override;
  void NotifyCloseObservers();

  mojo::ScopedHandle handle_;
  base::ObserverList<HandleCloseObserver> close_observers_;
};

}  // namespace edk
}  // namespace mojo

namespace gin {

// Note: It's important to use this converter rather than the one for
// MojoHandle, since that will do a simple int32_t conversion. It's unfortunate
// there's no way to prevent against accidental use.
// TODO(mpcomplete): define converters for all Handle subtypes.
template<>
struct Converter<mojo::Handle> {
  static v8::Handle<v8::Value> ToV8(v8::Isolate* isolate,
                                    const mojo::Handle& val);
  static bool FromV8(v8::Isolate* isolate, v8::Handle<v8::Value> val,
                     mojo::Handle* out);
};

template<>
struct Converter<mojo::MessagePipeHandle> {
  static v8::Handle<v8::Value> ToV8(v8::Isolate* isolate,
                                    mojo::MessagePipeHandle val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Handle<v8::Value> val,
                     mojo::MessagePipeHandle* out);
};

// We need to specialize the normal gin::Handle converter in order to handle
// converting |null| to a wrapper for an empty mojo::Handle.
template<>
struct Converter<gin::Handle<mojo::edk::HandleWrapper> > {
  static v8::Handle<v8::Value> ToV8(
      v8::Isolate* isolate,
      const gin::Handle<mojo::edk::HandleWrapper>& val) {
    return val.ToV8();
  }

  static bool FromV8(v8::Isolate* isolate, v8::Handle<v8::Value> val,
                     gin::Handle<mojo::edk::HandleWrapper>* out) {
    if (val->IsNull()) {
      *out = mojo::edk::HandleWrapper::Create(isolate, MOJO_HANDLE_INVALID);
      return true;
    }

    mojo::edk::HandleWrapper* object = NULL;
    if (!Converter<mojo::edk::HandleWrapper*>::FromV8(isolate, val, &object)) {
      return false;
    }
    *out = gin::Handle<mojo::edk::HandleWrapper>(val, object);
    return true;
  }
};

}  // namespace gin

#endif  // MOJO_EDK_JS_HANDLE_H_
