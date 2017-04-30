#include "Window.h"
#include <stdio.h>
#include <stdlib.h>

Nan::Persistent<v8::Function> Window::constructor;

xcb_visualtype_t *get_root_visual_type(xcb_screen_t *s)
{
  xcb_visualtype_t *visual_type = NULL;
  xcb_depth_iterator_t depth_iter;

  depth_iter = xcb_screen_allowed_depths_iterator(s);

  for(;depth_iter.rem;xcb_depth_next(&depth_iter)) {
          xcb_visualtype_iterator_t visual_iter;

          visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
          for(;visual_iter.rem;xcb_visualtype_next(&visual_iter)) {
              if(s->root_visual == visual_iter.data->visual_id) {
                  visual_type = visual_iter.data;
                  break;
              }
          }
  }

  return visual_type;
}

NAN_METHOD(Window::Open) {
  Window *window = Nan::ObjectWrap::Unwrap<Window>(info.This());
  window->open();
  printf("window opened\n");

}

NAN_METHOD(Window::SetCanvas) {
  Window *window = Nan::ObjectWrap::Unwrap<Window>(info.This());
  Canvas *canvas = Nan::ObjectWrap::Unwrap<Canvas>(info[0]->ToObject());
  printf("setting canvas\n");
  window->canvas = canvas;

  canvas->_surface = cairo_xcb_surface_create (window->xcb_conn, window->xcb_win,  get_root_visual_type(window->xcb_scrn) ,canvas->width ,canvas->height);

  printf("canvas set\n");
}

NAN_METHOD(Window::SetContext) {
  Window *window = Nan::ObjectWrap::Unwrap<Window>(info.This());
  Context2d *context2d = Nan::ObjectWrap::Unwrap<Context2d>(info[0]->ToObject());
  window->context2d = context2d;

}

NAN_METHOD(Window::GetCanvas) {
  Window *window = Nan::ObjectWrap::Unwrap<Window>(info.This());

}

void Window::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(Window::New);
  tpl->SetClassName(Nan::New("Window").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "open", Open);
  Nan::SetPrototypeMethod(tpl, "close", Close);
  Nan::SetPrototypeMethod(tpl, "flush", Flush);


  Nan::SetPrototypeMethod(tpl, "Event", Event);

  Nan::SetPrototypeMethod(tpl, "SetCanvas", SetCanvas);
  Nan::SetPrototypeMethod(tpl, "Getcanvas", GetCanvas);

  Nan::SetPrototypeMethod(tpl, "SetContext", SetContext);


  Nan::SetPrototypeMethod(tpl, "StartGroup", StartGroup);
  Nan::SetPrototypeMethod(tpl, "EndGroup", EndGroup);

  constructor.Reset(tpl->GetFunction());

  exports->Set(Nan::New("Window").ToLocalChecked(), tpl->GetFunction());
}
Window::Window() {
  this->canvas = NULL;
  read_watcher.data = this;
  write_watcher.data = this;
}

void Window::open() {
  printf("surface: %d %d\n", canvas, canvas == NULL);

  int w;
  int h;
  if(canvas == NULL){
    w = 320;
    h = 320;
  }else{
    w = canvas->width;
    h = canvas->height;
  }

  xcb_conn = xcb_connect (NULL, NULL);
  xcb_scrn = xcb_setup_roots_iterator (xcb_get_setup (xcb_conn)).data;

  // Window creation

  xcb_win = xcb_generate_id (xcb_conn);
  xcb_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  xcb_values[0] = xcb_scrn->white_pixel;
  xcb_values[1] = XCB_EVENT_MASK_EXPOSURE       | XCB_EVENT_MASK_BUTTON_PRESS   |
                  XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS      | XCB_EVENT_MASK_KEY_RELEASE    | XCB_EVENT_MASK_RESIZE_REDIRECT;

  xcb_create_window (xcb_conn,
                     XCB_COPY_FROM_PARENT,
                     xcb_win,
                     xcb_scrn->root,
                     0, 0,
                     w ,h,
                     0,
                     XCB_WINDOW_CLASS_INPUT_OUTPUT,
                     xcb_scrn->root_visual,
                     xcb_mask, xcb_values);

  // Cairo preparation

  xcb_map_window (xcb_conn, xcb_win);
  xcb_flush (xcb_conn);
  printf("win cool\n");
  printf("surface: %ul \n", canvas);

  int fd = xcb_get_file_descriptor(xcb_conn);

  uv_poll_init_socket(uv_default_loop(), &(this->read_watcher), fd);
  uv_poll_start(&read_watcher, UV_READABLE, on_io_readable);
}

void Window::on_io_readable(uv_poll_t* handle, int status, int revents){
  Nan::HandleScope scope;
  Window* self = (Window*) handle->data;
  xcb_generic_event_t* event = xcb_poll_for_event (self->xcb_conn);
  if(event != NULL){
    //printf("async type: %d %ul\n", event->response_type, event);
    Local<Value> argv[] = {
      Nan::New(event->response_type)
    };
    self->event->Call(1, argv);
  }
}

NAN_METHOD(Window::Close) {
  Window *window = Nan::ObjectWrap::Unwrap<Window>(info.This());
  xcb_destroy_window(window->xcb_conn, window->xcb_win);
}

NAN_METHOD(Window::Flush) {
  Window *window = Nan::ObjectWrap::Unwrap<Window>(info.This());
  xcb_flush(window->xcb_conn);
}

NAN_METHOD(Window::New) {
  Window *window = new Window();
  window->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Window::Event) {
  Window *window = Nan::ObjectWrap::Unwrap<Window>(info.This());
  window->event = new Callback(info[0].As<Function>());

}

NAN_METHOD(Window::StartGroup) {
  Window *window = Nan::ObjectWrap::Unwrap<Window>(info.This());
  cairo_push_group(window->context2d->_context);
}

NAN_METHOD(Window::EndGroup) {
  Window *window = Nan::ObjectWrap::Unwrap<Window>(info.This());
  cairo_pop_group_to_source(window->context2d->_context);
}
