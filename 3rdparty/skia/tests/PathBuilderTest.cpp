/*
 * Copyright 2020 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/core/SkPathBuilder.h"
#include "src/core/SkPathPriv.h"
#include "tests/Test.h"

static void is_empty(skiatest::Reporter* reporter, const SkPath& p) {
    REPORTER_ASSERT(reporter, p.getBounds().isEmpty());
    REPORTER_ASSERT(reporter, p.countPoints() == 0);
}

DEF_TEST(pathbuilder, reporter) {
    SkPathBuilder b;

    is_empty(reporter, b.snapshot());
    is_empty(reporter, b.detach());

    b.moveTo(10, 10).lineTo(20, 20).quadTo(30, 10, 10, 20);

    SkPath p0 = b.snapshot();
    SkPath p1 = b.snapshot();
    SkPath p2 = b.detach();

    REPORTER_ASSERT(reporter, p0.getBounds() == SkRect::MakeLTRB(10, 10, 30, 20));
    REPORTER_ASSERT(reporter, p0.countPoints() == 4);

    REPORTER_ASSERT(reporter, p0 == p1);
    REPORTER_ASSERT(reporter, p0 == p2);

    is_empty(reporter, b.snapshot());
    is_empty(reporter, b.detach());
}

static bool check_points(const SkPath& path, const SkPoint expected[], size_t count) {
    std::vector<SkPoint> iter_pts;

    for (auto [v, p, w] : SkPathPriv::Iterate(path)) {
        switch (v) {
            case SkPathVerb::kMove:
                iter_pts.push_back(p[0]);
                break;
            case SkPathVerb::kLine:
                iter_pts.push_back(p[1]);
                break;
            case SkPathVerb::kQuad:
            case SkPathVerb::kConic:
                iter_pts.push_back(p[1]);
                iter_pts.push_back(p[2]);
                break;
            case SkPathVerb::kCubic:
                iter_pts.push_back(p[1]);
                iter_pts.push_back(p[2]);
                iter_pts.push_back(p[3]);
                break;
            case SkPathVerb::kClose:
                break;
        }
    }
    if (iter_pts.size() != count) {
        return false;
    }
    for (size_t i = 0; i < count; ++i) {
        if (iter_pts[i] != expected[i]) {
            return false;
        }
    }
    return true;
}

DEF_TEST(pathbuilder_missing_move, reporter) {
    SkPathBuilder b;

    b.lineTo(10, 10).lineTo(20, 30);
    const SkPoint pts0[] = {
        {0, 0}, {10, 10}, {20, 30},
    };
    REPORTER_ASSERT(reporter, check_points(b.snapshot(), pts0, SK_ARRAY_COUNT(pts0)));

    b.reset().moveTo(20, 20).lineTo(10, 10).lineTo(20, 30).close().lineTo(60, 60);
    const SkPoint pts1[] = {
        {20, 20}, {10, 10}, {20, 30},
        {20, 20}, {60, 60},
    };
    REPORTER_ASSERT(reporter, check_points(b.snapshot(), pts1, SK_ARRAY_COUNT(pts1)));
}

DEF_TEST(pathbuilder_addRect, reporter) {
    const SkRect r = { 10, 20, 30, 40 };

    for (int i = 0; i < 4; ++i) {
        for (auto dir : {SkPathDirection::kCW, SkPathDirection::kCCW}) {
            SkPathBuilder b;
            b.addRect(r, dir, i);
            auto bp = b.detach();

            SkRect r2;
            bool   closed = false;
            SkPathDirection dir2;
            REPORTER_ASSERT(reporter, bp.isRect(&r2, &closed, &dir2));
            REPORTER_ASSERT(reporter, r2 == r);
            REPORTER_ASSERT(reporter, closed);
            REPORTER_ASSERT(reporter, dir == dir2);

            SkPath p;
            p.addRect(r, dir, i);
            REPORTER_ASSERT(reporter, p == bp);
        }
    }
}

DEF_TEST(pathbuilder_addOval, reporter) {
    const SkRect r = { 10, 20, 30, 40 };

    for (int i = 0; i < 4; ++i) {
        for (auto dir : {SkPathDirection::kCW, SkPathDirection::kCCW}) {
            SkPathBuilder b;
            b.addOval(r, dir, i);
            auto bp = b.detach();

            SkPath p;
            p.addOval(r, dir, i);
            REPORTER_ASSERT(reporter, p == bp);
        }
    }
}

DEF_TEST(pathbuilder_addRRect, reporter) {
    const SkRRect rr = SkRRect::MakeRectXY({ 10, 20, 30, 40 }, 5, 6);

    for (int i = 0; i < 4; ++i) {
        for (auto dir : {SkPathDirection::kCW, SkPathDirection::kCCW}) {
            SkPathBuilder b;
            b.addRRect(rr, dir, i);
            auto bp = b.detach();

            SkPath p;
            p.addRRect(rr, dir, i);
            REPORTER_ASSERT(reporter, p == bp);
        }
    }
}

#include "include/utils/SkRandom.h"

DEF_TEST(pathbuilder_make, reporter) {
    constexpr int N = 100;
    uint8_t vbs[N];
    SkPoint pts[N];

    SkRandom rand;
    SkPathBuilder b;
    b.moveTo(0, 0);
    pts[0] = {0, 0}; vbs[0] = (uint8_t)SkPathVerb::kMove;
    for (int i = 1; i < N; ++i) {
        float x = rand.nextF();
        float y = rand.nextF();
        b.lineTo(x, y);
        pts[i] = {x, y}; vbs[i] = (uint8_t)SkPathVerb::kLine;
    }
    auto p0 = b.detach();
    auto p1 = SkPathBuilder::Make(pts, N, vbs, N, nullptr, 0, p0.getFillType());
    REPORTER_ASSERT(reporter, p0 == p1);
}
