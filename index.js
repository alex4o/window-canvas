let xcb = require("./build/Debug/canvas-xcb")
let Canvas = require("canvas")
let clock = require("./clock")

console.log(xcb)
let window = new xcb.Window();
let canvas = new Canvas(320, 320);

window.open()

window.SetCanvas(canvas)


let timer
setInterval(() => {
  event = window.PollEvent()
  if(event != null){
    console.log(event)
  }
  if(event == 12 || event == 25){
    console.log("ctx pre")
    var ctx = canvas.getContext('2d')
    window.SetContext(ctx)
    console.log("ctx after")
    // ctx.clearRect(0, 0, 400, 200)
    clearInterval(timer)
    timer = setInterval(() => {
      //window.StartGroup()
      clock(ctx)
      //window.EndGroup()
    }, 120)

  }
}, 100)
