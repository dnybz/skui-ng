/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

// This test only works with the GL backend.

#include "gm/gm.h"

#ifdef SK_GL
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkFilterQuality.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkScalar.h"
#include "include/core/SkShader.h"
#include "include/core/SkSize.h"
#include "include/core/SkString.h"
#include "include/core/SkTileMode.h"
#include "include/core/SkTypes.h"
#include "include/effects/SkGradientShader.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContext.h"
#include "include/gpu/GrTypes.h"
#include "src/core/SkAutoPixmapStorage.h"
#include "src/gpu/GrContextPriv.h"
#include "src/gpu/GrGpu.h"
#include "src/gpu/gl/GrGLCaps.h"
#include "src/gpu/gl/GrGLDefines.h"

#include <algorithm>
#include <cstdint>
#include <memory>

class GrRenderTargetContext;

namespace skiagm {
class RectangleTexture : public GpuGM {
public:
    RectangleTexture() {
        this->setBGColor(0xFFFFFFFF);
    }

private:
    enum class ImageType {
        kGradientCircle,
        k2x2
    };

    SkString onShortName() override {
        return SkString("rectangle_texture");
    }

    SkISize onISize() override { return SkISize::Make(1180, 710); }

    SkBitmap makeImagePixels(int size, ImageType type) {
        auto ii = SkImageInfo::Make(size, size, kRGBA_8888_SkColorType, kOpaque_SkAlphaType);
        switch (type) {
            case ImageType::kGradientCircle: {
                SkBitmap bmp;
                bmp.allocPixels(ii);
                SkPaint paint;
                SkCanvas canvas(bmp);
                SkPoint pts[] = {{0, 0}, {0, SkIntToScalar(size)}};
                SkColor colors0[] = {0xFF1060B0, 0xFF102030};
                paint.setShader(
                        SkGradientShader::MakeLinear(pts, colors0, nullptr, 2, SkTileMode::kClamp));
                canvas.drawPaint(paint);
                SkColor colors1[] = {0xFFA07010, 0xFFA02080};
                paint.setAntiAlias(true);
                paint.setShader(
                        SkGradientShader::MakeLinear(pts, colors1, nullptr, 2, SkTileMode::kClamp));
                canvas.drawCircle(size/2.f, size/2.f, 2.f*size/5, paint);
                return bmp;
            }
            case ImageType::k2x2: {
                SkBitmap bmp;
                bmp.allocPixels(ii);
                *bmp.getAddr32(0, 0) = 0xFF0000FF;
                *bmp.getAddr32(1, 0) = 0xFF00FF00;
                *bmp.getAddr32(0, 1) = 0xFFFF0000;
                *bmp.getAddr32(1, 1) = 0xFFFFFFFF;
                return bmp;
            }
        }
        SkUNREACHABLE;
    }

    sk_sp<SkImage> createRectangleTextureImg(GrContext* context, GrSurfaceOrigin origin,
                                             const SkBitmap content) {
        SkASSERT(content.colorType() == kRGBA_8888_SkColorType);
        auto format = GrBackendFormat::MakeGL(GR_GL_RGBA8, GR_GL_TEXTURE_RECTANGLE);
        auto bet = context->createBackendTexture(content.width(), content.height(), format,
                                                 GrMipMapped::kNo, GrRenderable::kNo);
        if (!bet.isValid()) {
            return nullptr;
        }
        const SkPixmap* pm = &content.pixmap();
        SkAutoPixmapStorage tempPM;
        if (origin == kBottomLeft_GrSurfaceOrigin) {
            tempPM.alloc(pm->info());
            const uint32_t* src = pm->addr32();
            uint32_t* dst = tempPM.writable_addr32(0, content.height() - 1);
            for (int y = 0; y < content.height(); ++y,
                                                  src += pm->rowBytesAsPixels(),
                                                  dst -= tempPM.rowBytesAsPixels()) {
                std::copy_n(src, content.width(), dst);
            }
            pm = &tempPM;
        }
        if (!context->updateBackendTexture(bet, pm, 1, nullptr, nullptr)) {
            context->deleteBackendTexture(bet);
        }
        return SkImage::MakeFromAdoptedTexture(context, bet, origin, kRGBA_8888_SkColorType);
    }

