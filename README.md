# KineticUI and KineticUI Daemon

A modern, minimal ui renderer with html/css based stylimg written in headerless C.

[![alt text](screenshot.png)](https://www.youtube.com/watch?v=5_f0dCKl6Uo)

KineticUI Features :
- MacOS like smooth kinetic animations
- font animatioms
- deterministic session record and replay for automated testing
- gpu or cpu based alpha nblendig
- wayland connector with window and layer support

KineticUI daemon Features :
- text based protocol
- commands accepted on STDIN, events dispatched on STDOUT
- can be used with any kind of script or programming language

KineticUI Usage :

KineticUI is under heavy development, check out projects using KineticUI to see how to use it :

https://github.com/milgra/mmfm  
https://github.com/milgra/vmp  
https://github.com/milgra/sov  

KineticUI Daemon usage :

check out the controlling script of wcp and wfl

https://github.com/milgra/wcp
https://github.com/milgra/wfl

## Installation

### Compiling from source

Install the needed dependencies and libraries:

```
git meson ninja-build pkg-config
libpng,libfreetype,
libgl,libglew,libegl,libwegl,wayland-client,wayland-protocols,xkbcommon,gles2
fonts-terminus
````

On debian based systems ( over version 12 ):
```
sudo apg-get install git meson ninja-build pkg-config libpng-dev libfreetype-dev libgl-dev libegl-dev libglew-dev libwayland-dev libxkbcommon-dev wayland-protocols libgles2-mesa-dev

```

On arch based systems :
```
sudo pacman -Qs git meson pkg-config ninja glew wayland-protocols libpng freetype2 libgl libegl wayland wayland-protocols libxkbcommon 
```
or use the AUR

Then run these commands:

```
git clone git@github.com:milgra/kineticui.git
cd kineticui
meson build --buildtype=release
ninja -C build
sudo ninja -C build install
```

## Usage

Launch kuid in a terminal and type commands into its standard input, currently available commands are :


## Technical Info ##

KineticUI was written in Headerless C. It is about using the __INCLUDE_LEVEL__ preprocessor macro available in GCC/CLANG to guard the implementation part of a source file so you can place it together with the header definition in a single file and you include this file freely in other source files. Read more about this in (https://github.com/milgra/headerlessc);

## Creating a debug build ##

```
CC=clang meson build --buildtype=debug -Db_sanitize=address -Db_lundef=false
ninja -C build
```

## License ##

GPLv3, see [LICENSE](/LICENSE).

## Roadmap ##

- drag between slider bad dirty rect
- error handling in mt core functions ( add, del, rel, expand, etc should return with int)
- example projects, test app
- increase fault tolerance of css and html parser
- increase standards compatibility of css and html (type=button, blocks=true non standard stuff)
- figure out how to use dirty rects with double buffering in ku_renderer_egl
- update software renderer, cut out dirty rect, implement image zoom
- test dirty rect handling
- predictable screenshot when video playing
- use hold gesture during scrolling
- solve last column resize problem
- select/copy/paste in textfields
- vh_textinput should seamlessly switch between texture paragraph and glpyh-based paragraph
