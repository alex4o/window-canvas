#include <glib.h>
#include <nan.h>
#include "init.h"
#include "Window.h"





void Method(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(Nan::New("world").ToLocalChecked());
}

void Main(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("hello").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(Method)->GetFunction());
  Window::Init(exports);
}

NODE_MODULE(hello, Main)
