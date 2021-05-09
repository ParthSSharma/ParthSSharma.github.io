clear all;
close all;
clc;

UPPER_RANGE = %Your Upper Range%
LOWER_RANGE = %Your Upper Range%
FILE_NAME = %Audio File Name%
TEXT_FILE = %Text File Name%
BitRate = 8000

Range = [LOWER_RANGE, UPPER_RANGE];
[y, Fs] = audioread(FILE_NAME, Range);

yn = resample(y, BitRate, Fs);
ys = yn(:, 1);
yqfinal = rescale(ys, 0, 4095);
ysfinal = cast(yqfinal, 'uint16');
file = fopen(TEXT_FILE, 'w');
for i = 1 : length(ysfinal)
    yfinal(i) = bitor(ysfinal(i), 0b0011000000000000);
    fprintf(file, "%d, ", yfinal(i));
end
fclose(file);