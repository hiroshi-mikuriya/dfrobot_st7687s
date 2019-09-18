import cv2
import sys

width = 128
height = 128
wcount = int((width + 4) / 5)
image_t = 'uint16_t'
print('#include <avr/pgmspace.h>')
print('#ifndef IMG_WIDTH')
print('#define IMG_WIDTH', width)
print('#endif')
print('#ifndef IMG_HEIGHT')
print('#define IMG_HEIGHT', height)
print('#endif')
print('#ifndef WIDTH_COUNT')
print('#define WIDTH_COUNT', wcount)
print('#endif')
print('typedef %s image_t;' % image_t)
for i in range(1, len(sys.argv)):
    img = cv2.imread(sys.argv[i], cv2.IMREAD_COLOR)
    if img is None:
        print('failed to read image', file=sys.stderr)
        exit(1)
    img = cv2.resize(img, (height, width))
    print('// %s' % sys.argv[i])
    print('const image_t image%d[WIDTH_COUNT * IMG_HEIGHT] PROGMEM = {' % i)
    # img = img.transpose(1,0,2)
    for y in range(img.shape[0]):
        print('/* %2d */' % y, end = ' ')
        for x0 in range(wcount):
            color = 0
            for x1 in range(5):
                x = x0 * 5 + x1
                if img.shape[1] <= x:
                    break
                a = img[y][x]
                for i in range(3):
                    if 128 < a[i]:
                        color |= 1 << (x1 * 3 + i)
            print('0x%04X' % color, end = ', ')
        print()
    print('};')

print('#define COUNT_OF_IMAGES', len(sys.argv) - 1)
print('const image_t* images[COUNT_OF_IMAGES] = {',
      ', '.join(map(lambda i: 'image%d' % i, range(1, len(sys.argv)))), '};')
