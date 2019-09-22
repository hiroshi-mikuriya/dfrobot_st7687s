import cv2
import sys

width = 128
height = 128
image_t = 'uint8_t'
print('#include <pgmspace.h>')
print('typedef %s image_t;' % image_t)
sizes = []
for i in range(1, len(sys.argv)):
    img = cv2.imread(sys.argv[i], cv2.IMREAD_COLOR)
    if img is None:
        print('failed to read image', file=sys.stderr)
        exit(1)
    img = cv2.resize(img, (height, width))
    # img = img.transpose(1,0,2)
    dat = []
    count = 0
    pre = 0
    for y in range(img.shape[0]):
        for x in range(img.shape[1]):
            count += 1
            a = img[y][x]
            cur = (a[2] & 0xC0) + ((a[1] & 0xE0) >> 2) + ((a[0] & 0xE0) >> 5)
            if 0 < x + y and (255 <= count or pre != cur):
                dat.append(pre)
                dat.append(count)
                count = 0
            pre = cur
    if 0 < count:
        dat.append(pre)
        dat.append(count)
    print('// %s' % sys.argv[i])
    print('const image_t image%d[%d] PROGMEM = {' % (i, len(dat)), end='')
    print(', '.join(map(lambda i: str(i), dat)), end='')
    print('};')
    sizes.append(len(dat))

print('#define COUNT_OF_IMAGES', len(sys.argv) - 1)
print('const image_t* images[COUNT_OF_IMAGES] = {',
      ', '.join(map(lambda i: 'image%d' % i, range(1, len(sys.argv)))), '};')
print('const uint16_t image_sizes[COUNT_OF_IMAGES] = {', ', '.join(map(lambda i: str(i), sizes)), '};')
