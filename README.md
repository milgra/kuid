# KineticUi

A modern, minimal ui renderer with html/css based stylimg written in headerless C.

Features :
- MacOs like smooth kinetic animations
- font animatioms
- deterministic session record and replay for automated testing
- gpu or cpu based alpha nblendig
- wayland connector with window and layer support

Usage :

KineticUI is under heavy development, check out projects using KineticUI to see how to use it :

https://github.com/milgra/mmfm
https://github.com/milgra/vmp
https://github.com/milgra/sov
https://github.com/milgra/wcp

Roadmap :

- test dirty rect handling
- make dirty rect work on egl dirty rect solutions ( single buffering maybe? )
- predictable screenshot when video playing
- use hold gesture during scrolling
- remove non-standard css and html (type=button, blocks=true)
- solve last column resize problem
- select/copy/paste in textfields
- vh_textinput should seamlessly switch between texture paragraph and glpyh-based paragraph
