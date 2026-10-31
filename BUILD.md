
```sh
cc -std=c17 -Ivendor/SDL3/x86_64-w64-mingw32/include \
   src/main.c \
   vendor/SDL3/x86_64-w64-mingw32/lib/libSDL3.dll.a \
   -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -luuid -lversion \
   -mwindows \
   -o hello
cp vendor/SDL3/x86_64-w64-mingw32/bin/SDL3.dll .
```

or

```sh
cc -std=c17 -Ivendor/SDL3/x86_64-w64-mingw32/include \
   -Lvendor/SDL3/x86_64-w64-mingw32/lib \
   src/main.c \
   -lSDL3 \
   -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -luuid -lversion \
   -mwindows \
   -o hello
cp vendor/SDL3/x86_64-w64-mingw32/bin/SDL3.dll .
```





with ttf and image


```sh
cc -std=c23 \
   -Ivendor/SDL3/x86_64-w64-mingw32/include \
   -Ivendor/SDL3_image/x86_64-w64-mingw32/include \
   -Ivendor/SDL3_ttf/x86_64-w64-mingw32/include \
   -Lvendor/SDL3/x86_64-w64-mingw32/lib \
   -Lvendor/SDL3_image/x86_64-w64-mingw32/lib \
   -Lvendor/SDL3_ttf/x86_64-w64-mingw32/lib \
   src/main.c \
   -lSDL3 \
   -lSDL3_image \
   -lSDL3_ttf \
   -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -luuid -lversion \
   -mwindows \
   -o hello

cp vendor/SDL3/x86_64-w64-mingw32/bin/SDL3.dll .
cp vendor/SDL3_image/x86_64-w64-mingw32/bin/SDL3_image.dll .
cp vendor/SDL3_ttf/x86_64-w64-mingw32/bin/SDL3_ttf.dll .
```