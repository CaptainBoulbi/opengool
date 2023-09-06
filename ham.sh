g++ -Wall -Wextra -Iinclude -Ilib -Og -g -MP -MD -DDEBUG=1 -o build/te.o -c te.cpp
g++ -o build/te build/te.o build/glad.l build/stb_image_imp.l lib/libglfw3.a -Wall -Wextra -Iinclude -Ilib -Og -g -MP -MD -DDEBUG=1 -Llib -lX11
./build/te
