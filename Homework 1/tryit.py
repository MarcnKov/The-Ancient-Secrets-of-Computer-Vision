from uwimg import *

im = load_image("data/test_image.jpg")

a = nn_resize(im, im.w*4, im.h*4)
save_image(a, "test_image4x_nn")

a = bilinear_resize(im, im.w*4, im.h*4)
save_image(a, "test_image4x_bl")

f       = make_sharpen_filter()
f_false = convolve_image(im, f, 0)
f_true  = convolve_image(im, f, 1)

save_image(f_true, "test_image_sharpen_preserve=True")
save_image(f_false, "test_image_sharpen_preserve=False")

f       = make_gaussian_filter(3)

blur    = convolve_image(im, f, 1)
save_image(blur, "test_image-gauss_filter_size3")

f       = make_gaussian_filter(2)
lfreq   = convolve_image(im, f, 1)
hfreq   = im - lfreq
reconstruct = lfreq + hfreq

save_image(lfreq, "test_image_low_frequency")
save_image(hfreq, "test_image_high_frequency")
save_image(reconstruct, "test_image_reconstructed")

res = sobel_image(im)
col_sobel = colorize_sobel(im)
gx  = res[0]
feature_normalize(gx)
feature_normalize(col_sobel)

save_image(gx, "test_image_gx")
save_image(col_sobel, "test_image_color_sobel")

