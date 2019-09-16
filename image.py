import cv2
import sys

width = 128
height = 128
image_t = 'uint8_t'
print('#include <avr/pgmspace.h>')
print('#ifndef IMG_WIDTH')
print('#define IMG_WIDTH', width)
print('#endif')
print('#ifndef IMG_HEIGHT')
print('#define IMG_HEIGHT', height)
print('#endif')
print('typedef %s image_t;' % image_t)
for i in range(1, len(sys.argv)):
    img = cv2.imread(sys.argv[i], cv2.IMREAD_COLOR)
    if img is None:
        print('failed to read image', file=sys.stderr)
        exit(1)
    img = cv2.resize(img, (height, width))
    print('// %s' % sys.argv[i])
    print('const image_t image%d[IMG_WIDTH * IMG_HEIGHT] PROGMEM = {' % i)
    rows = []
    img = cv2.flip(img, 1)
    for x in range(img.shape[1]):
        cols = []
        for y in range(img.shape[0]):
            a = img[y][x]
            c = (a[2] & 0b11000000) + ((a[1] & 0b11100000) >> 2) + (
                (a[0] & 0b11100000) >> 5)
            cols.append('/* %2d-%d */ 0x%02X' % (x, y, c))
        rows.append('  ' + ', '.join(cols))
    print(',\n'.join(rows))
    print('};')

print('#define COUNT_OF_IMAGES', len(sys.argv) - 1)
print('const image_t* images[COUNT_OF_IMAGES] = {',
      ', '.join(map(lambda i: 'image%d' % i, range(1, len(sys.argv)))), '};')
