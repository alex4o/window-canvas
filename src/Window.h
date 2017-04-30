#include <nan.h>
#define private public
#include <Canvas.h>
#include <CanvasRenderingContext2d.h>
#include <cairo-xcb.h>

xcb_visualtype_t *get_root_visual_type(xcb_screen_t *s);

class Window : public Nan::ObjectWrap {
  xcb_connection_t *      xcb_conn;
  xcb_generic_event_t *   xcb_event;
  uint32_t                xcb_mask;
  xcb_screen_t *          xcb_scrn;
  uint32_t                xcb_values[2];
  xcb_window_t            xcb_win;

  uv_poll_t read_watcher;
  uv_poll_t write_watcher;

public:
  static Nan::Persistent<v8::Function> constructor;
  static void Init(v8::Local<v8::Object> exports);

  Window();
  void open();
  Canvas *canvas = NULL;
  Context2d *context2d = NULL;

  static NAN_METHOD(New);
  static NAN_METHOD(Open);
  static NAN_METHOD(Close);
  static NAN_METHOD(Flush);

  static NAN_METHOD(PollEvent);
  static NAN_METHOD(WaitEvent);

  static NAN_METHOD(StartGroup);
  static NAN_METHOD(EndGroup);

  static NAN_METHOD(SetCanvas);
  static NAN_METHOD(GetCanvas);
  static NAN_METHOD(SetContext);

  static void on_io_readable(uv_poll_t* handle, int status, int revents);
};
