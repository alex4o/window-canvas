
{
    "targets": [
        {
            "target_name": "canvas-xcb",
            "sources": [
                "src/init.cc",
                "src/Window.cc" ],
            "include_dirs" : [
                "<!(node -e \"require('nan')\")",
                '<!@(pkg-config cairo --cflags-only-I | sed s/-I//g)',
                '<!@(pkg-config libpng --cflags-only-I | sed s/-I//g)',
                '<!@(pkg-config pangocairo --cflags-only-I | sed s/-I//g)',
                '<!@(pkg-config freetype2 --cflags-only-I | sed s/-I//g)',
                '<!@(pkg-config x11-xcb --cflags-only-I | sed s/-I//g)',
                './node_modules/canvas/src'
            ],
            'libraries': [
                '<!@(pkg-config pixman-1 --libs)',
                '<!@(pkg-config cairo --libs)',
                '<!@(pkg-config libpng --libs)',
                '<!@(pkg-config pangocairo --libs)',
                '<!@(pkg-config freetype2 --libs)',
                '<!@(pkg-config x11-xcb --libs)'
            ],
        }
    ],
}
