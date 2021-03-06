/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gm.h"
#include "SkCanvas.h"
#include "SkData.h"
#include "SkImage.h"
#include "SkPictureRecorder.h"
#include "SkSurface.h"

static void draw_something(SkCanvas* canvas, const SkRect& bounds) {
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorRED);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(10);
    canvas->drawRect(bounds, paint);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setColor(SK_ColorBLUE);
    canvas->drawOval(bounds, paint);
}

typedef SkImage* (*ImageMakerProc)(GrContext*, const SkPicture*, const SkImageInfo&);

static SkImage* make_raster(GrContext*, const SkPicture* pic, const SkImageInfo& info) {
    SkAutoTUnref<SkSurface> surface(SkSurface::NewRaster(info));
    surface->getCanvas()->clear(0);
    surface->getCanvas()->drawPicture(pic);
    return surface->newImageSnapshot();
}

static SkImage* make_texture(GrContext* ctx, const SkPicture* pic, const SkImageInfo& info) {
    if (!ctx) {
        return nullptr;
    }
    SkAutoTUnref<SkSurface> surface(SkSurface::NewRenderTarget(ctx, SkSurface::kNo_Budgeted,
                                                               info, 0));
    surface->getCanvas()->clear(0);
    surface->getCanvas()->drawPicture(pic);
    return surface->newImageSnapshot();
}

static SkImage* make_pict_gen(GrContext*, const SkPicture* pic, const SkImageInfo& info) {
    return SkImage::NewFromPicture(pic, info.dimensions(), nullptr, nullptr);
}

static SkImage* make_encode_gen(GrContext* ctx, const SkPicture* pic, const SkImageInfo& info) {
    SkAutoTUnref<SkImage> src(make_raster(ctx, pic, info));
    if (!src) {
        return nullptr;
    }
    SkAutoTUnref<SkData> encoded(src->encode(SkImageEncoder::kPNG_Type, 100));
    if (!encoded) {
        return nullptr;
    }
    return SkImage::NewFromEncoded(encoded);
}

const ImageMakerProc gProcs[] = {
    make_raster,
    make_texture,
    make_pict_gen,
    make_encode_gen,
};

/*
 *  Exercise drawing pictures inside an image, showing that the image version is pixelated
 *  (correctly) when it is inside an image.
 */
class ImageShaderGM : public skiagm::GM {
    SkAutoTUnref<SkPicture> fPicture;

public:
    ImageShaderGM() {}

protected:
    SkString onShortName() override {
        return SkString("image-shader");
    }

    SkISize onISize() override {
        return SkISize::Make(850, 450);
    }

    void onOnceBeforeDraw() override {
        const SkRect bounds = SkRect::MakeWH(100, 100);
        SkPictureRecorder recorder;
        draw_something(recorder.beginRecording(bounds), bounds);
        fPicture.reset(recorder.endRecording());
    }

    void testImage(SkCanvas* canvas, SkImage* image) {
        SkAutoCanvasRestore acr(canvas, true);

        canvas->drawImage(image, 0, 0);
        canvas->translate(0, 120);

        const SkShader::TileMode tile = SkShader::kRepeat_TileMode;
        const SkMatrix localM = SkMatrix::MakeTrans(-50, -50);
        SkAutoTUnref<SkShader> shader(image->newShader(tile, tile, &localM));
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setShader(shader);
        canvas->drawCircle(50, 50, 50, paint);
    }

    void onDraw(SkCanvas* canvas) override {
        canvas->translate(20, 20);

        const SkImageInfo info = SkImageInfo::MakeN32Premul(100, 100);

        for (size_t i = 0; i < SK_ARRAY_COUNT(gProcs); ++i) {
            SkAutoTUnref<SkImage> image(gProcs[i](canvas->getGrContext(), fPicture, info));
            if (image) {
                this->testImage(canvas, image);
            }
            canvas->translate(120, 0);
        }
    }

private:
    typedef skiagm::GM INHERITED;
};
DEF_GM( return new ImageShaderGM; )