    DrawResult onDraw(GrContext* context, GrRenderTargetContext*, SkCanvas* canvas,
                      SkString* errorMsg) override {
        if (context->backend() != GrBackendApi::kOpenGL_GrBackend ||
            !static_cast<const GrGLCaps*>(context->priv().caps())->rectangleTextureSupport()) {
            *errorMsg = "This GM requires an OpenGL context that supports texture rectangles.";
            return DrawResult::kSkip;
        }

        auto gradCircle = this->makeImagePixels(50, ImageType::kGradientCircle);
        static constexpr SkScalar kPad = 5.f;

        sk_sp<SkImage> gradImgs[] = {
                this->createRectangleTextureImg(context, kTopLeft_GrSurfaceOrigin, gradCircle),
                this->createRectangleTextureImg(context, kBottomLeft_GrSurfaceOrigin, gradCircle),
        };
        SkASSERT(SkToBool(gradImgs[0]) == SkToBool(gradImgs[1]));
        if (!gradImgs[0]) {
            *errorMsg = "Could not create rectangle texture image.";
            return DrawResult::kFail;
        }

        constexpr SkFilterQuality kQualities[] = {
                kNone_SkFilterQuality,
                kLow_SkFilterQuality,
                kMedium_SkFilterQuality,
                kHigh_SkFilterQuality,
        };

        constexpr SkScalar kScales[] = {1.0f, 1.2f, 0.75f};

        canvas->translate(kPad, kPad);
        for (size_t i = 0; i < SK_ARRAY_COUNT(gradImgs); ++i) {
            auto img = gradImgs[i];
            int w = img->width();
            int h = img->height();
            for (auto s : kScales) {
                canvas->save();
                canvas->scale(s, s);
                for (auto q : kQualities) {
                    // drawImage
                    SkPaint plainPaint;
                    plainPaint.setFilterQuality(q);
                    canvas->drawImage(img, 0, 0, &plainPaint);
                    canvas->translate(w + kPad, 0);

                    // clamp/clamp shader
                    SkPaint clampPaint;
                    clampPaint.setFilterQuality(q);
                    clampPaint.setShader(gradImgs[i]->makeShader());
                    canvas->drawRect(SkRect::MakeWH(1.5f*w, 1.5f*h), clampPaint);
                    canvas->translate(1.5f*w + kPad, 0);

                    // repeat/mirror shader
                    SkPaint repeatPaint;
                    repeatPaint.setFilterQuality(q);
                    repeatPaint.setShader(gradImgs[i]->makeShader(SkTileMode::kRepeat,
                                                                  SkTileMode::kMirror));
                    canvas->drawRect(SkRect::MakeWH(1.5f*w, 1.5f*h), repeatPaint);
                    canvas->translate(1.5f*w + kPad, 0);

                    // drawImageRect with kStrict
                    auto srcRect = SkRect::MakeXYWH(.25f*w, .25f*h, .50f*w, .50f*h);
                    auto dstRect = SkRect::MakeXYWH(      0,     0, .50f*w, .50f*h);
                    canvas->drawImageRect(gradImgs[i], srcRect, dstRect, &plainPaint,
                                          SkCanvas::kStrict_SrcRectConstraint);
                    canvas->translate(.5f*w + kPad, 0);
                }
                canvas->restore();
                canvas->translate(0, kPad + 1.5f*h*s);
            }
        }

        auto smallImg = this->createRectangleTextureImg(context, kTopLeft_GrSurfaceOrigin,
                                                        this->makeImagePixels(2, ImageType::k2x2));
        static constexpr SkScalar kOutset = 25.f;
        canvas->translate(kOutset, kOutset);
        auto dstRect = SkRect::Make(smallImg->dimensions()).makeOutset(kOutset, kOutset);

        for (int fq = kNone_SkFilterQuality; fq <= kLast_SkFilterQuality; ++fq) {
            if (fq == kMedium_SkFilterQuality) {
                // Medium is the same as Low for upscaling.
                continue;
            }
            canvas->save();
            for (int ty = 0; ty < kSkTileModeCount; ++ty) {
                canvas->save();
                for (int tx = 0; tx < kSkTileModeCount; ++tx) {
                    SkMatrix lm;
                    lm.setRotate(45.f, 1, 1);
                    lm.postScale(6.5f, 6.5f);
                    auto shader = smallImg->makeShader(static_cast<SkTileMode>(tx),
                                                       static_cast<SkTileMode>(ty), &lm);
                    SkPaint paint;
                    paint.setShader(std::move(shader));
                    paint.setFilterQuality(static_cast<SkFilterQuality>(fq));
                    canvas->drawRect(dstRect, paint);
                    canvas->translate(dstRect.width() + kPad, 0);
                }
                canvas->restore();
                canvas->translate(0, dstRect.height() + kPad);
            }
            canvas->restore();
            canvas->translate((dstRect.width() + kPad)*kSkTileModeCount, 0);
        }

        return DrawResult::kOk;
    }

private:
    typedef GM INHERITED;
};

DEF_GM(return new RectangleTexture;)
}
#endif
