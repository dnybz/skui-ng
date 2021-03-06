/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/effects/SkTableColorFilter.h"

#include "include/core/SkBitmap.h"
#include "include/core/SkString.h"
#include "include/core/SkUnPreMultiply.h"
#include "include/private/SkColorData.h"
#include "include/private/SkTo.h"
#include "src/core/SkArenaAlloc.h"
#include "src/core/SkColorFilterBase.h"
#include "src/core/SkEffectPriv.h"
#include "src/core/SkRasterPipeline.h"
#include "src/core/SkReadBuffer.h"
#include "src/core/SkVM.h"
#include "src/core/SkWriteBuffer.h"

static const uint8_t gIdentityTable[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
    0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
    0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
    0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

class SkTable_ColorFilter : public SkColorFilterBase {
public:
    SkTable_ColorFilter(const uint8_t tableA[], const uint8_t tableR[],
                        const uint8_t tableG[], const uint8_t tableB[]) {
        fBitmap = nullptr;
        fFlags = 0;

        uint8_t* dst = fStorage;
        if (tableA) {
            memcpy(dst, tableA, 256);
            dst += 256;
            fFlags |= kA_Flag;
        }
        if (tableR) {
            memcpy(dst, tableR, 256);
            dst += 256;
            fFlags |= kR_Flag;
        }
        if (tableG) {
            memcpy(dst, tableG, 256);
            dst += 256;
            fFlags |= kG_Flag;
        }
        if (tableB) {
            memcpy(dst, tableB, 256);
            fFlags |= kB_Flag;
        }
    }

    ~SkTable_ColorFilter() override { delete fBitmap; }

#if SK_SUPPORT_GPU
    std::unique_ptr<GrFragmentProcessor> asFragmentProcessor(GrRecordingContext*,
                                                             const GrColorInfo&) const override;
#endif

    enum {
        kA_Flag = 1 << 0,
        kR_Flag = 1 << 1,
        kG_Flag = 1 << 2,
        kB_Flag = 1 << 3,
    };

    bool onAppendStages(const SkStageRec& rec, bool shaderIsOpaque) const override {
        const uint8_t *r = gIdentityTable,
                      *g = gIdentityTable,
                      *b = gIdentityTable,
                      *a = gIdentityTable;
        const uint8_t* ptr = fStorage;
        if (fFlags & kA_Flag) { a = ptr; ptr += 256; }
        if (fFlags & kR_Flag) { r = ptr; ptr += 256; }
        if (fFlags & kG_Flag) { g = ptr; ptr += 256; }
        if (fFlags & kB_Flag) { b = ptr;             }

        SkRasterPipeline* p = rec.fPipeline;
        if (!shaderIsOpaque) {
            p->append(SkRasterPipeline::unpremul);
        }

        struct Tables { const uint8_t *r, *g, *b, *a; };
        p->append(SkRasterPipeline::byte_tables, rec.fAlloc->make<Tables>(Tables{r,g,b,a}));

        bool definitelyOpaque = shaderIsOpaque && a[0xff] == 0xff;
        if (!definitelyOpaque) {
            p->append(SkRasterPipeline::premul);
        }
        return true;
    }

    skvm::Color onProgram(skvm::Builder* p, skvm::Color c,
                          SkColorSpace* dstCS,
                          skvm::Uniforms* uniforms, SkArenaAlloc*) const override {

        auto apply_table_to_component = [&](skvm::F32 c, const uint8_t* bytePtr) -> skvm::F32 {
            skvm::I32     index = to_unorm(8, clamp01(c));
            skvm::Uniform table = uniforms->pushPtr(bytePtr);
            return from_unorm(8, gather8(table, index));
        };

        c = unpremul(c);

        const uint8_t* ptr = fStorage;
        if (fFlags & kA_Flag) {
            c.a = apply_table_to_component(c.a, ptr);
            ptr += 256;
        }
        if (fFlags & kR_Flag) {
            c.r = apply_table_to_component(c.r, ptr);
            ptr += 256;
        }
        if (fFlags & kG_Flag) {
            c.g = apply_table_to_component(c.g, ptr);
            ptr += 256;
        }
        if (fFlags & kB_Flag) {
            c.b = apply_table_to_component(c.b, ptr);
        }
        return premul(c);
    }

protected:
    void flatten(SkWriteBuffer&) const override;

private:
    SK_FLATTENABLE_HOOKS(SkTable_ColorFilter)

    void getTableAsBitmap(SkBitmap* table) const;

    mutable const SkBitmap* fBitmap; // lazily allocated

    uint8_t fStorage[256 * 4];
    unsigned fFlags;

    friend class SkTableColorFilter;

    typedef SkColorFilter INHERITED;
};

static const uint8_t gCountNibBits[] = {
    0, 1, 1, 2,
    1, 2, 2, 3,
    1, 2, 2, 3,
    2, 3, 3, 4
};

#include "src/effects/SkPackBits.h"

void SkTable_ColorFilter::flatten(SkWriteBuffer& buffer) const {
    uint8_t storage[5*256];
    int count = gCountNibBits[fFlags & 0xF];
    size_t size = SkPackBits::Pack8(fStorage, count * 256, storage,
                                    sizeof(storage));

    buffer.write32(fFlags);
    buffer.writeByteArray(storage, size);
}

sk_sp<SkFlattenable> SkTable_ColorFilter::CreateProc(SkReadBuffer& buffer) {
    const int flags = buffer.read32();
    const size_t count = gCountNibBits[flags & 0xF];
    SkASSERT(count <= 4);

    uint8_t packedStorage[5*256];
    size_t packedSize = buffer.getArrayCount();
    if (!buffer.validate(packedSize <= sizeof(packedStorage))) {
        return nullptr;
    }
    if (!buffer.readByteArray(packedStorage, packedSize)) {
        return nullptr;
    }

    uint8_t unpackedStorage[4*256];
    size_t unpackedSize = SkPackBits::Unpack8(packedStorage, packedSize,
                              unpackedStorage, sizeof(unpackedStorage));
    // now check that we got the size we expected
    if (!buffer.validate(unpackedSize == count*256)) {
        return nullptr;
    }

    const uint8_t* a = nullptr;
    const uint8_t* r = nullptr;
    const uint8_t* g = nullptr;
    const uint8_t* b = nullptr;
    const uint8_t* ptr = unpackedStorage;

    if (flags & kA_Flag) {
        a = ptr;
        ptr += 256;
    }
    if (flags & kR_Flag) {
        r = ptr;
        ptr += 256;
    }
    if (flags & kG_Flag) {
        g = ptr;
        ptr += 256;
    }
    if (flags & kB_Flag) {
        b = ptr;
        ptr += 256;
    }
    return SkTableColorFilter::MakeARGB(a, r, g, b);
}

void SkTable_ColorFilter::getTableAsBitmap(SkBitmap* table) const {
    if (table) {
        if (nullptr == fBitmap) {
            SkBitmap* bmp = new SkBitmap;
            bmp->allocPixels(SkImageInfo::MakeA8(256, 4));
            uint8_t* bitmapPixels = bmp->getAddr8(0, 0);
            int offset = 0;
            static const unsigned kFlags[] = { kA_Flag, kR_Flag, kG_Flag, kB_Flag };

            for (int x = 0; x < 4; ++x) {
                if (!(fFlags & kFlags[x])) {
                    memcpy(bitmapPixels, gIdentityTable, sizeof(gIdentityTable));
                } else {
                    memcpy(bitmapPixels, fStorage + offset, 256);
                    offset += 256;
                }
                bitmapPixels += 256;
            }
            bmp->setImmutable();
            fBitmap = bmp;
        }
        *table = *fBitmap;
    }
}

#if SK_SUPPORT_GPU

#include "include/private/GrRecordingContext.h"
#include "src/gpu/GrColorInfo.h"
#include "src/gpu/GrFragmentProcessor.h"
#include "src/gpu/GrRecordingContextPriv.h"
#include "src/gpu/SkGr.h"
#include "src/gpu/effects/GrTextureEffect.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"

class ColorTableEffect : public GrFragmentProcessor {
public:
    static std::unique_ptr<GrFragmentProcessor> Make(GrRecordingContext* context,
                                                     const SkBitmap& bitmap);

    ~ColorTableEffect() override {}

    const char* name() const override { return "ColorTableEffect"; }

    std::unique_ptr<GrFragmentProcessor> clone() const override {
        return std::unique_ptr<GrFragmentProcessor>(new ColorTableEffect(*this));
    }

private:
    GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;

    void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override {}

    bool onIsEqual(const GrFragmentProcessor&) const override { return true; }

    ColorTableEffect(GrSurfaceProxyView view);

    explicit ColorTableEffect(const ColorTableEffect& that);

    GR_DECLARE_FRAGMENT_PROCESSOR_TEST

    typedef GrFragmentProcessor INHERITED;
};

ColorTableEffect::ColorTableEffect(GrSurfaceProxyView view)
        // Not bothering with table-specific optimizations.
        : INHERITED(kColorTableEffect_ClassID, kNone_OptimizationFlags) {
    auto te = GrTextureEffect::Make(std::move(view), kUnknown_SkAlphaType);
    this->registerExplicitlySampledChild(std::move(te));
}

ColorTableEffect::ColorTableEffect(const ColorTableEffect& that)
        : INHERITED(kColorTableEffect_ClassID, that.optimizationFlags()) {
    this->cloneAndRegisterAllChildProcessors(that);
}

GrGLSLFragmentProcessor* ColorTableEffect::onCreateGLSLInstance() const {
    class Impl : public GrGLSLFragmentProcessor {
    public:
        void emitCode(EmitArgs& args) override {
            GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
            fragBuilder->codeAppendf("\t\thalf4 coord = 255*unpremul(%s) + 0.5;\n",
                                     args.fInputColor);
            SkString a = this->invokeChild(0, args, "half2(coord.a, 0.5)");
            SkString r = this->invokeChild(0, args, "half2(coord.r, 1.5)");
            SkString g = this->invokeChild(0, args, "half2(coord.g, 2.5)");
            SkString b = this->invokeChild(0, args, "half2(coord.b, 3.5)");
            fragBuilder->codeAppendf("%s = half4(half3(%s.a, %s.a, %s.a) * %s.a, %s.a);",
                                     args.fOutputColor, r.c_str(), g.c_str(), b.c_str(), a.c_str(),
                                     a.c_str());
        }
    };
    return new Impl;
}

std::unique_ptr<GrFragmentProcessor> ColorTableEffect::Make(GrRecordingContext* context,
                                                            const SkBitmap& bitmap) {
    SkASSERT(kPremul_SkAlphaType == bitmap.alphaType());
    SkASSERT(bitmap.isImmutable());

    auto view = GrMakeCachedBitmapProxyView(context, bitmap);
    if (!view) {
        return nullptr;
    }

    return std::unique_ptr<GrFragmentProcessor>(new ColorTableEffect(std::move(view)));
}

///////////////////////////////////////////////////////////////////////////////

GR_DEFINE_FRAGMENT_PROCESSOR_TEST(ColorTableEffect);

#if GR_TEST_UTILS

#include "include/gpu/GrContext.h"

std::unique_ptr<GrFragmentProcessor> ColorTableEffect::TestCreate(GrProcessorTestData* d) {
    int flags = 0;
    uint8_t luts[256][4];
    do {
        for (int i = 0; i < 4; ++i) {
            flags |= d->fRandom->nextBool() ? (1  << i): 0;
        }
    } while (!flags);
    for (int i = 0; i < 4; ++i) {
        if (flags & (1 << i)) {
            for (int j = 0; j < 256; ++j) {
                luts[j][i] = SkToU8(d->fRandom->nextBits(8));
            }
        }
    }
    auto filter(SkTableColorFilter::MakeARGB(
        (flags & (1 << 0)) ? luts[0] : nullptr,
        (flags & (1 << 1)) ? luts[1] : nullptr,
        (flags & (1 << 2)) ? luts[2] : nullptr,
        (flags & (1 << 3)) ? luts[3] : nullptr
    ));
    sk_sp<SkColorSpace> colorSpace = GrTest::TestColorSpace(d->fRandom);
    auto fp = as_CFB(filter)->asFragmentProcessor(
            d->context(),
            GrColorInfo(GrColorType::kRGBA_8888, kUnknown_SkAlphaType, std::move(colorSpace)));
    SkASSERT(fp);
    return fp;
}
#endif

std::unique_ptr<GrFragmentProcessor> SkTable_ColorFilter::asFragmentProcessor(
        GrRecordingContext* context, const GrColorInfo&) const {
    SkBitmap bitmap;
    this->getTableAsBitmap(&bitmap);

    return ColorTableEffect::Make(context, bitmap);
}

#endif // SK_SUPPORT_GPU

///////////////////////////////////////////////////////////////////////////////

sk_sp<SkColorFilter> SkTableColorFilter::Make(const uint8_t table[256]) {
    return sk_make_sp<SkTable_ColorFilter>(table, table, table, table);
}

sk_sp<SkColorFilter> SkTableColorFilter::MakeARGB(const uint8_t tableA[256],
                                                  const uint8_t tableR[256],
                                                  const uint8_t tableG[256],
                                                  const uint8_t tableB[256]) {
    if (!tableA && !tableR && !tableG && !tableB) {
        return nullptr;
    }

    return sk_make_sp<SkTable_ColorFilter>(tableA, tableR, tableG, tableB);
}

void SkTableColorFilter::RegisterFlattenables() { SK_REGISTER_FLATTENABLE(SkTable_ColorFilter); }
