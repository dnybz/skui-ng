/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrEllipseEffect.fp; do not modify.
 **************************************************************************************************/
#ifndef GrEllipseEffect_DEFINED
#define GrEllipseEffect_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"

#include "src/gpu/GrShaderCaps.h"

#include "src/gpu/GrCoordTransform.h"
#include "src/gpu/GrFragmentProcessor.h"

class GrEllipseEffect : public GrFragmentProcessor {
public:
    static GrFPResult Make(std::unique_ptr<GrFragmentProcessor> inputFP,
                           GrClipEdgeType edgeType,
                           SkPoint center,
                           SkPoint radii,
                           const GrShaderCaps& caps) {
        // Small radii produce bad results on devices without full float.
        if (!caps.floatIs32Bits() && (radii.fX < 0.5f || radii.fY < 0.5f)) {
            return GrFPFailure(std::move(inputFP));
        }
        // Very narrow ellipses produce bad results on devices without full float
        if (!caps.floatIs32Bits() && (radii.fX > 255 * radii.fY || radii.fY > 255 * radii.fX)) {
            return GrFPFailure(std::move(inputFP));
        }
        // Very large ellipses produce bad results on devices without full float
        if (!caps.floatIs32Bits() && (radii.fX > 16384 || radii.fY > 16384)) {
            return GrFPFailure(std::move(inputFP));
        }
        return GrFPSuccess(std::unique_ptr<GrFragmentProcessor>(
                new GrEllipseEffect(std::move(inputFP), edgeType, center, radii)));
    }
    GrEllipseEffect(const GrEllipseEffect& src);
    std::unique_ptr<GrFragmentProcessor> clone() const override;
    const char* name() const override { return "EllipseEffect"; }
    int inputFP_index = -1;
    GrClipEdgeType edgeType;
    SkPoint center;
    SkPoint radii;

private:
    GrEllipseEffect(std::unique_ptr<GrFragmentProcessor> inputFP,
                    GrClipEdgeType edgeType,
                    SkPoint center,
                    SkPoint radii)
            : INHERITED(kGrEllipseEffect_ClassID,
                        (OptimizationFlags)(inputFP ? ProcessorOptimizationFlags(inputFP.get())
                                                    : kAll_OptimizationFlags) &
                                kCompatibleWithCoverageAsAlpha_OptimizationFlag)
            , edgeType(edgeType)
            , center(center)
            , radii(radii) {
        if (inputFP) {
            inputFP_index = this->registerChild(std::move(inputFP));
        }
    }
    GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
    void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
    bool onIsEqual(const GrFragmentProcessor&) const override;
    GR_DECLARE_FRAGMENT_PROCESSOR_TEST
    typedef GrFragmentProcessor INHERITED;
};
#endif
